#include "interpreter.h"
#include "../parser/ast_types.h"
#include <stdio.h>
#include <string.h>

double interpret(ASTNode *node, Environment *env);
double interpret_literal(ASTNode *node);
double interpret_assignment(ASTNode *node, Environment *env);
double interpret_binary_op(ASTNode *node, Environment *env);
void interpret_print(ASTNode *node, Environment *env);
void interpret_conditional(ASTNode *node, Environment *env);

double interpret(ASTNode *node, Environment *env)
{
    if (!node)
    {
        return 0;
    }

    switch (node->type)
    {
    case AST_LITERAL:
        return interpret_literal(node);
    case AST_ASSIGNMENT:
        interpret_assignment(node, env);
        return 0;
    case AST_BINARY_OP:
        return interpret_binary_op(node, env);
    case AST_PRINT:
        interpret_print(node, env);
        return 0;
    case AST_CONDITIONAL:
        interpret_conditional(node, env);
        return 0;
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

double interpret_literal(ASTNode *node)
{
    switch (node->literal.type)
    {
    case LITERAL_NUMBER:
        return node->literal.value.number;
    case LITERAL_STRING:
        fprintf("%s", node->literal.value.string);
        return 0;
    default:
        fprintf(stderr, "Error: Unsupported literal type.\n");
        exit(1);
    }
}

double interpret_assignment(ASTNode *node, Environment *env)
{
    double value = interpret(node->assignment.value, env);

    // Check if the variable exists
    for (size_t i = 0; i < env->variable_count; i++)
    {
        if (strcmp(env->variables[i].variable_name, node->assignment.variable_name) == 0)
        {
            env->variables[i].value = value;
            return;
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
}

double interpret_variable(ASTNode *node, Environment *env)
{
    for (size_t i = 0; i < env->variable_count; i++)
    {
        if (strcmp(env->variables[i].variable_name, node->variable_name) == 0)
        {
            return env->variables[i].value;
        }
    }

    fprintf(stderr, "Error: Variable `%s` not found.\n", node->variable_name);
}

double interpret_binary_op(ASTNode *node, Environment *env)
{
    double left = interpret(node->binary_op.left, env);
    double right = interpret(node->binary_op.right, env);

    if (strcmp(node->binary_op.operator, "+") == 0)
    {
        return left + right;
    }
    else if (strcmp(node->binary_op.operator, "-") == 0)
    {
        return left - right;
    }
    else if (strcmp(node->binary_op.operator, "*") == 0)
    {
        return left * right;
    }
    else if (strcmp(node->binary_op.operator, "/") == 0)
    {
        if (right == 0)
        {
            fprintf(stderr, "Error: Division by zero.\n");
            exit(1);
        }

        return left / right;
    }

    fprintf(stderr, "Error: Unsupported operator `%s`.\n", node->binary_op.operator);
    exit(1);
}

void interpret_print(ASTNode *node, Environment *env)
{
    for (size_t i = 0; i < node->to_print.arg_count; i++)
    {
        ASTNode *arg = node->to_print.arguments[i];

        if (arg->type == AST_LITERAL && arg->literal.type == LITERAL_STRING)
        {
            // Handle string literals directly
            printf("%s ", arg->literal.value.string);
        }
        else
        {
            // Evaluate and print numeric expressions
            double value = interpret(arg, env);
            printf("%f ", value);
        }
    }
    printf("\n");
}

void interpret_conditional(ASTNode *node, Environment *env)
{
    double condition_value = interpret(node->conditional.condition, env);

    if (condition_value)
    {
        interpret(node->conditional.body, env);
    }
    else if (node->conditional.else_branch)
    {
        interpret(node->conditional.else_branch, env);
    }
}
