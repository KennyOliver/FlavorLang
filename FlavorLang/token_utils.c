#include "token_utils.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define INITIAL_TOKEN_CAPACITY 1024
#define TOKEN_ARRAY_GROWTH_FACTOR 2

Token *create_token(TokenType type, const char *lexeme, int line)
{
    Token *token = malloc(sizeof(Token));
    if (!token)
    {
        token_error("Memory allocation failed", line);
        return NULL;
    }

    token->type = type;
    token->line = line;

    if (lexeme)
    {
        token->lexeme = strdup(lexeme);
        if (!token->lexeme)
        {
            free(token);
            token_error("Memory allocation failed for lexeme", line);
            return NULL;
        }
    }
    else
    {
        token->lexeme = NULL;
    }

    return token;
}

void free_token(Token *token)
{
    if (token)
    {
        free(token->lexeme);
        free(token);
    }
}

void free_token_array(Token *tokens)
{
    if (!tokens)
        return;

    for (size_t i = 0; tokens[i].type != TOKEN_EOF; i++)
    {
        free(tokens[i].lexeme);
    }
    free(tokens);
}

Token *resize_token_array(Token *tokens, size_t *capacity)
{
    size_t new_capacity = *capacity * TOKEN_ARRAY_GROWTH_FACTOR;
    Token *new_tokens = realloc(tokens, sizeof(Token) * new_capacity);

    if (!new_tokens)
    {
        free_token_array(tokens);
        token_error("Failed to resize token array", -1);
        return NULL;
    }

    *capacity = new_capacity;
    return new_tokens;
}

void append_token(Token **tokens, size_t *count, size_t *capacity,
                  TokenType type, const char *lexeme, int line)
{
    if (*count >= *capacity - 1)
    { // -1 to leave room for EOF token
        Token *new_tokens = resize_token_array(*tokens, capacity);
        if (!new_tokens)
            return;
        *tokens = new_tokens;
    }

    (*tokens)[*count] = (Token){
        .type = type,
        .lexeme = lexeme ? strdup(lexeme) : NULL,
        .line = line};

    (*count)++;
}

int is_valid_identifier_start(char c)
{
    return isalpha(c) || c == '_';
}

int is_valid_identifier_char(char c)
{
    return isalnum(c) || c == '_';
}

int is_whitespace(char c)
{
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

void token_error(const char *message, int line)
{
    if (line >= 0)
    {
        fprintf(stderr, "Error on line %d: %s\n", line, message);
    }
    else
    {
        fprintf(stderr, "Error: %s\n", message);
    }
    exit(1);
}

void print_token(const Token *token)
{
    const char *type_str;
    switch (token->type)
    {
    case TOKEN_KEYWORD:
        type_str = "KEYWORD";
        break;
    case TOKEN_IDENTIFIER:
        type_str = "IDENTIFIER";
        break;
    case TOKEN_STRING:
        type_str = "STRING";
        break;
    case TOKEN_NUMBER:
        type_str = "NUMBER";
        break;
    case TOKEN_OPERATOR:
        type_str = "OPERATOR";
        break;
    case TOKEN_DELIMITER:
        type_str = "DELIMITER";
        break;
    case TOKEN_EOF:
        type_str = "EOF";
        break;
    default:
        type_str = "UNKNOWN";
        break;
    }

    printf("Token{type: %s, lexeme: \"%s\", line: %d}\n",
           type_str,
           token->lexeme ? token->lexeme : "NULL",
           token->line);
}

void dump_token_array(const Token *tokens, size_t count)
{
    printf("Token Array (%zu tokens):\n", count);
    for (size_t i = 0; i < count; i++)
    {
        printf("[%zu] ", i);
        print_token(&tokens[i]);
    }
}
