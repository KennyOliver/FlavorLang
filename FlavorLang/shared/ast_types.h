#ifndef AST_TYPES_H
#define AST_TYPES_H

#include <stdbool.h>
#include <stdlib.h>

#define MAX_ARGUMENTS 1024

// AST Node Types
typedef enum {
    AST_ASSIGNMENT,
    AST_FUNCTION_DECLARATION,
    AST_FUNCTION_CALL,
    AST_FUNCTION_RETURN,
    AST_PRINT,
    AST_INPUT,
    AST_LITERAL,
    AST_CONDITIONAL,
    AST_BINARY_OP,
    AST_LOOP,
    AST_SWITCH,
    AST_BREAK,
    AST_VARIABLE,
    AST_ERROR
} ASTNodeType;

// Literal Node
typedef struct {
    enum {
        LITERAL_STRING,
        LITERAL_FLOAT,
        LITERAL_INTEGER,
        LITERAL_BOOLEAN
    } type;
    union {
        char *string;
        double floating_point;
        int integer;
        bool boolean;
    } value;
} LiteralNode;

// AST Case Node for `is` and `else` blocks in a switch-case
typedef struct ASTCaseNode {
    struct ASTNode *condition;
    struct ASTNode *body;
    struct ASTCaseNode *next;
} ASTCaseNode;

// AST Switch Node
typedef struct {
    struct ASTNode *expression; // The expression being checked
    ASTCaseNode *cases;         // Linked list of cases
} ASTSwitch;

// AST Conditional Node
typedef struct {
    struct ASTNode *condition;
    struct ASTNode *body;
    struct ASTNode *else_branch;
} ASTConditional;

// AST Binary Operation Node
typedef struct {
    struct ASTNode *left;
    struct ASTNode *right;
    char *operator;
} ASTBinaryOp;

// AST Loop Node
typedef struct {
    struct ASTNode *condition;
    int re_evaluate_condition;
    struct ASTNode *body;
} ASTLoop;

// AST Function Parameter
typedef struct ASTFunctionParameter {
    char *parameter_name; // Parameter name
    // LiteralNode *type;              // Optional: parameter type (e.g.,
    // int, string, etc.)
    struct ASTFunctionParameter *next; // Linked list for multiple parameters
} ASTFunctionParameter;

// AST Function Call
typedef struct {
    char *name;
    ASTFunctionParameter
        *parameters;           // For function declarations, parameter names
    struct ASTNode *arguments; // For function calls, argument values
    struct ASTNode *body;
    struct ASTNode *return_data;
} ASTFunctionCall;

// AST Node Structure
typedef struct ASTNode {
    ASTNodeType type;
    union {
        // Assignment
        struct {
            char *variable_name;
            struct ASTNode *value;
        } assignment;

        // Print statement
        struct {
            struct ASTNode **arguments;
            size_t arg_count;
        } to_print;

        // Literal
        LiteralNode literal;

        // Binary operation
        ASTBinaryOp binary_op;

        // Conditional
        ASTConditional conditional;

        // Switch
        ASTSwitch switch_case;

        // While loop
        ASTLoop loop;

        // Function
        ASTFunctionCall function_call;

        // Variable
        char *variable_name;
    };

    struct ASTNode *next; // Link to the next statement or node
} ASTNode;

#endif
