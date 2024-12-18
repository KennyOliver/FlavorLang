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
    "when",    // switch
    "is",      // case
    "prep",    // function
    "serve",   // return
    "try",     // try
    "crumbs",  // catch
    "burn",    // raise error
    "scran",   // output
    "taste",   // input
    "plate",   // write
    "garnish", // append
    "gather",  // read
    "book"     // import
};

const size_t KEYWORDS_COUNT = sizeof(KEYWORDS) / sizeof(KEYWORDS[0]);

const char *OPERATORS[] = {
    "=",
    "==",
    "+",
    "-",
    "*",
    "/",
    "<",
    ">",
    ">=",
    "<="};

const size_t OPERATORS_COUNT = sizeof(OPERATORS) / sizeof(OPERATORS[0]);
