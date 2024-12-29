#include "interpreter.h"
#include "../debug/debug.h"
#include <stdio.h>
#include <string.h>

// Helper function to create a default LiteralValue (zero number)
LiteralValue create_default_value() {
    LiteralValue value = {
        .type = TYPE_INTEGER,
        .data = {
            // .floating_point = 0.0,
            .integer = 0, // main value used for comparisons, etc
            // .string = "",
        }};
    return value;
}

// A helper to wrap `LiteralValue` in `InterpretResult`
static InterpretResult make_result(LiteralValue val, bool did_return) {
    InterpretResult r;
    r.value = val;
    r.did_return = did_return;
    return r;
}

InterpretResult interpret_node(ASTNode *node, Environment *env) {
    if (!node) {
        fprintf(stderr, "Error: Attempt to interpret NULL node\n");
        return make_result((LiteralValue){.type = TYPE_ERROR}, false);
    }

    debug_print_int("`interpret_node()` called\n");

    switch (node->type) {
    case AST_LITERAL:
        debug_print_int("\tMatched: `AST_LITERAL`\n");
        return make_result(interpret_literal(node), false);

    case AST_ASSIGNMENT:
        debug_print_int("\tMatched: `AST_ASSIGNMENT`\n");
        return make_result(interpret_assignment(node, env), false);

    case AST_BINARY_OP:
        debug_print_int("\tMatched: `AST_BINARY_OP`\n");
        return make_result(interpret_binary_op(node, env), false);

    case AST_PRINT:
        debug_print_int("\tMatched: `AST_PRINT`\n");
        interpret_print(node, env);
        return make_result(create_default_value(), false);

    case AST_INPUT: {
        debug_print_int("\tMatched: `AST_INPUT`\n");
        Variable v = interpret_input(env);

        // Wrap `v.value` in an `InterpretResult`, with `did_return=false`
        return make_result(v.value, false);
    }

    case AST_CONDITIONAL: {
        debug_print_int("\tMatched: `AST_CONDITIONAL`\n");
        return interpret_conditional(node, env);
    }

    case AST_FUNCTION_CALL: {
        debug_print_int("\tMatched: `AST_FUNCTION_CALL`\n");
        // interpret_function_call(...) returns a LiteralValue
        LiteralValue fc_val = interpret_function_call(node, env);
        // wrap it in an InterpretResult
        return make_result(fc_val, false);
    }

    case AST_FUNCTION_DECLARATION: {
        debug_print_int("\tMatched: `AST_FUNCTION_DECLARATION`\n");
        interpret_function_declaration(node, env);
        // again, no direct return from a function declaration
        return make_result(create_default_value(), false);
    }

    case AST_FUNCTION_RETURN: {
        debug_print_int("\tMatched: `AST_FUNCTION_RETURN`\n");

        if (!node->assignment.value) {
            fprintf(
                stderr,
                "Error: Return statement has no expression! (Parser bug?)\n");
            exit(1);
        }

        LiteralValue return_value =
            interpret_node(node->assignment.value, env).value;
        debug_print_int("Return value before returning: type=%d, value=%d\n",
                        return_value.type,
                        (return_value.type == TYPE_INTEGER)
                            ? return_value.data.integer
                            : 0);

        // Return this value, but also set did_return = true
        return make_result(return_value, true);
    }

    case AST_LOOP: {
        debug_print_int("\tMatched: `AST_LOOP`\n");
        interpret_while_loop(node, env);
        return make_result(create_default_value(), false);
    }

    case AST_VARIABLE: {
        debug_print_int("\tMatched: `AST_VARIABLE`\n");
        LiteralValue var_val = interpret_variable(node, env);
        return make_result(var_val, false);
    }

    case AST_SWITCH: {
        debug_print_int("\tMatched: `AST_SWITCH`\n");
        interpret_switch(node, env);
        return make_result(create_default_value(), false);
    }

    case AST_ERROR: {
        debug_print_int("\tMatched: `AST_ERROR`\n");
        interpret_raise_error(node, env);
        // interpret_raise_error usually calls exit(), but let's just do:
        return make_result(create_default_value(), false);
    }
    default:
        fprintf(stderr, "Error: Unsupported ASTNode type.\n");
        exit(1);
    }
}

void interpret_program(ASTNode *program, Environment *env) {
    ASTNode *current = program;
    while (current) {
        debug_print_int("Executing top-level statement\n");
        interpret_node(current, env);
        current = current->next;
    }
}

LiteralValue interpret_literal(ASTNode *node) {
    LiteralValue value;
    debug_print_int("Interpreting literal value...\n");
    debug_print_int("Literal type: %d\n", node->literal.type);

    switch (node->literal.type) {
    case LITERAL_STRING:
        value.type = TYPE_STRING;
        value.data.string = strdup(node->literal.value.string);
        debug_print_int("Created string literal: `%s`\n", value.data.string);
        break;
    case LITERAL_FLOAT:
        value.type = TYPE_FLOAT;
        value.data.floating_point = node->literal.value.floating_point;
        debug_print_int("Created float literal: `%f`\n",
                        value.data.floating_point);
        break;
    case LITERAL_INTEGER:
        value.type = TYPE_INTEGER;
        value.data.integer = node->literal.value.integer;
        debug_print_int("Created integer literal: `%d`\n", value.data.integer);
        break;
    case LITERAL_BOOLEAN:
        value.type = TYPE_BOOLEAN;
        value.data.boolean = node->literal.value.boolean;
        debug_print_int("Created boolean literal: `%s`\n",
                        value.data.boolean ? "True" : "False");
        break;
    default:
        fprintf(stderr, "Error: Unsupported literal type.\n");
        exit(1);
    }
    return value;
}

LiteralValue interpret_variable(ASTNode *node, Environment *env) {
    // printf("Env var 0: `%s`\n", env->variables[0].variable_name);
    Variable *var = get_variable(env, node->variable_name);
    if (!var) {
        fprintf(stderr, "Error: Undefined variable `%s`.\n",
                node->variable_name);
        exit(1);
    }

    return var->value;
}

LiteralValue interpret_assignment(ASTNode *node, Environment *env) {
    if (!node->assignment.variable_name) {
        debug_print_int("Assignment variable name missing for node type: %d\n",
                        node->type);
        return (LiteralValue){.type = TYPE_ERROR};
    }

    // Evaluate the right-hand side
    InterpretResult assign_r = interpret_node(node->assignment.value, env);
    // If the RHS triggered a return, we’re ignoring it for assignment, but you
    // could handle it
    LiteralValue new_value = assign_r.value;

    // Add or update variable
    Variable new_var = {.variable_name = strdup(node->assignment.variable_name),
                        .value = new_value};
    add_variable(env, new_var);

    return new_value;
}

// Function to handle string concatenation
LiteralValue handle_string_concatenation(LiteralValue left,
                                         LiteralValue right) {
    LiteralValue result;
    result.type = TYPE_STRING;

    // Convert numbers to strings
    char num_str1[50] = {0};
    char num_str2[50] = {0};

    if (left.type == TYPE_FLOAT) {
        snprintf(num_str1, sizeof(num_str1), "%f", left.data.floating_point);
    }
    if (right.type == TYPE_FLOAT) {
        snprintf(num_str2, sizeof(num_str2), "%f", right.data.floating_point);
    }

    // Allocate memory for the concatenated string
    size_t new_size =
        strlen(num_str1) + strlen(num_str2) +
        strlen(left.type == TYPE_STRING ? left.data.string : "") +
        strlen(right.type == TYPE_STRING ? right.data.string : "") + 1;

    char *new_string = malloc(new_size);
    if (!new_string) {
        fprintf(stderr,
                "Error: Memory allocation failed for string concatenation.\n");
        exit(1);
    }

    strcpy(new_string, left.type == TYPE_STRING ? left.data.string : num_str1);
    strcat(new_string,
           right.type == TYPE_STRING ? right.data.string : num_str2);
    result.data.string = new_string;

    return result;
}

LiteralValue interpret_binary_op(ASTNode *node, Environment *env) {
    debug_print_int("Interpreting binary operation\n");

    // First evaluate both operands
    InterpretResult left_r = interpret_node(node->binary_op.left, env);
    LiteralValue left = left_r.value;
    if (left.type == TYPE_ERROR) {
        return left; // or handle error
    }

    InterpretResult right_r = interpret_node(node->binary_op.right, env);
    LiteralValue right = right_r.value;
    if (right.type == TYPE_ERROR) {
        return right;
    }

    char *operator= node->binary_op.operator;
    debug_print_int("Operator: `%s`\n", operator);

    // Handle string concatenation with "+" operator
    if (operator[0] ==
        '+' &&(left.type == TYPE_STRING || right.type == TYPE_STRING)) {
        return handle_string_concatenation(left, right);
    }

    // Get numeric values for arithmetic and comparison
    double left_value = 0.0, right_value = 0.0;
    if (left.type == TYPE_FLOAT || right.type == TYPE_FLOAT) {
        left_value = (left.type == TYPE_FLOAT) ? left.data.floating_point
                                               : (double)left.data.integer;
        right_value = (right.type == TYPE_FLOAT) ? right.data.floating_point
                                                 : (double)right.data.integer;
    } else {
        left_value = (double)left.data.integer;
        right_value = (double)right.data.integer;
    }

    LiteralValue result;
    // Default to TYPE_INTEGER or TYPE_FLOAT for arithmetic operations
    if (strcmp(operator, "*") == 0 || strcmp(operator, "/") == 0 ||
        strcmp(operator, "+") == 0 || strcmp(operator, "-") == 0) {
        result.type = (left.type == TYPE_FLOAT || right.type == TYPE_FLOAT)
                          ? TYPE_FLOAT
                          : TYPE_INTEGER;
    }

    // First check for two-character operators
    if (operator[1] != '\0') {
        if (strcmp(operator, "<=") == 0) {
            result.type = TYPE_BOOLEAN; // Changed to TYPE_BOOLEAN
            result.data.boolean = (left_value <= right_value);
            return result;
        }
        if (strcmp(operator, ">=") == 0) {
            result.type = TYPE_BOOLEAN;
            result.data.boolean = (left_value >= right_value);
            return result;
        }
        if (strcmp(operator, "==") == 0) {
            result.type = TYPE_BOOLEAN;
            result.data.boolean = (left_value == right_value);
            return result;
        }
        if (strcmp(operator, "!=") == 0) {
            result.type = TYPE_BOOLEAN;
            result.data.boolean = (left_value != right_value);
            return result;
        }
    }

    // Then handle single-character operators
    switch (operator[0]) {
    case '*':
        if (result.type == TYPE_FLOAT)
            result.data.floating_point = left_value * right_value;
        else
            result.data.integer = (int)(left_value * right_value);
        break;
    case '+':
        if (result.type == TYPE_FLOAT)
            result.data.floating_point = left_value + right_value;
        else
            result.data.integer = (int)(left_value + right_value);
        break;
    case '-':
        if (result.type == TYPE_FLOAT)
            result.data.floating_point = left_value - right_value;
        else
            result.data.integer = (int)(left_value - right_value);
        break;
    case '/':
        if (right_value == 0) {
            fprintf(stderr, "Error: Division by zero\n");
            exit(1);
        }
        if (result.type == TYPE_FLOAT)
            result.data.floating_point = left_value / right_value;
        else
            result.data.integer = (int)(left_value / right_value);
        break;
    case '<':
        result.type = TYPE_BOOLEAN;
        result.data.boolean = (left_value < right_value);
        break;
    case '>':
        result.type = TYPE_BOOLEAN;
        result.data.boolean = (left_value > right_value);
        break;
    default:
        fprintf(stderr, "Error: Unknown operator `%s`\n", operator);
        exit(1);
    }

    return result;
}

Variable *get_variable(Environment *env, const char *variable_name) {
    debug_print_int("Looking up variable: `%s`\n", variable_name);
    for (size_t i = 0; i < env->variable_count; i++) {
        if (strcmp(env->variables[i].variable_name, variable_name) == 0) {
            if (env->variables[i].value.type == TYPE_FLOAT) {
                debug_print_int("Variable found: `%s` with value `%f`\n",
                                variable_name,
                                env->variables[i].value.data.floating_point);
            } else if (env->variables[i].value.type == TYPE_INTEGER) {
                debug_print_int("Variable found: `%s` with value `%d`\n",
                                variable_name,
                                env->variables[i].value.data.integer);
            } else if (env->variables[i].value.type == TYPE_STRING) {
                debug_print_int("Variable found: `%s` with value `%s`\n",
                                variable_name,
                                env->variables[i].value.data.string);
            }
            return &env->variables[i];
        }
    }
    debug_print_int("Variable not found: `%s`\n", variable_name);
    return NULL;
}

void add_variable(Environment *env, Variable var) {
    // Check if the variable already exists
    for (size_t i = 0; i < env->variable_count; i++) {
        if (strcmp(env->variables[i].variable_name, var.variable_name) == 0) {
            // Update the value of the existing variable
            if (env->variables[i].value.type == TYPE_STRING &&
                env->variables[i].value.data.string) {
                free(env->variables[i]
                         .value.data.string); // Free existing string memory
            }
            env->variables[i].value = var.value;

            debug_print_int("Updated variable: `%s`\n", var.variable_name);
            return;
        }
    }

    // Add a new variable
    if (env->variable_count == env->capacity) {
        // Resize the variables array if necessary
        env->capacity = env->capacity ? env->capacity * 2 : 4;
        env->variables =
            realloc(env->variables, env->capacity * sizeof(Variable));
        if (!env->variables) {
            fprintf(
                stderr,
                "Error: Memory allocation failed while adding variable `%s`.\n",
                var.variable_name);
            exit(1);
        }
    }

    // Copy variable name and value
    env->variables[env->variable_count].variable_name =
        strdup(var.variable_name);
    if (var.value.type == TYPE_STRING) {
        env->variables[env->variable_count].value.type = TYPE_STRING;
        env->variables[env->variable_count].value.data.string =
            strdup(var.value.data.string);
    } else {
        env->variables[env->variable_count].value = var.value;
    }

    debug_print_int("Added variable: `%s`\n", var.variable_name);
    env->variable_count++;
}

void interpret_print(ASTNode *node, Environment *env) {
    debug_print_int("`interpret_print()`\n");

    for (size_t i = 0; i < node->to_print.arg_count; i++) {
        ASTNode *arg = node->to_print.arguments[i];
        InterpretResult r = interpret_node(arg, env);
        LiteralValue lv = r.value;

        switch (lv.type) {
        case TYPE_FLOAT:
            if ((int)lv.data.floating_point == lv.data.floating_point) {
                printf("%.1f", lv.data.floating_point);
            } else {
                printf("%g", lv.data.floating_point);
            }
            break;
        case TYPE_INTEGER:
            printf("%d", lv.data.integer);
            break;
        case TYPE_STRING:
            printf("%s", lv.data.string);
            break;
        case TYPE_BOOLEAN:
            printf("%s", lv.data.boolean ? "True" : "False");
            break;
        case TYPE_ERROR:
            fprintf(stderr, "Error: Invalid literal type.\n");
            break;
        default:
            fprintf(stderr, "Error: Unknown literal type.\n");
            break;
        }
        printf(" "); // space for padding
    }
    printf("\n");
}

void interpret_raise_error(ASTNode *node, Environment *env) {
    int divider_length = 50;
    char *error_banner = "  !!! ERROR RAISED !!!  ";
    int error_banner_length = (divider_length - strlen(error_banner)) / 2 - 1;

    printf("\x1B[31m");
    printf("\x1B[01m");

    printf("<");
    for (int i = 1; i < divider_length; i++) {
        printf("=");
    }
    printf(">\n\n ");
    for (int i = 0; i < error_banner_length; i++) {
        printf("~");
    }
    printf("%s", error_banner);
    for (int i = 0; i < error_banner_length; i++) {
        printf("~");
    }
    printf(" \n\n");

    interpret_print(node, env);

    printf("\n<");
    for (int i = 1; i < divider_length; i++) {
        printf("=");
    }
    printf(">\n");

    printf("\x1B[0m");

    exit(1);
}

Variable *allocate_variable(Environment *env, const char *name) {
    // Check if the variable already exists
    for (size_t i = 0; i < env->variable_count; i++) {
        if (strcmp(env->variables[i].variable_name, name) == 0) {
            return &env->variables[i]; // return existing variable
        }
    }

    // If the variable doesn't exist, allocate it in memory
    if (env->variable_count == env->capacity) {
        env->capacity *= 2;
        env->variables =
            realloc(env->variables, env->capacity * sizeof(Variable));
        if (!env->variables) {
            fprintf(stderr, "Error: Memory allocation failed.\n");
            exit(1);
        }
    }

    env->variables[env->variable_count].variable_name = strdup(name);
    env->variable_count++;

    return &env->variables[env->variable_count - 1];
}

Variable interpret_input(Environment *env) {
    size_t buffer_size = 128;
    size_t input_length = 0;
    char *input_buffer = malloc(buffer_size);
    if (!input_buffer) {
        fprintf(stderr, "Error: Failed to allocate memory for input buffer.\n");
        // Return an empty or error `Variable`
        Variable error_var = {.value.type = TYPE_ERROR};
        return error_var;
    }

    // Read input
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
        if (input_length + 1 >= buffer_size) {
            buffer_size *= 2;
            char *new_buffer = realloc(input_buffer, buffer_size);
            if (!new_buffer) {
                fprintf(
                    stderr,
                    "Error: Failed to reallocate memory for input buffer.\n");
                free(input_buffer);
                // Return an empty or error `Variable`
                Variable error_var = {.value.type = TYPE_ERROR};
                return error_var;
            }
            input_buffer = new_buffer;
        }
        input_buffer[input_length++] = c;
    }
    input_buffer[input_length] = '\0';

    debug_print_int("Read input: `%s`\n", input_buffer);

    // Allocate variable
    Variable *var = allocate_variable(env, "input_value");
    if (!var) {
        fprintf(stderr, "Error: Failed to create variable for input.\n");
        free(input_buffer);
        // Return an empty or error `Variable`
        Variable error_var = {.value.type = TYPE_ERROR};
        return error_var;
    }

    var->value.type = TYPE_STRING;
    var->value.data.string = strdup(input_buffer);

    debug_print_int("Stored value: `%s`\n", var->value.data.string);

    free(input_buffer);

    return *var;
}

InterpretResult interpret_conditional(ASTNode *node, Environment *env) {
    debug_print_int("`interpret_conditional()` called\n");
    if (!node) {
        // Error
        return make_result((LiteralValue){.type = TYPE_ERROR}, false);
    }

    ASTNode *current_branch = node;
    bool condition_met = false;

    while (current_branch) {
        if (current_branch->conditional.condition) {
            InterpretResult cond_res =
                interpret_node(current_branch->conditional.condition, env);
            if (cond_res.did_return) {
                // Bubble up immediately
                return cond_res;
            }

            // Check for valid condition types
            if (cond_res.value.type != TYPE_INTEGER &&
                cond_res.value.type != TYPE_BOOLEAN) {
                fprintf(stderr, "Error: Condition expression must be boolean "
                                "or integer.\n");
                return make_result((LiteralValue){.type = TYPE_ERROR}, false);
            }

            bool condition_true = false;
            if (cond_res.value.type == TYPE_BOOLEAN) {
                condition_true = cond_res.value.data.boolean;
            } else if (cond_res.value.type == TYPE_INTEGER) {
                condition_true = (cond_res.value.data.integer != 0);
            }

            if (condition_true) {
                // Interpret the conditional body
                ASTNode *cs = current_branch->conditional.body;
                while (cs) {
                    InterpretResult body_res = interpret_node(cs, env);
                    if (body_res.did_return) {
                        return body_res;
                    }
                    cs = cs->next;
                }
                condition_met = true;

                // Exit after the first true condition
                break;
            }
        } else {
            // Else branch
            if (!condition_met) {
                ASTNode *cs = current_branch->conditional.body;
                while (cs) {
                    InterpretResult body_res = interpret_node(cs, env);
                    if (body_res.did_return) {
                        return body_res;
                    }
                    cs = cs->next;
                }
            }

            // Exit after else
            break;
        }

        current_branch = current_branch->conditional.else_branch;
    }

    debug_print_int("`interpret_conditional()` completed\n");

    // Return a default value if no conditions met
    return make_result(create_default_value(), false);
}

void interpret_while_loop(ASTNode *node, Environment *env) {
    debug_print_int("`interpret_while_loop()` called\n");

    ASTNode *condition = node->loop.condition;
    ASTNode *body = node->loop.body;

    while (1) {
        InterpretResult cond_r = interpret_node(condition, env);
        if (cond_r.did_return) {
            // Bubble up immediately
            return;
        }

        // Check for valid condition types
        if (cond_r.value.type != TYPE_INTEGER &&
            cond_r.value.type != TYPE_BOOLEAN) {
            fprintf(stderr, "While condition must be boolean or integer\n");
            return;
        }

        bool condition_true = false;
        if (cond_r.value.type == TYPE_BOOLEAN) {
            condition_true = cond_r.value.data.boolean;
        } else if (cond_r.value.type == TYPE_INTEGER) {
            condition_true = (cond_r.value.data.integer != 0);
        }

        if (condition_true) {
            // Interpret the loop body
            ASTNode *current = body;
            while (current) {
                InterpretResult body_r = interpret_node(current, env);
                if (body_r.did_return) {
                    // Bubble up
                    return;
                }
                current = current->next;
            }
        } else {
            // Exit loop if condition is false
            break;
        }
    }

    debug_print_int("`interpret_while_loop()` completed\n");
}

void interpret_switch(ASTNode *node, Environment *env) {
    debug_print_int("`interpret_switch()`\n");

    // Evaluate the switch expression
    InterpretResult switch_r =
        interpret_node(node->switch_case.expression, env);
    LiteralValue switch_val = switch_r.value;
    debug_print_int("Switch expression evaluated\n");

    ASTCaseNode *current_case = node->switch_case.cases;
    bool break_encountered = false;

    while (current_case && !break_encountered) {
        if (current_case->condition == NULL) {
            // `else` case
            debug_print_int("Executing `else` case\n");
            ASTNode *current_statement = current_case->body;
            while (current_statement && !break_encountered) {
                if (current_statement->type == AST_BREAK) {
                    break_encountered = true;
                    break;
                }
                // `interpret_node(...)` returning `InterpretResult`
                interpret_node(current_statement, env);
                current_statement = current_statement->next;
            }
            break;
        } else {
            // Evaluate the case condition
            InterpretResult case_r =
                interpret_node(current_case->condition, env);
            LiteralValue case_val = case_r.value;

            bool values_match = false;

            // Handle type comparison
            if (switch_val.type == case_val.type) {
                if (switch_val.type == TYPE_BOOLEAN) {
                    values_match =
                        (switch_val.data.boolean == case_val.data.boolean);
                } else if (switch_val.type == TYPE_FLOAT) {
                    values_match = (switch_val.data.floating_point ==
                                    case_val.data.floating_point);
                } else if (switch_val.type == TYPE_INTEGER) {
                    values_match =
                        (switch_val.data.integer == case_val.data.integer);
                } else if (switch_val.type == TYPE_STRING) {
                    values_match = (strcmp(switch_val.data.string,
                                           case_val.data.string) == 0);
                }
            }

            if (values_match) {
                debug_print_int("Match found, executing case body\n");

                ASTNode *current_statement = current_case->body;
                while (current_statement && !break_encountered) {
                    if (current_statement->type == AST_BREAK) {
                        break_encountered = true;
                        break;
                    }
                    interpret_node(current_statement, env);
                    current_statement = current_statement->next;
                }
                // If break_encountered, break
            }
        }

        current_case = current_case->next;
    }

    debug_print_int("Switch statement interpretation complete\n");
}

// Helper function to free a linked list of ASTFunctionParameter nodes
void free_parameter_list(ASTFunctionParameter *head) {
    ASTFunctionParameter *current = head;
    while (current != NULL) {
        ASTFunctionParameter *next = current->next;
        free(current->parameter_name);
        free(current);
        current = next;
    }
}

ASTFunctionParameter *
copy_function_parameters(ASTFunctionParameter *param_list) {
    if (param_list == NULL) {
        // No parameters to copy
        return NULL;
    }

    ASTFunctionParameter *new_head = NULL;
    ASTFunctionParameter *new_tail = NULL;

    while (param_list != NULL) {
        // Allocate memory for the new parameter
        ASTFunctionParameter *new_param = malloc(sizeof(ASTFunctionParameter));
        if (new_param == NULL) {
            fprintf(stderr, "Memory allocation failed for new parameter\n");
            free_parameter_list(
                new_head); // Clean up all previously allocated nodes
            exit(1);
        }

        // Duplicate parameter name, ensuring it is not NULL
        if (param_list->parameter_name) {
            new_param->parameter_name = strdup(param_list->parameter_name);
            if (new_param->parameter_name == NULL) {
                fprintf(stderr,
                        "Memory allocation failed for parameter name\n");
                free(new_param); // Free the current node
                free_parameter_list(
                    new_head); // Clean up previously allocated nodes
                exit(1);
            }
        } else {
            new_param->parameter_name = NULL;
        }

        // Initialize the `next` pointer to NULL
        new_param->next = NULL;

        // Add the new parameter to the linked list
        if (new_head == NULL) {
            new_head = new_param; // Set as head if it's the first node
        } else {
            new_tail->next = new_param; // Attach to the end of the list
        }
        new_tail = new_param; // Update the tail pointer

        // Move to the next parameter in the source list
        param_list = param_list->next;
    }

    return new_head;
}

ASTNode *copy_ast_node(ASTNode *node) {
    if (!node)
        return NULL;

    ASTNode *new_node = malloc(sizeof(ASTNode));
    if (!new_node) {
        fprintf(stderr, "Error: Memory allocation failed in `copy_ast_node`\n");
        exit(1);
    }

    memcpy(new_node, node, sizeof(ASTNode));

    // Deep copy for fields like `function_call`, `body`, or `arguments`
    if (node->function_call.arguments) {
        new_node->function_call.arguments =
            copy_ast_node(node->function_call.arguments);
    }
    if (node->function_call.body) {
        new_node->function_call.body = copy_ast_node(node->function_call.body);
    }

    // Handle other types like linked lists or child nodes as needed
    return new_node;
}

void add_function(Environment *env, Function func) {
    // Step 1: Ensure `functions` is initialized if not already done
    if (!env->functions && env->function_capacity == 0) {
        env->functions =
            calloc(4, sizeof(Function)); // initialize with capacity
        if (!env->functions) {
            fprintf(stderr,
                    "Error: Initial allocation for functions failed.\n");
            exit(1);
        }
        env->function_capacity = 4;
    }

    // Step 2: Resize `functions` array if necessary
    if (env->function_count == env->function_capacity) {
        size_t new_capacity = env->function_capacity * 2;
        Function *new_functions =
            realloc(env->functions, new_capacity * sizeof(Function));
        if (!new_functions) {
            fprintf(stderr, "Error: Memory allocation failed for functions.\n");
            exit(1);
        }
        env->functions = new_functions;
        env->function_capacity = new_capacity;
    }

    // Step 3: Verify `func.name` is valid
    if (!func.name) {
        fprintf(stderr, "Error: Function name is `NULL` or invalid.\n");
        exit(1);
    }

    // Step 4: Create a deep copy of the function being added
    Function *stored_func = &env->functions[env->function_count++];
    stored_func->parameters = copy_function_parameters(
        func.parameters); // Create a copy of parameters
    stored_func->body =
        copy_ast_node(func.body); // Still assuming shared ownership of the body
                                  // (might need a copy if modified)

    // Step 5: Safely duplicate the function name
    stored_func->name = strdup(func.name); // allocate memory for name
    if (!stored_func->name) {
        fprintf(stderr, "Error: Memory allocation failed for function name.\n");
        free(stored_func); // Free partially allocated function on error
        exit(1);
    }

    debug_print_int("Function `%s` added successfully.\n", stored_func->name);
}

Function *get_function(Environment *env, const char *name) {
    for (size_t i = 0; i < env->function_count; i++) {
        if (strcmp(env->functions[i].name, name) == 0) {
            return &env->functions[i];
        }
    }
    return NULL;
}

void interpret_function_declaration(ASTNode *node, Environment *env) {
    debug_print_int("`interpret_function_declaration()` called\n");

    if (!node || !node->function_call.name) {
        fprintf(stderr, "Error: Invalid function declaration\n");
        exit(1);
    }

    // Initialize param_list as NULL
    ASTFunctionParameter *param_list = NULL;
    ASTFunctionParameter *param_tail = NULL; // keep track of the last parameter

    // Copy parameters
    ASTFunctionParameter *param = node->function_call.parameters;
    while (param) {
        ASTFunctionParameter *new_param = malloc(sizeof(ASTFunctionParameter));
        if (!new_param) {
            fprintf(stderr,
                    "Error: Memory allocation failed for function parameter\n");
            exit(1);
        }

        new_param->parameter_name = strdup(param->parameter_name);
        if (!new_param->parameter_name) {
            fprintf(stderr,
                    "Error: Memory allocation failed for parameter name\n");
            free(new_param);
            exit(1);
        }

        new_param->next = NULL;

        // Add to list
        if (!param_list) {
            param_list = new_param;
            param_tail = new_param;
        } else {
            param_tail->next = new_param;
            param_tail = new_param;
        }

        param = param->next;
    }

    Function func = {.name = strdup(node->function_call.name),
                     .parameters = param_list,
                     .body = node->function_call.body};

    add_function(env, func);
}

LiteralValue interpret_function_call(ASTNode *node, Environment *env) {
    debug_print_int("Starting function call interpretation\n");

    if (!node || !node->function_call.name) {
        fprintf(stderr, "Error: Invalid function call\n");
        return (LiteralValue){.type = TYPE_ERROR};
    }

    Function *func = get_function(env, node->function_call.name);
    if (!func) {
        fprintf(stderr, "Error: Undefined function `%s`\n",
                node->function_call.name);
        return (LiteralValue){.type = TYPE_ERROR};
    }

    // Create local environment
    Environment local_env;
    init_environment(&local_env);

    // Copy parent's functions
    for (size_t i = 0; i < env->function_count; i++) {
        Function func_copy = {.name = strdup(env->functions[i].name),
                              .parameters = copy_function_parameters(
                                  env->functions[i].parameters),
                              .body = copy_ast_node(env->functions[i].body)};
        add_function(&local_env, func_copy);
    }

    // Bind parameters
    ASTFunctionParameter *p = func->parameters;
    ASTNode *arg = node->function_call.arguments;
    while (p && arg) {
        InterpretResult arg_res = interpret_node(arg, env);
        LiteralValue arg_value = arg_res.value;
        if (arg_value.type == TYPE_ERROR) {
            free_environment(&local_env);
            return arg_value; // or do something
        }

        Variable param_var = {.variable_name = strdup(p->parameter_name),
                              .value = arg_res.value};
        add_variable(&local_env, param_var);

        p = p->next;
        arg = arg->next;
    }

    // Now interpret function body
    LiteralValue result = create_default_value(); // default 0
    ASTNode *stmt = func->body;

    while (stmt) {
        InterpretResult r = interpret_node(stmt, &local_env);

        if (r.did_return) {
            // Short-circuit
            free_environment(&local_env);
            return r.value; // the literal from the return
        }

        if (r.value.type == TYPE_ERROR) {
            free_environment(&local_env);
            return r.value;
        }

        // else keep going
        stmt = stmt->next;
    }

    free_environment(&local_env);
    return result; // if no explicit return, we return 0
}

// Initialize the environment
void init_environment(Environment *env) {
    // Existing variable initialization
    env->variable_count = 0;
    env->capacity = 10;
    env->variables = malloc(env->capacity * sizeof(Variable));

    // Add function initialization
    env->function_count = 0;
    env->function_capacity = 10;
    env->functions = malloc(env->function_capacity * sizeof(Function));
}

// Free the environment
void free_environment(Environment *env) {
    // Free variables
    for (size_t i = 0; i < env->variable_count; i++) {
        free(env->variables[i].variable_name);
    }
    free(env->variables);

    // Free functions
    for (size_t i = 0; i < env->function_count; i++) {
        free(env->functions[i].name);
    }
    free(env->functions);
}
