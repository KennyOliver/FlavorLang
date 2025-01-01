#include "debug/debug.h"
#include "interpreter/interpreter.h"
#include "lexer/lexer.h"
#include "parser/parser.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void parse_cli_args(int argc, char *argv[]) {
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--debug") == 0) {
            debug_flag = true;
        }
    }
}

void print_logo_from_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    char buffer[256];
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        printf("%s", buffer);
    }

    fclose(file);
}

void print_about() {
    printf("\n");
    print_logo_from_file("../logo/logo.txt");
    const char *border =
        "+-------------------------------------------------+\n";
    printf("%s", border);
    printf("| FlavorLang                                      |\n");
    printf("%s", border);
    printf("| Copyright (c) 2024-2025, Kenneth Oliver.        |\n");
    printf("| All rights reserved.                            |\n");
    printf("%s", border);
    printf("| Licensed under the Apache License, Version 2.0. |\n");
    printf("| Visit https://github.com/KennyOliver/FlavorLang |\n");
    printf("| for more information.                           |\n");
    printf("%s", border);
}

int main(int argc, char **argv) {
    // Check for `--about` flag
    if (argc == 2 && strcmp(argv[1], "--about") == 0) {
        print_about();
        return 0; // Exit after printing "about" info
    }

    // Validate input file arguments
    if (argc < 2) {
        fprintf(stderr, "Usage: `%s <file.flv>`\n", argv[0]);
        fprintf(stderr, "Reason: no `.flv` source file was provided\n");
        return 1;
    } else if (argc > 3) {
        fprintf(stderr, "Usage: `%s <file.flv>`\n", argv[0]);
        fprintf(stderr, "Reason: FlavorLang currently only accepts one `.flv` "
                        "source file\n");
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
    debug_print_basic("Lexing complete!\n\n");

    // Parse
    ASTNode *ast = parse_program(tokens);
    debug_print_basic("Parsing complete!\n\n");
    print_ast(ast, 0);
    debug_print_basic("Finished printing AST\n\n");

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