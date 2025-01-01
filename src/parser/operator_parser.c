#include "operator_parser.h"
#include "../debug/debug.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "../shared/ast_types.h"
#include "parser_state.h"

// Implementation of the main expression parser
ASTNode *parse_operator_expression(ParserState *state) {
    return parse_logical(state);
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

// Primary Expressions: literals, variables, function calls, parentheses
ASTNode *parse_primary(ParserState *state) {
    Token *current = get_current_token(state);

    if (current->type == TOKEN_INTEGER || current->type == TOKEN_FLOAT ||
        current->type == TOKEN_STRING || current->type == TOKEN_BOOLEAN) {
        ASTNode *node = create_literal_node(current);
        advance_token(state);
        return node;
    }

    if (current->type == TOKEN_FUNCTION_NAME) {
        // Parse function call
        char *func_name = strdup(current->lexeme);
        if (!func_name) {
            parser_error("Memory allocation failed for function name", current);
        }
        advance_token(state); // Consume function name

        expect_token(state, TOKEN_PAREN_OPEN,
                     "Expected '(' after function name");

        ASTNode *args = parse_argument_list(state);

        expect_token(state, TOKEN_PAREN_CLOSE,
                     "Expected ')' after function arguments");

        ASTNode *node = create_function_call_node(func_name, args);
        return node;
    }

    if (current->type == TOKEN_IDENTIFIER) {
        // Check if identifier is followed by '(' indicating a function call
        Token *next = peek_next_token(state);
        if (next && next->type == TOKEN_PAREN_OPEN) {
            // It's a function call
            char *func_name = strdup(current->lexeme);
            if (!func_name) {
                parser_error("Memory allocation failed for function name",
                             current);
            }
            advance_token(state); // Consume function name

            expect_token(state, TOKEN_PAREN_OPEN,
                         "Expected '(' after function name");

            ASTNode *args = parse_argument_list(state);

            expect_token(state, TOKEN_PAREN_CLOSE,
                         "Expected ')' after function arguments");

            ASTNode *node = create_function_call_node(func_name, args);
            return node;
        } else {
            // It's a variable
            ASTNode *node = create_variable_node(current->lexeme);
            advance_token(state);
            return node;
        }
    }

    if (current->type == TOKEN_PAREN_OPEN) {
        advance_token(state); // consume `(`
        ASTNode *node = parse_operator_expression(state);
        expect_token(state, TOKEN_PAREN_CLOSE, "Expected `)` after expression");
        return node;
    }

    parser_error("Expected expression", current);
    return NULL; // unreachable
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

    // Assign the correct literal type and value based on the token type
    switch (token->type) {
    case TOKEN_INTEGER:
        node->literal.type = LITERAL_INTEGER;
        node->literal.value.integer = atoll(token->lexeme);
        break;
    case TOKEN_FLOAT:
        node->literal.type = LITERAL_FLOAT;
        node->literal.value.floating_point = strtold(token->lexeme, NULL);
        break;
    case TOKEN_STRING:
        node->literal.type = LITERAL_STRING;
        node->literal.value.string = strdup(token->lexeme);
        break;
    case TOKEN_BOOLEAN:
        node->literal.type = LITERAL_BOOLEAN;
        node->literal.value.boolean = strcmp(token->lexeme, "True") == 0;
        break;
    default:
        parser_error("Unsupported literal type", token);
    }

    node->next = NULL;
    return node;
}

ASTNode *create_variable_node(char *name) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) {
        parser_error("Memory allocation failed for variable node", NULL);
    }
    node->type = AST_VARIABLE;
    node->variable_name = strdup(name);
    node->next = NULL;
    return node;
}

// Create a function call node
ASTNode *create_function_call_node(char *name, ASTNode *args) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) {
        parser_error("Memory allocation failed for function call node", NULL);
    }
    node->type = AST_FUNCTION_CALL;
    node->function_call.name = name;
    node->function_call.arguments = args;
    node->next = NULL;
    return node;
}
