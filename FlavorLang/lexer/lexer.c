#include "lexer.h"

char *read_file(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        token_error("Could not open file", -1);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *buffer = malloc(length + 1);
    if (!buffer)
    {
        fclose(file);
        token_error("Out of memory while reading file", -1);
        return NULL;
    }

    size_t read_length = fread(buffer, 1, length, file);
    buffer[read_length] = '\0';
    fclose(file);

    return buffer;
}

Token *tokenize(const char *source)
{
    if (!source)
        return NULL;

    size_t pos = 0;
    size_t length = strlen(source);
    int line = 1;

    size_t capacity = INITIAL_TOKEN_CAPACITY;
    size_t token_count = 0;

    Token *tokens = malloc(sizeof(Token) * capacity);
    if (!tokens)
    {
        token_error("Failed to allocate memory for tokens", -1);
        return NULL;
    }

    while (pos < length)
    {
        char c = source[pos];

        if (is_whitespace(c))
        {
            if (c == '\n')
            {
                line++;
            }
            pos++;
            continue;
        }

        if (c == '#')
        {
            scan_comment(source, &pos);
            continue;
        }

        if (isdigit(c))
        {
            scan_number(source, &pos, length, &tokens, &token_count, &capacity,
                        line);
            continue;
        }

        if (c == '"')
        {
            scan_string(source, &pos, length, &tokens, &token_count, &capacity,
                        line);
            continue;
        }

        if (is_valid_identifier_start(c))
        {
            scan_identifier_or_keyword(source, &pos, length, &tokens, &token_count,
                                       &capacity, line);
            continue;
        }

        if (strchr("=+-*/<>", c))
        {
            scan_operator(source, &pos, length, &tokens, &token_count, &capacity,
                          line);
            continue;
        }

        if (strchr(",:;(){}", c))
        {
            if (c == '(' || c == '{')
            {
                // Peek previous token to check if it's an identifier
                if (token_count > 0 &&
                    tokens[token_count - 1].type == TOKEN_IDENTIFIER)
                {
                    // Retroactively convert identifier to function call
                    tokens[token_count - 1].type = TOKEN_FUNCTION_NAME;
                }
                append_token(&tokens, &token_count, &capacity, TOKEN_PAREN_OPEN,
                             strndup(&source[pos], 1), line);
            }
            else if (c == ')' || c == '}')
            {
                append_token(&tokens, &token_count, &capacity, TOKEN_PAREN_CLOSE,
                             strndup(&source[pos], 1), line);
            }
            else
            {
                append_token(&tokens, &token_count, &capacity, TOKEN_DELIMITER,
                             strndup(&source[pos], 1), line);
            }
            pos++;
            continue;
        }

        token_error("Unexpected character encountered", line);
    }

    append_token(&tokens, &token_count, &capacity, TOKEN_EOF, NULL, line);
    return tokens;
}
