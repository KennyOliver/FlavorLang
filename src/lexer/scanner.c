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
                prev_type == TOKEN_PAREN_CLOSE ||
                prev_type == TOKEN_SQ_BRACKET_CLOSE) {
                is_start_of_expression = false;
            }
        }

        if (is_start_of_expression) {
            is_negative = true;
            state->pos++; // Move past the negative sign
        } else {
            // The '-' is not part of a negative number; treat it as an operator
            scan_operator(state, tokens, token_count, capacity);
            return;
        }
    }

    // After handling '-', the next character must be a digit
    if (state->pos >= state->length || !isdigit(state->source[state->pos])) {
        if (is_negative) {
            // The '-' was not followed by a digit; treat it as an operator
            state->pos = start; // Reset position to include '-'
            scan_operator(state, tokens, token_count, capacity);
            return;
        }
        token_error("Invalid number format", state->line);
    }

    // Continue scanning digits and possible decimal point
    while (state->pos < state->length &&
           (isdigit(state->source[state->pos]) ||
            (state->source[state->pos] == '.' && !has_decimal_point))) {
        if (state->source[state->pos] == '.') {
            if (state->pos + 1 >= state->length ||
                !isdigit(state->source[state->pos + 1])) {
                break; // If there's no digit after '.', treat '.' as a separate
                       // token
            }
            has_decimal_point = true;
        }
        state->pos++;
    }

    // Calculate the end position
    size_t end = state->pos;

    // Allocate and construct the lexeme
    char *lexeme;
    if (is_negative) {
        // Allocate space for '-' and the number
        size_t lexeme_length = end - start;
        lexeme = malloc(lexeme_length + 1); // +1 for null terminator
        if (!lexeme) {
            token_error("Memory allocation failed for lexeme", state->line);
        }
        lexeme[0] = '-';
        // Copy the number part after '-'
        strncpy(lexeme + 1, &state->source[start + 1], lexeme_length - 1);
        lexeme[lexeme_length] = '\0';
    } else {
        // Positive number; no '-' sign
        lexeme = strndup(&state->source[start], end - start);
        if (!lexeme) {
            token_error("Memory allocation failed for lexeme", state->line);
        }
    }

    // Determine the token type based on the presence of a decimal point
    TokenType type = has_decimal_point ? TOKEN_FLOAT : TOKEN_INTEGER;

    // Append the token to the tokens array
    append_token(tokens, token_count, capacity, type, lexeme, state->line);

    // Free the lexeme if append_token duplicates it
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

            // Handle comments starting with `#`
            if (inner_c == '#') {
                scan_comment(state);
                // After scan_comment, current character is either '\n' or '\0'
                if (state->pos < state->length &&
                    state->source[state->pos] == '\n') {
                    state->line++;
                    state->pos++; // Move past the newline
                }
                continue;
            }

            // Handle nested array
            if (inner_c == '[') {
                // Recursively scan the nested array
                scan_array(state, tokens, token_count, capacity);
                continue;
            }

            // Handle string literals
            if (inner_c == '"') {
                scan_string(state, tokens, token_count, capacity);
                continue;
            }

            // Handle special array operations
            if (inner_c == '^') {
                // For '^', check if the next char forms a two-character array
                // operator (e.g., "^+", "^-", "^^")
                char next_c = (state->pos + 1 < state->length)
                                  ? state->source[state->pos + 1]
                                  : '\0';

                // Handle two-character array operators (e.g., ^+, +^, ^-, -^)
                if (next_c == '^' || next_c == '+' || next_c == '-') {
                    // Two-character array operator
                    char *op = strndup(&state->source[state->pos], 2);
                    append_token(tokens, token_count, capacity, TOKEN_ARRAY_OP,
                                 op, state->line);
                    free(op);
                    state->pos += 2; // Move past the two-character operator
                    continue;
                } else {
                    // If '^' appears alone, treat it as a normal operator
                    scan_operator(state, tokens, token_count, capacity);
                    continue;
                }
            } else if (inner_c == '+' || inner_c == '-') {
                char next_c = (state->pos + 1 < state->length)
                                  ? state->source[state->pos + 1]
                                  : '\0';
                // Check if '-' is starting a negative number
                if (inner_c == '-' && isdigit(next_c)) {
                    scan_number(state, tokens, token_count, capacity);
                    continue;
                }
                // If '+' or '-' is followed by '^', then it forms a
                // two-character array operator (like "+^" or "-^")
                if (next_c == '^') {
                    char *op = strndup(&state->source[state->pos], 2);
                    append_token(tokens, token_count, capacity, TOKEN_ARRAY_OP,
                                 op, state->line);
                    free(op);
                    state->pos += 2; // Move past the two-character operator
                    continue;
                } else {
                    // Otherwise, treat it as a normal operator
                    scan_operator(state, tokens, token_count, capacity);
                    continue;
                }
            }

            // Handle identifiers
            if (is_valid_identifier_start(inner_c)) {
                scan_identifier_or_keyword(state, tokens, token_count,
                                           capacity);
                // After scanning identifier, check for '(' to identify function
                // calls
                if (state->pos < state->length &&
                    state->source[state->pos] == '(') {
                    // Convert the last token to TOKEN_FUNCTION_NAME
                    if (*token_count > 0) {
                        (*tokens)[*token_count - 1].type = TOKEN_FUNCTION_NAME;
                    }
                    // Append the '(' token
                    append_token(tokens, token_count, capacity,
                                 TOKEN_PAREN_OPEN, "(", state->line);
                    state->pos++;
                }
                continue;
            }

            // Handle numbers (including negative numbers) after array operators
            if (isdigit(inner_c) ||
                (inner_c == '-' && state->pos + 1 < state->length &&
                 isdigit(state->source[state->pos + 1]))) {
                scan_number(state, tokens, token_count, capacity);
                continue;
            }

            // Handle parentheses
            if (inner_c == '(') {
                append_token(tokens, token_count, capacity, TOKEN_PAREN_OPEN,
                             "(", state->line);
                state->pos++;
                continue;
            }
            if (inner_c == ')') {
                append_token(tokens, token_count, capacity, TOKEN_PAREN_CLOSE,
                             ")", state->line);
                state->pos++;
                continue;
            }

            // Handle separators `,`
            if (inner_c == ',') {
                append_token(tokens, token_count, capacity, TOKEN_DELIMITER,
                             ",", state->line);
                state->pos++;
                continue;
            }

            // Handle slicing syntax `:`
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
    }
}

void scan_string(ScannerState *state, Token **tokens, size_t *token_count,
                 size_t *capacity) {
    size_t start = ++(state->pos); // Skip the opening quote
    bool is_escaped = false;

    while (state->pos < state->length) {
        char current_char = state->source[state->pos];

        if (is_escaped) {
            // Skip the current character as it's escaped
            is_escaped = false;
            state->pos++;
            continue;
        }

        if (current_char == '\\') {
            is_escaped = true;
            state->pos++;
            continue;
        }

        if (current_char == '"') {
            break; // End of string
        }

        // Handle newline characters within strings if allowed
        if (current_char == '\n') {
            token_error("Unterminated string literal (newline encountered)",
                        state->line);
        }

        state->pos++;
    }

    if (state->pos >= state->length || state->source[state->pos] != '"') {
        token_error("Unterminated string literal", state->line);
    }

    // Extract the string content, handling escape sequences as needed
    size_t lexeme_length = state->pos - start;
    char *lexeme = malloc(lexeme_length + 1); // +1 for null terminator
    if (!lexeme) {
        token_error("Memory allocation failed for string lexeme", state->line);
    }

    size_t lexeme_pos = 0;
    is_escaped = false;
    for (size_t i = start; i < state->pos; i++) {
        char c = state->source[i];
        if (is_escaped) {
            switch (c) {
            case 'n':
                lexeme[lexeme_pos++] = '\n';
                break;
            case 't':
                lexeme[lexeme_pos++] = '\t';
                break;
            case '\\':
                lexeme[lexeme_pos++] = '\\';
                break;
            case '"':
                lexeme[lexeme_pos++] = '"';
                break;
            default:
                // If the escape sequence is unrecognized, keep the backslash
                lexeme[lexeme_pos++] = '\\';
                lexeme[lexeme_pos++] = c;
                break;
            }
            is_escaped = false;
        } else if (c == '\\') {
            is_escaped = true;
        } else {
            lexeme[lexeme_pos++] = c;
        }
    }

    lexeme[lexeme_pos] = '\0'; // Null-terminate the string

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
