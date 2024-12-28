#ifndef LEXER_H
#define LEXER_H

#include "../shared/token_types.h"
#include "keywords.h"
#include "lexer_utils.h"
#include "scanner.h"
#include <ctype.h>
#include <string.h>

#define INITIAL_TOKEN_CAPACITY 1024

/**
 * Reads a file into a dynamically allocated buffer.
 *
 * @param filename The name of the file to read.
 * @return A string containing the file's content, or `NULL` if an error occurs.
 */
char *read_file(const char *filename);

/**
 * Tokenizes source code into a list of tokens.
 *
 * Scans the source code character by character and identifies various token
 * types (e.g., numbers, strings, operators, delimiters). Returns a dynamically
 * allocated array of tokens or `NULL` if an error occurs.
 *
 * @param source The source code to tokenize.
 * @return An array of tokens, or `NULL` if an error occurs.
 */
Token *tokenize(const char *source);

#endif
