#include "tokenizer.h"
#include "parser.h"

int main()
{
    char *source = read_file("test.flv");

    Token *tokens = tokenize(source);
    printf("Tokenization complete.\n");

    // for (int i = 0; tokens[i].type != TOKEN_EOF; i++)
    // {
    //     printf("Token Type: %d, Lexeme: %s, Line: %d\n", tokens[i].type, tokens[i].lexeme, tokens[i].line);
    //     free(tokens[i].lexeme);
    // }

    ASTNode *ast = parse(tokens);
    printf("Parsing complete.\n");

    free(tokens);
    free(source);
    free_ast(ast);

    return 0;
}
