#include "parser_state.h"
#include <stdlib.h>

ParserState *create_parser_state(Token *tokens) {
    ParserState *state = malloc(sizeof(ParserState));
    if (!state) {
        fprintf(stderr, "Failed to allocate parser state\n");
        exit(1);
    }
    state->tokens = tokens;
    state->current_token = 0;
    state->current = &tokens[0];
    state->previous = NULL;
    state->in_function_body = false;
    return state;
}

void free_parser_state(ParserState *state) { free(state); }

Token *get_current_token(ParserState *state) { return state->current; }

void advance_token(ParserState *state) {
    if (state->current->type != TOKEN_EOF) {
        state->previous = state->current;
        state->current_token++;
        state->current = &state->tokens[state->current_token];
    }
}

void expect_token(ParserState *state, TokenType expected,
                  const char *error_message) {
    Token *current = get_current_token(state);
    if (current->type != expected) {
        parser_error(error_message, current);
    }
    advance_token(state);
}

bool match_token(ParserState *state, const char *lexeme) {
    Token *token = get_current_token(state);
    return token && strcmp(token->lexeme, lexeme) == 0;
}

Token *peek_next_token(ParserState *state) {
    // Just look at the next token (but don’t advance)
    return &state->tokens[state->current_token + 1];
}

void parser_error(const char *message, Token *token) {
    if (token) {
        fprintf(stderr, "Parser Error [Line %d]: %s (found \"%s\")\n",
                token->line, message, token->lexeme);
    } else {
        fprintf(stderr, "Parser Error: %s\n", message);
    }
    exit(1);
}
