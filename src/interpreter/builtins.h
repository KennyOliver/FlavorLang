#ifndef BUILTINS_H
#define BUILTINS_H

#include "../debug/debug.h"
#include "../shared/ast_types.h"
#include "interpreter_types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef enum {
    ARG_TYPE_INTEGER,
    ARG_TYPE_FLOAT,
    ARG_TYPE_STRING,
    ARG_TYPE_BOOLEAN,
} ArgType;
typedef struct {
    ArgType type;  // expected argument type
    void *out_ptr; // pointer to store interpreted value
} ArgumentSpec;

// Built-in functions for the standard library
LiteralValue builtin_input(ASTNode *node, Environment *env);
LiteralValue builtin_random(ASTNode *node, Environment *env);
LiteralValue builtin_output(ASTNode *node, Environment *env);
LiteralValue builtin_error(ASTNode *node, Environment *env);
LiteralValue builtin_cast(ASTNode *node, Environment *env);
LiteralValue builtin_time();
LiteralValue builtin_file_read(ASTNode *node, Environment *env);
LiteralValue builtin_file_write(ASTNode *node, Environment *env);
LiteralValue builtin_file_append(ASTNode *node, Environment *env);

// Helpers
bool interpret_arguments(ASTNode *node, Environment *env, size_t num_args,
                         ArgumentSpec *specs);
void print_formatted_string(const char *str);
bool is_valid_int(const char *str, INT_SIZE *out_value);
bool is_valid_float(const char *str, FLOAT_SIZE *out_value);
char *process_escape_sequences(const char *input);

#endif
