#ifndef AST_TYPES_H
#define AST_TYPES_H

#include <stdlib.h>

#define MAX_ARGUMENTS 1024

// AST Node Types
typedef enum
{
    AST_ASSIGNMENT,
    AST_FUNCTION_CALL,
    AST_PRINT,
    AST_INPUT,
    AST_LITERAL,
    AST_CONDITIONAL,
    AST_BINARY_OP,
    AST_LOOP,
    AST_VARIABLE
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
    struct ASTNode *condition;   // the condition (e.g., oven_temperature > 180)
    struct ASTNode *body;        // the body of the `if` block
    struct ASTNode *else_branch; // optional: points to next `elif` or `else` block
} ASTConditional;

typedef struct
{
    struct ASTNode *left;
    struct ASTNode *right;
    char *operator;
} ASTBinaryOp;

typedef struct
{
    struct ASTNode *condition;
    int re_evaluate_condition;
    struct ASTNode *body;
} ASTLoop;

// AST Node Structure
typedef struct ASTNode
{
    ASTNodeType type;
    union
    {
        // Assignment
        struct
        {
            char *variable_name;
            struct ASTNode *value;
        } assignment;
        // Print statement
        struct
        {
            struct ASTNode **arguments;
            size_t arg_count;
        } to_print;

        // Literal
        LiteralNode literal;

        // Binary operation
        ASTBinaryOp binary_op;

        // Conditional
        ASTConditional conditional;

        // While loop
        union
        {
            ASTLoop loop;
        };

        // Variable
        char *variable_name;
    };
    struct ASTNode *next;
} ASTNode;

#endif
