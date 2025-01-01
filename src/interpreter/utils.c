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
        error_interpreter("Failed to allocate memory for variables.\n");
    }

    // Add function initialization
    env->function_count = 0;
    env->function_capacity = 10;
    env->functions = malloc(env->function_capacity * sizeof(Function));
    if (!env->functions) {
        error_interpreter("Failed to allocate memory for functions.\n");
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

ASTFunctionParameter *
copy_function_parameters(ASTFunctionParameter *param_list) {
    if (param_list == NULL) {
        // No parameters to copy
        return NULL;
    }

    ASTFunctionParameter *new_head = NULL;
    ASTFunctionParameter *new_tail = NULL;

    while (param_list != NULL) {
        // Allocate memory for the new parameter
        ASTFunctionParameter *new_param = malloc(sizeof(ASTFunctionParameter));
        if (new_param == NULL) {
            // Clean up all previously allocated nodes
            free_parameter_list(new_head);
            error_interpreter("Memory allocation failed for new parameter\n");
        }

        // Duplicate parameter name, ensuring it is not NULL
        if (param_list->parameter_name) {
            new_param->parameter_name = strdup(param_list->parameter_name);
            if (new_param->parameter_name == NULL) {
                free(new_param); // free the current node
                free_parameter_list(
                    new_head); // clean up previously allocated nodes
                error_interpreter(
                    "Memory allocation failed for parameter name\n");
            }
        } else {
            new_param->parameter_name = NULL;
        }

        // Initialize the `next` pointer to NULL
        new_param->next = NULL;

        // Add the new parameter to the linked list
        if (new_head == NULL) {
            new_head = new_param; // Set as head if it's the first node
        } else {
            new_tail->next = new_param; // Attach to the end of the list
        }
        new_tail = new_param; // Update the tail pointer

        // Move to the next parameter in the source list
        param_list = param_list->next;
    }

    return new_head;
}

ASTNode *copy_ast_node(ASTNode *node) {
    if (!node)
        return NULL;

    ASTNode *new_node = malloc(sizeof(ASTNode));
    if (!new_node) {
        error_interpreter("Memory allocation failed in `copy_ast_node`\n");
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
            error_interpreter("Initial allocation for functions failed.\n");
        }
        env->function_capacity = 4;
    }

    // Resize if needed
    if (env->function_count == env->function_capacity) {
        size_t new_capacity = env->function_capacity * 2;
        Function *new_functions =
            realloc(env->functions, new_capacity * sizeof(Function));
        if (!new_functions) {
            error_interpreter("Memory allocation failed for functions.\n");
        }
        env->functions = new_functions;
        env->function_capacity = new_capacity;
    }

    if (!func.name) {
        error_interpreter("Function name is `NULL` or invalid.\n");
    }

    // Create a deep copy
    Function *stored_func = &env->functions[env->function_count++];
    stored_func->parameters = copy_function_parameters(func.parameters);
    stored_func->body = copy_ast_node(func.body);
    stored_func->is_builtin = func.is_builtin;

    stored_func->name = strdup(func.name);
    if (!stored_func->name) {
        free(stored_func);
        error_interpreter("Memory allocation failed for function name.\n");
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
