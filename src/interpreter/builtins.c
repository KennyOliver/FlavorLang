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

// Built-in `random()` function with 0, 1, or 2 arguments
LiteralValue builtin_random(ASTNode *node, Environment *env) {
    FLOAT_SIZE min = 0.0L;
    FLOAT_SIZE max = 1.0L;
    size_t arg_count = 0;

    ASTNode *arg_node = node->function_call.arguments;
    while (arg_node != NULL) {
        if (arg_count >= 2) {
            error_interpreter("`random()` takes at most 2 arguments.\n");
        }

        // Interpret the argument
        InterpretResult arg_res = interpret_node(arg_node, env);
        LiteralValue lv = arg_res.value;

        // Ensure the argument is numeric
        if (lv.type == TYPE_FLOAT) {
            if (arg_count == 0) {
                min = lv.data.floating_point;
            } else if (arg_count == 1) {
                max = lv.data.floating_point;
            }
        } else if (lv.type == TYPE_INTEGER) {
            if (arg_count == 0) {
                min = (FLOAT_SIZE)lv.data.integer;
            } else if (arg_count == 1) {
                max = (FLOAT_SIZE)lv.data.integer;
            }
        } else {
            error_interpreter("`random()` arguments must be numeric.\n");
        }

        arg_count++;
        arg_node = arg_node->next;
    }

    // Seed the random number generator once
    static bool seeded = false;
    if (!seeded) {
        srand((unsigned int)time(NULL));
        seeded = true;
    }

    // Swap min and max if min > max
    if (min > max) {
        FLOAT_SIZE temp = min;
        min = max;
        max = temp;
    }

    FLOAT_SIZE random_number =
        min + ((FLOAT_SIZE)rand() / (FLOAT_SIZE)RAND_MAX) * (max - min);

    DEBUG_PRINT_FLOAT("Random number generated (min: %Lf, max: %Lf): `%Lf`\n",
                      min, max, random_number);

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

bool is_valid_int(const char *str, INT_SIZE *out_value) {
    char *endptr;
    errno = 0; // reset errno before conversion
    long long temp = strtoll(str, &endptr, 10);

    // Check for conversion errors
    if (errno != 0 || endptr == str || *endptr != '\0') {
        return false;
    }

    // Optionally, check for overflow
    if (temp < LLONG_MIN || temp > LLONG_MAX) {
        return false;
    }

    if (out_value) {
        *out_value = (INT_SIZE)temp;
    }

    return true;
}

bool is_valid_float(const char *str, FLOAT_SIZE *out_value) {
    char *endptr;
    errno = 0; // reset errno before conversion
    long double temp = strtold(str, &endptr);

    // Check for conversion errors
    if (errno != 0 || endptr == str || *endptr != '\0') {
        return false;
    }

    if (out_value) {
        *out_value = (FLOAT_SIZE)temp;
    }

    return true;
}

LiteralValue builtin_cast(ASTNode *node, Environment *env) {
    char *cast_type = node->cast.cast_type;
    if (cast_type) {
        error_interpreter(
            "No cast type in node provided to `builtin_cast()`.\n");
    }
    ASTNode *expr = node->cast.expr;

    // Evaluate the expression to be casted
    InterpretResult expr_result = interpret_node(expr, env);
    LiteralValue original = expr_result.value;

    // If the expression resulted in a return, propagate it
    if (expr_result.did_return) {
        return original;
    }

    LiteralValue result;
    memset(&result, 0, sizeof(LiteralValue)); // Initialize result

    if (strcmp(cast_type, "string") == 0) {
        result.type = TYPE_STRING;
        char buffer[256] = {0};

        switch (original.type) {
        case TYPE_INTEGER:
            snprintf(buffer, sizeof(buffer), INT_FORMAT_SPECIFIER,
                     original.data.integer);
            result.data.string = strdup(buffer);
            break;
        case TYPE_FLOAT:
            snprintf(buffer, sizeof(buffer), FLOAT_FORMAT_SPECIFIER,
                     original.data.floating_point);
            result.data.string = strdup(buffer);
            break;
        case TYPE_BOOLEAN:
            result.data.string =
                strdup(original.data.boolean ? "True" : "False");
            break;
        case TYPE_STRING:
            result.data.string = strdup(original.data.string);
            break;
        default:
            error_interpreter("Unsupported type for string cast.\n");
        }

        if (!result.data.string) {
            error_interpreter("Memory allocation failed during string cast.\n");
        }

        debug_print_int("Casted value to string: `%s`\n", result.data.string);
    } else if (strcmp(cast_type, "int") == 0) {
        result.type = TYPE_INTEGER;

        switch (original.type) {
        case TYPE_STRING: {
            INT_SIZE temp;
            if (!is_valid_int(original.data.string, &temp)) {
                error_interpreter("Cannot cast string \"%s\" to int.\n",
                                  original.data.string);
            }
            result.data.integer = temp;
            break;
        }
        case TYPE_FLOAT:
            result.data.integer = (INT_SIZE)original.data.floating_point;
            break;
        case TYPE_BOOLEAN:
            result.data.integer = original.data.boolean ? 1 : 0;
            break;
        case TYPE_INTEGER:
            result.data.integer = original.data.integer;
            break;
        default:
            error_interpreter("Unsupported type for int cast.\n");
        }

        debug_print_int("Casted value to int: `%lld`\n", result.data.integer);
    } else if (strcmp(cast_type, "float") == 0) {
        result.type = TYPE_FLOAT;

        switch (original.type) {
        case TYPE_STRING: {
            FLOAT_SIZE temp;
            if (!is_valid_float(original.data.string, &temp)) {
                error_interpreter("Cannot cast string \"%s\" to float.\n",
                                  original.data.string);
            }
            result.data.floating_point = temp;
            break;
        }
        case TYPE_INTEGER:
            result.data.floating_point = (FLOAT_SIZE)original.data.integer;
            break;
        case TYPE_BOOLEAN:
            result.data.floating_point = original.data.boolean ? 1.0 : 0.0;
            break;
        case TYPE_FLOAT:
            result.data.floating_point = original.data.floating_point;
            break;
        default:
            error_interpreter("Unsupported type for float cast.\n");
        }

        debug_print_int("Casted value to float: `%Lf`\n",
                        result.data.floating_point);
    } else {
        error_interpreter("Unsupported cast type: `%s`\n", cast_type);
    }

    return result;
}
