#include "interpreter.h"
#include "../parser/ast_types.h"
#include <stdio.h>
#include <string.h>

LiteralValue interpret(ASTNode *node, Environment *env);
LiteralValue interpret_literal(ASTNode *node);
LiteralValue interpret_assignment(ASTNode *node, Environment *env);
LiteralValue interpret_binary_op(ASTNode *node, Environment *env);
Variable *get_variable(Environment *env, const char *variable_name);
void interpret_print(ASTNode *node, Environment *env);
void interpret_conditional(ASTNode *node, Environment *env);

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

    switch (node->type)
    {
    case AST_LITERAL:
        return interpret_literal(node);
    case AST_ASSIGNMENT:
        printf("DEBUG: Interpreted node type: %d\n", node->type);
        if (node->type == TYPE_NUMBER)
        {
            printf("DEBUG: Node is a number: %f\n", node->literal.value.number);
        }
        else if (node->type == TYPE_STRING)
        {
            printf("DEBUG: Node is a string: %s\n", node->literal.value.string);
        }
        return interpret_assignment(node, env);
    case AST_BINARY_OP:
        return interpret_binary_op(node, env);
    case AST_PRINT:
        interpret_print(node, env);
        return create_default_value();
    case AST_CONDITIONAL:
        interpret_conditional(node, env);
        return create_default_value();
    case AST_FUNCTION_CALL:
        fprintf(stderr, "Error: Function calls not implemented yet.\n");
        exit(1);
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
        interpret(current, env);
        current = current->next;
    }
}

LiteralValue interpret_literal(ASTNode *node)
{
    LiteralValue value;
    printf("DEBUG: Interpreting literal value...\n");

    switch (node->literal.type)
    {
    case LITERAL_NUMBER:
        printf("DEBUG: LITERAL_NUMBER, value: %f\n", node->literal.value.number);
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

LiteralValue interpret_assignment(ASTNode *node, Environment *env)
{
    if (!node->assignment.variable_name)
    {
        fprintf(stderr, "Error: Variable name for assignment is NULL.\n");
        exit(1); // or handle gracefully
    }

    // Check if the variable already exists
    for (size_t i = 0; i < env->variable_count; i++)
    {
        if (strcmp(env->variables[i].variable_name, node->assignment.variable_name) == 0)
        {
            // Update existing variable
            if (node->assignment.value) // check if there's a new value to assign
            {
                env->variables[i].value = interpret(node->assignment.value, env);
                printf("DEBUG: Updated variable `%s` to value `%f`\n",
                       node->assignment.variable_name, env->variables[i].value.data.number);
            }
            return env->variables[i].value;
        }
    }

    // Add a new variable if it doesn't exist (original logic)
    if (env->variable_count == env->capacity)
    {
        env->capacity *= 2;
        env->variables = realloc(env->variables, env->capacity * sizeof(Variable));
    }

    env->variables[env->variable_count].variable_name = strdup(node->assignment.variable_name);
    env->variables[env->variable_count].value = interpret(node->assignment.value, env);
    env->variable_count++;

    printf("DEBUG: New variable `%s` with value `%f`\n", node->assignment.variable_name, env->variables[env->variable_count].value.data.number);
    return env->variables[env->variable_count].value;
}

LiteralValue interpret_binary_op(ASTNode *node, Environment *env)
{
    LiteralValue left = interpret(node->binary_op.left, env);
    LiteralValue right = interpret(node->binary_op.right, env);

    if (left.type != TYPE_NUMBER || right.type != TYPE_NUMBER)
    {
        fprintf(stderr, "Error: Binary operations only supported for numbers.\n");
        exit(1);
    }

    LiteralValue result;
    result.type = TYPE_NUMBER;

    char *operator= node->binary_op.operator;

    switch (operator[0])
    {
    case '+':
        result.data.number = left.data.number + right.data.number;
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
        if (operator[1] == '=')
        {
            result.data.number = (left.data.number <= right.data.number) ? 1.0 : 0.0;
        }
        else
        {
            result.data.number = (left.data.number < right.data.number) ? 1.0 : 0.0;
        }
        break;
    case '>':
        if (operator[1] == '=')
        {
            result.data.number = (left.data.number >= right.data.number) ? 1.0 : 0.0;
        }
        else
        {
            result.data.number = (left.data.number > right.data.number) ? 1.0 : 0.0;
        }
        break;
    case '=':
        if (operator[1] == '=')
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

    printf("DEBUG: Binary operation `%f %s %f`\n",
           left.data.number, node->binary_op.operator, right.data.number);

    return result;
}

Variable *get_variable(Environment *env, const char *variable_name)
{
    for (size_t i = 0; i < env->variable_count; i++)
    {
        if (strcmp(env->variables[i].variable_name, variable_name) == 0)
        {
            return &env->variables[i];
        }
    }

    return NULL; // variable not found
}

void interpret_print(ASTNode *node, Environment *env)
{
    // printf("interpret_print(ASTNode *node, Environment *env)\n");
    for (size_t i = 0; i < node->to_print.arg_count; i++)
    {
        ASTNode *arg = node->to_print.arguments[i];

        if (arg->type == AST_LITERAL)
        {
            // // Handle string literals directly
            // printf("%s ", arg->literal.value.string);
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
        else
        {
            // Evaluate and print numeric expressions
            LiteralValue value = interpret(arg, env);
            printf("%f ", value.data.number);
        }
    }
    printf("\n");
}

void interpret_conditional(ASTNode *node, Environment *env)
{
    printf("DEBUG: interpret_conditional called\n");

    // Debug: Print oven_temperature before conditional evaluation
    for (size_t i = 0; i < env->variable_count; i++)
    {
        if (strcmp(env->variables[i].variable_name, "oven_temperature") == 0)
        {
            printf("DEBUG: Before conditional, oven_temperature = %f\n", env->variables[i].value.data.number);
        }
    }

    ASTNode *current_branch = node;
    int condition_met = 0; // Initialize to false

    while (current_branch) // Continue as long as we have a branch
    {
        // Handle else branch
        if (!current_branch->conditional.condition) // This is an else branch
        {
            printf("`ELSE` BRANCH\n");
            if (!condition_met)
            {
                interpret(current_branch->conditional.body, env);
            }
            break;
        }

        // Handle if/elif branches
        printf("`IF/ELIF` BRANCH\n");
        LiteralValue condition_value = interpret(current_branch->conditional.condition, env);
        printf("DEBUG: condition evaluated to: %f\n", condition_value.data.number);

        if (condition_value.type == TYPE_NUMBER && condition_value.data.number != 0)
        {
            printf("DEBUG: executing true branch\n");
            interpret(current_branch->conditional.body, env);
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
            printf("DEBUG: After conditional, oven_temperature = %f\n", env->variables[i].value.data.number);
        }
    }
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
