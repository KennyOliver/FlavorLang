#ifndef INTERPRETER_TYPES_H
#define INTERPRETER_TYPES_H

// Forward declarations
struct ASTFunctionParameter;
struct ASTNode;
struct Function;

// Forward declaration and type alias for Environment
typedef struct Environment Environment;

typedef enum {
    TYPE_BOOLEAN,
    TYPE_FLOAT,
    TYPE_INTEGER,
    TYPE_STRING,
    TYPE_FUNCTION,
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
        struct Function *function_ptr;
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
    bool is_constant;
} Variable;

typedef struct Function {
    char *name;
    ASTFunctionParameter *parameters; // Linked list of parameters
    ASTNode *body;                    // Function body
    FunctionResult return_value;
    bool is_builtin;
} Function;

struct Environment {
    Variable *variables;
    size_t variable_count;
    size_t capacity; // to handle dynamic resizing

    Function *functions; // Array of functions
    size_t function_count;
    size_t function_capacity;

    Environment *parent; // Parent environment
};

typedef struct {
    LiteralValue value; // Result of interpreting a node
    bool did_return; // True if this node caused a function return to bubble up
    bool did_break;  // True if this node caused a loop break to bubble up
    bool is_error;   // True if this node caused an error
} InterpretResult;

#endif
