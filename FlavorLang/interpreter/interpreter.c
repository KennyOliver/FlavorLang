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

// Helper function to create a default LiteralValue (zero number)
LiteralValue create_default_value()
{
    LiteralValue value = {
        .type = TYPE_NUMBER,
        .data = {.number = 0}};
    return value;
}

LiteralValue interpret(ASTNode *node, Environment *env)
{
    if (!node)
    {
        return (LiteralValue){
            .type = TYPE_ERROR,
        };
    }

    debug_print_int("`interpret()` called\n");

    switch (node->type)
    {
    case AST_LITERAL:
        debug_print_int("Matched: `AST_LITERAL`\n");
        return interpret_literal(node);
    case AST_ASSIGNMENT:
        debug_print_int("Matched: `AST_ASSIGNMENT`\n");
        debug_print_int("Interpreted node type: `%d`\n", (int)node->type);
        if (node->type == TYPE_NUMBER)
        {
            debug_print_int("Node is a number: `%f`\n", node->literal.value.number);
        }
        else if (node->type == TYPE_STRING)
        {
            debug_print_int("Node is a string: `%s`\n", node->literal.value.string);
        }
        return interpret_assignment(node, env);
    case AST_BINARY_OP:
        debug_print_int("Matched: `AST_BINARY_OP`\n");
        return interpret_binary_op(node, env);
    case AST_PRINT:
        debug_print_int("Matched: `AST_PRINT`\n");
        interpret_print(node, env);
        return create_default_value();
    case AST_INPUT:
    {
        debug_print_int("Matched: `AST_INPUT`\n");
        Variable v = interpret_input(env);
        return v.value;
    }
    case AST_CONDITIONAL:
        debug_print_int("Matched: `AST_CONDITIONAL`\n");
        interpret_conditional(node, env);
        return create_default_value();
    case AST_FUNCTION_CALL:
        debug_print_int("Matched: `AST_FUNCTION_CALL`\n");
        return interpret_function_call(node, env);
    case AST_FUNCTION_DECLARATION:
        debug_print_int("Matched: `AST_FUNCTION_DECLARATION`\n");
        interpret_function_declaration(node, env);
        return create_default_value();
    case AST_LOOP:
        debug_print_int("Matched: `AST_LOOP`\n");
        interpret_while_loop(node, env);
        return create_default_value();
    case AST_VARIABLE:
        debug_print_int("Matched: `AST_VARIABLE`\n");
        return interpret_variable(node, env);
    case AST_SWITCH:
        debug_print_int("Matched: `AST_SWITCH`\n");
        interpret_switch(node, env);
        return create_default_value();
    case AST_ERROR:
        debug_print_int("Matched: `AST_ERROR`\n");
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

    switch (node->literal.type)
    {
    case LITERAL_NUMBER:
        debug_print_int("LITERAL_NUMBER, value: `%f`\n", node->literal.value.number);
        value.type = TYPE_NUMBER;
        value.data.number = node->literal.value.number;
        break;
    case LITERAL_STRING:
        value.type = TYPE_STRING;
        value.data.string = strdup(node->literal.value.string);
        break;
    default:
        fprintf(stderr, "Error: Unsupported literal type.\n");
        exit(1);
    }

    return value;
}

LiteralValue interpret_variable(ASTNode *node, Environment *env)
{
    printf("Env var 0: `%s`", env->variables[0].variable_name);
    Variable *var = get_variable(env, node->variable_name);
    if (!var)
    {
        fprintf(stderr, "Error: Undefined variable `%s`.\n", node->variable_name);
        exit(1);
    }

    if (var->value.type == TYPE_NUMBER)
    {
        debug_print_int("Retrieved variable `%s` with value `%f`\n",
                        node->variable_name,
                        var->value.data.number);
    }
    else if (var->value.type == TYPE_STRING)
    {
        debug_print_int("Retrieved variable `%s` with value `%s`\n",
                        node->variable_name,
                        var->value.data.string);
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

LiteralValue interpret_binary_op(ASTNode *node, Environment *env)
{
    LiteralValue left = interpret(node->binary_op.left, env);
    LiteralValue right = interpret(node->binary_op.right, env);

    debug_print_int("Binary operation: left=`%f`, operator=`%s`, right=`%f`",
                    left.data.number,
                    node->binary_op.operator,
                    right.data.number);

    LiteralValue result;
    result.type = TYPE_NUMBER;

    char *operator= node->binary_op.operator;

    switch (operator[0])
    {
    case '+':
        // Check if at least one of the operands is a string
        if (left.type == TYPE_STRING || right.type == TYPE_STRING)
        {
            result.type = TYPE_STRING;

            // If either operand is a number, convert it to a string
            char num_str1[50] = {0};
            char num_str2[50] = {0};
            if (left.type == TYPE_NUMBER)
            {
                snprintf(num_str1, sizeof(num_str1), "%f", left.data.number);
            }
            if (right.type == TYPE_NUMBER)
            {
                snprintf(num_str2, sizeof(num_str2), "%f", right.data.number);
            }

            // Concatenate the strings
            char *new_string = malloc(strlen(num_str1) + strlen(num_str2) +
                                      strlen(left.type == TYPE_STRING ? left.data.string : "") +
                                      strlen(right.type == TYPE_STRING ? right.data.string : "") + 1);
            if (new_string == NULL)
            {
                fprintf(stderr, "Error: Memory allocation failed for string concatenation.\n");
                exit(1);
            }
            strcpy(new_string, left.type == TYPE_STRING ? left.data.string : num_str1);
            strcat(new_string, right.type == TYPE_STRING ? right.data.string : num_str2);
            result.data.string = new_string;
        }
        else
        {
            result.data.number = left.data.number + right.data.number;
            debug_print_int("Addition result: `%f + %f = %f`",
                            left.data.number,
                            right.data.number,
                            result.data.number);
        }
        break;
    case '-':
        result.data.number = left.data.number - right.data.number;
        break;
    case '*':
        result.data.number = left.data.number * right.data.number;
        break;
    case '/':
        if (right.data.number == 0)
        {
            fprintf(stderr, "Error: Division by zero.\n");
            exit(1);
        }
        result.data.number = left.data.number / right.data.number;
        break;
    case '<':
        if (operator[1] && operator[1] == '=')
        {
            result.data.number = (left.data.number <= right.data.number) ? 1.0 : 0.0;
        }
        else
        {
            result.data.number = (left.data.number < right.data.number) ? 1.0 : 0.0;
        }
        break;
    case '>':
        if (operator[1] && operator[1] == '=')
        {
            result.data.number = (left.data.number >= right.data.number) ? 1.0 : 0.0;
        }
        else
        {
            result.data.number = (left.data.number > right.data.number) ? 1.0 : 0.0;
        }
        break;
    case '=':
        if (operator[1] && operator[1] == '=')
        {
            result.data.number = (left.data.number == right.data.number) ? 1.0 : 0.0;
        }
        else
        {
            fprintf(stderr, "Error: Unsupported operator `%s`.\n", operator);
            exit(1);
        }
        break;
    default:
        fprintf(stderr, "Error: Unsupported operator `%s`.\n", operator);
        exit(1);
    }

    switch (result.type)
    {
    case TYPE_STRING:
        if (left.type == TYPE_STRING && right.type == TYPE_STRING)
        {
            debug_print_int("Binary operation `\"%s\" %s %f`\n",
                            left.data.string, node->binary_op.operator, right.data.string);
        }
        else if (left.type == TYPE_STRING && right.type == TYPE_NUMBER)
        {
            debug_print_int("Binary operation `\"%s\" %s %f`\n",
                            left.data.string, node->binary_op.operator, right.data.number);
        }
        else if (left.type == TYPE_NUMBER && right.type == TYPE_STRING)
        {
            debug_print_int("Binary operation `%f %s \"%s\"`\n",
                            left.data.number, node->binary_op.operator, right.data.string);
        }
        else
        {
            debug_print_int("Binary operation `%s %s %s`\n",
                            left.data.string, node->binary_op.operator, right.data.string);
        }
        break;
    default:
        debug_print_int("Binary operation `%f %s %f`\n",
                        left.data.number, node->binary_op.operator, right.data.number);
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
            if (env->variables[i].value.type == TYPE_NUMBER)
            {
                debug_print_int("Variable found: `%s` with value `%f`\n",
                                variable_name,
                                env->variables[i].value.data.number);
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

        if (arg->type == AST_LITERAL)
        {
            // // Handle string literals directly
            if (arg->literal.type == LITERAL_STRING)
            {
                printf("%s ", arg->literal.value.string);
            }
            else if (arg->literal.type == LITERAL_NUMBER)
            {
                printf("%f ", arg->literal.value.number);
            }
        }
        else if (arg->type == AST_ASSIGNMENT)
        {
            Variable *var = get_variable(env, arg->variable_name);
            if (!var)
            {
                fprintf(stderr, "Error: Undefined variable `%s`.\n", arg->variable_name);
            }

            if (var->value.type == TYPE_STRING)
            {
                printf("%s ", var->value.data.string);
            }
            else if (var->value.type == TYPE_NUMBER)
            {
                printf("%f ", var->value.data.number);
            }
        }
        else if (arg->type == AST_VARIABLE)
        {
            Variable *var = get_variable(env, arg->variable_name);
            if (!var)
            {
                fprintf(stderr, "Error: Undefined variable `%s`.\n", arg->variable_name);
            }

            if (var->value.type == TYPE_STRING)
            {
                printf("%s ", var->value.data.string);
            }
            else if (var->value.type == TYPE_NUMBER)
            {
                printf("%f ", var->value.data.number);
            }
        }
        else
        {
            // Evaluate and print numeric expressions
            LiteralValue value = interpret(arg, env);
            printf("%f ", value.data.number);
        }
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

    ASTNode *current_branch = node;
    int condition_met = 0; // Initialize to false

    while (current_branch) // Continue as long as we have a branch
    {
        // Handle else branch
        if (!current_branch->conditional.condition) // This is an else branch
        {
            debug_print_int("Executing ELSE branch\n");
            if (!condition_met)
            {
                // Execute all statements in the body
                ASTNode *current_statement = current_branch->conditional.body;
                while (current_statement)
                {
                    debug_print_int("Executing statement in else branch\n");
                    interpret(current_statement, env);
                    current_statement = current_statement->next;
                }
            }
            break;
        }

        // Handle if/elif branches
        debug_print_int("Evaluating `if`/`elif` branch\n");
        LiteralValue condition_value = interpret(current_branch->conditional.condition, env);
        debug_print_int("Condition evaluated to: `%f`\n", condition_value.data.number);

        if (condition_value.type == TYPE_NUMBER && condition_value.data.number != 0)
        {
            debug_print_int("Condition is true, executing branch body\n");
            // Execute all statements in the body
            ASTNode *current_statement = current_branch->conditional.body;
            while (current_statement)
            {
                debug_print_int("Executing statement in true branch\n");
                interpret(current_statement, env);
                current_statement = current_statement->next;
            }
            condition_met = 1;
            break;
        }

        current_branch = current_branch->conditional.else_branch; // Move to the next branch
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

        if (condition_value.type == TYPE_NUMBER)
        {
            debug_print_int("Condition evaluated to: `%f`\n", condition_value.data.number);
        }

        if (condition_value.data.number != 0)
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
                debug_print_int("Variable `%s`: value = `%f`",
                                env->variables[i].variable_name,
                                env->variables[i].value.data.number);
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

            if (switch_value.type == TYPE_NUMBER)
            {
                values_match = (switch_value.data.number == case_value.data.number);
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
    debug_print_int("Function name before `strdup`: `%s`\n", func.name);

    // Step 4: Create a deep copy of the function being added
    Function *stored_func = &env->functions[env->function_count++];
    stored_func->parameters = copy_function_parameters(func.parameters); // Create a copy of parameters
    stored_func->body = func.body;                                       // Still assuming shared ownership of the body (might need a copy if modified)

    // Step 5: Safely duplicate the function name
    stored_func->name = strdup(func.name); // allocate memory for name
    if (!stored_func->name)
    {
        fprintf(stderr, "Error: Memory allocation failed for function name.\n");
        free(stored_func); // Free partially allocated function on error
        exit(1);
    }
    debug_print_int("Function name after `strdup`: `%s`\n", stored_func->name);

    // Step 6: Assign parameters and body (assume shared ownership)
    // stored_func->parameters = func.parameters;
    // stored_func->body = func.body;

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

    if (!node)
    {
        fprintf(stderr, "Error: Null `ASTNode` passed to interpret_function_declaration\n");
        exit(1);
    }

    if (!node->function_call.name)
    {
        fprintf(stderr, "Error: Function declaration missing name\n");
        exit(1);
    }
    debug_print_int("Function name validated\n");

    if (!node->function_call.parameters)
    {
        fprintf(stderr, "Error: Null parameters in function declaration\n");
        exit(1);
    }
    debug_print_int("Parameters validated\n");

    // Create a deep copy of the parameter list
    ASTFunctionParameter *param_list = malloc(sizeof(ASTFunctionParameter));
    if (!param_list)
    {
        fprintf(stderr, "Error: Memory allocation failed for `ASTFunctionParameter`.\n");
        exit(1);
    }
    debug_print_int("Parameter list created successfully\n");

    ASTFunctionParameter *param = node->function_call.parameters;
    if (!param)
    {
        fprintf(stderr, "Error: Function declaration has no parameters\n");
        exit(1);
    }

    while (param)
    {
        debug_print_int("1: Current param address: %p\n", (void *)param);

        if (!param->parameter_name)
        {
            fprintf(stderr, "Error: Encountered NULL parameter name in function declaration\n");
            exit(1);
        }

        debug_print_int("2: Current param name: %s\n", param->parameter_name);

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

        debug_print_int("3: Copied parameter name: %s\n", new_param->parameter_name);

        new_param->next = NULL;

        if (!param_list)
        {
            param_list = new_param;
        }
        else
        {
            ASTFunctionParameter *tmp = param_list;
            while (tmp->next)
            {
                tmp = tmp->next;
            }
            tmp->next = new_param;
        }

        param = param->next;
        debug_print_int("4: Moved to next parameter\n");
    }

    if (!node->function_call.body)
    {
        fprintf(stderr, "Error: Null body in function declaration\n");
        exit(1);
    }
    debug_print_int("Function body validated\n");

    Function func = {
        .name = strdup(node->function_call.name),
        .parameters = param_list,
        .body = node->function_call.body};

    if (!func.name)
    {
        fprintf(stderr, "Error: `strdup` failed for function name\n");
        exit(1);
    }

    if (!func.body)
    {
        fprintf(stderr, "Error: Null body in function declaration\n");
        exit(1);
    }

    add_function(env, func);

    debug_print_int("Function added to environment successfully\n");
}

LiteralValue interpret_function_call(ASTNode *node, Environment *env)
{
    if (!node)
    {
        fprintf(stderr, "Error: Null `ASTNode` passed to `interpret_function_call()`\n");
        exit(1);
    }

    if (!node->function_call.name)
    {
        fprintf(stderr, "Error: Function name is missing in `ASTNode`\n");
        exit(1);
    }
    if (!node->function_call.body)
    {
        fprintf(stderr, "Error: Function body is missing in `ASTNode`\n");
        exit(1);
    }

    if (!node->function_call.arguments)
    {
        fprintf(stderr, "Error: Null arguments in function call\n");
        exit(1);
    }

    Function *func = get_function(env, node->function_call.name);
    if (!func)
    {
        fprintf(stderr, "Error: Undefined function `%s`\n", node->function_call.name);
        exit(1);
    }

    Environment local_env;
    init_environment(&local_env);

    ASTFunctionParameter *param = func->parameters;
    ASTNode *arg = node->function_call.arguments;

    while (param || arg)
    {
        if (!param)
        {
            fprintf(stderr, "Error: Too many arguments provided to function `%s`\n", node->function_call.name);
            exit(1);
        }
        if (!arg)
        {
            fprintf(stderr, "Error: Missing argument for parameter `%s`\n", param->parameter_name);
            exit(1);
        }

        LiteralValue value = interpret(arg, env);
        if (value.type == TYPE_STRING && !value.data.string)
        {
            fprintf(stderr, "Error: Null string value returned by interpret\n");
            exit(1);
        }
        else if (value.type == TYPE_NUMBER && value.data.number == 0)
        {
            fprintf(stderr, "Error: Invalid number value returned by interpret\n");
            exit(1);
        }

        Variable var = {
            .variable_name = strdup(param->parameter_name),
            .value = value};
        add_variable(&local_env, var);

        param = param->next;
        arg = arg->next;
    }

    LiteralValue return_value = {.type = TYPE_STRING, .data.string = strdup("")};

    ASTNode *stmt = func->body;
    while (stmt)
    {
        if (stmt->type == AST_FUNCTION_CALL)
        {
            if (strcmp(stmt->function_call.name, "deliver") == 0)
            {
                LiteralValue delivered = interpret(stmt->function_call.arguments, &local_env);
                free(return_value.data.string);
                return_value = delivered;
                free_environment(&local_env);
                return return_value;
            }
            if (strcmp(stmt->function_call.name, "burn") == 0)
            {
                interpret_raise_error(stmt->function_call.arguments, &local_env);
                free_environment(&local_env);
                exit(1);
            }
        }
        interpret(stmt, &local_env);
        stmt = stmt->next;
    }

    free_environment(&local_env);
    return return_value;
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
