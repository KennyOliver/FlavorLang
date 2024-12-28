#ifndef KEYWORDS_H
#define KEYWORDS_H

#include <stdlib.h>

// Array of FlavorLang keywords
/**
 * Array of keywords used in FlavorLang.
 *
 * This array contains the keywords that are recognized by the FlavorLang lexer,
 * such as control flow keywords (`if`, `else`), function-related keywords (`create`, `deliver`),
 * and others specific to the language.
 */
extern const char *KEYWORDS[];

/**
 * The number of keywords in the `KEYWORDS` array.
 *
 * This constant represents the total number of keywords in the `KEYWORDS` array,
 * excluding the sentinel `NULL` value used to mark the end of the array.
 */
extern const size_t KEYWORDS_COUNT;

// Array of FlavorLang operators
/**
 * Array of operators used in FlavorLang.
 *
 * This array contains the operators that are recognized by the FlavorLang lexer,
 * such as arithmetic operators (`+`, `-`, `*`, `/`) and comparison operators (`==`, `>`, `<`).
 */
extern const char *OPERATORS[];

/**
 * The number of operators in the `OPERATORS` array.
 *
 * This constant represents the total number of operators in the `OPERATORS` array,
 * excluding the sentinel `NULL` value used to mark the end of the array.
 */
extern const size_t OPERATORS_COUNT;

// Utility functions for keyword & operator checking

/**
 * Checks if a lexeme is a keyword in FlavorLang.
 *
 * This function checks if the provided string (lexeme) is one of the keywords
 * defined in the `KEYWORDS` array. If the lexeme is found in the list of keywords,
 * the function returns `1`, indicating that it is a keyword. Otherwise, it returns `0`.
 *
 * @param lexeme The string to check.
 * @return `1` if the lexeme is a keyword, `0` otherwise.
 */
int is_keyword(const char *lexeme);

/**
 * Checks if a lexeme is an operator in FlavorLang.
 *
 * This function checks if the provided string (lexeme) is one of the operators
 * defined in the `OPERATORS` array. If the lexeme is found in the list of operators,
 * the function returns `1`, indicating that it is an operator. Otherwise, it returns `0`.
 *
 * @param lexeme The string to check.
 * @return `1` if the lexeme is an operator, `0` otherwise.
 */
int is_operator(const char *lexeme);

#endif