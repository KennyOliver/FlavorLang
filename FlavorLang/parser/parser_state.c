#include "parser_state.h"
#include <stdlib.h>

ParserState *create_parser_state(Token *tokens)
{
    ParserState *state = malloc(sizeof(ParserState));
    if (!state)
    {
        fprintf(stderr, "Failed to allocate parser state\n");
        exit(1);
    }
    state->tokens = tokens;
    state->current_token = 0;
    return state;
}

void free_parser_state(ParserState *state)
{
    free(state);
}

Token *get_current_token(ParserState *state)
{
    return &state->tokens[state->current_token];
}

Token *advance_token(ParserState *state)
{
    return &state->tokens[++state->current_token];
}

void expect_token(ParserState *state, TokenType expected, const char *error_message)
{
    Token *current = get_current_token(state);
    if (current->type != expected)
    {
        parser_error(error_message, current);
    }
    advance_token(state);
}

void parser_error(const char *message, Token *token)
{
    fprintf(stderr, "Parser Error: %s (found \"%s\" on line %d)\n",
            message, token->lexeme, token->line);
    exit(1);
}
