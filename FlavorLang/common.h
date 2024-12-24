#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Token types
typedef enum
{
    TOKEN_KEYWORD,
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_STRING,
    TOKEN_OPERATOR,
    TOKEN_DELIMITER,
    TOKEN_FUNCTION_DECLARATION,
    TOKEN_FUNCTION_PARAMETER,
    TOKEN_FUNCTION_CALL,
    TOKEN_PAREN_OPEN,
    TOKEN_PAREN_CLOSE,
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
