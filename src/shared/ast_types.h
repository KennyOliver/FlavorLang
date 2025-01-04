#ifndef AST_TYPES_H
#define AST_TYPES_H

#include "data_types.h"
#include <stdbool.h>
#include <stdlib.h>

#define MAX_ARGUMENTS 1024

// AST Node Types
typedef enum {
    AST_ASSIGNMENT,
    AST_FUNCTION_DECLARATION,
    AST_FUNCTION_CALL,
    AST_FUNCTION_RETURN,
    AST_LITERAL,
    AST_CONDITIONAL,
    AST_UNARY_OP,
    AST_BINARY_OP,
    AST_WHILE_LOOP,
    AST_FOR_LOOP,
    AST_SWITCH,
    AST_BREAK,
    AST_TERNARY,
    AST_VARIABLE,
    AST_CONSTANT,
    AST_TRY,
    AST_RESCUE,
    AST_FINISH
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
        FLOAT_SIZE floating_point;
        INT_SIZE integer;
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

// AST Unary Operation Node
typedef struct {
    char *operator;
    struct ASTNode *operand;
} ASTUnaryOp;

// AST Binary Operation Node
typedef struct {
    struct ASTNode *left;
    struct ASTNode *right;
    char *operator;
} ASTBinaryOp;

// AST While Loop Node
typedef struct {
    struct ASTNode *condition;
    int re_evaluate_condition;
    struct ASTNode *body;
} ASTWhileLoop;

// AST For Loop Node
typedef struct {
    char *loop_variable;
    struct ASTNode *start_expr;
    struct ASTNode *end_expr;
    bool inclusive;
    struct ASTNode *step_expr;
    struct ASTNode *body;
} ASTForLoop;

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

// AST Ternary
typedef struct {
    struct ASTNode *condition;
    struct ASTNode *true_expr;
    struct ASTNode *false_expr;
} ASTTernary;

// AST Rescue Node (optional error object)
typedef struct ASTCatchNode {
    char *error_variable; // Optional: Variable name to hold the error object
    struct ASTNode *body; // Body of the rescue block
    struct ASTCatchNode *next; // For multiple rescue clauses (future feature)
} ASTCatchNode;

// AST Try Node
typedef struct {
    struct ASTNode *try_block;     // Code within the try block
    ASTCatchNode *catch_blocks;    // Linked list of rescue blocks
    struct ASTNode *finally_block; // Optional finish block
} ASTTry;

// AST Node Structure
typedef struct ASTNode {
    ASTNodeType type;
    union {
        // Assignment
        struct {
            char *variable_name;
            struct ASTNode *value;
        } assignment;

        LiteralNode literal;           // Literal
        ASTUnaryOp unary_op;           // Unary operation
        ASTBinaryOp binary_op;         // Binary operation
        ASTConditional conditional;    // Conditional
        ASTSwitch switch_case;         // Switch
        ASTWhileLoop while_loop;       // While loop
        ASTForLoop for_loop;           // For loop
        ASTFunctionCall function_call; // Function
        ASTTernary ternary;            // Ternary

        ASTTry try_block; // Try Block
        // ASTCatchNode *catch_block;     // Rescue Block
        // struct ASTNode *finally_block; // Finish Block

        // Variable
        char *variable_name;
    };

    struct ASTNode *next;
} ASTNode;

#endif
