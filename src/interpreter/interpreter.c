#include "interpreter.h"

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

InterpretResult interpret_node(ASTNode *node, Environment *env) {
    if (!node) {
        return make_result(create_default_value(), false, false);
    }

    debug_print_int("`interpret_node()` called\n");

    InterpretResult result = {0};

    switch (node->type) {
    case AST_LITERAL:
        debug_print_int("\tMatched: `AST_LITERAL`\n");
        result = make_result(interpret_literal(node), false, false);
        break;

    case AST_ASSIGNMENT:
        debug_print_int("\tMatched: `AST_ASSIGNMENT`\n");
        result = interpret_assignment(node, env);
        break;

    case AST_UNARY_OP:
        debug_print_int("\tMatched: `AST_UNARY_OP`\n");
        result = interpret_unary_op(node, env);
        break;

    case AST_BINARY_OP:
        debug_print_int("\tMatched: `AST_BINARY_OP`\n");
        result = interpret_binary_op(node, env);
        break;

    case AST_CONDITIONAL: {
        debug_print_int("\tMatched: `AST_CONDITIONAL`\n");
        result = interpret_conditional(node, env);
        break;
    }

    case AST_FUNCTION_CALL: {
        debug_print_int("\tMatched: `AST_FUNCTION_CALL`\n");
        // interpret_function_call(...) returns an InterpretResult
        result = interpret_function_call(node, env);
        break;
    }

    case AST_FUNCTION_DECLARATION: {
        debug_print_int("\tMatched: `AST_FUNCTION_DECLARATION`\n");
        interpret_function_declaration(node, env);
        // No direct return from a function declaration
        result = make_result(create_default_value(), false, false);
        break;
    }

    case AST_FUNCTION_RETURN: {
        debug_print_int("\tMatched: `AST_FUNCTION_RETURN`\n");
        // Extract the return value
        InterpretResult return_res =
            interpret_node(node->assignment.value, env);
        if (return_res.is_error) {
            // Propagate the error
            return return_res;
        }
        // Set the did_return flag
        return_res.did_return = true;
        return return_res;
    }

    case AST_WHILE_LOOP: {
        debug_print_int("\tMatched: `AST_WHILE_LOOP`\n");
        result = interpret_while_loop(node, env);
        break;
    }

    case AST_FOR_LOOP: {
        debug_print_int("\tMatched: `AST_FOR_LOOP`\n");
        InterpretResult loop_res = interpret_for_loop(node, env);
        result = loop_res;
        break;
    }

    case AST_VARIABLE: {
        debug_print_int("\tMatched: `AST_VARIABLE`\n");
        // Replace LiteralValue with InterpretResult
        InterpretResult var_res = interpret_variable(node, env);
        result = var_res;
        break;
    }

    case AST_CONSTANT: {
        debug_print_int("\tMatched: `AST_CONSTANT`\n");
        result = interpret_constant(node, env);
        break;
    }

    case AST_SWITCH: {
        debug_print_int("\tMatched: `AST_SWITCH`\n");
        result = interpret_switch(node, env);
        break;
    }

    case AST_BREAK:
        debug_print_int("\tMatched: `AST_BREAK`\n");
        result = make_result(create_default_value(), false, true);
        break;

    case AST_TERNARY:
        debug_print_int("\tMatched: `AST_TERNARY`\n");
        result = interpret_ternary(node, env);
        break;

    case AST_TRY:
        debug_print_int("\tMatched: `AST_TRY`\n");
        result = interpret_try(node, env);
        break;

    default:
        return raise_error("Unsupported `ASTNode` type.\n");
    }

    return result;
}

void interpret_program(ASTNode *program, Environment *env) {
    ASTNode *current = program;
    while (current) {
        debug_print_int("Executing top-level statement\n");
        InterpretResult res = interpret_node(current, env);
        if (res.is_error) {
            fprintf(stderr, "Unhandled exception: %s\n", res.value.data.string);
            exit(1);
        }
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
        debug_print_int("Created float literal: `%Lf`\n",
                        value.data.floating_point);
        break;
    case LITERAL_INTEGER:
        value.type = TYPE_INTEGER;
        value.data.integer = node->literal.value.integer;
        debug_print_int("Created integer literal: `%lld`\n",
                        value.data.integer);
        break;
    case LITERAL_BOOLEAN:
        value.type = TYPE_BOOLEAN;
        value.data.boolean = node->literal.value.boolean;
        debug_print_int("Created boolean literal: `%s`\n",
                        value.data.boolean ? "True" : "False");
        break;
    default:
        // Let `interpret_node` handle unsupported literals
        value.type = TYPE_ERROR;
        value.data.string = strdup("Unsupported literal type.\n");
        break;
    }

    return value;
}

InterpretResult interpret_variable(ASTNode *node, Environment *env) {
    Variable *var = get_variable(env, node->variable_name);
    if (!var) {
        return raise_error("Undefined variable `%s`.\n", node->variable_name);
    }

    return make_result(var->value, false, false);
}

InterpretResult interpret_constant(ASTNode *node, Environment *env) {
    if (node->type != AST_CONSTANT) {
        return raise_error("Invalid node type for constant declaration.\n");
    }

    // Extract the constant name and value
    char *const_name = node->assignment.variable_name;
    InterpretResult value_res = interpret_node(node->assignment.value, env);
    LiteralValue const_value = value_res.value;

    if (value_res.is_error) {
        return value_res;
    }

    // Check if the constant already exists
    Variable *existing_var = get_variable(env, const_name);
    if (existing_var) {
        return raise_error("Constant `%s` is already defined.\n", const_name);
    }

    // Add the constant to the environment
    Variable new_var = {.variable_name = strdup(const_name),
                        .value = const_value,
                        .is_constant = true};
    add_variable(env, new_var);

    return make_result(const_value, false, false);
}

InterpretResult interpret_assignment(ASTNode *node, Environment *env) {
    if (!node->assignment.variable_name) {
        debug_print_int("Assignment variable name missing for node type: %d\n",
                        node->type);
        return make_result((LiteralValue){.type = TYPE_ERROR}, false, false);
    }

    // Evaluate the right-hand side
    InterpretResult assign_r = interpret_node(node->assignment.value, env);

    // If the RHS triggered a return or break, propagate it
    if (assign_r.did_return || assign_r.did_break) {
        return assign_r;
    }

    LiteralValue new_value = assign_r.value;

    // Add or update variable
    Variable new_var = {
        .variable_name = strdup(node->assignment.variable_name),
        .value = new_value,
        .is_constant = false // 'let' declarations are not constants
    };
    add_variable(env, new_var);

    return make_result(new_value, false, false);
}

LiteralValue handle_string_concatenation(LiteralValue left,
                                         LiteralValue right) {
    LiteralValue result;
    result.type = TYPE_STRING;

    // Convert numbers to strings
    char num_str1[50] = {0};
    char num_str2[50] = {0};

    if (left.type == TYPE_FLOAT) {
        snprintf(num_str1, sizeof(num_str1), FLOAT_FORMAT_SPECIFIER,
                 left.data.floating_point);
    }
    if (right.type == TYPE_FLOAT) {
        snprintf(num_str2, sizeof(num_str2), FLOAT_FORMAT_SPECIFIER,
                 right.data.floating_point);
    }

    // Allocate memory for the concatenated string
    size_t new_size =
        strlen(num_str1) + strlen(num_str2) +
        strlen(left.type == TYPE_STRING ? left.data.string : "") +
        strlen(right.type == TYPE_STRING ? right.data.string : "") + 1;

    char *new_string = malloc(new_size);
    if (!new_string) {
        error_interpreter(
            "Memory allocation failed for string concatenation.\n");
    }

    strcpy(new_string, left.type == TYPE_STRING ? left.data.string : num_str1);
    strcat(new_string,
           right.type == TYPE_STRING ? right.data.string : num_str2);
    result.data.string = new_string;

    return result;
}

LiteralValue evaluate_unary_operator(const char *op, LiteralValue operand) {
    // Debugging output
    debug_print_int("Unary Operator: `%s`\n", op);

    LiteralValue result;
    memset(&result, 0, sizeof(LiteralValue)); // initialize result

    // Handle logical NOT
    if (strcmp(op, "!") == 0) {
        // Ensure operand is boolean
        if (operand.type != TYPE_BOOLEAN) {
            error_interpreter(
                "Unary operator `%s` requires a boolean operand.\n", op);
        }

        result.type = TYPE_BOOLEAN;
        result.data.boolean = !operand.data.boolean;
        return result;
    }

    // Handle unary minus
    if (strcmp(op, "-") == 0) {
        if (operand.type == TYPE_INTEGER) {
            result.type = TYPE_INTEGER;
            result.data.integer = -operand.data.integer;
        } else if (operand.type == TYPE_FLOAT) {
            result.type = TYPE_FLOAT;
            result.data.floating_point = -operand.data.floating_point;
        } else {
            error_interpreter(
                "Unary operator `%s` requires a numeric operand.\n", op);
        }
        return result;
    }

    // Handle unary plus (no effect)
    if (strcmp(op, "+") == 0) {
        // Unary plus doesn't change the operand
        result = operand;
        return result;
    }

    // Add more unary operators as needed

    error_interpreter("Unknown unary operator `%s`\n", op);
    return result; // unreachable
}

InterpretResult interpret_unary_op(ASTNode *node, Environment *env) {
    if (!node || node->type != AST_UNARY_OP) {
        return raise_error("Invalid unary operation node.");
    }

    // Extract the operator and operand
    const char *op = node->unary_op.operator;
    ASTNode *operand_node = node->unary_op.operand;

    // Interpret the operand
    InterpretResult operand_r = interpret_node(operand_node, env);
    if (operand_r.is_error) {
        return operand_r;
    }

    LiteralValue operand = operand_r.value;

    // Evaluate the unary operator
    LiteralValue result = evaluate_unary_operator(op, operand);

    if (result.type == TYPE_ERROR) {
        InterpretResult res;
        res.value = result;
        res.did_return = false;
        res.did_break = false;
        res.is_error = true;
        return res;
    }

    return make_result(result, false, false);
}

LiteralValue evaluate_operator(const char *op, LiteralValue left,
                               LiteralValue right) {
    debug_print_int("Operator: `%s`\n", op);

    LiteralValue result;
    memset(&result, 0, sizeof(LiteralValue)); // Initialize result

    // Handle string concatenation with "+" operator
    if (strcmp(op, "+") == 0 &&
        (left.type == TYPE_STRING || right.type == TYPE_STRING)) {
        return handle_string_concatenation(left, right);
    }

    // Handle logical AND and OR
    if (strcmp(op, "&&") == 0 || strcmp(op, "||") == 0) {
        // Ensure both operands are boolean
        if (left.type != TYPE_BOOLEAN || right.type != TYPE_BOOLEAN) {
            LiteralValue error_val;
            error_val.type = TYPE_ERROR;
            error_val.data.string = strdup(
                "Logical operators `&&` and `||` require boolean operands.\n");
            return error_val;
        }

        result.type = TYPE_BOOLEAN;

        if (strcmp(op, "&&") == 0) {
            result.data.boolean = left.data.boolean && right.data.boolean;
        } else { // op == "||"
            result.data.boolean = left.data.boolean || right.data.boolean;
        }

        return result;
    }

    // Get numeric values for arithmetic and comparison
    FLOAT_SIZE left_value = 0.0, right_value = 0.0;
    if (left.type == TYPE_FLOAT || right.type == TYPE_FLOAT) {
        left_value = (left.type == TYPE_FLOAT) ? left.data.floating_point
                                               : (FLOAT_SIZE)left.data.integer;
        right_value = (right.type == TYPE_FLOAT)
                          ? right.data.floating_point
                          : (FLOAT_SIZE)right.data.integer;
    } else {
        left_value = (FLOAT_SIZE)left.data.integer;
        right_value = (FLOAT_SIZE)right.data.integer;
    }

    // Determine result type based on operands
    if (left.type == TYPE_FLOAT || right.type == TYPE_FLOAT) {
        result.type = TYPE_FLOAT;
    } else {
        result.type = TYPE_INTEGER;
    }

    // Handle operators
    if (strcmp(op, "*") == 0) {
        if (result.type == TYPE_FLOAT)
            result.data.floating_point = left_value * right_value;
        else
            result.data.integer = (INT_SIZE)(left_value * right_value);
    } else if (strcmp(op, "+") == 0) {
        if (result.type == TYPE_FLOAT)
            result.data.floating_point = left_value + right_value;
        else
            result.data.integer = (INT_SIZE)(left_value + right_value);
    } else if (strcmp(op, "-") == 0) {
        if (result.type == TYPE_FLOAT)
            result.data.floating_point = left_value - right_value;
        else
            result.data.integer = (INT_SIZE)(left_value - right_value);
    } else if (strcmp(op, "/") == 0) {
        if (right_value == 0) {
            LiteralValue error_val;
            error_val.type = TYPE_ERROR;
            error_val.data.string = strdup("Division by zero\n");
            return error_val;
        }
        result.type = TYPE_FLOAT;
        result.data.floating_point = left_value / right_value;
    } else if (strcmp(op, "//") == 0) {
        if (right_value == 0) {
            LiteralValue error_val;
            error_val.type = TYPE_ERROR;
            error_val.data.string = strdup("Floor division by zero\n");
            return error_val;
        }
        if (result.type == TYPE_FLOAT)
            result.data.floating_point = floor(left_value / right_value);
        else
            result.data.integer = (INT_SIZE)(left_value / right_value);
    } else if (strcmp(op, "%") == 0) {
        if (right_value == 0) {
            LiteralValue error_val;
            error_val.type = TYPE_ERROR;
            error_val.data.string = strdup("Modulo by zero\n");
            return error_val;
        }
        if (result.type == TYPE_FLOAT) {
            result.data.floating_point = fmod(left_value, right_value);
        } else {
            result.data.integer =
                (INT_SIZE)((INT_SIZE)left_value % (INT_SIZE)right_value);
        }
    } else if (strcmp(op, "**") == 0) {
        if (result.type == TYPE_FLOAT)
            result.data.floating_point = pow(left_value, right_value);
        else
            result.data.integer = (INT_SIZE)pow(left_value, right_value);
    } else if (strcmp(op, "<") == 0) {
        result.type = TYPE_BOOLEAN;
        result.data.boolean = (left_value < right_value);
    } else if (strcmp(op, ">") == 0) {
        result.type = TYPE_BOOLEAN;
        result.data.boolean = (left_value > right_value);
    } else if (strcmp(op, "<=") == 0) {
        result.type = TYPE_BOOLEAN;
        result.data.boolean = (left_value <= right_value);
    } else if (strcmp(op, ">=") == 0) {
        result.type = TYPE_BOOLEAN;
        result.data.boolean = (left_value >= right_value);
    } else if (strcmp(op, "==") == 0) {
        result.type = TYPE_BOOLEAN;
        result.data.boolean = (left_value == right_value);
    } else if (strcmp(op, "!=") == 0) {
        result.type = TYPE_BOOLEAN;
        result.data.boolean = (left_value != right_value);
    } else {
        LiteralValue error_val;
        error_val.type = TYPE_ERROR;
        error_val.data.string = strdup("Unknown operator");
        return error_val;
    }

    return result;
}

int get_operator_precedence(const char *op) {
    if (strcmp(op, "**") == 0)
        return 4; // highest precedence
    if (strcmp(op, "*") == 0 || strcmp(op, "/") == 0 || strcmp(op, "//") == 0 ||
        strcmp(op, "%") == 0)
        return 3;
    if (strcmp(op, "+") == 0 || strcmp(op, "-") == 0)
        return 2;
    if (strcmp(op, "<") == 0 || strcmp(op, ">") == 0 || strcmp(op, "<=") == 0 ||
        strcmp(op, ">=") == 0)
        return 1;
    if (strcmp(op, "==") == 0 || strcmp(op, "!=") == 0)
        return 1;

    // Return `0` for unknown operators
    return 0;
}

int is_right_associative(const char *op) {
    return (strcmp(op, "**") == 0); // exponentiation is right-associative
}

InterpretResult interpret_binary_op(ASTNode *node, Environment *env) {
    if (!node || node->type != AST_BINARY_OP) {
        return raise_error("Invalid binary operation node.");
    }

    // Interpret left operand
    InterpretResult left_r = interpret_node(node->binary_op.left, env);
    if (left_r.is_error) {
        return left_r;
    }
    LiteralValue left = left_r.value;

    // Interpret right operand
    InterpretResult right_r = interpret_node(node->binary_op.right, env);
    if (right_r.is_error) {
        return right_r;
    }
    LiteralValue right = right_r.value;

    // Evaluate based on operator
    const char *op = node->binary_op.operator;

    LiteralValue op_val = evaluate_operator(op, left, right);

    if (op_val.type == TYPE_ERROR) {
        InterpretResult res;
        res.value = op_val;
        res.did_return = false;
        res.did_break = false;
        res.is_error = true;
        return res;
    }

    // Return the operation result
    InterpretResult res;
    res.value = op_val;
    res.did_return = false;
    res.did_break = false;
    res.is_error = false;
    return res;
}

Variable *get_variable(Environment *env, const char *variable_name) {
    debug_print_int("Looking up variable: `%s`\n", variable_name);
    for (size_t i = 0; i < env->variable_count; i++) {
        if (strcmp(env->variables[i].variable_name, variable_name) == 0) {
            if (env->variables[i].value.type == TYPE_FLOAT) {
                debug_print_int("Variable found: `%s` with value `%Lf`\n",
                                variable_name,
                                env->variables[i].value.data.floating_point);
            } else if (env->variables[i].value.type == TYPE_INTEGER) {
                debug_print_int("Variable found: `%s` with value `%lld`\n",
                                variable_name,
                                env->variables[i].value.data.integer);
            } else if (env->variables[i].value.type == TYPE_STRING) {
                debug_print_int("Variable found: `%s` with value `%s`\n",
                                variable_name,
                                env->variables[i].value.data.string);
            } else if (env->variables[i].value.type == TYPE_FUNCTION) {
                debug_print_int(
                    "Variable found: `%s` with function reference `%s`\n",
                    variable_name,
                    env->variables[i].value.data.function_ptr->name);
            }
            return &env->variables[i];
        }
    }
    debug_print_int("Variable not found: `%s`\n", variable_name);
    return NULL;
}

InterpretResult add_variable(Environment *env, Variable var) {
    // Check if the variable already exists
    for (size_t i = 0; i < env->variable_count; i++) {
        if (strcmp(env->variables[i].variable_name, var.variable_name) == 0) {
            // If existing variable is a constant, prevent re-assignment
            if (env->variables[i].is_constant) {
                return raise_error("Error: Cannot reassign to constant `%s`.\n",
                                   var.variable_name);
            }

            // Update the value of the existing variable
            if (env->variables[i].value.type == TYPE_STRING &&
                env->variables[i].value.data.string) {
                free(env->variables[i].value.data.string);
            }

            env->variables[i].value = var.value;
            env->variables[i].is_constant = var.is_constant;
            return make_result(var.value, false, false);
        }
    }

    // Add a new variable
    if (env->variable_count == env->capacity) {
        // Resize the variables array if necessary
        size_t new_capacity = env->capacity ? env->capacity * 2 : 4;
        Variable *new_variables =
            realloc(env->variables, new_capacity * sizeof(Variable));
        if (!new_variables) {
            return raise_error(
                "Memory allocation failed while adding variable `%s`.\n",
                var.variable_name);
        }
        env->variables = new_variables;
        env->capacity = new_capacity;
    }

    // Copy variable name and value
    env->variables[env->variable_count].variable_name =
        strdup(var.variable_name);
    if (!env->variables[env->variable_count].variable_name) {
        return raise_error("Memory allocation failed for variable name `%s`.\n",
                           var.variable_name);
    }

    // Deep copy based on type
    if (var.value.type == TYPE_STRING) {
        env->variables[env->variable_count].value.type = TYPE_STRING;
        env->variables[env->variable_count].value.data.string =
            strdup(var.value.data.string);
        if (!env->variables[env->variable_count].value.data.string) {
            return raise_error(
                "Memory allocation failed for string variable `%s`.\n",
                var.variable_name);
        }
    } else if (var.value.type == TYPE_FUNCTION) {
        env->variables[env->variable_count].value.type = TYPE_FUNCTION;
        env->variables[env->variable_count].value.data.function_ptr =
            var.value.data.function_ptr;
    } else {
        env->variables[env->variable_count].value = var.value;
    }

    env->variables[env->variable_count].is_constant = var.is_constant;
    env->variable_count++;

    return make_result(var.value, false, false);
}

InterpretResult allocate_variable(Environment *env, const char *name) {
    // Check if the variable already exists
    for (size_t i = 0; i < env->variable_count; i++) {
        if (strcmp(env->variables[i].variable_name, name) == 0) {
            return make_result(env->variables[i].value, false, false);
        }
    }

    // If the variable doesn't exist, allocate it in memory
    if (env->variable_count == env->capacity) {
        size_t new_capacity = env->capacity ? env->capacity * 2 : 4;
        Variable *new_variables =
            realloc(env->variables, new_capacity * sizeof(Variable));
        if (!new_variables) {
            return raise_error("Memory allocation failed.\n");
        }
        env->variables = new_variables;
        env->capacity = new_capacity;
    }

    env->variables[env->variable_count].variable_name = strdup(name);
    if (!env->variables[env->variable_count].variable_name) {
        return raise_error("Memory allocation failed for variable name `%s`.\n",
                           name);
    }

    // Initialize the variable with default value
    env->variables[env->variable_count].value = create_default_value();
    env->variables[env->variable_count].is_constant = false;
    env->variable_count++;

    Variable *var = &env->variables[env->variable_count - 1];
    return make_result(var->value, false, false);
}

InterpretResult interpret_conditional(ASTNode *node, Environment *env) {
    debug_print_int("`interpret_conditional()` called\n");
    if (!node) {
        // Error
        return raise_error("Invalid conditional node.");
    }

    InterpretResult result = {0};
    bool condition_met = false;

    ASTNode *current_branch = node;

    while (current_branch) {
        if (current_branch->conditional.condition) {
            InterpretResult cond_res =
                interpret_node(current_branch->conditional.condition, env);

            if (cond_res.is_error) {
                // Propagate the error
                return cond_res;
            }

            // Check for valid condition types
            if (cond_res.value.type != TYPE_INTEGER &&
                cond_res.value.type != TYPE_BOOLEAN) {
                return raise_error(
                    "Condition expression must be boolean or integer.\n");
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

                    if (body_res.is_error || body_res.did_return ||
                        body_res.did_break) {
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

                    if (body_res.is_error || body_res.did_return ||
                        body_res.did_break) {
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
    return make_result(create_default_value(), false, false);
}

InterpretResult interpret_while_loop(ASTNode *node, Environment *env) {
    ASTNode *condition = node->while_loop.condition;
    ASTNode *body = node->while_loop.body;

    while (1) {
        // Check condition
        InterpretResult cond_r = interpret_node(condition, env);
        if (cond_r.did_return || cond_r.did_break) {
            // Propagate break or return upwards
            return cond_r;
        }

        // Evaluate condition as boolean/integer
        bool condition_true = false;
        if (cond_r.value.type == TYPE_BOOLEAN) {
            condition_true = cond_r.value.data.boolean;
        } else if (cond_r.value.type == TYPE_INTEGER) {
            condition_true = (cond_r.value.data.integer != 0);
        }

        // Exit if condition is false
        if (!condition_true) {
            break;
        }

        // Interpret the loop body
        ASTNode *current = body;
        while (current) {
            InterpretResult body_r = interpret_node(current, env);

            // If there's a return or break, propagate it up
            if (body_r.did_return || body_r.did_break) {
                return body_r;
            }

            current = current->next;
        }
    }

    return make_result(create_default_value(), false, false);
}

InterpretResult interpret_for_loop(ASTNode *node, Environment *env) {
    if (node->type != AST_FOR_LOOP) {
        return raise_error(
            "`interpret_for_loop` called with non-`for`-loop `ASTNode`\n");
    }

    // Extract loop components
    char *loop_var = node->for_loop.loop_variable;
    ASTNode *start_expr = node->for_loop.start_expr;
    ASTNode *end_expr = node->for_loop.end_expr;
    bool inclusive = node->for_loop.inclusive;
    ASTNode *step_expr = node->for_loop.step_expr; // may be `NULL`
    ASTNode *body = node->for_loop.body;

    // Evaluate start and end expressions
    InterpretResult start_res = interpret_node(start_expr, env);
    if (start_res.is_error) {
        return start_res;
    }

    InterpretResult end_res = interpret_node(end_expr, env);
    if (end_res.is_error) {
        return end_res;
    }

    // Determine start & end as floats for flexibility
    double start_val, end_val;
    if (start_res.value.type == TYPE_FLOAT) {
        start_val = start_res.value.data.floating_point;
    } else if (start_res.value.type == TYPE_INTEGER) {
        start_val = (FLOAT_SIZE)start_res.value.data.integer;
    } else {
        return raise_error("Start expression in `for` loop must be numeric\n");
    }

    if (end_res.value.type == TYPE_FLOAT) {
        end_val = end_res.value.data.floating_point;
    } else if (end_res.value.type == TYPE_INTEGER) {
        end_val = (FLOAT_SIZE)end_res.value.data.integer;
    } else {
        return raise_error("End expression in `for` loop must be numeric\n");
    }

    // Evaluate step expression if present
    double step = 1.0; // default
    if (step_expr) {
        InterpretResult step_res = interpret_node(step_expr, env);
        if (step_res.is_error) {
            return step_res;
        }

        if (step_res.value.type == TYPE_FLOAT) {
            step = step_res.value.data.floating_point;
        } else if (step_res.value.type == TYPE_INTEGER) {
            step = (FLOAT_SIZE)step_res.value.data.integer;
        } else {
            return raise_error(
                "Step expression in `for` loop must be numeric\n");
        }
    } else {
        // Determine implied step based on range direction
        if (start_val < end_val) {
            step = 1.0;
        } else {
            step = -1.0;
        }
    }

    // Validate step to prevent infinite loops
    if (step < 1e-9 && step > -1e-9) {
        return raise_error("Step value cannot be zero in `for` loop\n");
    }

    // Validate step to check if step is in correct direction
    if ((start_val < end_val && step < 0) ||
        (start_val > end_val && step > 0)) {
        return raise_error("Step value is in the wrong direction for the "
                           "specified range of the `for` loop\n");
    }

    // Assign or update loop variable in the environment
    Variable *var = get_variable(env, loop_var);
    if (!var) {
        // Variable does not exist; create it
        var = allocate_variable(env, loop_var);
        if (start_res.value.type == TYPE_FLOAT) {
            var->value.type = TYPE_FLOAT;
            var->value.data.floating_point = start_val;
        } else {
            var->value.type = TYPE_INTEGER;
            var->value.data.integer = (INT_SIZE)start_val;
        }
    } else {
        // Update existing variable
        if (start_res.value.type == TYPE_FLOAT ||
            var->value.type == TYPE_FLOAT) {
            var->value.type = TYPE_FLOAT;
            var->value.data.floating_point = start_val;
        } else {
            var->value.type = TYPE_INTEGER;
            var->value.data.integer = (INT_SIZE)start_val;
        }
    }

    // Determine loop direction
    bool is_ascending = step > 0.0;

    while (1) {
        // Fetch current value
        double current_val;
        if (var->value.type == TYPE_FLOAT) {
            current_val = var->value.data.floating_point;
        } else if (var->value.type == TYPE_INTEGER) {
            current_val = (FLOAT_SIZE)var->value.data.integer;
        } else {
            return raise_error("Loop variable `%s` must be numeric\n",
                               loop_var);
        }

        // Check if condition is still valid
        bool condition_true = false;
        if (is_ascending) {
            condition_true =
                inclusive ? (current_val <= end_val) : (current_val < end_val);
        } else {
            condition_true =
                inclusive ? (current_val >= end_val) : (current_val > end_val);
        }

        if (!condition_true) {
            break;
        }

        // Interpret the loop body
        ASTNode *current = body;
        while (current) {
            InterpretResult body_res = interpret_node(current, env);
            if (body_res.did_return || body_res.did_break) {
                return body_res;
            }
            current = current->next;
        }

        // Update loop variable
        if (var->value.type == TYPE_FLOAT) {
            var->value.data.floating_point += step;
        } else if (var->value.type == TYPE_INTEGER) {
            var->value.data.integer += (INT_SIZE)step;
        }
    }

    // Loops do not return values
    return make_result(create_default_value(), false, false);
}

InterpretResult interpret_switch(ASTNode *node, Environment *env) {
    debug_print_int("`interpret_switch()`\n");

    // Evaluate the switch expression
    InterpretResult switch_r =
        interpret_node(node->switch_case.expression, env);
    LiteralValue switch_val = switch_r.value;
    debug_print_int("Switch expression evaluated\n");

    ASTCaseNode *current_case = node->switch_case.cases;

    while (current_case) {
        if (current_case->condition == NULL) {
            // `else` case
            debug_print_int("Executing `else` case\n");
            ASTNode *current_statement = current_case->body;
            while (current_statement) {
                if (current_statement->type == AST_BREAK) {
                    // Return InterpretResult with did_break = true
                    debug_print_int("Break encountered in else case\n");
                    return make_result(create_default_value(), false, true);
                }
                // Interpret the statement
                InterpretResult stmt_res =
                    interpret_node(current_statement, env);
                if (stmt_res.did_return || stmt_res.did_break) {
                    // Propagate the flags upwards
                    return stmt_res;
                }
                current_statement = current_statement->next;
            }
            // After else, no more cases to execute
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
                while (current_statement) {
                    if (current_statement->type == AST_BREAK) {
                        // Return InterpretResult with did_break = true
                        debug_print_int("Break encountered in matched case\n");
                        return make_result(create_default_value(), false, true);
                    }
                    // Interpret the statement
                    InterpretResult stmt_res =
                        interpret_node(current_statement, env);
                    if (stmt_res.did_return || stmt_res.did_break) {
                        // Propagate the flags upwards
                        return stmt_res;
                    }
                    current_statement = current_statement->next;
                }
                // After matched case, no more cases to execute
                break;
            }
        }

        current_case = current_case->next;
    }

    debug_print_int("Switch statement interpretation complete\n");

    // Return a default value, no break or return encountered
    return make_result(create_default_value(), false, false);
}

InterpretResult call_user_defined_function(Function *func_ref,
                                           ASTNode *call_node,
                                           Environment *env) {
    debug_print_int("Calling user-defined function: `%s`\n", func_ref->name);

    // 1) Create a new local environment
    Environment local_env;
    init_environment(&local_env);

    // 2) Copy global functions into the local environment
    for (size_t i = 0; i < env->function_count; i++) {
        Function *global_func = &env->functions[i];

        // Deep copy the function
        Function func_copy = {
            .name = strdup(global_func->name),
            .parameters = copy_function_parameters(global_func->parameters),
            .body = copy_ast_node(global_func->body),
            .is_builtin = global_func->is_builtin};

        add_function(&local_env, func_copy);
    }

    // 3) Bind function parameters with arguments
    ASTFunctionParameter *param = func_ref->parameters;
    ASTNode *arg = call_node->function_call.arguments;

    while (param && arg) {
        InterpretResult arg_res = interpret_node(arg, env);
        if (arg_res.is_error) {
            free_environment(&local_env);
            return arg_res;
        }

        LiteralValue arg_value = arg_res.value;

        // Bind the argument to the parameter in the local environment
        Variable param_var = {.variable_name = strdup(param->parameter_name),
                              .value = arg_value,
                              .is_constant = false};
        add_variable(&local_env, param_var);

        param = param->next;
        arg = arg->next;
    }

    // 4) Check for argument count mismatch
    if (param || arg) {
        free_environment(&local_env);
        return raise_error(
            "Error: Argument count mismatch when calling function `%s`\n",
            func_ref->name);
    }

    // 5) Interpret the function body
    ASTNode *stmt = func_ref->body;
    InterpretResult func_res =
        make_result(create_default_value(), false, false);

    while (stmt) {
        InterpretResult r = interpret_node(stmt, &local_env);
        if (r.did_return) {
            func_res = r;
            break;
        }
        if (r.did_break) {
            free_environment(&local_env);
            return raise_error(
                "Error: 'break' statement outside of loop or switch.\n");
        }
        if (r.is_error) {
            free_environment(&local_env);
            return r;
        }
        stmt = stmt->next;
    }

    free_environment(&local_env);

    // If no explicit return, return default value (e.g., 0)
    return func_res;
}

void interpret_function_declaration(ASTNode *node, Environment *env) {
    debug_print_int("`interpret_function_declaration()` called\n");

    if (!node || !node->function_call.name) {
        error_interpreter("Invalid function declaration\n");
    }

    // Initialize param_list as NULL
    ASTFunctionParameter *param_list = NULL;
    ASTFunctionParameter *param_tail = NULL; // keep track of the last parameter

    // Copy parameters
    ASTFunctionParameter *param = node->function_call.parameters;
    while (param) {
        ASTFunctionParameter *new_param = malloc(sizeof(ASTFunctionParameter));
        if (!new_param) {
            error_interpreter(
                "Error: Memory allocation failed for function parameter\n");
        }

        new_param->parameter_name = strdup(param->parameter_name);
        if (!new_param->parameter_name) {
            free(new_param);
            error_interpreter("Memory allocation failed for parameter name\n");
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
                     .body = node->function_call.body,
                     .is_builtin = false};

    add_function(env, func);

    // Also add the function as a variable holding its reference
    LiteralValue func_ref = {.type = TYPE_FUNCTION,
                             // Point to the last added function
                             .data.function_ptr =
                                 &env->functions[env->function_count - 1]};

    Variable var = {.variable_name = strdup(func.name),
                    .value = func_ref,
                    .is_constant = false};

    add_variable(env, var);
}

InterpretResult interpret_function_call(ASTNode *node, Environment *env) {
    debug_print_int("Starting function call interpretation\n");

    if (!node || !node->function_call.name) {
        return raise_error("Invalid function call");
    }

    const char *func_name = node->function_call.name;

    // 1. Check if the function name is a variable holding a function reference
    Variable *func_var = get_variable(env, func_name);
    if (func_var && func_var->value.type == TYPE_FUNCTION) {
        Function *func_ref = func_var->value.data.function_ptr;
        // Call the user-defined function
        InterpretResult func_res =
            call_user_defined_function(func_ref, node, env);
        return func_res;
    }

    // 2. Else, check if it's a built-in or globally defined function
    Function *func = get_function(env, func_name);
    if (!func) {
        // Function not found
        return raise_error("Undefined function `%s`\n", func_name);
    }

    // If it’s a built-in => call the built-in logic
    if (func->is_builtin) {
        InterpretResult builtin_res = {0};
        if (strcmp(func->name, "sample") == 0) {
            builtin_res = builtin_input(node, env);
        } else if (strcmp(func->name, "serve") == 0) {
            builtin_res = builtin_output(node, env);
        } else if (strcmp(func->name, "burn") == 0) {
            builtin_res = builtin_error(node, env);
        } else if (strcmp(func->name, "random") == 0) {
            builtin_res = builtin_random(node, env);
        } else if (strcmp(func->name, "string") == 0 ||
                   strcmp(func->name, "int") == 0 ||
                   strcmp(func->name, "float") == 0) {
            builtin_res = builtin_cast(node, env);
        } else if (strcmp(func->name, "get_time") == 0) {
            builtin_res = builtin_time();
        } else if (strcmp(func->name, "taste_file") == 0) {
            builtin_res = builtin_file_read(node, env);
        } else if (strcmp(func->name, "plate_file") == 0) {
            builtin_res = builtin_file_write(node, env);
        } else if (strcmp(func->name, "garnish_file") == 0) {
            builtin_res = builtin_file_append(node, env);
        } else {
            // Unknown built-in
            return raise_error("Unknown built-in function `%s`\n", func->name);
        }

        return builtin_res;
    }

    // Otherwise, user-defined function.
    // Create local environment, bind parameters, interpret AST body, etc.

    Environment local_env;
    init_environment(&local_env);

    // Copy parent's functions so calls to other user-defined functions work
    for (size_t i = 0; i < env->function_count; i++) {
        // NOTE: If the function is built-in or user-defined,
        // copy them all anyway.
        Function func_copy = {.name = strdup(env->functions[i].name),
                              .parameters = copy_function_parameters(
                                  env->functions[i].parameters),
                              .body = copy_ast_node(env->functions[i].body),
                              .is_builtin = env->functions[i].is_builtin};
        add_function(&local_env, func_copy);
    }

    // Now interpret arguments & bind them
    ASTFunctionParameter *p = func->parameters;
    ASTNode *arg = node->function_call.arguments;
    while (p && arg) {
        InterpretResult arg_res = interpret_node(arg, env);
        LiteralValue arg_value = arg_res.value;
        if (arg_value.type == TYPE_ERROR) {
            free_environment(&local_env);
            return arg_value;
        }

        Variable param_var = {.variable_name = strdup(p->parameter_name),
                              .value = arg_value};
        add_variable(&local_env, param_var);

        p = p->next;
        arg = arg->next;
    }

    // interpret function body
    LiteralValue result = create_default_value();
    ASTNode *stmt = func->body;
    while (stmt) {
        InterpretResult r = interpret_node(stmt, &local_env);
        if (r.did_return) {
            // Short-circuit
            free_environment(&local_env);
            return r.value;
        }
        // Else keep going
        stmt = stmt->next;
    }

    free_environment(&local_env);

    // If no explicit return, return default value (i.e., `0`)
    return make_result(create_default_value(), false, false);
}

InterpretResult interpret_ternary(ASTNode *node, Environment *env) {
    if (!node || node->type != AST_TERNARY) {
        return raise_error("Invalid ternary operation node.\n");
    }

    InterpretResult cond_res = interpret_node(node->ternary.condition, env);
    if (cond_res.is_error || cond_res.did_return || cond_res.did_break) {
        return cond_res;
    }

    bool is_true = false;
    if (cond_res.value.type == TYPE_BOOLEAN) {
        is_true = cond_res.value.data.boolean;
    } else if (cond_res.value.type == TYPE_INTEGER) {
        is_true = (cond_res.value.data.integer != 0);
    } else {
        return raise_error("Ternary condition must be boolean or integer.\n");
    }

    if (is_true) {
        InterpretResult true_res = interpret_node(node->ternary.true_expr, env);
        return true_res;
    } else {
        InterpretResult false_res =
            interpret_node(node->ternary.false_expr, env);
        return false_res;
    }
}

InterpretResult interpret_try(ASTNode *node, Environment *env) {
    if (!node || node->type != AST_TRY) {
        return raise_error("Invalid AST node for try block.");
    }

    InterpretResult result = make_result(create_default_value(), false, false);
    bool exception_occurred = false;
    LiteralValue exception_value = create_default_value();

    // Execute try block
    ASTNode *stmt = node->try_block.try_block;
    while (stmt) {
        InterpretResult res = interpret_node(stmt, env);
        if (res.is_error) {
            // An exception has been thrown
            exception_occurred = true;
            exception_value = res.value;
            break;
        }
        if (res.did_return) {
            // Propagate return up
            return res;
        }
        stmt = stmt->next;
    }

    // If exception occurred, handle rescue blocks
    if (exception_occurred) {
        ASTCatchNode *catch = node->try_block.catch_blocks;
        bool handled = false;

        while (catch && !handled) {
            Environment catch_env;
            init_environment(&catch_env);

            // Copy global functions to rescue environment
            for (size_t i = 0; i < env->function_count; i++) {
                Function *global_func = &env->functions[i];
                Function func_copy = {.name = strdup(global_func->name),
                                      .parameters = copy_function_parameters(
                                          global_func->parameters),
                                      .body = copy_ast_node(global_func->body),
                                      .is_builtin = global_func->is_builtin};
                add_function(&catch_env, func_copy);
            }

            // If there's an error variable, bind the exception to it
            if (catch->error_variable) {
                Variable error_var = {.variable_name =
                                          strdup(catch->error_variable),
                                      .value = exception_value,
                                      .is_constant = false};
                add_variable(&catch_env, error_var);
            }

            // Execute catch block
            ASTNode *catch_stmt = catch->body;
            while (catch_stmt) {
                InterpretResult res = interpret_node(catch_stmt, &catch_env);
                if (res.is_error) {
                    // Nested exception, propagate
                    free_environment(&catch_env);
                    return res;
                }
                if (res.did_return) {
                    // Propagate return up
                    free_environment(&catch_env);
                    return res;
                }
                catch_stmt = catch_stmt->next;
            }

            handled = true; // Currently handling only one catch block
            free_environment(&catch_env);
            catch = catch->next;
        }

        if (!handled) {
            // No rescue block handled the exception, propagate it
            result.value = exception_value;
            result.is_error = true;
            return result;
        }
    }

    // Execute finish block if it exists
    if (node->try_block.finally_block) {
        ASTNode *finish_stmt = node->try_block.finally_block;
        while (finish_stmt) {
            InterpretResult res = interpret_node(finish_stmt, env);
            if (res.is_error) {
                // If an exception occurs in finish, prioritize it
                return res;
            }
            if (res.did_return) {
                // Propagate return up
                return res;
            }
            finish_stmt = finish_stmt->next;
        }
    }

    // Normal execution
    return result;
}
