#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <stdlib.h>
#include "../parser/ast_types.h"

typedef enum
{
    TYPE_NUMBER,
    TYPE_STRING
} LiteralType;

typedef union
{
    double number;
    char *string;
} LiteralValue;

typedef struct
{
    char *variable_name;
    LiteralType type;
    LiteralValue value;
} Variable;

typedef struct
{
    Variable *variables;
    size_t variable_count;
    size_t capacity; // to handle dynamic resizing
} Environment;

// Initialize the environment
void init_environment(Environment *env);

// Free the environment
void free_environment(Environment *env);

// Interpret program
void interpret_program(ASTNode *program, Environment *env);

#endif
