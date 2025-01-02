#ifndef INTERPRETER_UTILS_H
#define INTERPRETER_UTILS_H

#include "../debug/debug.h"
#include "../shared/ast_types.h"
#include "interpreter_types.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

// Initialize the environment
void init_environment(Environment *env);

// Free the environment
void free_environment(Environment *env);

void error_interpreter(const char *format, ...);

void free_parameter_list(ASTFunctionParameter *head);
ASTFunctionParameter *
copy_function_parameters(ASTFunctionParameter *param_list);
ASTNode *copy_ast_node(ASTNode *node);
void add_function(Environment *env, Function func);
Function *get_function(Environment *env, const char *name);

#endif
