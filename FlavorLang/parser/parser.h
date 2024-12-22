#ifndef PARSER_H
#define PARSER_H

#include "ast_types.h"
#include "parser_state.h"

// Main parsing functions
ASTNode *parse_program(Token *tokens);
void free_ast(ASTNode *node);

// Statement parsing
ASTNode *parse_variable_declaration(ParserState *state);
ASTNode *parse_variable_assignment(ParserState *state);
ASTNode *parse_print_statement(ParserState *state);
ASTNode *parse_raise_error(ParserState *state);
ASTNode *parse_input(ParserState *state);
ASTNode *parse_conditional_block(ParserState *state);
ASTNode *parse_while_block(ParserState *state);
ASTNode *parse_while_block(ParserState *state);

// Expression parsing
ASTNode *parse_expression(ParserState *state);
ASTNode *parse_identifier(ParserState *state);
ASTNode *parse_literal_or_identifier(ParserState *state);
ASTNode *parse_block(ParserState *state);

#endif
