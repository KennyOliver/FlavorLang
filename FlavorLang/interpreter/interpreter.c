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
    switch (node->literal.type)
    {
    case LITERAL_NUMBER:
        value.type = TYPE_NUMBER;
        value.data.number = node->literal.value.number;
        break;
    case LITERAL_STRING:
        value.type = TYPE_STRING;
        value.data.string = strdup(node->literal.value.string); // copy string
        break;
    default:
        fprintf(stderr, "Error: Unsupported literal type.\n");
        exit(1);
    }
    return value;
}

LiteralValue interpret_assignment(ASTNode *node, Environment *env)
{
    LiteralValue value = interpret(node->assignment.value, env);

    // Debug: Value being assigned
    printf("DEBUG: Assigning variable `%s` with value `%f`\n",
           node->assignment.variable_name, value.data.number);

    // Check if the variable already exists
    for (size_t i = 0; i < env->variable_count; i++)
    {
        if (strcmp(env->variables[i].variable_name, node->assignment.variable_name) == 0)
        {
            // Updatge existing variable
            env->variables[i].value = value;
            return value;
        }
    }

    // Add a new variable
    if (env->variable_count == env->capacity)
    {
        env->capacity *= 2;
        env->variables = realloc(env->variables, env->capacity * sizeof(Variable));
    }

    env->variables[env->variable_count].variable_name = strdup(node->assignment.variable_name);
    env->variables[env->variable_count].value = value;
    env->variable_count++;

    return value;
}

double interpret_variable(ASTNode *node, Environment *env)
{
    for (size_t i = 0; i < env->variable_count; i++)
    {
        if (strcmp(env->variables[i].variable_name, node->variable_name) == 0)
        {
            if (env->variables[i].value.type == TYPE_NUMBER)
            {
                return env->variables[i].value.data.number;
            }
            else if (env->variables[i].value.type == TYPE_STRING)
            {
                //
            }
        }
    }

    fprintf(stderr, "Error: Variable `%s` not found.\n", node->variable_name);
    exit(1);
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

    if (strcmp(operator, "+") == 0)
    {
        result.data.number = left.data.number + right.data.number;
    }
    else if (strcmp(operator, "-") == 0)
    {
        result.data.number = left.data.number - right.data.number;
    }
    else if (strcmp(operator, "*") == 0)
    {
        result.data.number = left.data.number * right.data.number;
    }
    else if (strcmp(operator, "/") == 0)
    {
        if (right.data.number == 0)
        {
            fprintf(stderr, "Error: Division by zero.\n");
            exit(1);
        }
        result.data.number = left.data.number / right.data.number;
    }
    else if (strcmp(operator, "<") == 0)
    {
        result.data.number = (left.data.number < right.data.number) ? 1.0 : 0.0;
    }
    else if (strcmp(operator, "<=") == 0)
    {
        result.data.number = (left.data.number <= right.data.number) ? 1.0 : 0.0;
    }
    else if (strcmp(operator, ">") == 0)
    {
        result.data.number = (left.data.number > right.data.number) ? 1.0 : 0.0;
    }
    else if (strcmp(operator, ">=") == 0)
    {
        result.data.number = (left.data.number >= right.data.number) ? 1.0 : 0.0;
    }
    else if (strcmp(operator, "==") == 0)
    {
        result.data.number = (left.data.number == right.data.number) ? 1.0 : 0.0;
    }
    else
    {
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
    printf("interpret_print(ASTNode *node, Environment *env)");
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
    LiteralValue condition_value = interpret(node->conditional.condition, env);
    printf("DEBUG: condition evaluated to: %f\n", condition_value.data.number);

    if (condition_value.type == TYPE_NUMBER && condition_value.data.number != 0)
    {
        printf("DEBUG: executing true branch\n");
        interpret(node->conditional.body, env);
    }
    else if (node->conditional.else_branch)
    {
        printf("DEBUG: executing else branch\n");
        interpret(node->conditional.else_branch, env);
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
