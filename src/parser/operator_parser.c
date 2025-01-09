#include "operator_parser.h"
#include "../debug/debug.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "../shared/ast_types.h"
#include "array_parser.h"
#include "parser_state.h"

// Implementation of the main expression parser
ASTNode *parse_operator_expression(ParserState *state) {
    return parse_ternary(state);
}

// Ternary Operations: <expression> ? <value> : <value>
ASTNode *parse_ternary(ParserState *state) {
    // First, parse "condition" using the next higher-precedence function
    ASTNode *condition = parse_logical(state);

    Token *current = get_current_token(state);
    if (current->type == TOKEN_OPERATOR && strcmp(current->lexeme, "?") == 0) {
        advance_token(state); // consume `?`

        // Recursively parse expression for `True` branch (allows for nesting)
        ASTNode *true_expr = parse_ternary(state);

        expect_token(state, TOKEN_COLON, "Expected `:` in ternary expression");

        // Recursively parse expression for `False` branch
        ASTNode *false_expr = parse_ternary(state);

        ASTNode *ternary_node = malloc(sizeof(ASTNode));
        if (!ternary_node) {
            parser_error("Memory allocation failed for ternary node", current);
        }
        ternary_node->type = AST_TERNARY;
        ternary_node->ternary.condition = condition;
        ternary_node->ternary.true_expr = true_expr;
        ternary_node->ternary.false_expr = false_expr;
        ternary_node->next = NULL;

        return ternary_node;
    }

    // If there’s no `?`, just return condition `ASTNode` (no ternary)
    return condition;
}

// Logical Operators: &&, ||
ASTNode *parse_logical(ParserState *state) {
    ASTNode *node = parse_equality(state);

    while (match_operator(state, "&&") || match_operator(state, "||")) {
        char *operator= strdup(state->previous->lexeme);
        if (!operator) {
            parser_error("Memory allocation failed for operator",
                         state->current);
        }
        ASTNode *right = parse_equality(state);
        node = create_binary_op_node(operator, node, right);
    }

    return node;
}

// Equality Operators: ==, !=
ASTNode *parse_equality(ParserState *state) {
    ASTNode *node = parse_comparison(state);

    while (match_operator(state, "==") || match_operator(state, "!=")) {
        char *operator= strdup(state->previous->lexeme);
        if (!operator) {
            parser_error("Memory allocation failed for operator",
                         state->current);
        }
        ASTNode *right = parse_comparison(state);
        node = create_binary_op_node(operator, node, right);
    }

    return node;
}

// Comparison Operators: <, >, <=, >=
ASTNode *parse_comparison(ParserState *state) {
    ASTNode *node = parse_term(state);

    while (match_operator(state, "<") || match_operator(state, ">") ||
           match_operator(state, "<=") || match_operator(state, ">=")) {
        char *operator= strdup(state->previous->lexeme);
        if (!operator) {
            parser_error("Memory allocation failed for operator",
                         state->current);
        }
        ASTNode *right = parse_term(state);
        node = create_binary_op_node(operator, node, right);
    }

    return node;
}

// Additive Operators: +, -
ASTNode *parse_term(ParserState *state) {
    ASTNode *node = parse_factor(state);

    while (match_operator(state, "+") || match_operator(state, "-")) {
        char *operator= strdup(state->previous->lexeme);
        if (!operator) {
            parser_error("Memory allocation failed for operator",
                         state->current);
        }
        ASTNode *right = parse_factor(state);
        node = create_binary_op_node(operator, node, right);
    }

    return node;
}

// Multiplicative Operators: *, /, //, %
ASTNode *parse_factor(ParserState *state) {
    ASTNode *node = parse_power(state);

    while (match_operator(state, "*") || match_operator(state, "/") ||
           match_operator(state, "//") || match_operator(state, "%")) {
        char *operator= strdup(state->previous->lexeme);
        if (!operator) {
            parser_error("Memory allocation failed for operator",
                         state->current);
        }
        ASTNode *right = parse_power(state);
        node = create_binary_op_node(operator, node, right);
    }

    return node;
}

// Exponentiation Operator: **
ASTNode *parse_power(ParserState *state) {
    ASTNode *node = parse_unary(state);

    if (match_operator(state, "**")) {
        char *operator= strdup(state->previous->lexeme);
        if (!operator) {
            parser_error("Memory allocation failed for operator",
                         state->current);
        }
        ASTNode *right = parse_power(state); // Right-associative
        node = create_binary_op_node(operator, node, right);
    }

    return node;
}

// Unary Operators: -, +, !
ASTNode *parse_unary(ParserState *state) {
    if (match_operator(state, "-") || match_operator(state, "+") ||
        match_operator(state, "!")) {
        char *operator= strdup(state->previous->lexeme);
        if (!operator) {
            parser_error("Memory allocation failed for operator",
                         state->current);
        }
        ASTNode *operand = parse_unary(state);
        return create_unary_op_node(operator, operand);
    }

    return parse_primary(state);
}

// Primary Expressions: literals, variables, function calls, parentheses, arrays
ASTNode *parse_primary(ParserState *state) {
    // Identify the "base" expression
    ASTNode *node = NULL;
    Token *current = get_current_token(state);

    if (current->type == TOKEN_INTEGER || current->type == TOKEN_FLOAT ||
        current->type == TOKEN_STRING || current->type == TOKEN_BOOLEAN) {
        node = create_literal_node(current);
        advance_token(state);
    } else if (current->type == TOKEN_FUNCTION_NAME) {
        // Parse function call
        char *func_name = strdup(current->lexeme);
        if (!func_name) {
            parser_error("Memory allocation failed for function name", current);
        }
        ASTNode *func_ref_node = create_variable_reference_node(func_name);
        advance_token(state); // consume function name

        expect_token(state, TOKEN_PAREN_OPEN,
                     "Expected `(` after function name");
        ASTNode *args = parse_argument_list(state);
        expect_token(state, TOKEN_PAREN_CLOSE,
                     "Expected `)` after function arguments");

        node = create_function_call_node(func_ref_node, args);
    } else if (current->type == TOKEN_IDENTIFIER) {
        // Check if identifier is followed by '(' indicating a function call
        Token *next = peek_next_token(state);
        if (next && next->type == TOKEN_PAREN_OPEN) {
            // It's a function call
            char *func_name = strdup(current->lexeme);
            if (!func_name) {
                parser_error("Memory allocation failed for function name",
                             current);
            }
            ASTNode *func_ref_node = create_variable_reference_node(func_name);
            advance_token(state); // consume function name

            expect_token(state, TOKEN_PAREN_OPEN,
                         "Expected `(` after function name");
            ASTNode *args = parse_argument_list(state);
            expect_token(state, TOKEN_PAREN_CLOSE,
                         "Expected `)` after function arguments");

            node = create_function_call_node(func_ref_node, args);
        } else {
            // It's a variable reference
            node = create_variable_reference_node(current->lexeme);
            advance_token(state);
        }
    } else if (current->type == TOKEN_PAREN_OPEN) {
        advance_token(state); // consume `(`
        node = parse_operator_expression(state);
        expect_token(state, TOKEN_PAREN_CLOSE, "Expected `)` after expression");
    } else if (current->type == TOKEN_SQ_BRACKET_OPEN) {
        node = parse_array_literal(state);
        node->type = AST_ARRAY_LITERAL;
    } else {
        parser_error("Expected expression", current);
    }

    // Handle chained indexing/slicing
    while (get_current_token(state)->type == TOKEN_SQ_BRACKET_OPEN) {
        node = parse_index_access(node, state);
    }

    // Handle chained function calls
    while (get_current_token(state)->type == TOKEN_PAREN_OPEN) {
        node = parse_function_call_on_expression(state, node);
    }

    return node;
}

// Parses a function call on an existing expression
ASTNode *parse_function_call_on_expression(ParserState *state,
                                           ASTNode *function_ref) {
    expect_token(state, TOKEN_PAREN_OPEN,
                 "Expected `(` after function reference");
    ASTNode *arguments = parse_argument_list(state);
    expect_token(state, TOKEN_PAREN_CLOSE, "Expected `)` after argument list");
    return create_function_call_node(function_ref, arguments);
}

ASTNode *parse_argument_list(ParserState *state) {
    // Check for empty argument list first
    if (get_current_token(state)->type == TOKEN_PAREN_CLOSE) {
        return NULL; // return NULL for empty argument list
    }

    ASTNode *head = NULL;
    ASTNode *tail = NULL;

    while (1) {
        // Parse arguments as expressions
        ASTNode *arg = parse_operator_expression(state);

        // Add argument to linked list
        if (!head) {
            head = arg;
            tail = arg;
        } else {
            tail->next = arg;
            tail = arg;
        }

        // Check for comma (indicates another argument)
        if (get_current_token(state)->type == TOKEN_DELIMITER &&
            strcmp(get_current_token(state)->lexeme, ",") == 0) {
            advance_token(state); // consume `,`
        } else {
            break;
        }
    }

    return head;
}

// Helper function to match specific operators
bool match_operator(ParserState *state, const char *op) {
    Token *current = get_current_token(state);
    if (current->type == TOKEN_OPERATOR && strcmp(current->lexeme, op) == 0) {
        state->previous =
            current; // Assuming ParserState has a 'previous' field
        advance_token(state);
        return true;
    }
    return false;
}

// Creation functions

ASTNode *create_binary_op_node(char *operator, ASTNode * left, ASTNode *right) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) {
        parser_error("Memory allocation failed for binary operation node",
                     NULL);
    }
    node->type = AST_BINARY_OP;
    node->binary_op.operator= operator;
    node->binary_op.left = left;
    node->binary_op.right = right;
    node->next = NULL;
    return node;
}

ASTNode *create_unary_op_node(char *operator, ASTNode * operand) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) {
        parser_error("Memory allocation failed for unary operation node", NULL);
    }
    node->type = AST_UNARY_OP;
    node->unary_op.operator= operator;
    node->unary_op.operand = operand;
    node->next = NULL;
    return node;
}

ASTNode *create_literal_node(Token *token) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) {
        parser_error("Memory allocation failed for literal node", token);
    }

    node->type = AST_LITERAL;
    node->literal.type = LITERAL_INTEGER;

    switch (token->type) {
    case TOKEN_INTEGER:
        node->literal.type = LITERAL_INTEGER;
        node->literal.value.integer = atoll(token->lexeme);
        break;
    case TOKEN_FLOAT:
        node->literal.type = LITERAL_FLOAT;
        node->literal.value.floating_point = atof(token->lexeme);
        break;
    case TOKEN_STRING:
        node->literal.type = LITERAL_STRING;
        node->literal.value.string = strdup(token->lexeme);
        if (!node->literal.value.string) {
            free(node);
            parser_error("Memory allocation failed for string literal", token);
        }
        break;
    case TOKEN_BOOLEAN:
        node->literal.type = LITERAL_BOOLEAN;
        node->literal.value.boolean = (strcmp(token->lexeme, "True") == 0);
        break;
    default:
        free(node);
        parser_error("Unknown literal type", token);
    }
    node->next = NULL;
    return node;
}

// Create a function call node
ASTNode *create_function_call_node(ASTNode *function_ref, ASTNode *args) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) {
        parser_error("Memory allocation failed for function call node", NULL);
    }
    node->type = AST_FUNCTION_CALL;
    node->function_call.function_ref = function_ref;
    node->function_call.arguments = args;
    node->next = NULL;
    return node;
}
