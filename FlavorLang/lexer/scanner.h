#ifndef SCANNER_H
#define SCANNER_H

#include "../shared/token_types.h"
#include "keywords.h"
#include "lexer_utils.h"
#include <stdbool.h>
#include <string.h>

typedef struct
{
    const char *source;
    size_t length;
    size_t pos;
    int line;
} ScannerState;

/**
 * Skips over a comment in the source code.
 *
 * This function increments the position pointer in the `ScannerState` until the
 * end of the comment is reached, which is marked by a newline or the end of the string.
 *
 * @param state The current scanning state, which includes the source code,
 *              the current position, and the line number.
 */
void scan_comment(ScannerState *state);

/**
 * Processes a number (integer or float) in the source code.
 *
 * This function identifies and processes a number, handling optional negative
 * signs, decimals, and invalid formats. It also decides whether the number is
 * an integer or a floating-point number based on the presence of a decimal point.
 * If an invalid number format is encountered, it will handle it as an error or
 * as an operator, depending on the context.
 *
 * @param state The current scanning state, which includes the source code,
 *              the current position, and the line number.
 * @param tokens A pointer to the token array being built.
 * @param token_count The current number of tokens.
 * @param capacity The allocated capacity for the token array.
 */
void scan_number(ScannerState *state, Token **tokens, size_t *token_count,
                 size_t *capacity);

/**
 * Processes a string literal in the source code.
 *
 * This function handles the extraction of string literals, ensuring that they
 * are properly terminated by a closing quote. If an unterminated string is
 * found, it triggers an error.
 *
 * @param state The current scanning state, which includes the source code,
 *              the current position, and the line number.
 * @param tokens A pointer to the token array being built.
 * @param token_count The current number of tokens.
 * @param capacity The allocated capacity for the token array.
 */
void scan_string(ScannerState *state, Token **tokens, size_t *token_count,
                 size_t *capacity);

/**
 * Processes an identifier or keyword in the source code.
 *
 * This function handles identifiers and keywords, distinguishing between the
 * two based on a lookup. It also checks if the identifier is followed by an
 * opening parenthesis `(`, indicating a function call. If so, the identifier is
 * treated as a function name.
 *
 * @param state The current scanning state, which includes the source code,
 *              the current position, and the line number.
 * @param tokens A pointer to the token array being built.
 * @param token_count The current number of tokens.
 * @param capacity The allocated capacity for the token array.
 */
void scan_identifier_or_keyword(ScannerState *state, Token **tokens,
                                size_t *token_count, size_t *capacity);

/**
 * Processes an operator in the source code.
 *
 * This function identifies and processes operators, including multi-character
 * operators like `==`, `>=`, and `<=`. It increments the position in the `ScannerState`
 * appropriately after each operator is processed.
 *
 * @param state The current scanning state, which includes the source code,
 *              the current position, and the line number.
 * @param tokens A pointer to the token array being built.
 * @param token_count The current number of tokens.
 * @param capacity The allocated capacity for the token array.
 */
void scan_operator(ScannerState *state, Token **tokens, size_t *token_count,
                   size_t *capacity);

#endif
