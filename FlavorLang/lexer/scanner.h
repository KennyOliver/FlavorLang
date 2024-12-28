#ifndef SCANNER_H
#define SCANNER_H

#include "../shared/token_types.h"
#include <stdbool.h>
#include <string.h>
#include "keywords.h"
#include "lexer_utils.h"

/**
 * Skips over a comment in the source code.
 *
 * This function increments the position pointer until the end of the comment
 * is reached, which is marked by a newline or the end of the string.
 *
 * @param source The source code being tokenized.
 * @param pos A pointer to the current position in the source code.
 */
static void scan_comment(const char *source, size_t *pos);

/**
 * Processes a number (integer or float) in the source code.
 *
 * This function identifies and processes a number, handling optional negative
 * signs, decimals, and invalid formats. It also decides whether the number is
 * an integer or a floating-point number based on the presence of a decimal point.
 * If an invalid number format is encountered, it will handle it as an error or
 * as an operator, depending on the context.
 *
 * @param source The source code being tokenized.
 * @param pos A pointer to the current position in the source code.
 * @param length The total length of the source code.
 * @param tokens A pointer to the token array being built.
 * @param token_count The current number of tokens.
 * @param capacity The allocated capacity for the token array.
 * @param line The current line number in the source code.
 */
static void scan_number(const char *source, size_t *pos, size_t length,
                        Token **tokens, size_t *token_count, size_t *capacity,
                        int line);

/**
 * Processes a string literal in the source code.
 *
 * This function handles the extraction of string literals, ensuring that they
 * are properly terminated by a closing quote. If an unterminated string is found,
 * it triggers an error.
 *
 * @param source The source code being tokenized.
 * @param pos A pointer to the current position in the source code.
 * @param length The total length of the source code.
 * @param tokens A pointer to the token array being built.
 * @param token_count The current number of tokens.
 * @param capacity The allocated capacity for the token array.
 * @param line The current line number in the source code.
 */
static void scan_string(const char *source, size_t *pos, size_t length,
                        Token **tokens, size_t *token_count, size_t *capacity,
                        int line);

/**
 * Processes an identifier or keyword in the source code.
 *
 * This function handles identifiers and keywords, distinguishing between the
 * two based on a lookup. It also checks if the identifier is followed by an opening
 * parenthesis `(`, indicating a function call. If so, the identifier is treated
 * as a function name.
 *
 * @param source The source code being tokenized.
 * @param pos A pointer to the current position in the source code.
 * @param length The total length of the source code.
 * @param tokens A pointer to the token array being built.
 * @param token_count The current number of tokens.
 * @param capacity The allocated capacity for the token array.
 * @param line The current line number in the source code.
 */
static void scan_identifier_or_keyword(const char *source, size_t *pos,
                                       size_t length, Token **tokens,
                                       size_t *token_count, size_t *capacity,
                                       int line);

/**
 * Processes an operator in the source code.
 *
 * This function identifies and processes operators, including multi-character
 * operators like `==`, `>=`, and `<=`. It increments the position in the source
 * code appropriately after each operator is processed.
 *
 * @param source The source code being tokenized.
 * @param pos A pointer to the current position in the source code.
 * @param length The total length of the source code.
 * @param tokens A pointer to the token array being built.
 * @param token_count The current number of tokens.
 * @param capacity The allocated capacity for the token array.
 * @param line The current line number in the source code.
 */
static void scan_operator(const char *source, size_t *pos, size_t length,
                          Token **tokens, size_t *token_count,
                          size_t *capacity, int line);

#endif
