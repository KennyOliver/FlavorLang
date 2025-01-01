// builtins.c
#include "builtins.h"
#include "../interpreter/interpreter.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Helper function to handle debug printing for floats
#define DEBUG_PRINT_FLOAT(format, ...)                                         \
    do {                                                                       \
        if (debug_flag)                                                        \
            printf(format, __VA_ARGS__);                                       \
    } while (0)

void print_formatted_string(const char *str) {
    for (const char *p = str; *p != '\0'; p++) {
        if (*p == '\\') {
            p++; // look at the next character
            switch (*p) {
            case 'n':
                putchar('\n');
                break;
            case 't':
                putchar('\t');
                break;
            case '\\':
                putchar('\\');
                break;
            case '"':
                putchar('"');
                break;
            default:
                putchar('\\'); // print the backslash
                putchar(*p);   // print the unrecognized character that follows
                break;
            }
        } else {
            putchar(*p); // print non-escape characters as-is
        }
    }
}

// Built-in `input()` function
LiteralValue builtin_input(ASTNode *node, Environment *env) {
    (void)node; // Unused parameter, suppress compiler warning
    (void)env;  // Unused parameter

    size_t buffer_size = 128;
    size_t input_length = 0;
    char *input_buffer = malloc(buffer_size);
    if (!input_buffer) {
        fprintf(stderr, "Error: Failed to allocate memory for input buffer.\n");
        return (LiteralValue){.type = TYPE_ERROR};
    }

    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
        if (input_length + 1 >= buffer_size) {
            buffer_size *= 2;
            char *new_buffer = realloc(input_buffer, buffer_size);
            if (!new_buffer) {
                fprintf(
                    stderr,
                    "Error: Failed to reallocate memory for input buffer.\n");
                free(input_buffer);
                return (LiteralValue){.type = TYPE_ERROR};
            }
            input_buffer = new_buffer;
        }
        input_buffer[input_length++] = c;
    }
    input_buffer[input_length] = '\0';

    LiteralValue result;
    result.type = TYPE_STRING;
    result.data.string = strdup(input_buffer);
    free(input_buffer);

    if (!result.data.string) {
        fprintf(stderr, "Error: Failed to duplicate input string.\n");
        return (LiteralValue){.type = TYPE_ERROR};
    }

    DEBUG_PRINT_FLOAT("Input received: `%s`\n", result.data.string);

    return result;
}

LiteralValue builtin_random() {
    srand(time(NULL));

    FLOAT_SIZE min = 0.0L;
    FLOAT_SIZE max = 1.0L;
    FLOAT_SIZE random_number =
        min + ((FLOAT_SIZE)rand() / (FLOAT_SIZE)RAND_MAX) * (max - min);

    debug_print_int("Random number (min: %d, max: %d) generated: `%f`\n", min,
                    max, random_number);

    LiteralValue result;
    result.type = TYPE_FLOAT;
    result.data.floating_point = random_number;

    return result;
}

// Built-in `serve()` function for printing
LiteralValue builtin_output(ASTNode *node, Environment *env) {
    ASTNode *arg_node = node->function_call.arguments;
    while (arg_node != NULL) {
        InterpretResult r = interpret_node(arg_node, env);
        LiteralValue lv = r.value;

        switch (lv.type) {
        case TYPE_FLOAT:
            if ((INT_SIZE)lv.data.floating_point == lv.data.floating_point) {
                printf("%.1Lf", lv.data.floating_point);
            } else {
                printf("%Lf", lv.data.floating_point);
            }
            break;
        case TYPE_INTEGER:
            printf("%lld", lv.data.integer);
            break;
        case TYPE_STRING:
            print_formatted_string(lv.data.string);
            break;
        case TYPE_BOOLEAN:
            printf("%s", lv.data.boolean ? "True" : "False");
            break;
        case TYPE_ERROR:
            fprintf(stderr, "Error: Invalid literal type in `serve()`.\n");
            break;
        default:
            fprintf(stderr, "Error: Unknown literal type in s`erve()`.\n");
            break;
        }
        printf(" "); // Space padding
        arg_node = arg_node->next;
    }
    printf("\n");

    return (LiteralValue){.type = TYPE_INTEGER,
                          .data.integer = 0}; // Return 0 as default
}

// Built-in `burn()` function to raise errors
LiteralValue builtin_error(ASTNode *node, Environment *env) {
    ASTNode *arg_node = node->function_call.arguments;
    char error_message[512] = "Error raised by burn(): ";

    while (arg_node != NULL) {
        InterpretResult r = interpret_node(arg_node, env);
        LiteralValue lv = r.value;

        switch (lv.type) {
        case TYPE_STRING:
            strncat(error_message, lv.data.string,
                    sizeof(error_message) - strlen(error_message) - 1);
            break;
        case TYPE_FLOAT: {
            char buffer[64];
            snprintf(buffer, sizeof(buffer), "%Lf", lv.data.floating_point);
            strncat(error_message, buffer,
                    sizeof(error_message) - strlen(error_message) - 1);
            break;
        }
        case TYPE_INTEGER: {
            char buffer[64];
            snprintf(buffer, sizeof(buffer), "%lld", lv.data.integer);
            strncat(error_message, buffer,
                    sizeof(error_message) - strlen(error_message) - 1);
            break;
        }
        case TYPE_BOOLEAN:
            strncat(error_message, lv.data.boolean ? "True" : "False",
                    sizeof(error_message) - strlen(error_message) - 1);
            break;
        case TYPE_ERROR:
            strncat(error_message, "Invalid argument type in `burn()`.",
                    sizeof(error_message) - strlen(error_message) - 1);
            break;
        default:
            strncat(error_message, "Unknown literal type in b`urn()`.",
                    sizeof(error_message) - strlen(error_message) - 1);
            break;
        }

        if (arg_node->next != NULL) {
            strncat(error_message, " ",
                    sizeof(error_message) - strlen(error_message) - 1);
        }

        arg_node = arg_node->next;
    }

    error_interpreter(error_message);

    return (LiteralValue){.type = TYPE_ERROR}; // keep compiler happy
}
