#include "lexer.h"

char *read_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        token_error("Could not open file", -1);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *buffer = malloc(length + 1);
    if (!buffer) {
        fclose(file);
        token_error("Out of memory while reading file", -1);
        return NULL;
    }

    size_t read_length = fread(buffer, 1, length, file);
    buffer[read_length] = '\0';
    fclose(file);

    return buffer;
}

Token *tokenize(const char *source) {
    if (!source)
        return NULL;

    // Initialize ScannerState
    ScannerState state = {
        .source = source, .length = strlen(source), .pos = 0, .line = 1};

    size_t capacity = INITIAL_TOKEN_CAPACITY;
    size_t token_count = 0;

    Token *tokens = malloc(sizeof(Token) * capacity);
    if (!tokens) {
        token_error("Failed to allocate memory for tokens", -1);
        return NULL;
    }

    while (state.pos < state.length) {
        char c = state.source[state.pos];

        if (is_whitespace(c)) {
            if (c == '\n') {
                state.line++;
            }
            state.pos++;
            continue;
        }

        if (c == '#') {
            scan_comment(&state);
            continue;
        }

        if (isdigit(c)) {
            scan_number(&state, &tokens, &token_count, &capacity);
            continue;
        }

        if (c == '"') {
            scan_string(&state, &tokens, &token_count, &capacity);
            continue;
        }

        if (is_valid_identifier_start(c)) {
            scan_identifier_or_keyword(&state, &tokens, &token_count,
                                       &capacity);
            continue;
        }

        if (strchr("=+-*/<>", c)) {
            scan_operator(&state, &tokens, &token_count, &capacity);
            continue;
        }

        if (strchr(",:;(){}", c)) {
            if (c == '(') {
                // Peek previous token to check if it's an identifier for
                // function call
                if (token_count > 0 &&
                    tokens[token_count - 1].type == TOKEN_IDENTIFIER) {
                    // Convert identifier to function name
                    tokens[token_count - 1].type = TOKEN_FUNCTION_NAME;
                }
                append_token(&tokens, &token_count, &capacity, TOKEN_PAREN_OPEN,
                             strndup(&state.source[state.pos], 1), state.line);
            } else if (c == ')') {
                append_token(&tokens, &token_count, &capacity,
                             TOKEN_PAREN_CLOSE,
                             strndup(&state.source[state.pos], 1), state.line);
            } else if (c == '{') {
                append_token(&tokens, &token_count, &capacity, TOKEN_BRACE_OPEN,
                             strndup(&state.source[state.pos], 1), state.line);
            } else if (c == '}') {
                append_token(&tokens, &token_count, &capacity,
                             TOKEN_BRACE_CLOSE,
                             strndup(&state.source[state.pos], 1), state.line);
            } else {
                append_token(&tokens, &token_count, &capacity, TOKEN_DELIMITER,
                             strndup(&state.source[state.pos], 1), state.line);
            }
            state.pos++;
            continue;
        }

        token_error("Unexpected character encountered", state.line);
    }

    append_token(&tokens, &token_count, &capacity, TOKEN_EOF, NULL, state.line);
    return tokens;
}
