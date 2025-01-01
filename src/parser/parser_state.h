#ifndef PARSER_STATE_H
#define PARSER_STATE_H

#include "../shared/token_types.h"
#include <stdbool.h>
#include <stdio.h>

typedef struct {
    Token *tokens;         // Array of tokens
    size_t current_token;  // Current token index
    Token *current;        // Pointer to current token
    Token *previous;       // Pointer to previous token
    bool in_function_body; // Flag to indicate if parsing inside a function body
} ParserState;

// Create and destroy parser state
ParserState *create_parser_state(Token *tokens);
void free_parser_state(ParserState *state);

// Token navigation
Token *get_current_token(ParserState *state);
void advance_token(ParserState *state);
void expect_token(ParserState *state, TokenType type,
                  const char *error_message);
bool match_token(ParserState *state, const char *lexeme);
Token *peek_next_token(ParserState *state);

// Error handling
void parser_error(const char *message, Token *token);

#endif
