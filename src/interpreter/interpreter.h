#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "../shared/ast_types.h"
#include "../shared/data_types.h"
#include "builtins.h"
#include "interpreter_types.h"
#include "utils.h"
#include <errno.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

InterpretResult interpret_node(ASTNode *node, Environment *env);
InterpretResult interpret_literal(ASTNode *node);
InterpretResult interpret_variable(ASTNode *node, Environment *env);
InterpretResult interpret_constant(ASTNode *node, Environment *env);
InterpretResult interpret_assignment(ASTNode *node, Environment *env);
InterpretResult interpret_binary_op(ASTNode *node, Environment *env);
InterpretResult interpret_conditional(ASTNode *node, Environment *env);
InterpretResult interpret_while_loop(ASTNode *node, Environment *env);
InterpretResult interpret_for_loop(ASTNode *node, Environment *env);
InterpretResult interpret_switch(ASTNode *node, Environment *env);
InterpretResult interpret_function_declaration(ASTNode *node, Environment *env);
InterpretResult interpret_function_call(ASTNode *node, Environment *env);
InterpretResult interpret_unary_op(ASTNode *node, Environment *env);
LiteralValue evaluate_unary_operator(const char *op, LiteralValue operand);
InterpretResult interpret_ternary(ASTNode *node, Environment *env);
InterpretResult call_user_defined_function(Function *func_ref,
                                           ASTNode *call_node,
                                           Environment *env);

// Interpret program
void interpret_program(ASTNode *program, Environment *env);

// Helpers
Variable *get_variable(Environment *env, const char *variable_name);
void add_variable(Environment *env, Variable var);
ASTFunctionParameter *copy_function_parameters(ASTFunctionParameter *params);

#endif