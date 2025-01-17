#include "main.h"

// Global variable to store the resolved plugin path
char resolved_plugin_path[PATH_MAX] = {0};

// Declare the embedded archive (generated by `xxd`)
extern unsigned char headers_tar_gz[];
extern unsigned int headers_tar_gz_len;

// Utility to write embedded header files to disk
void write_header_to_disk(const char *header_name, const char *content,
                          const char *output_dir) {
    char header_path[PATH_MAX];
    snprintf(header_path, sizeof(header_path), "%s/%s", output_dir,
             header_name);

    FILE *file = fopen(header_path, "w");
    if (!file) {
        perror("Error creating header file");
        exit(EXIT_FAILURE);
    }

    fprintf(file, "%s", content);
    fclose(file);
}

// Extract the embedded tarball to a directory
void extract_embedded_headers(const char *output_dir) {
    // Ensure output directory exists
    if (mkdir(output_dir, 0755) != 0 && errno != EEXIST) {
        perror("Failed to create temporary directory");
        exit(EXIT_FAILURE);
    }

    // Write the tarball to a temporary file
    char tar_path[PATH_MAX];
    snprintf(tar_path, sizeof(tar_path), "%s/headers.tar.gz", output_dir);
    FILE *tar_file = fopen(tar_path, "wb");
    if (!tar_file) {
        perror("Error writing embedded tarball");
        exit(EXIT_FAILURE);
    }
    fwrite(headers_tar_gz, 1, headers_tar_gz_len, tar_file);
    fclose(tar_file);

    // Extract the tarball with preserved directory structure
    char command[512];
    snprintf(command, sizeof(command), "tar -xzf %s -C %s", tar_path,
             output_dir);
    if (system(command) != 0) {
        fprintf(stderr, "Error extracting embedded headers.\n");
        unlink(tar_path);
        exit(EXIT_FAILURE);
    }

    // Clean up the tarball
    unlink(tar_path);
}

// Print usage instructions
void print_usage(const char *prog_name) {
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "  %s --help\n", prog_name);
    fprintf(stderr, "  %s --github\n", prog_name);
    fprintf(stderr, "  %s --about\n", prog_name);
    fprintf(stderr, "  %s <file.flv> [--debug | --minify]\n", prog_name);
    fprintf(stderr, "  %s <file.c> --make-plugin\n", prog_name);
    fprintf(stderr, "Note: Combining --debug and --minify is invalid.\n");
}

// Parse CLI arguments
void parse_cli_args(int argc, char *argv[], Options *options) {
    if (argc < 2) {
        fprintf(stderr, "Error: No source file or flag provided.\n");
        print_usage(argv[0]);
        exit(EXIT_FAILURE);
    }

    // Initialize flags
    debug_flag = false;
    options->minify = false;
    options->make_plugin = false;
    options->filename = NULL;

    // Process each argument
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            exit(EXIT_SUCCESS);
        } else if (strcmp(argv[i], "--github") == 0) {
            const char *github_repo_url =
                "https://github.com/KennyOliver/FlavorLang";
            open_url(github_repo_url);
            exit(EXIT_SUCCESS);
        } else if (strcmp(argv[i], "--about") == 0) {
            print_about();
            exit(EXIT_SUCCESS);
        } else if (strcmp(argv[i], "--debug") == 0) {
            if (options->minify || options->make_plugin) {
                fprintf(stderr, "Error: --debug cannot be combined with "
                                "--minify or --make-plugin.\n");
                print_usage(argv[0]);
                exit(EXIT_FAILURE);
            }
            debug_flag = true;
        } else if (strcmp(argv[i], "--minify") == 0) {
            if (debug_flag || options->make_plugin) {
                fprintf(stderr, "Error: --minify cannot be combined with "
                                "--debug or --make-plugin.\n");
                print_usage(argv[0]);
                exit(EXIT_FAILURE);
            }
            options->minify = true;
        } else if (strcmp(argv[i], "--make-plugin") == 0) {
            if (debug_flag || options->minify) {
                fprintf(stderr, "Error: --make-plugin cannot be combined with "
                                "--debug or --minify.\n");
                print_usage(argv[0]);
                exit(EXIT_FAILURE);
            }
            options->make_plugin = true;
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
        fprintf(stderr, "Error: No source file provided.\n");
        print_usage(argv[0]);
        exit(EXIT_FAILURE);
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

// Print FlavorLang logo
void print_logo(void) {
    for (int i = 0; FLAVOR_LOGO[i] != NULL; i++) {
        printf("%s", FLAVOR_LOGO[i]);
    }
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

// Open a URL in the user's default browser
void open_url(const char *url) {
#if defined(_WIN32) // for future Windows support
    ShellExecute(0, 0, url, 0, 0, SW_SHOW);
#elif defined(__APPLE__)
    char command[256];
    snprintf(command, sizeof(command), "open \"%s\"", url);
    system(command);
#elif defined(__linux__)
    char command[256];
    snprintf(command, sizeof(command), "xdg-open \"%s\"", url);
    system(command);
#else
    fprintf(stderr, "Error: Unsupported platform. Cannot open URL.\n");
    exit(EXIT_FAILURE);
#endif
}

int main(int argc, char **argv) {
    Options options;
    parse_cli_args(argc, argv, &options);

    char absolute_path[PATH_MAX];
    if (!realpath(options.filename, absolute_path)) {
        perror("Error obtaining absolute path");
        exit(EXIT_FAILURE);
    }

    char script_dir[PATH_MAX];
    strncpy(script_dir, absolute_path, PATH_MAX);
    char *last_slash = strrchr(script_dir, '/');
    if (last_slash) {
        *last_slash = '\0'; // NULL-terminate to get directory
    } else {
        fprintf(stderr, "Error: Could not determine script directory.\n");
        exit(EXIT_FAILURE);
    }

    const char *dot = strrchr(absolute_path, '.');
    if (!dot) {
        fprintf(stderr, "Error: Input file must have an extension.\n");
        exit(EXIT_FAILURE);
    }

    if (strcmp(dot, ".c") == 0) { // C plugin
        if (!options.make_plugin) {
            fprintf(stderr,
                    "Error: To build a plugin, specify --make-plugin.\n");
            exit(EXIT_FAILURE);
        }

        const char *temp_header_dir = "/tmp/flavor_headers";
        extract_embedded_headers(temp_header_dir);

        char output_filename[PATH_MAX];
        strncpy(output_filename, absolute_path, PATH_MAX);
        char *dot_ptr = strrchr(output_filename, '.');
        if (dot_ptr) {
            strcpy(dot_ptr, ".so");
        }

        // Construct the GCC command
        char command[1024];
        snprintf(
            command, sizeof(command),
            "gcc -fPIC -Wall -Wextra -O2 -shared -undefined dynamic_lookup "
            "-I%s -I%s/shared -I%s/interpreter -I%s/lexer -I%s/parser "
            "-I%s/debug "
            "-o %s %s",
            temp_header_dir, temp_header_dir, temp_header_dir, temp_header_dir,
            temp_header_dir, temp_header_dir, output_filename, absolute_path);

        debug_print_basic("Compiling plugin with command:\n%s\n", command);
        if (system(command) != 0) {
            fprintf(stderr, "Plugin compilation failed.\n");
            snprintf(command, sizeof(command), "rm -rf %s", temp_header_dir);
            system(command);
            exit(EXIT_FAILURE);
        }

        printf("Plugin compiled successfully: %s\n", output_filename);
        snprintf(resolved_plugin_path, sizeof(resolved_plugin_path), "%s",
                 output_filename);
        snprintf(command, sizeof(command), "rm -rf %s", temp_header_dir);
        system(command);

        return EXIT_SUCCESS;
    } else if (strcmp(dot, ".flv") == 0) { // .flv script
        snprintf(resolved_plugin_path, sizeof(resolved_plugin_path),
                 "%s/example_plugin.so", script_dir);
        debug_print_basic("Resolved plugin path: %s\n", resolved_plugin_path);

        void *plugin_handle = dlopen(resolved_plugin_path, RTLD_LAZY);
        if (!plugin_handle) {
            fprintf(stderr, "Error: dlopen error: %s\n", dlerror());
            fprintf(stderr, "Plugin file could not be found: %s\n",
                    resolved_plugin_path);
            exit(EXIT_FAILURE);
        }

        debug_print_basic("Plugin loaded successfully: %s\n",
                          resolved_plugin_path);

        // Read .flv script
        char *source = read_file(absolute_path);
        if (!source) {
            fprintf(stderr, "Error: Could not read file '%s'.\n",
                    absolute_path);
            exit(EXIT_FAILURE);
        }

        // Execute script
        Token *tokens = tokenize(source);
        ASTNode *ast = parse_program(tokens);
        Environment env;
        init_environment(&env);
        env.script_dir = strdup(script_dir);
        interpret_program(ast, &env);

        // Clean up memory
        free(tokens);
        free(source);
        free_environment(&env);
        free_ast(ast);

        return EXIT_SUCCESS;
    }

    fprintf(stderr, "Error: Unknown file extension.\n");
    return EXIT_FAILURE;
}
