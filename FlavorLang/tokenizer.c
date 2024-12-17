#include "tokenizer.h"

const char *KEYWORDS[] = {
    "let",
    ""};

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

    return tokens;
}
