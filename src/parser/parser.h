#ifndef PARSER_H
#define PARSER_H

#include "../shared/ast_types.h"
#include "array_parser.h"
#include "operator_parser.h"
#include "parser_state.h"

// Main parsing functions
ASTNode *parse_program(Token *tokens);
void free_ast(ASTNode *node);

// Print AST
void print_ast(ASTNode *node, int depth);

// Statement parsing
ASTNode *parse_statement(ParserState *state);
ASTNode *parse_variable_declaration(ParserState *state);
ASTNode *parse_constant_declaration(ParserState *state);
ASTNode *parse_variable_assignment(ParserState *state);
ASTNode *parse_conditional_block(ParserState *state);
ASTNode *parse_while_loop(ParserState *state);
ASTNode *parse_for_loop(ParserState *state);
ASTNode *parse_break_statement(ParserState *state);
ASTNode *parse_switch_block(ParserState *state);
ASTNode *parse_function_declaration(ParserState *state);
ASTNode *parse_function_call(ParserState *state);
ASTNode *parse_function_return(ParserState *state);
ASTNode *parse_try_block(ParserState *state);
ASTCatchNode *parse_catch_block(ParserState *state);
ASTNode *parse_finally_block(ParserState *state);
ASTNode *parse_import_statement(ParserState *state);
ASTNode *parse_export_statement(ParserState *state);

// Expression parsing
ASTNode *parse_expression(ParserState *state);
ASTNode *parse_literal_or_identifier(ParserState *state);
ASTNode *parse_block(ParserState *state);

ASTNode *create_variable_reference_node(char *name);

// Helper functions
ASTNode *parse_declaration(ParserState *state, ASTNodeType type);
Token *peek_next_token(ParserState *state);
Token *peek_ahead(ParserState *state, size_t n);
ASTNode *parse_expression_statement(ParserState *state);
ASTNode *parse_block(ParserState *state);
ASTNode *parse_case_body(ParserState *state);
ASTNode *parse_function_body(ParserState *state);
bool is_assignment(ParserState *state);

#endif
