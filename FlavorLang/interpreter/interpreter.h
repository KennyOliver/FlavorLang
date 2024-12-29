#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "../shared/ast_types.h"
#include <stdbool.h>
#include <stdlib.h>

typedef enum {
    TYPE_BOOLEAN,
    TYPE_FLOAT,
    TYPE_INTEGER,
    TYPE_STRING,
    TYPE_ERROR
} LiteralType;

typedef enum { RETURN_NORMAL, RETURN_ERROR } ReturnType;

typedef struct {
    LiteralType type;
    union {
        double floating_point;
        int integer;
        char *string;
        bool boolean;
    } data;
} LiteralValue;

typedef struct {
    LiteralValue value;
    bool should_return; // Flag to indicate if function should return early
    bool is_error;      // Flag to indicate if this is an error return (burn)
} FunctionResult;

typedef struct {
    char *variable_name;
    LiteralValue value;
} Variable;

typedef struct {
    char *name;
    ASTFunctionParameter *parameters; // Linked list of parameters
    ASTNode *body;                    // Function body
    FunctionResult return_value;
} Function;

typedef struct {
    Variable *variables;
    size_t variable_count;
    size_t capacity; // to handle dynamic resizing

    Function *functions; // Array of functions
    size_t function_count;
    size_t function_capacity;
} Environment;

typedef struct {
    LiteralValue value; // The result of interpreting a node
    bool did_return; // True if this node caused a function return to bubble up
} InterpretResult;

InterpretResult interpret_node(ASTNode *node, Environment *env);
LiteralValue interpret_literal(ASTNode *node);
LiteralValue interpret_variable(ASTNode *node, Environment *env);
LiteralValue interpret_assignment(ASTNode *node, Environment *env);
LiteralValue interpret_binary_op(ASTNode *node, Environment *env);
Variable *get_variable(Environment *env, const char *variable_name);
void add_variable(Environment *env, Variable var);
void interpret_print(ASTNode *node, Environment *env);
void interpret_raise_error(ASTNode *node, Environment *env);
Variable interpret_input(Environment *env);
InterpretResult interpret_conditional(ASTNode *node, Environment *env);
void interpret_while_loop(ASTNode *node, Environment *env);
LiteralValue interpret_for_loop(ASTNode *node, Environment *env);
void interpret_switch(ASTNode *node, Environment *env);
void interpret_function_declaration(ASTNode *node, Environment *env);
LiteralValue interpret_function_call(ASTNode *node, Environment *env);
ASTNode *copy_ast_node(ASTNode *node);

// Initialize the environment
void init_environment(Environment *env);

// Free the environment
void free_environment(Environment *env);

// Interpret program
void interpret_program(ASTNode *program, Environment *env);

#endif
