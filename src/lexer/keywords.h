#ifndef KEYWORDS_H
#define KEYWORDS_H

#include "../shared/token_types.h"
#include <ctype.h>
#include <stdlib.h>

// Array of FlavorLang keywords
/**
 * Array of keywords used in FlavorLang.
 *
 * This array contains the keywords that are recognized by the FlavorLang lexer,
 * such as control flow keywords (`if`, `else`), function-related keywords
 * (`create`, `deliver`), and others specific to the language.
 */
extern const char *KEYWORDS[];

/**
 * The number of keywords in the `KEYWORDS` array.
 *
 * This constant represents the total number of keywords in the `KEYWORDS`
 * array, excluding the sentinel `NULL` value used to mark the end of the array.
 */
extern const size_t KEYWORDS_COUNT;

// Array of FlavorLang operators
/**
 * Array of operators used in FlavorLang.
 *
 * This array contains the operators that are recognized by the FlavorLang
 * lexer, such as arithmetic operators (`+`, `-`, `*`, `/`) and comparison
 * operators (`==`, `>`, `<`).
 */
extern const char *OPERATORS[];

/**
 * The number of operators in the `OPERATORS` array.
 *
 * This constant represents the total number of operators in the `OPERATORS`
 * array, excluding the sentinel `NULL` value used to mark the end of the array.
 */
extern const size_t OPERATORS_COUNT;

// Utility functions for keyword & operator checking

/**
 * Checks if a lexeme is a keyword in FlavorLang.
 *
 * This function checks if the provided string (lexeme) is one of the keywords
 * defined in the `KEYWORDS` array. If the lexeme is found in the list of
 * keywords, the function returns `1`, indicating that it is a keyword.
 * Otherwise, it returns `0`.
 *
 * @param lexeme The string to check.
 * @return `1` if the lexeme is a keyword, `0` otherwise.
 */
int is_keyword(const char *lexeme);

/**
 * Checks if a lexeme is an operator in FlavorLang.
 *
 * This function checks if the provided string (lexeme) is one of the operators
 * defined in the `OPERATORS` array. If the lexeme is found in the list of
 * operators, the function returns `1`, indicating that it is an operator.
 * Otherwise, it returns `0`.
 *
 * @param lexeme The string to check.
 * @return `1` if the lexeme is an operator, `0` otherwise.
 */
int is_operator(const char *lexeme);

/**
 * Checks if a character can be the start of an identifier.
 *
 * This function checks if the character is a letter (a-z, A-Z) or an underscore
 * (`_`), which are valid starting characters for an identifier.
 *
 * @param c The character to check.
 * @return 1 if the character can be the start of an identifier, 0 otherwise.
 */
int is_valid_identifier_start(char c);

/**
 * Checks if a character can be part of an identifier.
 *
 * This function checks if the character is alphanumeric (a-z, A-Z, 0-9) or an
 * underscore (`_`), which are valid characters for an identifier after the
 * first character.
 *
 * @param c The character to check.
 * @return 1 if the character can be part of an identifier, 0 otherwise.
 */
int is_valid_identifier_char(char c);

/**
 * Checks if a character is whitespace.
 *
 * This function checks if the character is one of the following whitespace
 * characters: space, tab, carriage return, or newline.
 *
 * @param c The character to check.
 * @return 1 if the character is whitespace, 0 otherwise.
 */
int is_whitespace(char c);

#endif
