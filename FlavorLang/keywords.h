#ifndef KEYWORDS_H
#define KEYWORDS_H

// Array of FlavorLang keywords
extern const char *KEYWORDS[];
extern const size_t KEYWORDS_COUNT;

// Array of FlavorLang operators
extern const char *OPERATORS[];
extern const size_t OPERATORS_COUNT;

// Utility functions for keyword & operator checking
int is_keyword(const char *lexeme);
int is_operator(const char *lexeme);

#endif
