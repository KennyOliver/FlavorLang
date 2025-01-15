#include "main.h"

// Print usage instructions
void print_usage(const char *prog_name) {
    fprintf(stderr, "Usage: `%s <file.flv> [--debug | --minify]`\n", prog_name);
    fprintf(stderr, "Note: Combining --debug and --minify is invalid.\n");
}

// Parse command-line arguments
void parse_cli_args(int argc, char *argv[], Options *options) {
    if (argc < 2) {
        fprintf(stderr, "Error: No `.flv` source file provided.\n");
        print_usage(argv[0]);
        exit(EXIT_FAILURE);
    }

    debug_flag = false;
    options->minify = false;
    options->filename = NULL;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--debug") == 0) {
            if (options->minify) {
                fprintf(stderr,
                        "Error: Cannot use `--debug` & `--minify` together.\n");
                print_usage(argv[0]);
                exit(EXIT_FAILURE);
            }
            debug_flag = true;
        } else if (strcmp(argv[i], "--minify") == 0) {
            if (debug_flag) {
                fprintf(stderr,
                        "Error: Cannot use `--debug` & `--minify` together.\n");
                print_usage(argv[0]);
                exit(EXIT_FAILURE);
            }
            options->minify = true;
        } else if (argv[i][0] == '-') {
            fprintf(stderr, "Error: Unknown option '%s'.\n", argv[i]);
            print_usage(argv[0]);
            exit(EXIT_FAILURE);
        } else {
            if (options->filename != NULL) {
                fprintf(stderr, "Error: Multiple input files provided.\n");
                print_usage(argv[0]);
                exit(EXIT_FAILURE);
            }
            options->filename = argv[i];
        }
    }

    if (options->filename == NULL) {
        fprintf(stderr, "Error: No `.flv` source file provided.\n");
        print_usage(argv[0]);
        exit(EXIT_FAILURE);
    }

    // Validate file extension
    const char *dot = strrchr(options->filename, '.');
    if (!dot || strcmp(dot, ".flv") != 0) {
        fprintf(stderr, "Error: Input file must have a `.flv` extension.\n");
        exit(EXIT_FAILURE);
    }
}

// Print FlavorLang logo
void print_logo(void) {
    for (int i = 0; FLAVOR_LOGO[i] != NULL; i++) {
        printf("%s", FLAVOR_LOGO[i]);
    }
}

// Generate minified filename
char *generate_minified_filename(const char *input_filename) {
    const char *dot = strrchr(input_filename, '.');
    size_t min_filename_len;

    if (!dot || dot == input_filename) {
        // No extension found, append .min.flv
        min_filename_len = strlen(input_filename) + strlen(".min.flv") + 1;
    } else {
        // Calculate position to insert ".min"
        size_t basename_len = dot - input_filename;
        min_filename_len = basename_len + strlen(".min") + strlen(dot) + 1;
    }

    char *min_filename = malloc(min_filename_len);
    if (!min_filename) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    if (!dot || dot == input_filename) {
        snprintf(min_filename, min_filename_len, "%s.min.flv", input_filename);
    } else {
        size_t basename_len = dot - input_filename;
        snprintf(min_filename, min_filename_len, "%.*s.min%s",
                 (int)basename_len, input_filename, dot);
    }

    return min_filename;
}

// Minify tokens
void minify_tokens(Token *tokens, const char *output_file) {
    FILE *output = fopen(output_file, "w");
    if (!output) {
        perror("Error opening output file for minification");
        exit(EXIT_FAILURE);
    }

    Token *current = tokens;
    Token *next = tokens + 1;

    while (current->type != TOKEN_EOF) {
        // Handle the current token
        switch (current->type) {
        case TOKEN_STRING:
            fputc('"', output);
            fputs(current->lexeme, output);
            fputc('"', output);
            break;
        default:
            fputs(current->lexeme, output);
            break;
        }

        // Add necessary spacing between tokens
        if (next->type != TOKEN_EOF) {
            bool add_space = false;

            // Always add space after keywords
            if (current->type == TOKEN_KEYWORD) {
                add_space = true;
            }
            // Space between identifiers/literals
            else if ((current->type == TOKEN_IDENTIFIER ||
                      current->type == TOKEN_STRING ||
                      current->type == TOKEN_INTEGER ||
                      current->type == TOKEN_FLOAT ||
                      current->type == TOKEN_BOOLEAN) &&
                     (next->type == TOKEN_IDENTIFIER ||
                      next->type == TOKEN_STRING ||
                      next->type == TOKEN_INTEGER ||
                      next->type == TOKEN_FLOAT ||
                      next->type == TOKEN_BOOLEAN)) {
                add_space = true;
            }
            // Handle operator spacing
            else if ((current->type == TOKEN_IDENTIFIER ||
                      current->type == TOKEN_STRING ||
                      current->type == TOKEN_INTEGER ||
                      current->type == TOKEN_FLOAT) &&
                     next->type == TOKEN_OPERATOR) {
                add_space = false;
            } else if (current->type == TOKEN_OPERATOR &&
                       (next->type == TOKEN_IDENTIFIER ||
                        next->type == TOKEN_STRING ||
                        next->type == TOKEN_INTEGER ||
                        next->type == TOKEN_FLOAT)) {
                add_space = false;
            }

            if (add_space) {
                fputc(' ', output);
            }
        }

        current++;
        next++;
    }

    fclose(output);
}

// Print about information
void print_about(void) {
    printf("\n");
    print_logo();
    printf("\n");
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
    // Handle --about flag separately
    if (argc == 2 && strcmp(argv[1], "--about") == 0) {
        print_about();
        return EXIT_SUCCESS;
    }

    // Parse command-line arguments
    Options options;
    parse_cli_args(argc, argv, &options);

    // Read source `.flv` file
    char *source = read_file(options.filename);
    if (!source) {
        fprintf(stderr, "Error: Could not read file '%s'\n", options.filename);
        return EXIT_FAILURE;
    }

    // Tokenize
    Token *tokens = tokenize(source);
    if (!tokens) {
        fprintf(stderr, "Error: Tokenization failed\n");
        free(source);
        return EXIT_FAILURE;
    }

    // If --minify flag is set, perform minification and exit
    if (options.minify) {
        char *minified_filename = generate_minified_filename(options.filename);
        minify_tokens(tokens, minified_filename);
        printf("Minified script written to '%s'\n", minified_filename);
        free(minified_filename);

        // Clean up memory
        free(tokens);
        free(source);
        return EXIT_SUCCESS;
    }

    // If --debug flag is set, print debug information
    if (debug_flag) {
        debug_print_tokens(tokens);
        debug_print_basic("Lexing complete!\n\n");
    }

    // Parse
    ASTNode *ast = parse_program(tokens);
    if (debug_flag) {
        debug_print_basic("Parsing complete!\n\n");
        print_ast(ast, 0);
        debug_print_basic("Finished printing AST\n\n");
    }

    // Create environment
    Environment env;
    init_environment(&env);

    // Interpret
    interpret_program(ast, &env);
    if (debug_flag) {
        debug_print_basic("Execution complete!\n");
    }

    // Clean up memory
    free(tokens);
    free(source);
    free_environment(&env);
    free_ast(ast);

    return EXIT_SUCCESS;
}
