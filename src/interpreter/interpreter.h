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
InterpretResult interpret_variable_reference(ASTNode *node, Environment *env);
InterpretResult interpret_var_declaration(ASTNode *node, Environment *env);
InterpretResult interpret_const_declaration(ASTNode *node, Environment *env);
InterpretResult interpret_assignment(ASTNode *node, Environment *env);
InterpretResult interpret_binary_op(ASTNode *node, Environment *env);
InterpretResult interpret_conditional(ASTNode *node, Environment *env);
InterpretResult interpret_while_loop(ASTNode *node, Environment *env);
InterpretResult interpret_for_loop(ASTNode *node, Environment *env);
InterpretResult interpret_switch(ASTNode *node, Environment *env);
InterpretResult interpret_function_declaration(ASTNode *node, Environment *env);
InterpretResult interpret_function_call(ASTNode *node, Environment *env);
InterpretResult interpret_unary_op(ASTNode *node, Environment *env);
InterpretResult evaluate_unary_operator(const char *op,
                                        InterpretResult operand_res);
InterpretResult interpret_ternary(ASTNode *node, Environment *env);
InterpretResult call_user_defined_function(Function *func_ref,
                                           ASTNode *call_node,
                                           Environment *env);
InterpretResult interpret_try(ASTNode *node, Environment *env);

// Interpret program
void interpret_program(ASTNode *program, Environment *env);

// Helpers
LiteralValue create_default_value(void);
Variable *get_variable(Environment *env, const char *variable_name);
InterpretResult add_variable(Environment *env, Variable var);
ASTFunctionParameter *copy_function_parameters(ASTFunctionParameter *params);

#endif
