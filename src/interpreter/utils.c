#include "utils.h"

InterpretResult raise_error(const char *format, ...) {
    char error_message[1024];
    va_list args;
    va_start(args, format);
    printf("\033[31m"); // red text color
    printf("Error: ");
    vsnprintf(error_message, sizeof(error_message), format, args);
    printf("\033[0m\n"); // reset text color
    va_end(args);

    // Create an error LiteralValue
    LiteralValue error_value;
    error_value.type = TYPE_ERROR;
    error_value.data.string = strdup(error_message);

    if (!error_value.data.string) {
        fprintf(stderr,
                "Error: Failed to allocate memory for error message.\n");
        exit(1);
    }

    InterpretResult res = {.value = error_value,
                           .did_return = false,
                           .did_break = false,
                           .is_error = true};
    return res;
}

void fatal_error(const char *format, ...) {
    char error_message[1024];
    va_list args;
    va_start(args, format);
    fprintf(stderr, "\033[31mError: ");
    vsnprintf(error_message, sizeof(error_message), format, args);
    fprintf(stderr, "%s\033[0m\n", error_message);
    va_end(args);
    exit(1);
}

void initialize_builtin_function(Environment *env, const char *name) {
    Function func;
    memset(&func, 0, sizeof(Function)); // zero out for safety
    func.name = strdup(name);
    func.parameters = NULL;
    func.body = NULL;
    func.is_builtin = true;
    add_function(env, func);
}

void initialize_all_builtin_functions(Environment *env) {
    const char *builtin_functions[] = {
        "string", "float",    "int",        "sample",     "serve",       "burn",
        "random", "get_time", "taste_file", "plate_file", "garnish_file"};

    for (size_t i = 0;
         i < sizeof(builtin_functions) / sizeof(builtin_functions[0]); i++) {
        initialize_builtin_function(env, builtin_functions[i]);
    }
}

void init_environment(Environment *env) {
    // Existing variable initialization
    env->variable_count = 0;
    env->capacity = 10;
    env->variables = malloc(env->capacity * sizeof(Variable));
    if (!env->variables) {
        fatal_error("Failed to allocate memory for variables.\n");
    }

    // Add function initialization
    env->function_count = 0;
    env->function_capacity = 10;
    env->functions = malloc(env->function_capacity * sizeof(Function));
    if (!env->functions) {
        fatal_error("Failed to allocate memory for functions.\n");
    }

    initialize_all_builtin_functions(env);
}

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

// Helper function to free a linked list of ASTFunctionParameter nodes
void free_parameter_list(ASTFunctionParameter *head) {
    ASTFunctionParameter *current = head;
    while (current != NULL) {
        ASTFunctionParameter *next = current->next;
        free(current->parameter_name);
        free(current);
        current = next;
    }
}

ASTFunctionParameter *copy_function_parameters(ASTFunctionParameter *params) {
    if (!params)
        return NULL;

    ASTFunctionParameter *new_params = NULL;
    ASTFunctionParameter *tail = NULL;

    while (params) {
        ASTFunctionParameter *new_param = malloc(sizeof(ASTFunctionParameter));
        if (!new_param) {
            fatal_error(
                "Memory allocation failed for function parameter copy.\n");
        }

        new_param->parameter_name = strdup(params->parameter_name);
        if (!new_param->parameter_name) {
            free(new_param);
            fatal_error("Memory allocation failed for parameter name copy.\n");
        }

        new_param->next = NULL;

        if (!new_params) {
            new_params = tail = new_param;
        } else {
            tail->next = new_param;
            tail = new_param;
        }

        params = params->next;
    }

    return new_params;
}

ASTNode *copy_ast_node(ASTNode *node) {
    if (!node)
        return NULL;

    ASTNode *new_node = malloc(sizeof(ASTNode));
    if (!new_node) {
        fatal_error("Memory allocation failed in `copy_ast_node`\n");
    }

    memcpy(new_node, node, sizeof(ASTNode));

    // Deep copy for fields like `function_call`, `body`, or `arguments`
    if (node->function_call.arguments) {
        new_node->function_call.arguments =
            copy_ast_node(node->function_call.arguments);
    }
    if (node->function_call.body) {
        new_node->function_call.body = copy_ast_node(node->function_call.body);
    }

    // Handle other types like linked lists or child nodes as needed
    return new_node;
}

void add_function(Environment *env, Function func) {
    // Check if this function name already exists in env
    for (size_t i = 0; i < env->function_count; i++) {
        if (strcmp(env->functions[i].name, func.name) == 0) {
            // We already have this function, so just skip
            // or optionally do an "update" logic if you prefer.
            debug_print_int("Skipping re-add of function `%s`\n", func.name);
            return;
        }
    }

    // Ensure `functions` is initialized if not ...
    if (!env->functions && env->function_capacity == 0) {
        env->functions = calloc(4, sizeof(Function));
        if (!env->functions) {
            fatal_error("Initial allocation for functions failed.\n");
        }
        env->function_capacity = 4;
    }

    // Resize if needed
    if (env->function_count == env->function_capacity) {
        size_t new_capacity = env->function_capacity * 2;
        Function *new_functions =
            realloc(env->functions, new_capacity * sizeof(Function));
        if (!new_functions) {
            fatal_error("Memory allocation failed for functions.\n");
        }
        env->functions = new_functions;
        env->function_capacity = new_capacity;
    }

    if (!func.name) {
        fatal_error("Function name is `NULL` or invalid.\n");
    }

    // Create a deep copy
    Function *stored_func = &env->functions[env->function_count++];
    stored_func->parameters = copy_function_parameters(func.parameters);
    stored_func->body = copy_ast_node(func.body);
    stored_func->is_builtin = func.is_builtin;

    stored_func->name = strdup(func.name);
    if (!stored_func->name) {
        free(stored_func);
        fatal_error("Memory allocation failed for function name.\n");
    }

    debug_print_int("Function `%s` added successfully.\n", stored_func->name);
}

Function *get_function(Environment *env, const char *name) {
    for (size_t i = 0; i < env->function_count; i++) {
        if (strcmp(env->functions[i].name, name) == 0) {
            return &env->functions[i];
        }
    }
    return NULL;
}

// A helper to wrap `LiteralValue` in `InterpretResult`
InterpretResult make_result(LiteralValue val, bool did_return, bool did_break) {
    InterpretResult r;
    r.value = val;
    r.did_return = did_return;
    r.did_break = did_break;
    return r;
}
