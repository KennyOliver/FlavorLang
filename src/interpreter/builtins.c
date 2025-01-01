#include "builtins.h"

LiteralValue builtin_input() {
    // Potential future place to implement prompt for user???
    //    printf("Enter your input: ");

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
