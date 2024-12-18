#ifndef PARSER_H
#define PARSER_H

#include "common.h"

#define MAX_ARGUMENTS 1024

// AST Node Types
typedef enum
{
    AST_ASSIGNMENT,
    AST_FUNCTION_CALL,
    AST_PRINT,
    AST_LITERAL
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

        // Variable: identifier
        char *variable_name;
    };

    struct ASTNode *next; // linked list for multiple statements
} ASTNode;

// Parser functions
ASTNode *parse(Token *tokens);
void free_ast(ASTNode *node);

#endif
