#ifndef LEXER_UTILS_H
#define LEXER_UTILS_H

#include "../shared/token_types.h"

#define INITIAL_TOKEN_CAPACITY 1024

// Token management functions
Token *create_token(TokenType type, const char *lexeme, int line);
void free_token(Token *token);
void free_token_array(Token *tokens);

// Token array management
Token *resize_token_array(Token *tokens, size_t *capacity);
void append_token(Token **tokens, size_t *count, size_t *capacity,
                  TokenType type, const char *lexeme, int line);

// Token validation and checking
int is_valid_identifier_start(char c);
int is_valid_identifier_char(char c);
int is_whitespace(char c);

// Error handling
void token_error(const char *message, int line);

// Debug utilities
void print_token(const Token *token);
void dump_token_array(const Token *tokens, size_t count);

#endif
