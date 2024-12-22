#include "interpreter.h"
#include "../parser/ast_types.h"
#include <stdio.h>
#include <string.h>
#include "../debug/debug.h"

LiteralValue interpret(ASTNode *node, Environment *env);
LiteralValue interpret_literal(ASTNode *node);
LiteralValue interpret_variable(ASTNode *node, Environment *env);
LiteralValue interpret_assignment(ASTNode *node, Environment *env);
LiteralValue interpret_binary_op(ASTNode *node, Environment *env);
Variable *get_variable(Environment *env, const char *variable_name);
void interpret_print(ASTNode *node, Environment *env);
Variable interpret_input(Environment *env);
void interpret_conditional(ASTNode *node, Environment *env);
void interpret_while_loop(ASTNode *node, Environment *env);

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
        return create_default_value();
    }

    debug_print("`interpret()` called\n");

    switch (node->type)
    {
    case AST_LITERAL:
        debug_print("Matched: `AST_LITERAL`");
        return interpret_literal(node);
    case AST_ASSIGNMENT:
        debug_print("Matched: `AST_ASSIGNMENT`");
        debug_print("Interpreted node type: %d", (int)node->type);
        if (node->type == TYPE_NUMBER)
        {
            debug_print("Node is a number: %f", node->literal.value.number);
        }
        else if (node->type == TYPE_STRING)
        {
            debug_print("Node is a string: %s", node->literal.value.string);
        }
        return interpret_assignment(node, env);
    case AST_BINARY_OP:
        debug_print("Matched: `AST_BINARY_OP`");
        return interpret_binary_op(node, env);
    case AST_PRINT:
        debug_print("Matched: `AST_PRINT`");
        interpret_print(node, env);
        return create_default_value();
    case AST_INPUT:
    {
        debug_print("Matched: `AST_INPUT`");
        Variable v = interpret_input(env);
        return v.value;
    }
    case AST_CONDITIONAL:
        debug_print("Matched: `AST_CONDITIONAL`");
        interpret_conditional(node, env);
        return create_default_value();
    case AST_FUNCTION_CALL:
        debug_print("Matched: `AST_FUNCTION_CALL`");
        fprintf(stderr, "Error: Function calls not implemented yet.\n");
        exit(1);
    case AST_LOOP:
        debug_print("Matched: `AST_LOOP`");
        interpret_while_loop(node, env);
        return create_default_value();
    case AST_VARIABLE:
        debug_print("Matched: `AST_VARIABLE`");
        return interpret_variable(node, env);
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
        debug_print("Executing top-level statement\n");
        interpret(current, env);
        current = current->next;
    }
}

LiteralValue interpret_literal(ASTNode *node)
{
    LiteralValue value;
    debug_print("Interpreting literal value...\n");

    switch (node->literal.type)
    {
    case LITERAL_NUMBER:
        debug_print("LITERAL_NUMBER, value: %f", node->literal.value.number);
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
    Variable *var = get_variable(env, node->variable_name);
    if (!var)
    {
        fprintf(stderr, "Error: Undefined variable `%s`.\n", node->variable_name);
        exit(1);
    }

    if (var->value.type == TYPE_NUMBER)
    {
        debug_print("Retrieved variable `%s` with value `%f`",
                    node->variable_name,
                    var->value.data.number);
    }
    else if (var->value.type == TYPE_STRING)
    {
        debug_print("Retrieved variable `%s` with value `%s...`",
                    node->variable_name,
                    var->value.data.string);
    }

    return var->value;
}

LiteralValue interpret_assignment(ASTNode *node, Environment *env)
{
    if (!node->assignment.variable_name)
    {
        fprintf(stderr, "Error: Variable name for assignment is NULL.\n");
        exit(1);
    }

    debug_print("Interpreting assignment for variable: `%s`", node->assignment.variable_name);

    // Get the new value first
    LiteralValue new_value = interpret(node->assignment.value, env);

    // Debug print the value
    if (new_value.type == TYPE_STRING)
    {
        debug_print("Assignment value is string: '%s'\n", new_value.data.string);
    }
    else
    {
        debug_print("Assignment value is number: %f\n", new_value.data.number);
    }

    // Check if the variable already exists
    for (size_t i = 0; i < env->variable_count; i++)
    {
        if (strcmp(env->variables[i].variable_name, node->assignment.variable_name) == 0)
        {
            debug_print("Updating existing variable `%s` from `%f` to `%f`",
                        node->assignment.variable_name,
                        env->variables[i].value.data.number,
                        new_value.data.number);

            // Update existing variable
            env->variables[i].value = new_value;

            debug_print("Variable `%s` is now `%f`",
                        node->assignment.variable_name,
                        env->variables[i].value.data.number);

            return env->variables[i].value;
        }
    }

    // Add new variable if it doesn't exist
    if (env->variable_count == env->capacity)
    {
        env->capacity *= 2;
        env->variables = realloc(env->variables, env->capacity * sizeof(Variable));
        if (!env->variables)
        {
            fprintf(stderr, "Error: Memory reallocation failed.\n");
            exit(1);
        }
    }

    // debug_print("New variable `%s` with value ", node->assignment.variable_name);
    // if (env->variables[env->variable_count - 1].value.type == TYPE_STRING)
    // {
    //     printf("'%s'\n", env->variables[env->variable_count - 1].value.data.string);
    // }
    // else
    // {
    //     printf("%f\n", env->variables[env->variable_count - 1].value.data.number);
    // }

    env->variables[env->variable_count].variable_name = strdup(node->assignment.variable_name);
    env->variables[env->variable_count].value = new_value;

    if (new_value.type == TYPE_NUMBER)
    {
        debug_print("Created new variable `%s` with value `%f`",
                    node->assignment.variable_name,
                    new_value.data.number);
    }
    else if (new_value.type == TYPE_STRING)
    {
        debug_print("Created new variable `%s` with value `%s`",
                    node->assignment.variable_name,
                    new_value.data.string);
    }

    env->variable_count++;

    return new_value;
}

LiteralValue interpret_binary_op(ASTNode *node, Environment *env)
{
    LiteralValue left = interpret(node->binary_op.left, env);
    LiteralValue right = interpret(node->binary_op.right, env);

    debug_print("Binary operation: left=`%f`, operator=`%s`, right=`%f`",
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
            debug_print("Addition result: `%f + %f = %f`",
                        left.data.number,
                        right.data.number,
                        result.data.number);
        }
        break;
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
            debug_print("Binary operation `\"%s\" %s %f`\n",
                        left.data.string, node->binary_op.operator, right.data.string);
        }
        else if (left.type == TYPE_STRING && right.type == TYPE_NUMBER)
        {
            debug_print("Binary operation `\"%s\" %s %f`\n",
                        left.data.string, node->binary_op.operator, right.data.number);
        }
        else if (left.type == TYPE_NUMBER && right.type == TYPE_STRING)
        {
            debug_print("Binary operation `%f %s \"%s\"`\n",
                        left.data.number, node->binary_op.operator, right.data.string);
        }
        else
        {
            debug_print("Binary operation `%s %s %s`\n",
                        left.data.string, node->binary_op.operator, right.data.string);
        }
        break;
    default:
        debug_print("Binary operation `%f %s %f`\n",
                    left.data.number, node->binary_op.operator, right.data.number);
    }

    return result;
}

Variable *get_variable(Environment *env, const char *variable_name)
{
    debug_print("Looking up variable: `%s`", variable_name);
    for (size_t i = 0; i < env->variable_count; i++)
    {
        if (strcmp(env->variables[i].variable_name, variable_name) == 0)
        {
            if (env->variables[i].value.type == TYPE_NUMBER)
            {
                debug_print("Variable found: `%s` with value `%f`",
                            variable_name,
                            env->variables[i].value.data.number);
            }
            else if (env->variables[i].value.type == TYPE_STRING)
            {
                debug_print("Variable found: `%s` with value `%s`",
                            variable_name,
                            env->variables[i].value.data.string);
            }
            return &env->variables[i];
        }
    }
    debug_print("Variable not found: `%s`", variable_name);
    return NULL;
}

void interpret_print(ASTNode *node, Environment *env)
{
    debug_print("`interpret_print()`");
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

    debug_print("Read input: '%s'\n", input_buffer);

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

    debug_print("Stored value: '%s'\n", var->value.data.string);

    free(input_buffer);

    return *var;
}

void interpret_conditional(ASTNode *node, Environment *env)
{
    debug_print("`interpret_conditional` called");

    // Debug: Print oven_temperature before conditional evaluation
    for (size_t i = 0; i < env->variable_count; i++)
    {
        if (strcmp(env->variables[i].variable_name, "oven_temperature") == 0)
        {
            debug_print("Before conditional, oven_temperature = %f\n", env->variables[i].value.data.number);
        }
    }

    ASTNode *current_branch = node;
    int condition_met = 0; // Initialize to false

    while (current_branch) // Continue as long as we have a branch
    {
        // Handle else branch
        if (!current_branch->conditional.condition) // This is an else branch
        {
            debug_print("Executing ELSE branch\n");
            if (!condition_met)
            {
                // Execute all statements in the body
                ASTNode *current_statement = current_branch->conditional.body;
                while (current_statement)
                {
                    debug_print("Executing statement in else branch\n");
                    interpret(current_statement, env);
                    current_statement = current_statement->next;
                }
            }
            break;
        }

        // Handle if/elif branches
        debug_print("Evaluating IF/ELIF branch\n");
        LiteralValue condition_value = interpret(current_branch->conditional.condition, env);
        debug_print("Condition evaluated to: %f\n", condition_value.data.number);

        if (condition_value.type == TYPE_NUMBER && condition_value.data.number != 0)
        {
            debug_print("Condition is true, executing branch body\n");
            // Execute all statements in the body
            ASTNode *current_statement = current_branch->conditional.body;
            while (current_statement)
            {
                debug_print("Executing statement in true branch\n");
                interpret(current_statement, env);
                current_statement = current_statement->next;
            }
            condition_met = 1;
            break;
        }

        current_branch = current_branch->conditional.else_branch; // Move to the next branch
    }

    // Ensure oven_temperature is updated after conditional execution
    for (size_t i = 0; i < env->variable_count; i++)
    {
        if (strcmp(env->variables[i].variable_name, "oven_temperature") == 0)
        {
            debug_print("After conditional, oven_temperature = %f\n", env->variables[i].value.data.number);
        }
    }

    debug_print("`interpret_conditional` completed");
}

void interpret_while_loop(ASTNode *node, Environment *env)
{
    debug_print("`interpret_while_loop` called");

    ASTNode *condition = node->loop.condition;
    ASTNode *body = node->loop.body;

    while (1)
    {
        debug_print("Starting new iteration of `while` loop");

        LiteralValue condition_value = interpret(condition, env);

        if (condition_value.type == TYPE_NUMBER)
        {
            debug_print("Condition evaluated to: %f", condition_value.data.number);
        }

        if (condition_value.data.number != 0)
        {
            debug_print("`while` condition is true, executing body");

            ASTNode *current = body;
            while (current != NULL)
            {
                debug_print("Executing body statement of type: %d", current->type);
                interpret(current, env);
                current = current->next;
            }

            debug_print("After body execution:");
            for (size_t i = 0; i < env->variable_count; i++)
            {
                debug_print("Variable '%s': value = %f",
                            env->variables[i].variable_name,
                            env->variables[i].value.data.number);
            }
        }
        else
        {
            debug_print("`while` condition is false, exiting loop");
            break;
        }
    }

    debug_print("`interpret_while_loop` completed");
}

// Initialize the environment
void init_environment(Environment *env)
{
    env->variable_count = 0;
    env->capacity = 10;
    env->variables = malloc(env->capacity * sizeof(Variable));
}

// Free the environment
void free_environment(Environment *env)
{
    for (size_t i = 0; i < env->variable_count; i++)
    {
        free(env->variables[i].variable_name);
    }

    free(env->variables);
}
