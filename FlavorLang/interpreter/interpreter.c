#include "interpreter.h"
#include "../parser/ast_types.h"
#include <stdio.h>

void interpret_program(ASTNode *program, Environment *env);
double interpret(ASTNode *node, Environment *env);
double interpret_literal(ASTNode *node);
double interpret_assignment(ASTNode *node, Environment *env);
double interpret_binary_op(ASTNode *node, Environment *env);
double interpret_print(ASTNode *node, Environment *env);
double interpret_conditional(ASTNode *node, Environment *env);

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

double interpret_print(ASTNode *node, Environment *env)
{
    for (size_t i = 0; i < node->to_print.arg_count; i++)
    {
        double value = interpret(node->to_print.arguments[i], env);
        printf("%f ", value);
    }
    printf("\n");
}
