#include "keywords.h"
#include <string.h>

const char *KEYWORDS[] = {
    "let",     // variable declaration
    "const",   // constant declaration
    "if",      // if
    "elif",    // else if
    "else",    // else
    "for",     // for
    "in",      // for in
    "by",      // for in by
    "while",   // while
    "check",   // switch
    "is",      // case
    "break",   // break
    "create",  // function
    "deliver", // return
    "try",     // try block
    "rescue",  // catch block
    "finish",  // finally block
    "plate",   // write file
    "garnish", // append file
    "taste",   // read file
    "True",    // Boolean True
    "False",   // Boolean False
    "import",  // Import `.flv` script
    "export",  // Export identifiers in `.flv` script
    NULL       // sentinel value
};

const size_t KEYWORDS_COUNT =
    sizeof(KEYWORDS) / sizeof(KEYWORDS[0]) - 1; // - 1 for sentinel value

const char *OPERATORS[] = {
    "=", "==", "!=", "+",  "-",  "*",   "**", "/",  "//", "%",
    "<", ">",  ">=", "<=", "..", "..=", "&&", "||", "!",  NULL // sentinel value
};

const size_t OPERATORS_COUNT =
    sizeof(OPERATORS) / sizeof(OPERATORS[0]) - 1; // - 1 for sentinel value

int is_keyword(const char *lexeme) {
    if (!lexeme) {
        return 0;
    }

    for (size_t i = 0; KEYWORDS[i] != NULL; i++) {
        if (strcmp(lexeme, KEYWORDS[i]) == 0) {
            // Check if it's a boolean
            if (strcmp(lexeme, "True") == 0 || strcmp(lexeme, "False") == 0) {
                return TOKEN_BOOLEAN;
            }
            return TOKEN_KEYWORD;
        }
    }

    return TOKEN_IDENTIFIER;
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
