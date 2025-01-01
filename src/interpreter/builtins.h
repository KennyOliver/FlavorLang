#ifndef BUILTINS_H
#define BUILTINS_H

#include "../debug/debug.h"
#include "../shared/ast_types.h"
#include "interpreter_types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

LiteralValue builtin_input(ASTNode *node, Environment *env);
LiteralValue builtin_random(ASTNode *node, Environment *env);
LiteralValue builtin_output(ASTNode *node, Environment *env);
LiteralValue builtin_error(ASTNode *node, Environment *env);
LiteralValue builtin_cast(ASTNode *node, Environment *env);
LiteralValue builtin_time();

// Helpers
bool interpret_arguments(ASTNode *node, Environment *env, size_t max_args, ...);
void print_formatted_string(const char *str);
bool is_valid_int(const char *str, INT_SIZE *out_value);
bool is_valid_float(const char *str, FLOAT_SIZE *out_value);

#endif
