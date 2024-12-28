#include "keywords.h"
#include <string.h>

const char *KEYWORDS[] = {
    "let",     // let
    "if",      // if
    "elif",    // else if
    "else",    // else
    "for",     // for
    "to",      // for range
    "while",   // while
    "check",   // switch
    "is",      // case
    "break",   // break
    "create",  // function
    "deliver", // return
    "try",     // try
    "rescue",  // catch
    "burn",    // raise error
    "show",    // output
    "taste",   // input
    "plate",   // write
    "garnish", // append
    "gather",  // read
    "recipe",  // import
    NULL       // sentinel value
};

const size_t KEYWORDS_COUNT =
    sizeof(KEYWORDS) / sizeof(KEYWORDS[0]) - 1; // - 1 for sentinel value

const char *OPERATORS[] = {
    "=", "==", "!=", "+",  "-",  "*",
    "/", "<",  ">",  ">=", "<=", NULL // sentinel value
};

const size_t OPERATORS_COUNT =
    sizeof(OPERATORS) / sizeof(OPERATORS[0]) - 1; // - 1 for sentinel value

int is_keyword(const char *lexeme) {
    if (!lexeme) {
        return 0;
    }

    for (size_t i = 0; KEYWORDS[i] != NULL; i++) {
        if (strcmp(lexeme, KEYWORDS[i]) == 0) {
            return 1;
        }
    }

    return 0;
}

int is_operator(const char *lexeme) {
    if (!lexeme) {
        return 0;
    }

    for (size_t i = 0; OPERATORS[i] != NULL; i++) {
        if (strcmp(lexeme, OPERATORS[i]) == 0) {
            return 1;
        }
    }

    return 0;
}

int is_valid_identifier_start(char c) { return isalpha(c) || c == '_'; }

int is_valid_identifier_char(char c) { return isalnum(c) || c == '_'; }

int is_whitespace(char c) {
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}
