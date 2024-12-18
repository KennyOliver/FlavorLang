#include "tokenizer.h"
#include "parser.h"

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: `%s <file.flv>`\n", argv[0]);
        fprintf(stderr, "Reason: no `.flv` source file was provided\n");
        return 1;
    }
    else if (argc > 2)
    {
        fprintf(stderr, "Usage: `%s <file.flv>`\n", argv[0]);
        fprintf(stderr, "Reason: FlavorLang currently only accepts one `.flv` source file\n");
        return 1;
    }

    // Get filename
    const char *filename = argv[1];

    // Read source `.flv` file
    char *source = read_file(filename);

    // Tokenize
    Token *tokens = tokenize(source);
    print_tokens(tokens);
    printf("Tokenization complete.\n");

    // Parse
    ASTNode *ast = parse(tokens);
    printf("Parsing complete.\n");

    // Clean up memory
    free(tokens);
    free(source);
    free_ast(ast);

    return 0;
}
