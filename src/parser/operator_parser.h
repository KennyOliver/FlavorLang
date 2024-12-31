#ifndef OPERATOR_PARSER_H
#define OPERATOR_PARSER_H

#include "../shared/ast_types.h"
#include "parser_state.h"

// Parse an expression with operator precedence
ASTNode *parse_operator_expression(ParserState *state);

// Function declarations for each precedence level
ASTNode *parse_logical(ParserState *state);
ASTNode *parse_equality(ParserState *state);
ASTNode *parse_comparison(ParserState *state);
ASTNode *parse_term(ParserState *state);
ASTNode *parse_factor(ParserState *state);
ASTNode *parse_power(ParserState *state);
ASTNode *parse_unary(ParserState *state);
ASTNode *parse_primary(ParserState *state);

// Helper functions for token matching
bool match_operator(ParserState *state, const char *op);

// AST Node creation helper functions
ASTNode *create_binary_op_node(char *operator, ASTNode * left, ASTNode *right);
ASTNode *create_unary_op_node(char *operator, ASTNode * operand);
ASTNode *create_literal_node(Token *token);
ASTNode *create_variable_node(char *name);

#endif
