#include "debug/debug.h"
#include "interpreter/interpreter.h"
#include "lexer/lexer.h"
#include "parser/parser.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Global flags
bool minify_flag = false;

// Parse command-line arguments
void parse_cli_args(int argc, char *argv[]) {
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--debug") == 0) {
            debug_flag = true;
        } else if (strcmp(argv[i], "--minify") == 0) {
            minify_flag = true;
        }
    }
}

// Print logo from a file
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

// Generate minified filename
char *generate_minified_filename(const char *input_filename) {
    // Find the last occurrence of '.'
    const char *dot = strrchr(input_filename, '.');
    if (!dot || dot == input_filename) {
        // No extension found, append .min.flv
        size_t len = strlen(input_filename) + 9; // ".min.flv" + null terminator
        char *min_filename = malloc(len);
        if (!min_filename) {
            perror("Memory allocation failed");
            exit(EXIT_FAILURE);
        }
        snprintf(min_filename, len, "%s.min.flv", input_filename);
        return min_filename;
    }

    // Calculate position to insert ".min"
    size_t basename_len = dot - input_filename;
    size_t min_filename_len =
        basename_len + 5 + strlen(dot) +
        1; // ".min" + original extension + null terminator
    char *min_filename = malloc(min_filename_len);
    if (!min_filename) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    // Copy the base name
    strncpy(min_filename, input_filename, basename_len);
    min_filename[basename_len] = '\0';

    // Append ".min" and the original extension
    snprintf(min_filename + basename_len, min_filename_len - basename_len,
             ".min%s", dot);

    return min_filename;
}

// Minify tokens
void minify_tokens(Token *tokens, const char *output_file) {
    FILE *output = fopen(output_file, "w");
    if (!output) {
        perror("Error opening output file");
        exit(EXIT_FAILURE);
    }

    Token *current = tokens;
    Token *next = tokens + 1;

    while (current->type != TOKEN_EOF) {

        // Handle different token types
        switch (current->type) {
        case TOKEN_STRING:
            fputc('"', output);             // Opening quote
            fputs(current->lexeme, output); // String content
            fputc('"', output);             // Closing quote
            break;

        default:
            fputs(current->lexeme, output);
            break;
        }

        // Add necessary spacing between tokens
        if (next->type != TOKEN_EOF) {
            // Space after keywords (e.g., `const`) before identifiers
            if (current->type == TOKEN_KEYWORD &&
                next->type == TOKEN_IDENTIFIER) {
                fputc(' ', output);
            }
            // Space between consecutive identifiers/literals (excluding
            // operators)
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
                fputc(' ', output);
            }
            // **Removed**: Conditions adding spaces around operators
        }

        current++;
        next++;
    }

    fclose(output);
}

// Print about information
void print_about(void) {
    printf("\n");
    print_logo_from_file("../logo/logo.txt");
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
    // Check for `--about` flag
    if (argc == 2 && strcmp(argv[1], "--about") == 0) {
        print_about();
        return 0; // Exit after printing "about" info
    }

    // Parse command-line arguments for flags
    parse_cli_args(argc, argv);

    // Validate input file arguments
    // Determine the position of the input filename
    int filename_index = -1;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--debug") != 0 &&
            strcmp(argv[i], "--minify") != 0) {
            filename_index = i;
            break;
        }
    }

    if (filename_index == -1) {
        fprintf(stderr, "Usage: `%s <file.flv> [--debug] [--minify]`\n",
                argv[0]);
        fprintf(stderr, "Reason: no `.flv` source file was provided\n");
        return 1;
    } else {
        // Check if there are multiple input files
        int input_file_count = 0;
        for (int i = 1; i < argc; i++) {
            if (strcmp(argv[i], "--debug") != 0 &&
                strcmp(argv[i], "--minify") != 0) {
                input_file_count++;
            }
        }
        if (input_file_count > 1) {
            fprintf(stderr, "Usage: `%s <file.flv> [--debug] [--minify]`\n",
                    argv[0]);
            fprintf(stderr, "Reason: FlavorLang currently only accepts one "
                            "`.flv` source file\n");
            return 1;
        }
    }

    const char *filename = argv[filename_index];

    // Read source `.flv` file
    char *source = read_file(filename);
    if (!source) {
        fprintf(stderr, "Error: Could not read file '%s'\n", filename);
        return 1;
    }

    // Tokenize
    Token *tokens = tokenize(source);
    if (!tokens) {
        fprintf(stderr, "Error: Tokenization failed\n");
        free(source);
        return 1;
    }

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

    // Minify if flag is set
    if (minify_flag) {
        char *minified_filename = generate_minified_filename(filename);
        minify_tokens(tokens, minified_filename);
        printf("Minified script written to '%s'\n", minified_filename);
        free(minified_filename);
    }

    // Clean up memory
    free(tokens);
    free(source);
    free_environment(&env);
    free_ast(ast);

    return 0;
}
