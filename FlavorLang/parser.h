#ifndef PARSER_H
#define PARSER_H

#include "common.h"

// AST Node Types
typedef enum
{
    AST_ASSIGNMENT,
    AST_FUNCTION_CALL,
    AST_PRINT,
    AST_STRING_LITERAL
} ASTNodeType;

// AST Node Structure
typedef struct ASTNode
{
    ASTNodeType type;
    char *value;          // for literals, variable names, or function names
    struct ASTNode *next; // linked list for multiple nodes
} ASTNode;

// Parser functions
ASTNode *parse(Token *tokens);
void free_ast(ASTNode *node);

#endif
