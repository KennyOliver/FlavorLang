#include "tokenizer.h"

const char *KEYWORDS[] = {
    "let",
    "if",
    "elif",
    "else",
    "for",
    "to",
    "while",
    "when",
    "is",
    "prep",
    "serve",
    "try",
    "crumbs",
    "burn",
    "scran",
    "taste",
    "plate",
    "garnish",
    "gather",
    "book"};

const char OPERATORS[] = {
    '=',
    '+',
    '-',
    '*',
    '/',
    '<',
    '>'};

char *read_file(const char *filename)
{
    // Open file in read mode
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        fprintf(stderr, "Error: Could not open file %s\n", filename);
        exit(1);
    }

    // Get file length
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *buffer = malloc(length + 1);
    if (!buffer)
    {
        fprintf(stderr, "Error: Out of memory\n");
        exit(1);
    }

    fread(buffer, 1, length, file);
    buffer[length] = '\0';
    fclose(file);
    return buffer;
}

Token *tokenize(const char *source)
{
    int line = 1;
    size_t pos = 0;
    size_t length = strlen(source);

    // Preallocate memory for tokens
    // 1024 is a magic number picked for tokenizing small-medium programs
    Token *tokens = malloc(sizeof(Token) * 1024);

    size_t token_count = 0;

    while (pos < length)
    {
        char c = source[pos];

        // Skip whitespace
        if (c == ' ' || c == '\t' || c == '\r')
        {
            pos++;
            continue;
        }

        // Newline
        if (c == '\n')
        {
            line++;
            pos++;
            continue;
        }

        // Comments (lines starting with '#')
        if (c == '#')
        {
            while (pos < length && source[pos] != '\n')
            {
                pos++;
            }
            continue;
        }

        // Numbers (ints only for now for simplicity)
        if (isdigit(c))
        {
            size_t start = pos;
            while (pos < length && isdigit(source[pos]))
            {
                pos++;
            }
            tokens[token_count] = (Token){
                TOKEN_NUMBER,
                strndup(&source[start], pos - start),
                line};
            token_count++;
            continue;
        }

        // Strings ('"')
        if (c == '"')
        {
            size_t start = ++pos;
            while (pos < length && source[pos] != '"')
            {
                // Stop at closing quote mark "
                pos++;
            }
            if (source[pos] != '"')
            {
                fprintf(stderr, "Error: Unterminated string on line %d\n", line);
                exit(1);
            }
            tokens[token_count] = (Token){
                TOKEN_STRING,
                strndup(&source[start], pos - start),
                line};
            token_count++;
            pos++; // skip closing quote mark "
            continue;
        }

        // Identifiers & Keywords
        if (isalpha(c) || c == '_')
        {
            size_t start = pos;
            while (pos < length && (isalnum(source[pos]) || source[pos] == '_'))
            {
                pos++;
            }
            char *lexeme = strndup(&source[start], pos - start);

            // Check if keyword
            int is_keyword = 0;
            for (int i = 0; KEYWORDS[i]; i++)
            {
                if (strcmp(lexeme, KEYWORDS[i]) == 0)
                {
                    is_keyword = 1;
                    tokens[token_count] = (Token){
                        TOKEN_KEYWORD,
                        lexeme,
                        line};
                    break;
                }
            }

            if (!is_keyword)
            {
                tokens[token_count] = (Token){
                    TOKEN_IDENTIFIER,
                    lexeme,
                    line};
                token_count++;
            }

            continue;
        }

        // Operators (+, -, *, /)
        if (strchr(OPERATORS, c))
        {
            tokens[token_count] = (Token){
                TOKEN_OPERATOR,
                strndup(&source[pos], 1),
                line};
            token_count++;
            pos++;
            continue;
        }

        // Delimiters (e.g., ';', '(', ')', etc.)
        if (c == ',' || c == ':' || c == ';' || c == '(' || c == ')')
        {
            tokens[token_count] = (Token){
                TOKEN_DELIMITER,
                strndup(&source[pos], 1),
                line};
            token_count++;
            pos++;
            continue;
        }

        // Unknown character
        fprintf(stderr, "Error: Unexpected character '%c', on line %d\n", c, line);
        exit(1);
    }

    // Add EOF token
    tokens[token_count] = (Token){
        TOKEN_EOF,
        NULL,
        line};

    return tokens;
}
