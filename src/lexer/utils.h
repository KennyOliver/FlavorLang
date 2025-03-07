#ifndef LEXER_UTILS_H
#define LEXER_UTILS_H

#include "../debug/debug.h"
#include "../shared/token_types.h"
#include <ctype.h>
#include <stdlib.h>

// Token management functions

/**
 * Creates a new token with the given type and lexeme.
 *
 * This function allocates memory for a new token, assigns its type and lexeme,
 * and sets the line number where the token was found. If memory allocation
 * fails, an error is reported, and the function returns `NULL`.
 *
 * @param type The type of the token being created.
 * @param lexeme The lexeme (text) of the token. Can be `NULL` if no lexeme is
 * needed.
 * @param line The line number where the token was found in the source code.
 * @return A pointer to the newly created token, or `NULL` if allocation fails.
 */
Token *create_token(TokenType type, const char *lexeme, int line);

/**
 * Frees the memory allocated for a token.
 *
 * This function frees both the token structure and its lexeme (if present).
 *
 * @param token The token to be freed.
 */
void free_token(Token *token);

/**
 * Frees the memory allocated for an array of tokens.
 *
 * This function iterates over the token array and frees the lexeme of each
 * token, and then frees the array itself. It does nothing if the token array is
 * `NULL`.
 *
 * @param tokens The array of tokens to be freed.
 */
void free_token_array(Token *tokens);

// Token array management

/**
 * Resizes the token array to accommodate more tokens.
 *
 * This function increases the capacity of the token array by a defined growth
 * factor (TOKEN_ARRAY_GROWTH_FACTOR). If memory allocation fails, the current
 * token array is freed, and an error is reported.
 *
 * @param tokens The token array to be resized.
 * @param capacity A pointer to the current capacity of the token array.
 * @return A resized token array, or `NULL` if resizing fails.
 */
Token *resize_token_array(Token *tokens, size_t *capacity);

/**
 * Appends a new token to the token array.
 *
 * This function checks if the token array has enough capacity to add a new
 * token. If not, it resizes the array. It then appends the new token, updating
 * the token count.
 *
 * @param tokens A pointer to the token array.
 * @param count A pointer to the current number of tokens in the array.
 * @param capacity A pointer to the capacity of the token array.
 * @param type The type of the token to be appended.
 * @param lexeme The lexeme of the token to be appended. Can be `NULL`.
 * @param line The line number where the token was found.
 */
void append_token(Token **tokens, size_t *count, size_t *capacity,
                  TokenType type, const char *lexeme, int line);

// Error handling

/**
 * Prints an error message and exits the program.
 *
 * This function prints an error message with the line number (if available) and
 * then terminates the program with an error code (exit status 1).
 *
 * @param message The error message to print.
 * @param line The line number where the error occurred (use -1 if no line
 * number is available).
 */
void token_error(const char *message, int line);

// Debug utilities

/**
 * Prints a single token in a human-readable format.
 *
 * This function prints the token's type, lexeme, and line number to the
 * console.
 *
 * @param token The token to print.
 */
void print_token(const Token *token);

/**
 * Dumps the token array to the console.
 *
 * This function prints all the tokens in the array, including their index,
 * type, lexeme, and line number. It is useful for debugging and inspecting the
 * token array.
 *
 * @param tokens The token array to print.
 * @param count The number of tokens in the array.
 */
void dump_token_array(const Token *tokens, size_t count);

/**
 * Prints tokens in a debug-friendly format.
 *
 * This function prints tokens with their line number, type, and lexeme in a
 * debug format, but only if the `debug_flag` is set. It groups tokens by line
 * for readability.
 *
 * @param tokens The token array to print.
 */
void debug_print_tokens(Token *tokens);

#endif
