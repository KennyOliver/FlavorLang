#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "common.h"

// Read a file into a string
char *read_file(const char *filename);

// Converts source code to tokens
Token *tokenize(const char *source);

#endif
