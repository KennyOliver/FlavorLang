#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Token types
typedef enum
{
    TOKEN_KEYWORD,
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_STRING,
    TOKEN_OPERATOR,
    TOKEN_DELIMITER,
    TOKEN_EOF
} TokenType;

// Token structure
typedef struct
{
    TokenType type; // type of token
    char *lexeme;   // the actual text of the token
    int line;       // line number for error reporting
} Token;

#endif
