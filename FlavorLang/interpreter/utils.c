#include "utils.h"

void error_interpreter(const char *format, ...) {
    va_list args;
    va_start(args, format);

    printf("\033[31m"); // red text color
    printf("Error: ");
    vprintf(format, args);
    printf("\033[0m\n"); // reset text color

    va_end(args);
    fflush(stdout);
    exit(1);
}

// Initialize the environment
void init_environment(Environment *env) {
    // Existing variable initialization
    env->variable_count = 0;
    env->capacity = 10;
    env->variables = malloc(env->capacity * sizeof(Variable));

    // Add function initialization
    env->function_count = 0;
    env->function_capacity = 10;
    env->functions = malloc(env->function_capacity * sizeof(Function));
}

// Free the environment
void free_environment(Environment *env) {
    // Free variables
    for (size_t i = 0; i < env->variable_count; i++) {
        free(env->variables[i].variable_name);
    }
    free(env->variables);

    // Free functions
    for (size_t i = 0; i < env->function_count; i++) {
        free(env->functions[i].name);
    }
    free(env->functions);
}
