#ifndef TOKEN_TYPES_H
#define TOKEN_TYPES_H

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Token types
typedef enum {
  TOKEN_KEYWORD,
  TOKEN_IDENTIFIER,
  TOKEN_FLOAT,
  TOKEN_INTEGER,
  TOKEN_STRING,
  TOKEN_OPERATOR,
  TOKEN_DELIMITER,
  TOKEN_FUNCTION_NAME,
  TOKEN_FUNCTION_PARAMETER,
  TOKEN_PAREN_OPEN,
  TOKEN_PAREN_CLOSE,
  TOKEN_EOF
} TokenType;

// Token structure
typedef struct {
  TokenType type; // type of token
  char *lexeme;   // the actual text of the token
  int line;       // line number for error reporting
} Token;

#endif
