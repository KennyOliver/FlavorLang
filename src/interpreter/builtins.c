#include "builtins.h"

char *literal_value_to_string(LiteralValue lv) {
    char buffer[128];
    switch (lv.type) {
    case TYPE_INTEGER: {
        snprintf(buffer, sizeof(buffer), INT_FORMAT, lv.data.integer);
        return strdup(buffer);
    }
    case TYPE_FLOAT: {
        snprintf(buffer, sizeof(buffer), FLOAT_FORMAT, lv.data.floating_point);
        return strdup(buffer);
    }
    case TYPE_STRING: {
        return strdup(lv.data.string);
    }
    case TYPE_BOOLEAN: {
        return strdup(lv.data.boolean ? "True" : "False");
    }
    case TYPE_ARRAY: {
        // Estimate needed buffer size: assume roughly 32 chars per element plus
        // brackets
        size_t estimate = lv.data.array.count * 32 + 3;
        char *result = malloc(estimate);
        if (!result)
            return NULL;
        strcpy(result, "[");
        for (size_t i = 0; i < lv.data.array.count; i++) {
            char *elemStr = literal_value_to_string(lv.data.array.elements[i]);
            if (!elemStr) {
                free(result);
                return NULL;
            }
            strncat(result, elemStr, estimate - strlen(result) - 1);
            free(elemStr);
            if (i < lv.data.array.count - 1) {
                strncat(result, ", ", estimate - strlen(result) - 1);
            }
        }
        strncat(result, "]", estimate - strlen(result) - 1);
        return result;
    }
    default:
        return strdup("<Unsupported>");
    }
}

/**
 * @brief Builds a single string by converting each function call argument
 * (using literal_value_to_string) and concatenating them (separated by a single
 * space).
 *
 * @param arg_node
 * @param env
 * @return char*
 */
char *build_arguments_string(ASTNode *arg_node, Environment *env) {
    char buffer[1024] = {0};
    size_t buffer_index = 0;

    while (arg_node != NULL) {
        InterpretResult r = interpret_node(arg_node, env);
        if (r.is_error) {
            return NULL; // If an error occurs, return NULL.
        }
        // Convert the evaluated argument to string.
        char *s = literal_value_to_string(r.value);
        if (!s)
            return NULL;
        size_t len = strlen(s);
        if (buffer_index + len + 1 < sizeof(buffer)) {
            strcpy(&buffer[buffer_index], s);
            buffer_index += len;
            if (arg_node->next != NULL && buffer_index < sizeof(buffer) - 1) {
                buffer[buffer_index++] = ' ';
            }
        }
        free(s);
        arg_node = arg_node->next;
    }
    return strdup(buffer);
}

// Helper function to check if a LiteralType matches an ArgType
bool literal_type_matches_arg_type(LiteralType lit_type, ArgType arg_type) {
    switch (arg_type) {
    case ARG_TYPE_INTEGER:
        return lit_type == TYPE_INTEGER;
    case ARG_TYPE_FLOAT:
        return lit_type == TYPE_FLOAT;
    case ARG_TYPE_STRING:
        return lit_type == TYPE_STRING;
    case ARG_TYPE_BOOLEAN:
        return lit_type == TYPE_BOOLEAN;
    case ARG_TYPE_ARRAY:
        return lit_type == TYPE_ARRAY;
    case ARG_TYPE_NUMERIC:
        return (lit_type == TYPE_INTEGER) || (lit_type == TYPE_FLOAT);
    default:
        return false; // Unknown ArgType
    }
}

// Function to interpret arguments with single expected type per ArgumentSpec
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
        ArgumentSpec current_spec = specs[i];
        bool type_matched =
            literal_type_matches_arg_type(lv.type, current_spec.type);

        if (type_matched) {
            // Assign the value based on the expected type
            switch (current_spec.type) {
            case ARG_TYPE_INTEGER:
                *((INT_SIZE *)current_spec.out_ptr) = lv.data.integer;
                break;
            case ARG_TYPE_FLOAT:
                *((FLOAT_SIZE *)current_spec.out_ptr) = lv.data.floating_point;
                break;
            case ARG_TYPE_NUMERIC:
                if (lv.type == TYPE_INTEGER) {
                    *((FLOAT_SIZE *)current_spec.out_ptr) =
                        (FLOAT_SIZE)lv.data.integer;
                } else { // TYPE_FLOAT
                    *((FLOAT_SIZE *)current_spec.out_ptr) =
                        lv.data.floating_point;
                }
                break;
            case ARG_TYPE_STRING:
                *((char **)current_spec.out_ptr) = lv.data.string;
                break;
            case ARG_TYPE_BOOLEAN:
                *((bool *)current_spec.out_ptr) = lv.data.boolean;
                break;
            case ARG_TYPE_ARRAY:
                *((ArrayValue **)current_spec.out_ptr) = &lv.data.array;
                break;
            default:
                return raise_error("Unknown argument type for argument %zu.\n",
                                   i + 1);
            }
        } else {
            // Construct a string of expected type for the error message
            char expected_type[64] = "";
            switch (current_spec.type) {
            case ARG_TYPE_INTEGER:
                strcpy(expected_type, "integer");
                break;
            case ARG_TYPE_FLOAT:
                strcpy(expected_type, "float");
                break;
            case ARG_TYPE_NUMERIC:
                strcpy(expected_type, "numeric (integer or float)");
                break;
            case ARG_TYPE_STRING:
                strcpy(expected_type, "string");
                break;
            case ARG_TYPE_BOOLEAN:
                strcpy(expected_type, "boolean");
                break;
            case ARG_TYPE_ARRAY:
                strcpy(expected_type, "array");
                break;
            default:
                strcpy(expected_type, "unknown");
                break;
            }

            return raise_error("Expected %s for argument %zu.\n", expected_type,
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
            case 'x': {
                // Handle \xNN where NN is two-digit hex number
                char hex_buffer[3] = {0};
                if (isxdigit(*(p + 1)) && isxdigit(*(p + 2))) {
                    hex_buffer[0] = *(p + 1);
                    hex_buffer[1] = *(p + 2);
                    char decoded_char = (char)strtol(hex_buffer, NULL, 16);
                    putchar(decoded_char);
                    p += 2; // skip the two hex digits
                } else {
                    // If malformed, print as-is
                    putchar('\\');
                    putchar('x');
                }
                break;
            }
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

// Built-in `input()` function with optional arguments for a prompt
InterpretResult builtin_input(ASTNode *node, Environment *env) {
    // If a prompt argument is provided, build and print it.
    if (node->function_call.arguments != NULL) {
        char *prompt =
            build_arguments_string(node->function_call.arguments, env);
        if (prompt) {
            printf("%s", prompt);
            fflush(stdout);
            free(prompt);
        }
    }

    // Now read input from the user.
    size_t buffer_size = 128;
    size_t input_length = 0;
    char *input_buffer = malloc(buffer_size);
    if (!input_buffer) {
        fprintf(stderr, "Error: Failed to allocate memory for input buffer.\n");
        LiteralValue lv = {.type = TYPE_ERROR};
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
                LiteralValue lv = {.type = TYPE_ERROR};
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
        LiteralValue lv = {.type = TYPE_ERROR};
        return make_result(lv, false, false);
    }
    debug_print_int("Input received: `%s`\n", result.data.string);
    return make_result(result, false, false);
}

// Built-in `random()` function with 0, 1, or 2 numeric arguments
InterpretResult builtin_random(ASTNode *node, Environment *env) {
    FLOAT_SIZE min = 0.0L; // default min
    FLOAT_SIZE max = 1.0L; // default max

    // Determine how many arguments were passed
    ASTNode *arg_node = node->function_call.arguments;
    size_t num_args = 0;
    ASTNode *temp = arg_node;
    while (temp) {
        num_args++;
        temp = temp->next;
    }
    if (num_args > 2) {
        return raise_error("`random()` takes at most 2 numeric arguments "
                           "(integer or float), but %zu provided.\n",
                           num_args);
    }
    if (num_args == 1) {
        ArgumentSpec specs[1];
        specs[0].type = ARG_TYPE_NUMERIC;
        specs[0].out_ptr = &max;
        InterpretResult args_res = interpret_arguments(arg_node, env, 1, specs);
        if (args_res.is_error) {
            return args_res;
        }
    } else if (num_args == 2) {
        ArgumentSpec specs[2];
        specs[0].type = ARG_TYPE_NUMERIC;
        specs[0].out_ptr = &min;
        specs[1].type = ARG_TYPE_NUMERIC;
        specs[1].out_ptr = &max;
        InterpretResult args_res = interpret_arguments(arg_node, env, 2, specs);
        if (args_res.is_error) {
            return args_res;
        }
    }

    if (min > max) {
        FLOAT_SIZE temp_val = min;
        min = max;
        max = temp_val;
    }

    static bool seeded = false;
    if (!seeded) {
#ifdef _WIN32
        srand((unsigned int)(time(NULL) ^ GetTickCount()));
#else
        struct timeval tv;
        gettimeofday(&tv, NULL);
        srand((unsigned int)(tv.tv_sec ^ tv.tv_usec ^ getpid()));
#endif
        seeded = true;
    }

    FLOAT_SIZE random_fraction = ((FLOAT_SIZE)rand() / (FLOAT_SIZE)RAND_MAX);
    FLOAT_SIZE random_number = min + random_fraction * (max - min);

    debug_print_int("Random number generated (min: " FLOAT_FORMAT
                    ", max: " FLOAT_FORMAT "): `" FLOAT_FORMAT "`\n",
                    min, max, random_number);

    LiteralValue result;
    result.type = TYPE_FLOAT;
    result.data.floating_point = random_number;
    return make_result(result, false, false);
}

void print_literal_value(LiteralValue lv) {
    switch (lv.type) {
    case TYPE_FLOAT:
        if ((INT_SIZE)lv.data.floating_point == lv.data.floating_point) {
            printf("%.1Lf", lv.data.floating_point);
        } else {
            printf(FLOAT_FORMAT, lv.data.floating_point);
        }
        break;
    case TYPE_INTEGER:
        printf(INT_FORMAT, lv.data.integer);
        break;
    case TYPE_STRING:
        printf("\"%s\"", lv.data.string);
        break;
    case TYPE_BOOLEAN:
        printf("%s", lv.data.boolean ? "True" : "False");
        break;
    case TYPE_ARRAY:
        printf("[");
        for (size_t i = 0; i < lv.data.array.count; i++) {
            print_literal_value(lv.data.array.elements[i]);
            if (i < lv.data.array.count - 1) {
                printf(", ");
            }
        }
        printf("]");
        break;
    case TYPE_FUNCTION:
        printf("<Function %s>", lv.data.function_name);
        break;
    case TYPE_ERROR:
        printf("<Error>");
        break;
    default:
        printf("<Unknown>");
    }
}

// Built-in `serve()` function for printing with optional newline control
InterpretResult builtin_output(ASTNode *node, Environment *env) {
    char *output = build_arguments_string(node->function_call.arguments, env);
    if (output) {
        printf("%s\n", output);
        free(output);
    }
    LiteralValue lv = {.type = TYPE_INTEGER, .data.integer = 0};
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
            snprintf(buffer, sizeof(buffer), FLOAT_FORMAT,
                     lv.data.floating_point);
            strncat(error_message, buffer,
                    sizeof(error_message) - strlen(error_message) - 1);
            break;
        }
        case TYPE_INTEGER: {
            char buffer[64];
            snprintf(buffer, sizeof(buffer), INT_FORMAT, lv.data.integer);
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

    // Extract the cast type from function_ref
    ASTNode *func_ref = node->function_call.function_ref;
    if (func_ref->type != AST_VARIABLE_REFERENCE) {
        return raise_error("`builtin_cast()` expects the function reference to "
                           "be a variable name.\n");
    }

    char *cast_type = strdup(func_ref->variable_name);
    if (!cast_type) {
        return raise_error("No cast type provided to `builtin_cast()`.\n");
    }

    ASTNode *arg_node = node->function_call.arguments;
    if (!arg_node) {
        free(cast_type);
        return raise_error(
            "No expression provided for cast in `builtin_cast()`.\n");
    }

    // Ensure there's only one argument
    if (arg_node->next != NULL) {
        free(cast_type);
        return raise_error("`%s` cast function takes exactly one argument.\n",
                           cast_type);
    }

    ASTNode *expr = arg_node;

    // Interpret the expression to be casted
    InterpretResult expr_result = interpret_node(expr, env);
    if (expr_result.is_error) {
        free(cast_type);
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
            snprintf(buffer, sizeof(buffer), INT_FORMAT, original.data.integer);
            cast_val.data.string = strdup(buffer);
            break;
        case TYPE_FLOAT:
            snprintf(buffer, sizeof(buffer), FLOAT_FORMAT,
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
            free(cast_type);
            return raise_error("Unsupported type for string cast.\n");
        }

        if (!cast_val.data.string) {
            free(cast_type);
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
                free(cast_type);
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
            free(cast_type);
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
                free(cast_type);
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
            free(cast_type);
            return raise_error("Unsupported type for float cast.\n");
        }

        result_res.value = cast_val;
    } else {
        free(cast_type);
        return raise_error("Unsupported cast type: `%s`\n", cast_type);
    }

    free(cast_type);
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

    char *dst = processed;   // destination
    const char *src = input; // source

    while (*src) {
        if (*src == '\\' && *(src + 1)) {
            src++; // skip backslash
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
            case '"':
                *dst++ = '\"';
                break;
            case '\'':
                *dst++ = '\'';
                break;
            case 'x': {
                // Look ahead two characters; if they are hex digits, decode
                // them.
                if (isxdigit(*(src + 1)) && isxdigit(*(src + 2))) {
                    char hex[3] = {*(src + 1), *(src + 2), '\0'};
                    long value = strtol(hex, NULL, 16);
                    *dst++ = (char)value;
                    src += 2; // Skip past the two hex digits
                } else {
                    // If the two characters after \x aren’t hex, just output
                    // "x"
                    *dst++ = 'x';
                }
                break;
            }
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

    *dst = '\0'; // NULL-termination
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

/**
 * @brief Built-in function to calculate the length of an array or string.
 *
 * @param node The AST node representing the function call.
 * @param env  The current environment.
 * @return InterpretResult containing the length as an integer or an error.
 */
InterpretResult builtin_length(ASTNode *node, Environment *env) {
    // Ensure the function call has exactly one argument
    size_t num_args = 0;
    ASTNode *arg_node = node->function_call.arguments;
    ASTNode *temp = arg_node;
    while (temp) {
        num_args++;
        temp = temp->next;
    }

    if (num_args != 1) {
        return raise_error("`length()` expects exactly one argument, but "
                           "%zu were given.\n",
                           num_args);
    }

    // Interpret the single argument
    InterpretResult arg_res = interpret_node(arg_node, env);
    if (arg_res.is_error) {
        // Propagate the error
        return arg_res;
    }

    LiteralValue lv = arg_res.value;

    // Initialize the result
    LiteralValue result;
    result.type = TYPE_INTEGER;

    // Determine the type of the argument and calculate length accordingly
    if (lv.type == TYPE_STRING) {
        result.data.integer = (INT_SIZE)strlen(lv.data.string);
    } else if (lv.type == TYPE_ARRAY) {
        result.data.integer = (INT_SIZE)lv.data.array.count;
    } else {
        // Unsupported type
        return raise_error("`length()` expects an array or a string as an "
                           "argument, but received type `%d`.\n",
                           lv.type);
    }

    // Return the length as a LiteralValue
    return make_result(result, false, false);
}

/**
 * @brief Built-in function to sleep (pause execution) for a given time (in
 * milliseconds).
 *
 * @param node The AST node representing the function call.
 * @param env  The current environment.
 * @return InterpretResult with a default value.
 */
InterpretResult builtin_sleep(ASTNode *node, Environment *env) {
    // Define expected arguments: exactly one integer (> 0).
    INT_SIZE ms;

    ArgumentSpec specs[1];
    specs[0].type = ARG_TYPE_INTEGER;
    specs[0].out_ptr = &ms;

    // Interpret arguments
    InterpretResult args_res =
        interpret_arguments(node->function_call.arguments, env, 1, specs);
    if (args_res.is_error) {
        return args_res;
    }

    // Validate the argument
    if (ms <= 0) {
        return raise_error("`sleep` requires a positive integer argument "
                           "representing milliseconds.\n");
    }

// Perform the sleep based on the platform
#ifdef _WIN32
    Sleep((DWORD)ms); // sleep takes milliseconds on Windows
#else
    if (ms > (INT_MAX / 1000)) {
        return raise_error("`sleep` argument is too large.\n");
    }

    useconds_t us = (useconds_t)(ms * 1000); // usleep needs microseconds
    usleep(us);
#endif

    LiteralValue result = create_default_value();
    return make_result(result, false, false);
}

InterpretResult builtin_cimport(ASTNode *node, Environment *env) {
    // Expect exactly two arguments: library path & function name
    char *lib_path, *func_name;
    ArgumentSpec specs[2] = {{.type = ARG_TYPE_STRING, .out_ptr = &lib_path},
                             {.type = ARG_TYPE_STRING, .out_ptr = &func_name}};
    InterpretResult args_res =
        interpret_arguments(node->function_call.arguments, env, 2, specs);
    if (args_res.is_error) {
        return args_res;
    }

    // If lib_path is not absolute path, prepend script directory
    if (lib_path[0] != '/' && env->script_dir != NULL) {
        size_t base_len = strlen(env->script_dir);
        size_t lib_len = strlen(lib_path);
        // Allocate enough room for "script_dir/lib_path" plus NULL terminator.
        char *full_path = malloc(base_len + lib_len + 2);
        if (!full_path) {
            return raise_error("Memory allocation failed while constructing "
                               "the full library path.");
        }
        strcpy(full_path, env->script_dir);
        // Ensure env->script_dir ends with a slash
        if (env->script_dir[base_len - 1] != '/') {
            strcat(full_path, "/");
        }
        strcat(full_path, lib_path);
        lib_path = full_path; // Now use this full path for dlopen()
    }

    // Declare function pointer variable
    FlavorLangCFunc func_ptr = NULL;

#if defined(__unix__) || defined(__APPLE__)
    char full_lib_path[PATH_MAX];
    // If lib_path is not absolute, prepend env->script_dir.
    if (lib_path[0] != '/') {
        snprintf(full_lib_path, sizeof(full_lib_path), "%s/%s", env->script_dir,
                 lib_path);
    } else {
        strncpy(full_lib_path, lib_path, sizeof(full_lib_path));
        full_lib_path[sizeof(full_lib_path) - 1] = '\0';
    }

    // Now open the shared library using the full path.
    void *handle = dlopen(full_lib_path, RTLD_LAZY);
    if (!handle) {
        return raise_error("dlopen error: %s", dlerror());
    }
    // Clear any existing errors.
    dlerror();
    func_ptr = (FlavorLangCFunc)dlsym(handle, func_name);
    char *error_msg = dlerror();
    if (error_msg != NULL) {
        dlclose(handle);
        return raise_error("dlsym error: %s", error_msg);
    }
#else
    return raise_error("`cimport` is not implemented on this platform.\n");
#endif

    // Create new `Function` record for external function
    Function cfunc;
    cfunc.name = safe_strdup(func_name);
    cfunc.parameters = NULL;
    cfunc.body = NULL;       // No AST body — it’s external
    cfunc.is_builtin = true; // Mark as builtin/external
    cfunc.c_function = func_ptr;

    add_function(env, cfunc);

    // Also create corresponding variable so that function is callable
    LiteralValue func_val;
    func_val.type = TYPE_FUNCTION;
    func_val.data.function_name = safe_strdup(cfunc.name);
    Variable var;
    var.variable_name = safe_strdup(cfunc.name);
    var.value = func_val;
    var.is_constant = false;
    add_variable(env, var);

    // Clean up the allocated full_path if it was used
    if (env->script_dir != NULL && lib_path != NULL &&
        lib_path != node->function_call.arguments->literal.value.string) {
        free(lib_path);
    }

    // Return a default value (0)
    LiteralValue ret;
    ret.type = TYPE_INTEGER;
    ret.data.integer = 0;
    return make_result(ret, false, false);
}

InterpretResult builtin_floor(ASTNode *node, Environment *env) {
    // Expect exactly one numeric argument
    FLOAT_SIZE value;
    ArgumentSpec specs[1];
    specs[0].type = ARG_TYPE_NUMERIC;
    specs[0].out_ptr = &value;

    InterpretResult args_res =
        interpret_arguments(node->function_call.arguments, env, 1, specs);
    if (args_res.is_error) {
        return args_res;
    }

    LiteralValue result;
    result.type = TYPE_INTEGER;
    result.data.integer = floor(value);
    return make_result(result, false, false);
}

InterpretResult builtin_ceil(ASTNode *node, Environment *env) {
    FLOAT_SIZE value;
    ArgumentSpec specs[1];
    specs[0].type = ARG_TYPE_NUMERIC;
    specs[0].out_ptr = &value;

    InterpretResult args_res =
        interpret_arguments(node->function_call.arguments, env, 1, specs);
    if (args_res.is_error) {
        return args_res;
    }

    LiteralValue result;
    result.type = TYPE_INTEGER;
    result.data.integer = ceil(value);
    return make_result(result, false, false);
}

InterpretResult builtin_round(ASTNode *node, Environment *env) {
    FLOAT_SIZE value;
    ArgumentSpec specs[1];
    specs[0].type = ARG_TYPE_NUMERIC;
    specs[0].out_ptr = &value;

    InterpretResult args_res =
        interpret_arguments(node->function_call.arguments, env, 1, specs);
    if (args_res.is_error) {
        return args_res;
    }

    LiteralValue result;
    result.type = TYPE_INTEGER;
    result.data.integer = round(value);
    return make_result(result, false, false);
}

InterpretResult builtin_abs(ASTNode *node, Environment *env) {
    // Get the first argument node
    ASTNode *arg = node->function_call.arguments;
    if (!arg) {
        return raise_error("abs() requires one numeric argument.\n");
    }

    // Interpret the argument without forcing a conversion
    InterpretResult arg_res = interpret_node(arg, env);
    if (arg_res.is_error) {
        return arg_res;
    }

    LiteralValue original = arg_res.value;
    LiteralValue result;

    // Check the type of the argument and compute absolute accordingly
    if (original.type == TYPE_INTEGER) {
        result.type = TYPE_INTEGER;
        result.data.integer = (original.data.integer < 0)
                                  ? -original.data.integer
                                  : original.data.integer;
    } else if (original.type == TYPE_FLOAT) {
        result.type = TYPE_FLOAT;
        result.data.floating_point = fabs(original.data.floating_point);
    } else {
        return raise_error("abs() requires a numeric argument.\n");
    }

    return make_result(result, false, false);
}
