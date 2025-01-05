#include "builtins.h"
#include "../interpreter/interpreter.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Function to interpret a mix of argument types
InterpretResult interpret_arguments(ASTNode *node, Environment *env,
                                    size_t num_args, ArgumentSpec *specs) {
    ASTNode *arg_node = node;

    for (size_t i = 0; i < num_args; i++) {
        if (arg_node == NULL) {
            return raise_error("Too few arguments provided.\n");
        }

        // Interpret the current argument
        InterpretResult arg_res = interpret_node(arg_node, env);
        if (arg_res.is_error) {
            // Propagate the error
            return arg_res;
        }
        LiteralValue lv = arg_res.value;

        // Reference to the current argument specification
        ArgType expected_type = specs[i].type;
        void *output_ptr = specs[i].out_ptr;

        // Verify and assign the argument based on its expected type
        switch (expected_type) {
        case ARG_TYPE_INTEGER:
            if (lv.type == TYPE_INTEGER) {
                *((INT_SIZE *)output_ptr) = lv.data.integer;
            } else if (lv.type == TYPE_FLOAT) {
                *((INT_SIZE *)output_ptr) = (INT_SIZE)lv.data.floating_point;
            } else if (lv.type == TYPE_BOOLEAN) {
                *((INT_SIZE *)output_ptr) = lv.data.boolean ? 1 : 0;
            } else {
                return raise_error("Expected integer for argument %zu.\n",
                                   i + 1);
            }
            break;

        case ARG_TYPE_FLOAT:
            if (lv.type == TYPE_FLOAT) {
                *((FLOAT_SIZE *)output_ptr) = lv.data.floating_point;
            } else if (lv.type == TYPE_INTEGER) {
                *((FLOAT_SIZE *)output_ptr) = (FLOAT_SIZE)lv.data.integer;
            } else if (lv.type == TYPE_BOOLEAN) {
                *((FLOAT_SIZE *)output_ptr) = lv.data.boolean ? 1.0 : 0.0;
            } else {
                return raise_error("Expected float for argument %zu.\n", i + 1);
            }
            break;

        case ARG_TYPE_STRING:
            if (lv.type == TYPE_STRING) {
                *((char **)output_ptr) = lv.data.string;
            } else {
                return raise_error("Expected string for argument %zu.\n",
                                   i + 1);
            }
            break;

        case ARG_TYPE_BOOLEAN:
            if (lv.type == TYPE_BOOLEAN) {
                *((bool *)output_ptr) = lv.data.boolean;
            } else if (lv.type == TYPE_INTEGER) {
                *((bool *)output_ptr) = (lv.data.integer != 0);
            } else if (lv.type == TYPE_FLOAT) {
                *((bool *)output_ptr) = (lv.data.floating_point != 0.0);
            } else {
                return raise_error("Expected boolean for argument %zu.\n",
                                   i + 1);
            }
            break;

        default:
            return raise_error("Unknown argument type for argument %zu.\n",
                               i + 1);
        }

        arg_node = arg_node->next;
    }

    if (arg_node != NULL) {
        return raise_error("Too many arguments provided.\n");
    }

    // Indicate success
    LiteralValue success_val = {.type = TYPE_BOOLEAN, .data.boolean = true};
    return make_result(success_val, false, false);
}

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
InterpretResult builtin_input(ASTNode *node, Environment *env) {
    (void)node; // Unused parameter, suppress compiler warning
    (void)env;  // Unused parameter

    size_t buffer_size = 128;
    size_t input_length = 0;
    char *input_buffer = malloc(buffer_size);
    if (!input_buffer) {
        fprintf(stderr, "Error: Failed to allocate memory for input buffer.\n");
        LiteralValue lv = (LiteralValue){.type = TYPE_ERROR};
        return make_result(lv, false, false);
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
                LiteralValue lv = (LiteralValue){.type = TYPE_ERROR};
                return make_result(lv, false, false);
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
        LiteralValue lv = (LiteralValue){.type = TYPE_ERROR};
        return make_result(lv, false, false);
    }

    debug_print_int("Input received: `%s`\n", result.data.string);

    return make_result(result, false, false);
}

// Built-in `random()` function with 0, 1, or 2 arguments
InterpretResult builtin_random(ASTNode *node, Environment *env) {
    FLOAT_SIZE min = 0.0L; // default min
    FLOAT_SIZE max = 1.0L; // default max

    ASTNode *arg_node = node->function_call.arguments;

    size_t num_args = 0;
    ASTNode *temp = arg_node;
    while (temp) {
        num_args++;
        temp = temp->next;
    }
    if (num_args > 2) {
        return raise_error(
            "`random()` takes at most 2 arguments, but %zu provided.\n",
            num_args);
    }

    if (num_args == 1) {
        // One argument provided: set max, min remains 0.0
        ArgumentSpec specs[1];
        specs[0].type = ARG_TYPE_FLOAT;
        specs[0].out_ptr = &max;

        InterpretResult args_res = interpret_arguments(arg_node, env, 1, specs);
        if (args_res.is_error) {
            return args_res;
        }
    } else if (num_args == 2) {
        // Two arguments provided: set min and max
        ArgumentSpec specs[2];
        specs[0].type = ARG_TYPE_FLOAT;
        specs[0].out_ptr = &min;
        specs[1].type = ARG_TYPE_FLOAT;
        specs[1].out_ptr = &max;

        InterpretResult args_res = interpret_arguments(arg_node, env, 2, specs);
        if (args_res.is_error) {
            return args_res;
        }
    }

    // Seed the random number generator once
    static bool seeded = false;
    if (!seeded) {
        srand((unsigned int)time(NULL));
        seeded = true;
    }

    // Swap min & max if min > max to ensure correct range
    if (min > max) {
        FLOAT_SIZE temp_val = min;
        min = max;
        max = temp_val;
    }

    // Generate random number
    FLOAT_SIZE random_number =
        min + ((FLOAT_SIZE)rand() / (FLOAT_SIZE)RAND_MAX) * (max - min);

    debug_print_int("Random number generated (min: %Lf, max: %Lf): `%Lf`\n",
                    min, max, random_number);

    LiteralValue result;
    result.type = TYPE_FLOAT;
    result.data.floating_point = random_number;

    return make_result(result, false, false);
}

// Built-in `serve()` function for printing
InterpretResult builtin_output(ASTNode *node, Environment *env) {
    debug_print_int("builtin_output() called\n");

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
            fprintf(
                stderr,
                "Error: Invalid literal type in `serve()` (`TYPE_ERROR`).\n");
            break;
        default:
            fprintf(stderr, "Error: Unknown literal type in `serve()`.\n");
            break;
        }
        printf(" "); // Space padding
        arg_node = arg_node->next;
    }
    printf("\n");

    LiteralValue lv = {.type = TYPE_INTEGER,
                       .data.integer = 0}; // return 0 as default
    return make_result(lv, false, false);
}

// Built-in `burn()` function to raise errors
InterpretResult builtin_error(ASTNode *node, Environment *env) {
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
            strncat(error_message, "Unknown literal type in `burn()`.",
                    sizeof(error_message) - strlen(error_message) - 1);
            break;
        }

        if (arg_node->next != NULL) {
            strncat(error_message, " ",
                    sizeof(error_message) - strlen(error_message) - 1);
        }

        arg_node = arg_node->next;
    }

    // Propagate the exception
    return raise_error("%s", error_message);
}

InterpretResult builtin_cast(ASTNode *node, Environment *env) {
    if (node->type != AST_FUNCTION_CALL) {
        return raise_error(
            "`builtin_cast()` expects an `AST_FUNCTION_CALL` node.\n");
    }

    char *cast_type = strdup(node->function_call.name);
    if (!cast_type) {
        return raise_error("No cast type provided to `builtin_cast()`.\n");
    }

    ASTNode *arg_node = node->function_call.arguments;
    if (!arg_node) {
        return raise_error(
            "No expression provided for cast in `builtin_cast()`.\n");
    }

    // Ensure there's only one argument
    if (arg_node->next != NULL) {
        return raise_error("`%s` cast function takes exactly one argument.\n",
                           cast_type);
    }

    ASTNode *expr = arg_node;

    // Interpret the expression to be casted
    InterpretResult expr_result = interpret_node(expr, env);
    if (expr_result.is_error) {
        return expr_result; // Propagate the error
    }

    LiteralValue original = expr_result.value;

    // Initialize the result
    InterpretResult result_res = {0};
    result_res.did_return = false;
    result_res.did_break = false;
    result_res.is_error = false;

    // Perform the cast based on `cast_type`
    if (strcmp(cast_type, "string") == 0) {
        LiteralValue cast_val;
        cast_val.type = TYPE_STRING;
        char buffer[256] = {0};

        switch (original.type) {
        case TYPE_INTEGER:
            snprintf(buffer, sizeof(buffer), INT_FORMAT_SPECIFIER,
                     original.data.integer);
            cast_val.data.string = strdup(buffer);
            break;
        case TYPE_FLOAT:
            snprintf(buffer, sizeof(buffer), FLOAT_FORMAT_SPECIFIER,
                     original.data.floating_point);
            cast_val.data.string = strdup(buffer);
            break;
        case TYPE_BOOLEAN:
            cast_val.data.string =
                strdup(original.data.boolean ? "True" : "False");
            break;
        case TYPE_STRING:
            cast_val.data.string = strdup(original.data.string);
            break;
        default:
            return raise_error("Unsupported type for string cast.\n");
        }

        if (!cast_val.data.string) {
            return raise_error(
                "Memory allocation failed during string cast.\n");
        }

        result_res.value = cast_val;
    } else if (strcmp(cast_type, "int") == 0) {
        LiteralValue cast_val;
        cast_val.type = TYPE_INTEGER;

        switch (original.type) {
        case TYPE_STRING: {
            INT_SIZE temp;
            if (!is_valid_int(original.data.string, &temp)) {
                return raise_error("Cannot cast string \"%s\" to int.\n",
                                   original.data.string);
            }
            cast_val.data.integer = temp;
            break;
        }
        case TYPE_FLOAT:
            cast_val.data.integer = (INT_SIZE)original.data.floating_point;
            break;
        case TYPE_BOOLEAN:
            cast_val.data.integer = original.data.boolean ? 1 : 0;
            break;
        case TYPE_INTEGER:
            cast_val.data.integer = original.data.integer;
            break;
        default:
            return raise_error("Unsupported type for int cast.\n");
        }

        result_res.value = cast_val;
    } else if (strcmp(cast_type, "float") == 0) {
        LiteralValue cast_val;
        cast_val.type = TYPE_FLOAT;

        switch (original.type) {
        case TYPE_STRING: {
            FLOAT_SIZE temp;
            if (!is_valid_float(original.data.string, &temp)) {
                return raise_error("Cannot cast string \"%s\" to float.\n",
                                   original.data.string);
            }
            cast_val.data.floating_point = temp;
            break;
        }
        case TYPE_INTEGER:
            cast_val.data.floating_point = (FLOAT_SIZE)original.data.integer;
            break;
        case TYPE_BOOLEAN:
            cast_val.data.floating_point = original.data.boolean ? 1.0 : 0.0;
            break;
        case TYPE_FLOAT:
            cast_val.data.floating_point = original.data.floating_point;
            break;
        default:
            return raise_error("Unsupported type for float cast.\n");
        }

        result_res.value = cast_val;
    } else {
        return raise_error("Unsupported cast type: `%s`\n", cast_type);
    }

    return result_res;
}

InterpretResult builtin_time(void) {
    time_t current_time = time(NULL);

    if (current_time == -1) {
        return raise_error("Failed to get the current time\n");
    }

    LiteralValue time_val = {.type = TYPE_INTEGER,
                             .data.integer = (INT_SIZE)current_time};

    return make_result(time_val, false, false);
}

char *process_escape_sequences(const char *input) {
    size_t length = strlen(input);
    char *processed = malloc(length + 1);
    if (!processed) {
        perror("Failed to allocate memory for processed string");
        return NULL;
    }

    char *dst = processed;
    const char *src = input;

    while (*src) {
        if (*src == '\\' && *(src + 1)) {
            src++;
            switch (*src) {
            case 'n':
                *dst++ = '\n';
                break;
            case 't':
                *dst++ = '\t';
                break;
            case '\\':
                *dst++ = '\\';
                break;
            case 'r':
                *dst++ = '\r';
                break;
            case '\"':
                *dst++ = '\"';
                break;
            case '\'':
                *dst++ = '\'';
                break;
            default:
                *dst++ = '\\';
                *dst++ = *src;
                break;
            }
        } else {
            *dst++ = *src;
        }
        src++;
    }
    *dst = '\0'; // NULL-terminate the string
    return processed;
}

InterpretResult builtin_file_read(ASTNode *node, Environment *env) {
    char *filepath;

    ArgumentSpec specs[1];
    specs[0].type = ARG_TYPE_STRING;
    specs[0].out_ptr = &filepath;

    InterpretResult args_res =
        interpret_arguments(node->function_call.arguments, env, 1, specs);
    if (args_res.is_error) {
        LiteralValue lv = (LiteralValue){.type = TYPE_ERROR};
        return make_result(lv, false, false);
    }

    FILE *file = fopen(filepath, "r");
    if (file == NULL) {
        perror("Failed to open file");
        LiteralValue lv = (LiteralValue){.type = TYPE_ERROR};
        return make_result(lv, false, false);
    }

    size_t buffer_size = 1024;
    size_t content_size = 0;

    // Initialize string for holding all of the file's contents
    char *file_contents = malloc(buffer_size);
    if (file_contents == NULL) {
        perror("Failed to allocate memory for file contents");
        fclose(file);
        LiteralValue lv = (LiteralValue){.type = TYPE_ERROR};
        return make_result(lv, false, false);
    }

    // Initialize buffer for reading each line
    char *buffer = malloc(buffer_size);

    while (fgets(buffer, buffer_size, file) != NULL) {
        size_t line_length = strlen(buffer);

        while (content_size + line_length + 1 > buffer_size) {
            buffer_size *= 2;
            char *temp = realloc(file_contents, buffer_size);

            if (temp == NULL) {
                perror("Failed to reallocate memory for file contents");
                free(buffer);
                free(file_contents);
                fclose(file);
                LiteralValue lv = (LiteralValue){.type = TYPE_ERROR};
                return make_result(lv, false, false);
            }

            file_contents = temp;
        }

        // Append buffer to file_contents
        strcpy(file_contents + content_size, buffer);
        debug_print_int("Read line: `%s`\n", buffer);
        content_size += line_length;
    }

    free(buffer);
    fclose(file);

    LiteralValue lv = {.type = TYPE_STRING, .data.string = file_contents};
    return make_result(lv, false, false);
}

InterpretResult helper_file_writer(ASTNode *node, Environment *env,
                                   bool append) {
    char *filepath;
    char *content;

    ArgumentSpec specs[2];
    specs[0].type = ARG_TYPE_STRING;
    specs[0].out_ptr = &filepath;

    specs[1].type = ARG_TYPE_STRING;
    specs[1].out_ptr = &content;

    InterpretResult args_res =
        interpret_arguments(node->function_call.arguments, env, 2, specs);
    if (args_res.is_error) {
        LiteralValue lv = (LiteralValue){.type = TYPE_ERROR};
        return make_result(lv, false, false);
    }

    // Process the content to handle escape sequences
    char *processed_content = process_escape_sequences(content);
    if (processed_content == NULL) {
        LiteralValue lv = (LiteralValue){.type = TYPE_ERROR};
        return make_result(lv, false, false);
    }

    FILE *file = fopen(filepath, append ? "a" : "w");
    if (file == NULL) {
        perror("Failed to open file for writing");
        free(processed_content);
        LiteralValue lv = (LiteralValue){.type = TYPE_ERROR};
        return make_result(lv, false, false);
    }

    if (fputs(processed_content, file) == EOF) {
        perror("Failed to write to file");
        free(processed_content);
        fclose(file);
        LiteralValue lv = (LiteralValue){.type = TYPE_ERROR};
        return make_result(lv, false, false);
    }

    free(processed_content);
    fclose(file);

    LiteralValue lv = {.type = TYPE_BOOLEAN, .data.boolean = true};
    return make_result(lv, false, false);
}

InterpretResult builtin_file_write(ASTNode *node, Environment *env) {
    return helper_file_writer(node, env, false);
}

InterpretResult builtin_file_append(ASTNode *node, Environment *env) {
    return helper_file_writer(node, env, true);
}
