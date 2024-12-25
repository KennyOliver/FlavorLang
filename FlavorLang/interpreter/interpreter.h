#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <stdlib.h>
#include "../parser/ast_types.h"

typedef enum
{
    TYPE_NUMBER,
    TYPE_STRING,
    TYPE_ERROR
} LiteralType;

typedef enum
{
    RETURN_NORMAL,
    RETURN_ERROR
} ReturnType;

typedef struct
{
    LiteralType type;
    union
    {
        double number;
        char *string;
    } data;
} LiteralValue;

typedef struct
{
    ReturnType type;
    LiteralValue value;
} FunctionReturn;

typedef struct
{
    char *variable_name;
    LiteralValue value;
} Variable;

typedef struct
{
    char *name;
    ASTFunctionParameter *parameters; // Linked list of parameters
    ASTNode *body;                    // Function body
    FunctionReturn return_value;
} Function;

typedef struct
{
    Variable *variables;
    size_t variable_count;
    size_t capacity; // to handle dynamic resizing

    Function *functions; // Array of functions
    size_t function_count;
    size_t function_capacity;
} Environment;

// Initialize the environment
void init_environment(Environment *env);

// Free the environment
void free_environment(Environment *env);

// Interpret program
void interpret_program(ASTNode *program, Environment *env);

#endif
