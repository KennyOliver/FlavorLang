#ifndef AST_TYPES_H
#define AST_TYPES_H

#include "data_types.h"
#include <stdbool.h>
#include <stdlib.h>

// AST Node Types
typedef enum {
    AST_VAR_DECLARATION,
    AST_CONST_DECLARATION,
    AST_ASSIGNMENT,
    AST_LITERAL,
    AST_FUNCTION_DECLARATION,
    AST_FUNCTION_CALL,
    AST_FUNCTION_RETURN,
    AST_CONDITIONAL,
    AST_UNARY_OP,
    AST_BINARY_OP,
    AST_WHILE_LOOP,
    AST_FOR_LOOP,
    AST_SWITCH,
    AST_BREAK,
    AST_TERNARY,
    AST_TRY,
    AST_CATCH,
    AST_FINALLY,
    AST_ARRAY_LITERAL,
    AST_ARRAY_OPERATION,
    AST_ARRAY_INDEX_ACCESS,
    AST_ARRAY_SLICE_ACCESS,
    AST_VARIABLE_REFERENCE,
    AST_IMPORT,
    AST_EXPORT
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

    // Range-based
    struct ASTNode *start_expr;
    struct ASTNode *end_expr;
    bool inclusive;
    struct ASTNode *step_expr;

    // Iterables
    bool is_iterable_loop;           // true if `for X in Y`
    struct ASTNode *collection_expr; // e.g. `recipes` or any expression

    struct ASTNode *body;
} ASTForLoop;

// AST Function Parameter
typedef struct ASTFunctionParameter {
    char *parameter_name;              // Parameter name
    struct ASTFunctionParameter *next; // Linked list for multiple parameters
} ASTFunctionParameter;

// AST Function Declaration Node
typedef struct {
    char *name;
    ASTFunctionParameter *parameters; // Function parameters
    struct ASTNode *body;             // Function body
} ASTFunctionDeclaration;

// AST Function Call Node
typedef struct {
    struct ASTNode *function_ref; // Expression representing the function
    struct ASTNode *arguments;    // Function call arguments
} ASTFunctionCall;

// AST Function Return Node
typedef struct {
    struct ASTNode *return_data; // Expression to return
} ASTFunctionReturn;

// AST Ternary
typedef struct {
    struct ASTNode *condition;
    struct ASTNode *true_expr;
    struct ASTNode *false_expr;
} ASTTernary;

// AST Catch Node
typedef struct ASTCatchNode {
    char *error_variable; // Optional: Variable name to hold the error object
    struct ASTNode *body; // Body of the rescue block
    struct ASTCatchNode *next; // For multiple `rescue` clauses
} ASTCatchNode;

// AST Try Node
typedef struct {
    struct ASTNode *try_block;     // Code within the try block
    ASTCatchNode *catch_blocks;    // Linked list of rescue blocks
    struct ASTNode *finally_block; // Optional `finish` block
} ASTTry;

// AST Array Literal Node
typedef struct {
    struct ASTNode **elements; // Array of element expressions
    size_t count;              // Number of elements
} ASTArrayLiteral;

// AST Index Access Node
typedef struct {
    struct ASTNode *array; // The array expression
    struct ASTNode *index; // The index expression
} ASTArrayIndexAccess;

// AST Slice Access Node
typedef struct {
    struct ASTNode *array; // The array expression
    struct ASTNode *start; // Start index (can be NULL)
    struct ASTNode *end;   // End index (can be NULL)
    struct ASTNode *step;  // Step value (can be NULL)
} ASTArraySliceAccess;

// AST Array Operation Node
typedef struct {
    char *operator; // Array operation operator (e.g., `^+`, `+^`, `^-`, `-^`)
    struct ASTNode *array;   // Array on which the operation is performed
    struct ASTNode *operand; // Element to operate with (e.g., to append)
} ASTArrayOperation;

typedef struct {
    char *import_path; // Module script file path string
} ASTImport;
typedef struct {
    struct ASTNode *decl; // Declaration node that's being exported
} ASTExport;

// AST Node Structure
typedef struct ASTNode {
    ASTNodeType type;
    union {
        // Variable Declaration
        struct {
            char *variable_name;
            struct ASTNode *initializer;
        } var_declaration;

        // Constant Declaration
        struct {
            char *constant_name;
            struct ASTNode *initializer;
        } const_declaration;

        // Assignment
        struct {
            struct ASTNode *lhs;
            struct ASTNode *rhs;
        } assignment;

        // Function Nodes
        ASTFunctionDeclaration function_declaration;
        ASTFunctionCall function_call;
        ASTFunctionReturn function_return;

        // Control Flow Nodes
        ASTConditional conditional;
        ASTSwitch switch_case;
        ASTWhileLoop while_loop;
        ASTForLoop for_loop;
        ASTTernary ternary;
        ASTTry try_block;

        // Operations
        ASTUnaryOp unary_op;
        ASTBinaryOp binary_op;

        // Array Nodes
        ASTArrayLiteral array_literal;
        ASTArrayOperation array_operation;
        ASTArrayIndexAccess array_index_access;
        ASTArraySliceAccess array_slice_access;

        // Literal and Reference
        LiteralNode literal;
        char *variable_name; // For AST_VARIABLE_REFERENCE

        // Import & Export
        ASTImport import;
        ASTExport export;
    };

    struct ASTNode *next;
} ASTNode;

#endif
