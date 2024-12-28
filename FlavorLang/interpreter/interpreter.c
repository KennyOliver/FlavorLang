#include "interpreter.h"
#include <stdio.h>
#include <string.h>
#include "../debug/debug.h"

LiteralValue interpret(ASTNode *node, Environment *env);
LiteralValue interpret_literal(ASTNode *node);
LiteralValue interpret_variable(ASTNode *node, Environment *env);
LiteralValue interpret_assignment(ASTNode *node, Environment *env);
LiteralValue interpret_binary_op(ASTNode *node, Environment *env);
Variable *get_variable(Environment *env, const char *variable_name);
void add_variable(Environment *env, Variable var);
void interpret_print(ASTNode *node, Environment *env);
void interpret_raise_error(ASTNode *node, Environment *env);
Variable interpret_input(Environment *env);
void interpret_conditional(ASTNode *node, Environment *env);
void interpret_while_loop(ASTNode *node, Environment *env);
void interpret_switch(ASTNode *node, Environment *env);
void interpret_function_declaration(ASTNode *node, Environment *env);
LiteralValue interpret_function_call(ASTNode *node, Environment *env);
ASTNode *copy_ast_node(ASTNode *node);

// Helper function to create a default LiteralValue (zero number)
LiteralValue create_default_value()
{
    LiteralValue value = {
        .type = TYPE_INTEGER,
        .data = {
            // .floating_point = 0.0,
            .integer = 0, // main value used for comparisons, etc
            // .string = "",
        }};
    return value;
}

LiteralValue interpret(ASTNode *node, Environment *env)
{
    if (!node)
    {
        fprintf(stderr, "Error: Attempt to interpret NULL node\n");
        return (LiteralValue){.type = TYPE_ERROR};
    }

    debug_print_int("`interpret()` called\n");

    switch (node->type)
    {
    case AST_LITERAL:
        debug_print_int("\tMatched: `AST_LITERAL`\n");
        return interpret_literal(node);

    case AST_ASSIGNMENT:
        debug_print_int("\tMatched: `AST_ASSIGNMENT`\n");
        return interpret_assignment(node, env);

    case AST_BINARY_OP:
        debug_print_int("\tMatched: `AST_BINARY_OP`\n");
        return interpret_binary_op(node, env);

    case AST_PRINT:
        debug_print_int("\tMatched: `AST_PRINT`\n");
        interpret_print(node, env);
        return create_default_value();

    case AST_INPUT:
    {
        debug_print_int("\tMatched: `AST_INPUT`\n");
        Variable v = interpret_input(env);
        return v.value;
    }

    case AST_CONDITIONAL:
        debug_print_int("\tMatched: `AST_CONDITIONAL`\n");
        interpret_conditional(node, env);
        return create_default_value();

    case AST_FUNCTION_CALL:
        debug_print_int("\tMatched: `AST_FUNCTION_CALL`\n");
        return interpret_function_call(node, env);

    case AST_FUNCTION_DECLARATION:
        debug_print_int("\tMatched: `AST_FUNCTION_DECLARATION`\n");
        interpret_function_declaration(node, env);
        return create_default_value();

    case AST_FUNCTION_RETURN:
    {
        debug_print_int("\tMatched: `AST_FUNCTION_RETURN`\n");

        // If there's no expression, treat it as an error
        if (!node->assignment.value)
        {
            // Error out, so we can notice "phantom returns" in the AST
            fprintf(stderr, "Error: Return statement has no expression! (Parser bug?)\n");
            exit(1);
        }

        // Otherwise interpret the actual return expression
        LiteralValue return_value = interpret(node->assignment.value, env);
        debug_print_int("Return value before returning: type=%d, value=%d\n",
                        return_value.type,
                        return_value.type == TYPE_INTEGER ? return_value.data.integer : 0);
        return return_value;
    }

    case AST_LOOP:
        debug_print_int("\tMatched: `AST_LOOP`\n");
        interpret_while_loop(node, env);
        return create_default_value();

    case AST_VARIABLE:
        debug_print_int("\tMatched: `AST_VARIABLE`\n");
        return interpret_variable(node, env);

    case AST_SWITCH:
        debug_print_int("\tMatched: `AST_SWITCH`\n");
        interpret_switch(node, env);
        return create_default_value();

    case AST_ERROR:
        debug_print_int("\tMatched: `AST_ERROR`\n");
        interpret_raise_error(node, env);
        return create_default_value();

    default:
        fprintf(stderr, "Error: Unsupported ASTNode type.\n");
        exit(1);
    }
}

void interpret_program(ASTNode *program, Environment *env)
{
    ASTNode *current = program;
    while (current)
    {
        debug_print_int("Executing top-level statement\n");
        interpret(current, env);
        current = current->next;
    }
}

LiteralValue interpret_literal(ASTNode *node)
{
    LiteralValue value;
    debug_print_int("Interpreting literal value...\n");
    debug_print_int("Literal type: %d\n", node->literal.type);

    if (node->literal.type == LITERAL_INTEGER)
    {
        debug_print_int("Integer value: %d\n", node->literal.value.integer);
    }

    switch (node->literal.type)
    {
    case LITERAL_STRING:
        value.type = TYPE_STRING;
        value.data.string = strdup(node->literal.value.string);
        debug_print_int("Created string literal: `%s`\n", value.data.string);
        break;
    case LITERAL_FLOAT:
        value.type = TYPE_FLOAT;
        value.data.floating_point = node->literal.value.floating_point;
        break;
    case LITERAL_INTEGER:
        value.type = TYPE_INTEGER;
        value.data.integer = node->literal.value.integer;
        break;
    default:
        fprintf(stderr, "Error: Unsupported literal type.\n");
        exit(1);
    }
    return value;
}

LiteralValue interpret_variable(ASTNode *node, Environment *env)
{
    // printf("Env var 0: `%s`\n", env->variables[0].variable_name);
    Variable *var = get_variable(env, node->variable_name);
    if (!var)
    {
        fprintf(stderr, "Error: Undefined variable `%s`.\n", node->variable_name);
        exit(1);
    }

    return var->value;
}

LiteralValue interpret_assignment(ASTNode *node, Environment *env)
{
    if (!node->assignment.variable_name)
    {
        debug_print_int("Assignment variable name missing for node type: %d\n", node->type);
        return (LiteralValue){.type = TYPE_ERROR};
    }

    LiteralValue new_value;
    if (node->assignment.value->type == AST_FUNCTION_CALL)
    {
        new_value = interpret_function_call(node->assignment.value, env);
    }
    else
    {
        new_value = interpret(node->assignment.value, env);
    }

    Variable new_var = {
        .variable_name = strdup(node->assignment.variable_name),
        .value = new_value};
    add_variable(env, new_var);
    return new_value;
}

// Function to handle string concatenation
LiteralValue handle_string_concatenation(LiteralValue left, LiteralValue right)
{
    LiteralValue result;
    result.type = TYPE_STRING;

    // Convert numbers to strings
    char num_str1[50] = {0};
    char num_str2[50] = {0};

    if (left.type == TYPE_FLOAT)
    {
        snprintf(num_str1, sizeof(num_str1), "%f", left.data.floating_point);
    }
    if (right.type == TYPE_FLOAT)
    {
        snprintf(num_str2, sizeof(num_str2), "%f", right.data.floating_point);
    }

    // Allocate memory for the concatenated string
    size_t new_size = strlen(num_str1) + strlen(num_str2) +
                      strlen(left.type == TYPE_STRING ? left.data.string : "") +
                      strlen(right.type == TYPE_STRING ? right.data.string : "") + 1;

    char *new_string = malloc(new_size);
    if (!new_string)
    {
        fprintf(stderr, "Error: Memory allocation failed for string concatenation.\n");
        exit(1);
    }

    strcpy(new_string, left.type == TYPE_STRING ? left.data.string : num_str1);
    strcat(new_string, right.type == TYPE_STRING ? right.data.string : num_str2);
    result.data.string = new_string;

    return result;
}

LiteralValue interpret_binary_op(ASTNode *node, Environment *env)
{
    debug_print_int("Interpreting binary operation\n");

    // First evaluate both operands
    LiteralValue left = interpret(node->binary_op.left, env);
    if (left.type == TYPE_ERROR)
    {
        return left;
    }

    LiteralValue right = interpret(node->binary_op.right, env);
    if (right.type == TYPE_ERROR)
    {
        return right;
    }

    // In interpret_binary_op, replace the switch and operator handling with:
    char *operator= node->binary_op.operator;
    debug_print_int("Operator: `%s`\n", operator);

    // Handle string concatenation with "+" operator
    if (operator[0] == '+' &&(left.type == TYPE_STRING || right.type == TYPE_STRING))
    {
        return handle_string_concatenation(left, right);
    }

    // Get numeric values
    double left_value = (left.type == TYPE_FLOAT) ? left.data.floating_point : (double)left.data.integer;
    double right_value = (right.type == TYPE_FLOAT) ? right.data.floating_point : (double)right.data.integer;

    LiteralValue result;
    result.type = (left.type == TYPE_FLOAT || right.type == TYPE_FLOAT) ? TYPE_FLOAT : TYPE_INTEGER;

    // First check for two-character operators
    if (operator[1] != '\0')
    {
        if (strcmp(operator, "<=") == 0)
        {
            result.type = TYPE_INTEGER;
            result.data.integer = left_value <= right_value;
            return result;
        }
        if (strcmp(operator, ">=") == 0)
        {
            result.type = TYPE_INTEGER;
            result.data.integer = left_value >= right_value;
            return result;
        }
        if (strcmp(operator, "==") == 0)
        {
            result.type = TYPE_INTEGER;
            result.data.integer = left_value == right_value;
            return result;
        }
    }

    // Then handle single-character operators
    switch (operator[0])
    {
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
        if (right_value == 0)
        {
            fprintf(stderr, "Error: Division by zero\n");
            exit(1);
        }
        if (result.type == TYPE_FLOAT)
            result.data.floating_point = left_value / right_value;
        else
            result.data.integer = (int)(left_value / right_value);
        break;
    case '<':
        result.type = TYPE_INTEGER;
        result.data.integer = left_value < right_value;
        break;
    case '>':
        result.type = TYPE_INTEGER;
        result.data.integer = left_value > right_value;
        break;
    default:
        fprintf(stderr, "Error: Unknown operator '%s'\n", operator);
        exit(1);
    }

    return result;
}

Variable *get_variable(Environment *env, const char *variable_name)
{
    debug_print_int("Looking up variable: `%s`\n", variable_name);
    for (size_t i = 0; i < env->variable_count; i++)
    {
        if (strcmp(env->variables[i].variable_name, variable_name) == 0)
        {
            if (env->variables[i].value.type == TYPE_FLOAT)
            {
                debug_print_int("Variable found: `%s` with value `%f`\n",
                                variable_name,
                                env->variables[i].value.data.floating_point);
            }
            else if (env->variables[i].value.type == TYPE_INTEGER)
            {
                debug_print_int("Variable found: `%s` with value `%d`\n",
                                variable_name,
                                env->variables[i].value.data.integer);
            }
            else if (env->variables[i].value.type == TYPE_STRING)
            {
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

void add_variable(Environment *env, Variable var)
{
    // Check if the variable already exists
    for (size_t i = 0; i < env->variable_count; i++)
    {
        if (strcmp(env->variables[i].variable_name, var.variable_name) == 0)
        {
            // Update the value of the existing variable
            if (env->variables[i].value.type == TYPE_STRING && env->variables[i].value.data.string)
            {
                free(env->variables[i].value.data.string); // Free existing string memory
            }
            env->variables[i].value = var.value;

            debug_print_int("Updated variable: `%s`\n", var.variable_name);
            return;
        }
    }

    // Add a new variable
    if (env->variable_count == env->capacity)
    {
        // Resize the variables array if necessary
        env->capacity = env->capacity ? env->capacity * 2 : 4;
        env->variables = realloc(env->variables, env->capacity * sizeof(Variable));
        if (!env->variables)
        {
            fprintf(stderr, "Error: Memory allocation failed while adding variable `%s`.\n", var.variable_name);
            exit(1);
        }
    }

    // Copy variable name and value
    env->variables[env->variable_count].variable_name = strdup(var.variable_name);
    if (var.value.type == TYPE_STRING)
    {
        env->variables[env->variable_count].value.type = TYPE_STRING;
        env->variables[env->variable_count].value.data.string = strdup(var.value.data.string);
    }
    else
    {
        env->variables[env->variable_count].value = var.value;
    }

    debug_print_int("Added variable: `%s`\n", var.variable_name);
    env->variable_count++;
}

void interpret_print(ASTNode *node, Environment *env)
{
    debug_print_int("`interpret_print()`\n");

    for (size_t i = 0; i < node->to_print.arg_count; i++)
    {
        ASTNode *arg = node->to_print.arguments[i];
        LiteralValue value = interpret(arg, env);

        switch (value.type)
        {
        case TYPE_FLOAT:
            if ((int)value.data.floating_point == value.data.floating_point)
            {
                // Value is effectively an integer, force one decimal place
                printf("%.1f", value.data.floating_point);
            }
            else
            {
                // Otherwise, use `%g` for trimming unnecessary zeroes
                printf("%g", value.data.floating_point);
            }
            break;
        case TYPE_INTEGER:
            printf("%d", value.data.integer);
            break;
        case TYPE_STRING:
            printf("%s", value.data.string);
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

void interpret_raise_error(ASTNode *node, Environment *env)
{
    int divider_length = 50;
    char *error_banner = "  !!! ERROR RAISED !!!  ";
    int error_banner_length = (divider_length - strlen(error_banner)) / 2 - 1;

    printf("\x1B[31m");
    printf("\x1B[01m");

    printf("<");
    for (int i = 1; i < divider_length; i++)
    {
        printf("=");
    }
    printf(">\n\n ");
    for (int i = 0; i < error_banner_length; i++)
    {
        printf("~");
    }
    printf("%s", error_banner);
    for (int i = 0; i < error_banner_length; i++)
    {
        printf("~");
    }
    printf(" \n\n");

    interpret_print(node, env);

    printf("\n<");
    for (int i = 1; i < divider_length; i++)
    {
        printf("=");
    }
    printf(">\n");

    printf("\x1B[0m");

    exit(1);
}

Variable *allocate_variable(Environment *env, const char *name)
{
    // Check if the variable already exists
    for (size_t i = 0; i < env->variable_count; i++)
    {
        if (strcmp(env->variables[i].variable_name, name) == 0)
        {
            return &env->variables[i]; // return existing variable
        }
    }

    // If the variable doesn't exist, allocate it in memory
    if (env->variable_count == env->capacity)
    {
        env->capacity *= 2;
        env->variables = realloc(env->variables, env->capacity * sizeof(Variable));
        if (!env->variables)
        {
            fprintf(stderr, "Error: Memory allocation failed.\n");
            exit(1);
        }
    }

    env->variables[env->variable_count].variable_name = strdup(name);
    env->variable_count++;

    return &env->variables[env->variable_count - 1];
}

Variable interpret_input(Environment *env)
{
    size_t buffer_size = 128;
    size_t input_length = 0;
    char *input_buffer = malloc(buffer_size);
    if (!input_buffer)
    {
        fprintf(stderr, "Error: Failed to allocate memory for input buffer.\n");
        // Return an empty or error `Variable`
        Variable error_var = {.value.type = TYPE_ERROR};
        return error_var;
    }

    // Read input
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
    {
        if (input_length + 1 >= buffer_size)
        {
            buffer_size *= 2;
            char *new_buffer = realloc(input_buffer, buffer_size);
            if (!new_buffer)
            {
                fprintf(stderr, "Error: Failed to reallocate memory for input buffer.\n");
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
    if (!var)
    {
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

void interpret_conditional(ASTNode *node, Environment *env)
{
    debug_print_int("`interpret_conditional()` called\n");

    if (!node)
    {
        fprintf(stderr, "Error: Null node passed to `interpret_conditional()`.\n");
        return;
    }

    ASTNode *current_branch = node;
    int condition_met = 0;

    while (current_branch)
    {
        if (current_branch->conditional.condition)
        {
            LiteralValue condition_value = interpret(current_branch->conditional.condition, env);
            debug_print_int("Condition evaluated to: `%d` (%s)\n", condition_value.data.integer, condition_value.data.integer ? "true" : "false");

            if (condition_value.type != TYPE_INTEGER)
            {
                fprintf(stderr, "Error: Condition must evaluate to an integer.\n");
                return;
            }

            // Change condition check for factorial base case
            if (condition_value.data.integer) // changed from != 0 to just the value
            {
                debug_print_int("Condition is true, executing branch body\n");
                ASTNode *current_statement = current_branch->conditional.body;
                while (current_statement)
                {
                    interpret(current_statement, env);
                    current_statement = current_statement->next;
                }
                condition_met = 1;
                break;
            }
        }
        else
        {
            debug_print_int("Executing ELSE branch\n");
            if (!condition_met)
            {
                ASTNode *current_statement = current_branch->conditional.body;
                while (current_statement)
                {
                    interpret(current_statement, env);
                    current_statement = current_statement->next;
                }
            }
            break;
        }
        current_branch = current_branch->conditional.else_branch;
    }

    debug_print_int("`interpret_conditional()` completed\n");
}
void interpret_while_loop(ASTNode *node, Environment *env)
{
    debug_print_int("`interpret_while_loop()` called\n");

    ASTNode *condition = node->loop.condition;
    ASTNode *body = node->loop.body;

    while (1)
    {
        debug_print_int("Starting new iteration of `while` loop\n");

        LiteralValue condition_value = interpret(condition, env);

        if (condition_value.type == TYPE_INTEGER)
        {
            debug_print_int("Condition evaluated to: `%f`\n", condition_value.data.integer);
        }

        if (condition_value.data.integer != 0)
        {
            debug_print_int("`while` condition is true, executing body\n");

            ASTNode *current = body;
            while (current != NULL)
            {
                debug_print_int("Executing body statement of type: `%d`\n", current->type);
                interpret(current, env);
                current = current->next;
            }

            debug_print_int("After body execution:\n");
            for (size_t i = 0; i < env->variable_count; i++)
            {
                debug_print_int("Variable `%s`: value = `%s`",
                                env->variables[i].variable_name,
                                env->variables[i].value.data.integer ? "true" : "false");
            }
        }
        else
        {
            debug_print_int("`while` condition is false, exiting loop\n");
            break;
        }
    }

    debug_print_int("`interpret_while_loop()` completed\n");
}

void interpret_switch(ASTNode *node, Environment *env)
{
    debug_print_int("`interpret_switch()`\n");

    // First evaluate the switch expression
    LiteralValue switch_value = interpret(node->switch_case.expression, env);
    debug_print_int("Switch expression evaluated\n");

    // Go through each case
    ASTCaseNode *current_case = node->switch_case.cases;
    int break_encountered = 0;

    while (current_case && !break_encountered)
    {
        if (current_case->condition == NULL)
        {
            // This is the `else` case
            debug_print_int("Executing `else` case\n");
            if (!break_encountered)
            {
                // Execute the `else` block
                ASTNode *current_statement = current_case->body;
                while (current_statement && !break_encountered)
                {
                    if (current_statement->type == AST_BREAK)
                    {
                        break_encountered = 1;
                        break;
                    }
                    interpret(current_statement, env);
                    current_statement = current_statement->next;
                }
            }
            break;
        }

        // Evaluate the case condition
        LiteralValue case_value = interpret(current_case->condition, env);

        // Check if types match and values are equal
        if (switch_value.type == case_value.type)
        {
            int values_match = 0;

            if (switch_value.type == TYPE_FLOAT)
            {
                values_match = (switch_value.data.floating_point == case_value.data.floating_point);
            }
            else if (switch_value.type == TYPE_INTEGER)
            {
                values_match = (switch_value.data.integer == case_value.data.integer);
            }
            else if (switch_value.type == TYPE_STRING)
            {
                values_match = (strcmp(switch_value.data.string, case_value.data.string) == 0);
            }

            if (values_match)
            {
                debug_print_int("Match found, executing case body\n");

                // Execute all statements in the matching case
                ASTNode *current_statement = current_case->body;
                while (current_statement && !break_encountered)
                {
                    if (current_statement->type == AST_BREAK)
                    {
                        break_encountered = 1;
                        break;
                    }
                    interpret(current_statement, env);
                    current_statement = current_statement->next;
                }

                if (break_encountered)
                {
                    break;
                }
                // Otherwise fall-through to next case (allowing case stacking)
            }
        }

        current_case = current_case->next;
    }

    debug_print_int("Switch statement interpretation complete\n");
}

// Helper function to free a linked list of ASTFunctionParameter nodes
void free_parameter_list(ASTFunctionParameter *head)
{
    ASTFunctionParameter *current = head;
    while (current != NULL)
    {
        ASTFunctionParameter *next = current->next;
        free(current->parameter_name);
        free(current);
        current = next;
    }
}

ASTFunctionParameter *copy_function_parameters(ASTFunctionParameter *param_list)
{
    if (param_list == NULL)
    {
        // No parameters to copy
        return NULL;
    }

    ASTFunctionParameter *new_head = NULL;
    ASTFunctionParameter *new_tail = NULL;

    while (param_list != NULL)
    {
        // Allocate memory for the new parameter
        ASTFunctionParameter *new_param = malloc(sizeof(ASTFunctionParameter));
        if (new_param == NULL)
        {
            fprintf(stderr, "Memory allocation failed for new parameter\n");
            free_parameter_list(new_head); // Clean up all previously allocated nodes
            exit(1);
        }

        // Duplicate parameter name, ensuring it is not NULL
        if (param_list->parameter_name)
        {
            new_param->parameter_name = strdup(param_list->parameter_name);
            if (new_param->parameter_name == NULL)
            {
                fprintf(stderr, "Memory allocation failed for parameter name\n");
                free(new_param);               // Free the current node
                free_parameter_list(new_head); // Clean up previously allocated nodes
                exit(1);
            }
        }
        else
        {
            new_param->parameter_name = NULL;
        }

        // Initialize the `next` pointer to NULL
        new_param->next = NULL;

        // Add the new parameter to the linked list
        if (new_head == NULL)
        {
            new_head = new_param; // Set as head if it's the first node
        }
        else
        {
            new_tail->next = new_param; // Attach to the end of the list
        }
        new_tail = new_param; // Update the tail pointer

        // Move to the next parameter in the source list
        param_list = param_list->next;
    }

    return new_head;
}

ASTNode *copy_ast_node(ASTNode *node)
{
    if (!node)
        return NULL;

    ASTNode *new_node = malloc(sizeof(ASTNode));
    if (!new_node)
    {
        fprintf(stderr, "Error: Memory allocation failed in `copy_ast_node`\n");
        exit(1);
    }

    memcpy(new_node, node, sizeof(ASTNode));

    // Deep copy for fields like `function_call`, `body`, or `arguments`
    if (node->function_call.arguments)
    {
        new_node->function_call.arguments = copy_ast_node(node->function_call.arguments);
    }
    if (node->function_call.body)
    {
        new_node->function_call.body = copy_ast_node(node->function_call.body);
    }

    // Handle other types like linked lists or child nodes as needed
    return new_node;
}

void add_function(Environment *env, Function func)
{
    // Step 1: Ensure `functions` is initialized if not already done
    if (!env->functions && env->function_capacity == 0)
    {
        env->functions = calloc(4, sizeof(Function)); // initialize with capacity
        if (!env->functions)
        {
            fprintf(stderr, "Error: Initial allocation for functions failed.\n");
            exit(1);
        }
        env->function_capacity = 4;
    }

    // Step 2: Resize `functions` array if necessary
    if (env->function_count == env->function_capacity)
    {
        size_t new_capacity = env->function_capacity * 2;
        Function *new_functions = realloc(env->functions, new_capacity * sizeof(Function));
        if (!new_functions)
        {
            fprintf(stderr, "Error: Memory allocation failed for functions.\n");
            exit(1);
        }
        env->functions = new_functions;
        env->function_capacity = new_capacity;
    }

    // Step 3: Verify `func.name` is valid
    if (!func.name)
    {
        fprintf(stderr, "Error: Function name is `NULL` or invalid.\n");
        exit(1);
    }

    // Step 4: Create a deep copy of the function being added
    Function *stored_func = &env->functions[env->function_count++];
    stored_func->parameters = copy_function_parameters(func.parameters); // Create a copy of parameters
    stored_func->body = copy_ast_node(func.body);                        // Still assuming shared ownership of the body (might need a copy if modified)

    // Step 5: Safely duplicate the function name
    stored_func->name = strdup(func.name); // allocate memory for name
    if (!stored_func->name)
    {
        fprintf(stderr, "Error: Memory allocation failed for function name.\n");
        free(stored_func); // Free partially allocated function on error
        exit(1);
    }

    debug_print_int("Function `%s` added successfully.\n", stored_func->name);
}

Function *get_function(Environment *env, const char *name)
{
    for (size_t i = 0; i < env->function_count; i++)
    {
        if (strcmp(env->functions[i].name, name) == 0)
        {
            return &env->functions[i];
        }
    }
    return NULL;
}

void interpret_function_declaration(ASTNode *node, Environment *env)
{
    debug_print_int("`interpret_function_declaration()` called\n");

    if (!node || !node->function_call.name)
    {
        fprintf(stderr, "Error: Invalid function declaration\n");
        exit(1);
    }

    // Initialize param_list as NULL
    ASTFunctionParameter *param_list = NULL;
    ASTFunctionParameter *param_tail = NULL; // keep track of the last parameter

    // Copy parameters
    ASTFunctionParameter *param = node->function_call.parameters;
    while (param)
    {
        ASTFunctionParameter *new_param = malloc(sizeof(ASTFunctionParameter));
        if (!new_param)
        {
            fprintf(stderr, "Error: Memory allocation failed for function parameter\n");
            exit(1);
        }

        new_param->parameter_name = strdup(param->parameter_name);
        if (!new_param->parameter_name)
        {
            fprintf(stderr, "Error: Memory allocation failed for parameter name\n");
            free(new_param);
            exit(1);
        }

        new_param->next = NULL;

        // Add to list
        if (!param_list)
        {
            param_list = new_param;
            param_tail = new_param;
        }
        else
        {
            param_tail->next = new_param;
            param_tail = new_param;
        }

        param = param->next;
    }

    Function func = {
        .name = strdup(node->function_call.name),
        .parameters = param_list,
        .body = node->function_call.body};

    add_function(env, func);
}

LiteralValue interpret_function_call(ASTNode *node, Environment *env)
{
    debug_print_int("Starting function call interpretation\n");

    if (!node || !node->function_call.name)
    {
        fprintf(stderr, "Error: Invalid function call\n");
        return (LiteralValue){.type = TYPE_ERROR};
    }

    // Get the function from environment
    Function *func = get_function(env, node->function_call.name);
    if (!func)
    {
        fprintf(stderr, "Error: Undefined function `%s`\n", node->function_call.name);
        return (LiteralValue){.type = TYPE_ERROR};
    }

    // Create a local environment
    Environment local_env;
    init_environment(&local_env);

    // Copy all parent functions into the local environment
    for (size_t i = 0; i < env->function_count; i++)
    {
        Function func_copy = {
            .name = strdup(env->functions[i].name),
            .parameters = copy_function_parameters(env->functions[i].parameters),
            .body = copy_ast_node(env->functions[i].body)};
        add_function(&local_env, func_copy);
    }

    // Bind parameters
    ASTFunctionParameter *param = func->parameters;
    ASTNode *arg = node->function_call.arguments;
    while (param && arg)
    {
        LiteralValue arg_value = interpret(arg, env);
        if (arg_value.type == TYPE_ERROR)
        {
            free_environment(&local_env);
            return arg_value;
        }

        Variable param_var = {
            .variable_name = strdup(param->parameter_name),
            .value = arg_value};
        add_variable(&local_env, param_var);

        param = param->next;
        arg = arg->next;
    }

    // Interpret the function body
    LiteralValue result = create_default_value(); // default = 0
    ASTNode *stmt = func->body;

    while (stmt)
    {
        LiteralValue current_val = interpret(stmt, &local_env);

        // Short-circuit if we hit a return
        if (stmt->type == AST_FUNCTION_RETURN)
        {
            debug_print_int("Function returning with value type=%d, value=%d\n",
                            current_val.type,
                            current_val.type == TYPE_INTEGER ? current_val.data.integer : 0);

            // Copy the returned value
            LiteralValue return_val = {.type = current_val.type};
            switch (current_val.type)
            {
            case TYPE_INTEGER:
                return_val.data.integer = current_val.data.integer;
                break;
            case TYPE_FLOAT:
                return_val.data.floating_point = current_val.data.floating_point;
                break;
            case TYPE_STRING:
                if (current_val.data.string)
                    return_val.data.string = strdup(current_val.data.string);
                break;
            default:
                break;
            }

            free_environment(&local_env);
            return return_val;
        }

        // If there's an error, bail
        if (current_val.type == TYPE_ERROR)
        {
            free_environment(&local_env);
            return current_val;
        }

        // Otherwise keep going
        result = current_val;
        stmt = stmt->next;
    }

    free_environment(&local_env);
    return result;
}

// Initialize the environment
void init_environment(Environment *env)
{
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
void free_environment(Environment *env)
{
    // Free variables
    for (size_t i = 0; i < env->variable_count; i++)
    {
        free(env->variables[i].variable_name);
    }
    free(env->variables);

    // Free functions
    for (size_t i = 0; i < env->function_count; i++)
    {
        free(env->functions[i].name);
    }
    free(env->functions);
}
