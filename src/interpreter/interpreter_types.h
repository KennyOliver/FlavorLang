#ifndef INTERPRETER_TYPES_H
#define INTERPRETER_TYPES_H

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
        char *string;
        FLOAT_SIZE floating_point;
        INT_SIZE integer;
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
    bool is_builtin;
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
    bool did_break;
} InterpretResult;

#endif
