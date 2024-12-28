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

    // Skip whitespace to check for function call
    size_t temp_pos = state->pos;
    while (temp_pos < state->length && isspace(state->source[temp_pos]))
        temp_pos++;

    if (temp_pos < state->length && state->source[temp_pos] == '(' &&
        !is_keyword(lexeme)) {
        append_token(tokens, token_count, capacity, TOKEN_FUNCTION_NAME, lexeme,
                     state->line);
    } else if (is_keyword(lexeme)) {
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

    // Check for two-character operators
    if (second_char != '\0' &&
        ((first_char == '=' && second_char == '=') ||  // ==
         (first_char == '>' && second_char == '=') ||  // >=
         (first_char == '<' && second_char == '=') ||  // <=
         (first_char == '!' && second_char == '='))) { // !=
        char *lexeme = strndup(&state->source[state->pos], 2);
        append_token(tokens, token_count, capacity, TOKEN_OPERATOR, lexeme,
                     state->line);
        free(lexeme);
        state->pos += 2;
    } else {
        // Handle single-character operators
        char *lexeme = strndup(&state->source[state->pos], 1);
        append_token(tokens, token_count, capacity, TOKEN_OPERATOR, lexeme,
                     state->line);
        free(lexeme);
        state->pos++;
    }
}
