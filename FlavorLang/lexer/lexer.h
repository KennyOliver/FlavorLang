#ifndef LEXER_H
#define LEXER_H

#include "../shared/token_types.h"

// Read a file into a string
char *read_file(const char *filename);

// Converts source code to tokens
Token *tokenize(const char *source);

void debug_print_tokens(Token *tokens);

#endif
