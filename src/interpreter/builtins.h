#ifndef BUILTINS_H
#define BUILTINS_H

#include "../debug/debug.h"
#include "../shared/ast_types.h"
#include "interpreter_types.h"
#include "utils.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef enum {
    ARG_TYPE_INTEGER,
    ARG_TYPE_FLOAT,
    ARG_TYPE_STRING,
    ARG_TYPE_BOOLEAN,
    ARG_TYPE_ARRAY,
    ARG_TYPE_NUMERIC // Either float or int
} ArgType;
typedef struct {
    size_t num_types; // number of acceptable types
    ArgType type;     // expected argument type
    void *out_ptr;    // pointer to store interpreted value
} ArgumentSpec;

// Built-in functions for the standard library
InterpretResult builtin_input(ASTNode *node, Environment *env);
InterpretResult builtin_random(ASTNode *node, Environment *env);
InterpretResult builtin_output(ASTNode *node, Environment *env);
InterpretResult builtin_error(ASTNode *node, Environment *env);
InterpretResult builtin_cast(ASTNode *node, Environment *env);
InterpretResult builtin_time(void);
InterpretResult builtin_file_read(ASTNode *node, Environment *env);
InterpretResult builtin_file_write(ASTNode *node, Environment *env);
InterpretResult builtin_file_append(ASTNode *node, Environment *env);
InterpretResult builtin_length(ASTNode *node, Environment *env);
InterpretResult builtin_sleep(ASTNode *node, Environment *env);
InterpretResult builtin_cimport(ASTNode *node, Environment *env);
InterpretResult builtin_floor(ASTNode *node, Environment *env);
InterpretResult builtin_ceil(ASTNode *node, Environment *env);
InterpretResult builtin_round(ASTNode *node, Environment *env);
InterpretResult builtin_abs(ASTNode *node, Environment *env);

// Helpers
bool literal_type_matches_arg_type(LiteralType lit_type, ArgType arg_type);
InterpretResult interpret_arguments(ASTNode *node, Environment *env,
                                    size_t num_args, ArgumentSpec *specs);
void print_formatted_string(const char *str);
bool is_valid_int(const char *str, INT_SIZE *out_value);
bool is_valid_float(const char *str, FLOAT_SIZE *out_value);
char *process_escape_sequences(const char *input);
void print_literal_value(LiteralValue lv);

#endif
