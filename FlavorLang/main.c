#include "tokenizer.h"

int main()
{
    char *source = read_file("test.flv");

    Token *tokens = tokenize(source);

    for (int i = 0; tokens[i].type != TOKEN_EOF; i++)
    {
        printf("Token Type: %d, Lexeme: %s, Line: %d\n", tokens[i].type, tokens[i].lexeme, tokens[i].line);
        free(tokens[i].lexeme);
    }

    free(tokens);
    free(source);
    return 0;
}
