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
