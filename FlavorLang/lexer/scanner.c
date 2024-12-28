#include "scanner.h"

static void handle_comment(const char *source, size_t *pos)
{
    while (source[*pos] != '\0' && source[*pos] != '\n')
    {
        (*pos)++;
    }
}

static void handle_number(const char *source, size_t *pos, size_t length,
                          Token **tokens, size_t *token_count, size_t *capacity,
                          int line)
{
    size_t start = *pos;
    bool has_decimal_point = false;
    bool is_negative = false;

    // Check for negative sign
    if (source[*pos] == '-')
    {
        // Need to verify this is actually a negative number and not a subtraction
        // operator Look backwards for any previous token that would indicate this
        // is a number (start of input, opening paren, operator, etc.)
        bool is_start_of_expression = true;
        if (*token_count > 0)
        {
            TokenType prev_type = (*tokens)[*token_count - 1].type;
            if (prev_type == TOKEN_INTEGER || prev_type == TOKEN_FLOAT ||
                prev_type == TOKEN_IDENTIFIER || prev_type == TOKEN_STRING ||
                prev_type == TOKEN_PAREN_CLOSE)
            {
                // Likely a subtraction operator, not a negative sign
                is_start_of_expression = false;
            }
        }

        if (is_start_of_expression)
        {
            is_negative = true;
            (*pos)++; // move past the negative sign
        }
        else
        {
            // Handle this as an operator instead
            handle_operator(source, pos, length, tokens, token_count, capacity, line);
            return;
        }
    }

    // Verify there's at least one digit after the negative sign
    if (*pos >= length || !isdigit(source[*pos]))
    {
        if (is_negative)
        {
            (*pos) = start; // reset position
            handle_operator(source, pos, length, tokens, token_count, capacity, line);
            return;
        }
        token_error("Invalid number format", line);
    }

    // Handle digits before & after a potential decimal point
    while (*pos < length && (isdigit(source[*pos]) ||
                             (source[*pos] == '.' && !has_decimal_point)))
    {
        if (source[*pos] == '.')
        {
            if (*pos + 1 >= length || !isdigit(source[*pos + 1]))
            {
                // If there's no digit after the decimal point, treat the decimal as a
                // separate token
                break;
            }
            has_decimal_point = true;
        }
        (*pos)++;
    }

    char *lexeme = strndup(&source[start], *pos - start);
    TokenType type = has_decimal_point ? TOKEN_FLOAT : TOKEN_INTEGER;
    append_token(tokens, token_count, capacity, type, lexeme, line);
    free(lexeme);
}

static void handle_string(const char *source, size_t *pos, size_t length,
                          Token **tokens, size_t *token_count, size_t *capacity,
                          int line)
{
    size_t start = ++(*pos); // skip opening quote
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

static void handle_identifier_or_keyword(const char *source, size_t *pos,
                                         size_t length, Token **tokens,
                                         size_t *token_count, size_t *capacity,
                                         int line)
{
    size_t start = *pos;
    while (*pos < length && is_valid_identifier_char(source[*pos]))
    {
        (*pos)++;
    }

    char *lexeme = strndup(&source[start], *pos - start);

    // Skip whitespace to check for function call
    size_t temp_pos = *pos;
    while (temp_pos < length && isspace(source[temp_pos]))
        temp_pos++;

    if (temp_pos < length && source[temp_pos] == '(' && !is_keyword(lexeme))
    {
        append_token(tokens, token_count, capacity, TOKEN_FUNCTION_NAME, lexeme,
                     line);
    }
    else if (is_keyword(lexeme))
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
                            Token **tokens, size_t *token_count,
                            size_t *capacity, int line)
{
    if ((*pos < length - 1) &&
        ((source[*pos] == '=' && source[*pos + 1] == '=') ||
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