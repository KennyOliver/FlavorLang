#include "tokenizer.h"
#include "keywords.h"
#include "token_utils.h"
#include <string.h>
#include <ctype.h>

// Forward declarations of internal functions
// static void handle_comment(const char *source, size_t *pos, int *line);
static void handle_comment(const char *source, size_t *pos);
static void handle_number(const char *source, size_t *pos, size_t length,
                          Token **tokens, size_t *token_count, size_t *capacity, int line);
static void handle_string(const char *source, size_t *pos, size_t length,
                          Token **tokens, size_t *token_count, size_t *capacity, int line);
static void handle_identifier_or_keyword(const char *source, size_t *pos, size_t length,
                                         Token **tokens, size_t *token_count, size_t *capacity, int line);
static void handle_operator(const char *source, size_t *pos, size_t length,
                            Token **tokens, size_t *token_count, size_t *capacity, int line);
void print_tokens(Token *tokens);

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
            // handle_comment(source, &pos, &line);
            handle_comment(source, &pos);
            continue;
        }

        if (isdigit(c))
        {
            handle_number(source, &pos, length, &tokens, &token_count, &capacity, line);
            continue;
        }

        if (c == '"')
        {
            handle_string(source, &pos, length, &tokens, &token_count, &capacity, line);
            continue;
        }

        if (is_valid_identifier_start(c))
        {
            handle_identifier_or_keyword(source, &pos, length, &tokens, &token_count, &capacity, line);
            continue;
        }

        if (strchr("=+-*/<>", c))
        {
            handle_operator(source, &pos, length, &tokens, &token_count, &capacity, line);
            continue;
        }

        if (strchr(",:;()", c))
        {
            append_token(&tokens, &token_count, &capacity, TOKEN_DELIMITER, strndup(&source[pos], 1), line);
            pos++;
            continue;
        }

        token_error("Unexpected character encountered", line);
    }

    append_token(&tokens, &token_count, &capacity, TOKEN_EOF, NULL, line);
    return tokens;
}

// static void handle_comment(const char *source, size_t *pos, int *line)
static void handle_comment(const char *source, size_t *pos)
{
    while (source[*pos] != '\0' && source[*pos] != '\n')
    {
        (*pos)++;
    }
}

static void handle_number(const char *source, size_t *pos, size_t length,
                          Token **tokens, size_t *token_count, size_t *capacity, int line)
{
    size_t start = *pos;
    while (*pos < length && isdigit(source[*pos]))
    {
        (*pos)++;
    }
    char *lexeme = strndup(&source[start], *pos - start);
    append_token(tokens, token_count, capacity, TOKEN_NUMBER, lexeme, line);
    free(lexeme);
}

static void handle_string(const char *source, size_t *pos, size_t length,
                          Token **tokens, size_t *token_count, size_t *capacity, int line)
{
    size_t start = ++(*pos); // Skip opening quote
    while (*pos < length && source[*pos] != '"')
    {
        (*pos)++;
    }
    if (*pos >= length || source[*pos] != '"')
    {
        token_error("Unterminated string literal", line);
    }
    char *lexeme = strndup(&source[start], *pos - start);
    append_token(tokens, token_count, capacity, TOKEN_STRING, lexeme, line);
    free(lexeme);
    (*pos)++; // Skip closing quote
}

static void handle_identifier_or_keyword(const char *source, size_t *pos, size_t length,
                                         Token **tokens, size_t *token_count, size_t *capacity, int line)
{
    size_t start = *pos;
    while (*pos < length && is_valid_identifier_char(source[*pos]))
    {
        (*pos)++;
    }
    char *lexeme = strndup(&source[start], *pos - start);
    if (is_keyword(lexeme))
    {
        append_token(tokens, token_count, capacity, TOKEN_KEYWORD, lexeme, line);
    }
    else
    {
        append_token(tokens, token_count, capacity, TOKEN_IDENTIFIER, lexeme, line);
    }
    free(lexeme);
}

static void handle_operator(const char *source, size_t *pos, size_t length,
                            Token **tokens, size_t *token_count, size_t *capacity, int line)
{
    if ((*pos < length - 1) && ((source[*pos] == '=' && source[*pos + 1] == '=') ||
                                (source[*pos] == '>' && source[*pos + 1] == '=') ||
                                (source[*pos] == '<' && source[*pos + 1] == '=')))
    {
        char *lexeme = strndup(&source[*pos], 2);
        append_token(tokens, token_count, capacity, TOKEN_OPERATOR, lexeme, line);
        free(lexeme);
        *pos += 2;
    }
    else
    {
        char *lexeme = strndup(&source[*pos], 1);
        append_token(tokens, token_count, capacity, TOKEN_OPERATOR, lexeme, line);
        free(lexeme);
        (*pos)++;
    }
}

void print_tokens(Token *tokens)
{
    for (int i = 0; tokens[i].type != TOKEN_EOF; i++)
    {
        printf("Token Type: %d, Lexeme: %s, Line: %d\n", tokens[i].type, tokens[i].lexeme, tokens[i].line);
    }
}
