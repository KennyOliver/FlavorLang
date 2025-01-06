#include "scanner.h"

void scan_comment(ScannerState *state) {
    while (state->source[state->pos] != '\0' &&
           state->source[state->pos] != '\n') {
        state->pos++;
    }
}

void scan_number(ScannerState *state, Token **tokens, size_t *token_count,
                 size_t *capacity) {
    size_t start = state->pos;
    bool has_decimal_point = false;
    bool is_negative = false;

    // Check for negative sign
    if (state->source[state->pos] == '-') {
        bool is_start_of_expression = true;
        if (*token_count > 0) {
            TokenType prev_type = (*tokens)[*token_count - 1].type;
            if (prev_type == TOKEN_INTEGER || prev_type == TOKEN_FLOAT ||
                prev_type == TOKEN_IDENTIFIER || prev_type == TOKEN_STRING ||
                prev_type == TOKEN_PAREN_CLOSE) {
                is_start_of_expression = false;
            }
        }

        if (is_start_of_expression) {
            is_negative = true;
            state->pos++; // move past the negative sign
        } else {
            scan_operator(state, tokens, token_count, capacity);
            return;
        }
    }

    if (state->pos >= state->length || !isdigit(state->source[state->pos])) {
        if (is_negative) {
            state->pos = start; // reset position
            scan_operator(state, tokens, token_count, capacity);
            return;
        }
        token_error("Invalid number format", state->line);
    }

    while (state->pos < state->length &&
           (isdigit(state->source[state->pos]) ||
            (state->source[state->pos] == '.' && !has_decimal_point))) {
        if (state->source[state->pos] == '.') {
            if (state->pos + 1 >= state->length ||
                !isdigit(state->source[state->pos + 1])) {
                break; // if there's no digit after the decimal point, treat the
                       // decimal as a separate token
            }
            has_decimal_point = true;
        }
        state->pos++;
    }

    char *lexeme = strndup(&state->source[start], state->pos - start);
    TokenType type = has_decimal_point ? TOKEN_FLOAT : TOKEN_INTEGER;
    append_token(tokens, token_count, capacity, type, lexeme, state->line);
    free(lexeme);
}

void scan_array(ScannerState *state, Token **tokens, size_t *token_count,
                size_t *capacity) {
    char c = state->source[state->pos];

    if (c == '[') {
        // Add opening bracket token
        append_token(tokens, token_count, capacity, TOKEN_SQ_BRACKET_OPEN, "[",
                     state->line);
        state->pos++; // Move past `[`

        // Scan inside the brackets
        while (state->pos < state->length && state->source[state->pos] != ']') {
            char inner_c = state->source[state->pos];

            // Handle whitespace
            if (is_whitespace(inner_c)) {
                if (inner_c == '\n') {
                    state->line++;
                }
                state->pos++;
                continue;
            }

            // **Handle special array operations first**
            if (inner_c == '^' || inner_c == '+' || inner_c == '-') {
                // Check if next character is also '^', '+', or '-'
                if (state->pos + 1 < state->length &&
                    (state->source[state->pos + 1] == '^' ||
                     state->source[state->pos + 1] == '+' ||
                     state->source[state->pos + 1] == '-')) {
                    // Two-character array operator
                    char *op = strndup(&state->source[state->pos], 2);
                    append_token(tokens, token_count, capacity, TOKEN_ARRAY_OP,
                                 op, state->line);
                    free(op);
                    state->pos += 2; // Move past the two-character operator
                    continue;
                } else {
                    // Single-character array operator (if valid)
                    char op[2] = {inner_c, '\0'};
                    append_token(tokens, token_count, capacity, TOKEN_ARRAY_OP,
                                 op, state->line);
                    state->pos++;
                    continue;
                }
            }

            // **Handle numbers after array operators**
            if (isdigit(inner_c) ||
                (inner_c == '-' && isdigit(state->source[state->pos + 1]))) {
                scan_number(state, tokens, token_count, capacity);
                continue;
            }

            // Handle separators (`,`)
            if (inner_c == ',') {
                append_token(tokens, token_count, capacity, TOKEN_DELIMITER,
                             ",", state->line);
                state->pos++;
                continue;
            }

            // Handle slicing syntax (`:`)
            if (inner_c == ':') {
                append_token(tokens, token_count, capacity, TOKEN_COLON, ":",
                             state->line);
                state->pos++;
                continue;
            }

            // Unexpected character
            fprintf(stderr,
                    "Unexpected character `%c` at position %zu (line %d)\n",
                    inner_c, state->pos, state->line);
            token_error("Unexpected character in array", state->line);
        }

        // Check for closing bracket
        if (state->pos < state->length && state->source[state->pos] == ']') {
            append_token(tokens, token_count, capacity, TOKEN_SQ_BRACKET_CLOSE,
                         "]", state->line);
            state->pos++; // Move past `]`.
        } else {
            token_error("Unmatched opening bracket `[`", state->line);
        }
    } else if (c == ']') {
        token_error("Unmatched closing bracket `]`", state->line);
    }
}

void scan_string(ScannerState *state, Token **tokens, size_t *token_count,
                 size_t *capacity) {
    size_t start = ++(state->pos); // skip opening quote
    while (state->pos < state->length && state->source[state->pos] != '"') {
        state->pos++;
    }
    if (state->pos >= state->length || state->source[state->pos] != '"') {
        token_error("Unterminated string literal", state->line);
    }
    char *lexeme = strndup(&state->source[start], state->pos - start);
    append_token(tokens, token_count, capacity, TOKEN_STRING, lexeme,
                 state->line);
    free(lexeme);
    state->pos++; // skip closing quote
}

void scan_boolean(ScannerState *state, Token **tokens, size_t *token_count,
                  size_t *capacity) {
    size_t start = state->pos;

    // Check if it's a valid `True` or `False` literal
    if (state->pos + 3 <= state->length &&
        strncmp(&state->source[state->pos], "True", 4) == 0) {
        state->pos += 4; // skip past "true"
    } else if (state->pos + 4 <= state->length &&
               strncmp(&state->source[state->pos], "False", 5) == 0) {
        state->pos += 5; // skip past "false"
    } else {
        token_error("Invalid boolean literal", state->line);
    }

    // Extract the boolean lexeme and add it to the token array
    char *lexeme = strndup(&state->source[start], state->pos - start);
    append_token(tokens, token_count, capacity, TOKEN_BOOLEAN, lexeme,
                 state->line);
    free(lexeme);
}

void scan_identifier_or_keyword(ScannerState *state, Token **tokens,
                                size_t *token_count, size_t *capacity) {
    size_t start = state->pos;
    while (state->pos < state->length &&
           is_valid_identifier_char(state->source[state->pos])) {
        state->pos++;
    }

    char *lexeme = strndup(&state->source[start], state->pos - start);

    // Determine if the lexeme is a keyword or identifier
    int keyword_type = is_keyword(lexeme);
    if (keyword_type == TOKEN_BOOLEAN) {
        append_token(tokens, token_count, capacity, TOKEN_BOOLEAN, lexeme,
                     state->line);
    } else if (keyword_type == TOKEN_KEYWORD) {
        append_token(tokens, token_count, capacity, TOKEN_KEYWORD, lexeme,
                     state->line);
    } else {
        append_token(tokens, token_count, capacity, TOKEN_IDENTIFIER, lexeme,
                     state->line);
    }
    free(lexeme);
}

void scan_operator(ScannerState *state, Token **tokens, size_t *token_count,
                   size_t *capacity) {
    char first_char = state->source[state->pos];
    char second_char =
        state->pos < state->length - 1 ? state->source[state->pos + 1] : '\0';
    char third_char =
        state->pos < state->length - 2 ? state->source[state->pos + 2] : '\0';

    if (first_char == ':') {
        append_token(tokens, token_count, capacity, TOKEN_COLON, ":",
                     state->line);
        state->pos++;
        return;
    }

    // Check for two-character & three-character operators
    if (second_char != '\0') {
        if ((first_char == '=' && second_char == '=') || // ==
            (first_char == '>' && second_char == '=') || // >=
            (first_char == '<' && second_char == '=') || // <=
            (first_char == '!' && second_char == '=') || // !=
            (first_char == '.' && second_char == '.') || // ..
            (first_char == '.' && second_char == '.' &&
             third_char == '=') ||                       // ..=
            (first_char == '/' && second_char == '/') || // //
            (first_char == '*' && second_char == '*') || // **
            (first_char == '&' && second_char == '&') || // &&
            (first_char == '|' && second_char == '|')) { // ||
            int length =
                (third_char == '=' ? 3 : 2); // determine operator length
            char *lexeme = strndup(&state->source[state->pos], length);
            append_token(tokens, token_count, capacity, TOKEN_OPERATOR, lexeme,
                         state->line);
            free(lexeme);
            state->pos += length;
            return;
        }
    }

    // Handle single-character operators
    if (strchr("=<>!+-*/%.?", first_char)) {
        char lexeme[2] = {first_char, '\0'};
        append_token(tokens, token_count, capacity, TOKEN_OPERATOR, lexeme,
                     state->line);
        state->pos++;
    } else {
        fprintf(
            stderr,
            "Error: Unknown operator or invalid character `%c` on line %d\n",
            first_char, state->line);
        exit(1);
    }
}
