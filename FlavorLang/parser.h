#ifndef PARSER_H
#define PARSER_H

#include "common.h"

#define MAX_ARGUMENTS 1024

typedef struct ASTNode ASTNode;

// AST Node Types
typedef enum
{
    AST_ASSIGNMENT,
    AST_FUNCTION_CALL,
    AST_PRINT,
    AST_LITERAL,
    AST_CONDITIONAL,
    AST_BINARY_OP
} ASTNodeType;

typedef struct
{
    enum
    {
        LITERAL_STRING,
        LITERAL_NUMBER
    } type;
    union
    {
        char *string;
        double number;
    } value;
} LiteralNode;

typedef struct
{
    ASTNode *condition;   // the condition (e.g., oven_temperature > 180)
    ASTNode *body;        // the body of the `if` block
    ASTNode *else_branch; // optional: points to next `elif` or `else` block (otherwise NULL)
} ASTConditional;

typedef struct
{
    ASTNode *left;
    ASTNode *right;
    char *operator; // operator (e.g., '<', '>', etc)
} ASTBinaryOp;

// AST Node Structure
typedef struct ASTNode
{
    ASTNodeType type;
    union
    {
        // Assignment
        struct
        {
            char *variable_name;   // variable being assigned
            struct ASTNode *value; // value being assigned (literal or variable)
        } assignment;

        // Print statement: list of arguments
        struct
        {
            struct ASTNode **arguments; // array of arguments to print
            size_t arg_count;
        } to_print;

        // Literal: a string or number
        LiteralNode literal;

        // Binary operation
        ASTBinaryOp binary_op;

        // Conditional: `if`, `elif`, or `else`
        ASTConditional conditional;

        // Variable: identifier
        char *variable_name;
    };

    struct ASTNode *next; // linked list for multiple statements
} ASTNode;

// Parser functions
ASTNode *parse(Token *tokens);
void free_ast(ASTNode *node);

#endif
