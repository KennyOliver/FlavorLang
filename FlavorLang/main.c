#include "tokenizer/tokenizer.h"
#include "parser/parser.h"
#include "interpreter/interpreter.h"
#include "debug/debug.h"

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: `%s <file.flv>`\n", argv[0]);
        fprintf(stderr, "Reason: no `.flv` source file was provided\n");
        return 1;
    }
    else if (argc > 3)
    {
        fprintf(stderr, "Usage: `%s <file.flv>`\n", argv[0]);
        fprintf(stderr, "Reason: FlavorLang currently only accepts one `.flv` source file\n");
        return 1;
    }

    parse_cli_args(argc, argv);

    // Get filename
    const char *filename = argv[1];

    // Read source `.flv` file
    char *source = read_file(filename);

    // Tokenize
    Token *tokens = tokenize(source);
    debug_print_tokens(tokens);
    debug_print_basic("Tokenization complete!\n\n");

    // Parse
    ASTNode *ast = parse_program(tokens);
    debug_print_basic("Parsing complete!\n\n");

    // Create environment
    Environment env;
    init_environment(&env);

    // Interpret
    interpret_program(ast, &env);
    debug_print_basic("Execution complete!\n");

    // Clean up memory
    free(tokens);
    free(source);
    free_environment(&env);
    free_ast(ast);

    return 0;
}
