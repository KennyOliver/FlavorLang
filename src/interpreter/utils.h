#ifndef INTERPRETER_UTILS_H
#define INTERPRETER_UTILS_H

#include "../debug/debug.h"
#include "../shared/ast_types.h"
#include "interpreter.h"
#include "interpreter_types.h"
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

// Initialize the environment without a parent (global)
void init_environment(Environment *env);

// Initialize the environment with a parent (local)
void init_environment_with_parent(Environment *env, Environment *parent);

// Free the environment
void free_environment(Environment *env);

// Errors
InterpretResult raise_error(const char *format, ...);
void fatal_error(const char *format, ...);

// Functions
void free_parameter_list(ASTFunctionParameter *head);
ASTFunctionParameter *
copy_function_parameters(ASTFunctionParameter *param_list);
ASTNode *copy_ast_node(ASTNode *node);
void add_function(Environment *env, Function func);
Function *get_function(Environment *env, const char *name);

// Helpers
InterpretResult make_result(LiteralValue val, bool did_return, bool did_break);
ASTCatchNode *copy_catch_node(ASTCatchNode *catch_node);
ASTCaseNode *copy_ast_case_node(ASTCaseNode *case_node);
char *safe_strdup(const char *str);

// Type Helpers
bool is_numeric_type(LiteralType type);
bool is_boolean_type(LiteralType type);
const char *literal_type_to_string(LiteralType type);

// Casting Helpers
bool is_valid_int(const char *str, INT_SIZE *out);
bool is_valid_float(const char *str, FLOAT_SIZE *out);

#endif
