#ifndef PARSER_STATE_H
#define PARSER_STATE_H

#include "../shared/token_types.h"
#include <stdio.h>
#include <stdbool.h>

typedef struct
{
    Token *tokens;
    size_t current_token;
    bool in_switch_block;
    bool in_function_body;
} ParserState;

// Create and destroy parser state
ParserState *create_parser_state(Token *tokens);
void free_parser_state(ParserState *state);

// Token navigation
Token *get_current_token(ParserState *state);
Token *advance_token(ParserState *state);
void expect_token(ParserState *state, TokenType type, const char *error_message);

// Error handling
void parser_error(const char *message, Token *token);

#endif
