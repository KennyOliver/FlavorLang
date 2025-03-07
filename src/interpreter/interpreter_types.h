#ifndef INTERPRETER_TYPES_H
#define INTERPRETER_TYPES_H

#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Forward declarations for AST types and Environment
struct ASTFunctionParameter;
struct ASTNode;
struct Function;
typedef struct Environment Environment;

// Enum for Literal Types
typedef enum {
    TYPE_BOOLEAN,
    TYPE_FLOAT,
    TYPE_INTEGER,
    TYPE_STRING,
    TYPE_ARRAY,
    TYPE_FUNCTION,
    TYPE_ERROR
} LiteralType;

// Enum for Return Types
typedef enum { RETURN_NORMAL, RETURN_ERROR } ReturnType;

// Structure for Array Values
typedef struct ArrayValue {
    struct LiteralValue *elements; // Dynamic array of LiteralValue
    size_t count;                  // Number of elements
    size_t capacity;               // Allocated capacity
} ArrayValue;

// Structure for Literal Values
typedef struct LiteralValue {
    LiteralType type;
    union {
        char *string;
        long double floating_point;
        long long integer;
        bool boolean;
        char *function_name;
        ArrayValue array; // By value
    } data;
} LiteralValue;

// Structure for Interpreted Results
typedef struct {
    LiteralValue value; // Using the complete type name
    bool did_return;    // True if this node caused a function return
    bool did_break;     // True if this node caused a loop break
    bool is_error;      // True if this node caused an error
} InterpretResult;

// FlavorLang cimport function pointer type
typedef InterpretResult (*FlavorLangCFunc)(struct ASTNode *args,
                                           Environment *env);

// Structure for Function Results
typedef struct {
    LiteralValue value;
    bool should_return; // Flag to indicate if function should return early
    bool is_error;      // Flag to indicate if this is an error return (burn)
} FunctionResult;

// Structure for Variables
typedef struct {
    char *variable_name;
    LiteralValue value;
    bool is_constant;
} Variable;

// Structure for Functions
typedef struct Function {
    char *name;
    struct ASTFunctionParameter *parameters; // Linked list of parameters
    struct ASTNode *body;                    // Function body
    FunctionResult return_value;
    bool is_builtin;
    FlavorLangCFunc c_function;
} Function;

// Structure for Environment
struct Environment {
    Variable *variables;
    size_t variable_count;
    size_t capacity; // To handle dynamic resizing

    Function *functions; // Array of functions
    size_t function_count;
    size_t function_capacity;

    Environment *parent; // Parent environment for nested scopes

    // For modules
    char **exported_symbols;
    size_t exported_count;
    size_t exported_capacity;

    char *script_dir; // Store directory of main script
};

#endif
