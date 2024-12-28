#ifndef LEXER_H
#define LEXER_H

#include "../shared/token_types.h"
#include "keywords.h"
#include "lexer_utils.h"
#include "scanner.h"
#include <ctype.h>
#include <string.h>

// Read a file into a string
char *read_file(const char *filename);

// Converts source code to tokens
Token *tokenize(const char *source);

#endif
