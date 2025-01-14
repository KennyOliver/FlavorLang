#include "utils.h"
#include <errno.h>

InterpretResult raise_error(const char *format, ...) {
    char error_message[1024];
    va_list args;
    va_start(args, format);

    // Format the error message into the buffer
    vsnprintf(error_message, sizeof(error_message), format, args);
    va_end(args);

    // Print "Error:" in red, followed by the formatted error message
    printf("\033[31mError: %s\033[0m\n", error_message);

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
    memset(&func, 0, sizeof(Function)); // Zero out for safety
    func.name = safe_strdup(name);
    func.parameters = NULL;
    func.body = NULL;
    func.is_builtin = true;
    add_function(env, func);

    // Add a variable referencing this built-in function by name
    Variable var;
    var.variable_name = safe_strdup(name);
    var.is_constant = false;
    var.value.type = TYPE_FUNCTION;
    var.value.data.function_name = safe_strdup(name); // Store function name

    InterpretResult add_var_res = add_variable(env, var);
    if (add_var_res.is_error) {
        fatal_error("Failed to register built-in function variable `%s`.\n",
                    name);
    }
}

void initialize_all_builtin_functions(Environment *env) {
    const char *builtin_functions[] = {
        "string",       "float",  "int",      "sample",     "serve",
        "burn",         "random", "get_time", "taste_file", "plate_file",
        "garnish_file", "length", "sleep"};

    for (size_t i = 0;
         i < sizeof(builtin_functions) / sizeof(builtin_functions[0]); i++) {
        initialize_builtin_function(env, builtin_functions[i]);
    }
}

// Function to initialize the environment without a parent (global)
void init_environment(Environment *env) {
    env->parent = NULL;

    // Initialize variables
    env->variable_count = 0;
    env->capacity = 10;
    env->variables = malloc(env->capacity * sizeof(Variable));
    if (!env->variables) {
        fatal_error("Failed to allocate memory for variables.\n");
    }

    // Initialize functions
    env->function_count = 0;
    env->function_capacity = 10;
    env->functions = malloc(env->function_capacity * sizeof(Function));
    if (!env->functions) {
        fatal_error("Failed to allocate memory for functions.\n");
    }

    // Initialize built-in functions ONLY for the GLOBAL environment
    initialize_all_builtin_functions(env);
}

// Function to initialize the environment with a parent (local)
void init_environment_with_parent(Environment *env, Environment *parent) {
    env->parent = parent;

    // Initialize variables
    env->variable_count = 0;
    env->capacity = 10;
    env->variables = malloc(env->capacity * sizeof(Variable));
    if (!env->variables) {
        fatal_error("Failed to allocate memory for variables.\n");
    }

    // Initialize functions
    env->function_count = 0;
    env->function_capacity = 10;
    env->functions = malloc(env->function_capacity * sizeof(Function));
    if (!env->functions) {
        fatal_error("Failed to allocate memory for functions.\n");
    }

    // Do NOT initialize built-in functions in local environments
}

// Free the environment and its resources
void free_environment(Environment *env) {
    // Free variables
    for (size_t i = 0; i < env->variable_count; i++) {
        free(env->variables[i].variable_name);
        if (env->variables[i].value.type == TYPE_STRING &&
            env->variables[i].value.data.string) {
            free(env->variables[i].value.data.string);
        }
    }
    free(env->variables);

    // Free functions
    for (size_t i = 0; i < env->function_count; i++) {
        // Free function name and parameters
        free(env->functions[i].name);
        free_parameter_list(env->functions[i].parameters);
        // Only free the AST if you’re completely done with it!
        if (!env->functions[i].is_builtin && env->functions[i].body) {
            free(env->functions[i].body);
            env->functions[i].body = NULL;
        }
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
        ASTFunctionParameter *copied_param =
            malloc(sizeof(ASTFunctionParameter));
        if (!copied_param) {
            fatal_error("Memory allocation failed for ASTFunctionParameter.\n");
        }

        // Duplicate the parameter name
        if (params->parameter_name) {
            copied_param->parameter_name = strdup(params->parameter_name);
            if (!copied_param->parameter_name) {
                free(copied_param);
                fatal_error("Memory allocation failed for parameter name.\n");
            }
        } else {
            copied_param->parameter_name = NULL;
        }

        copied_param->next = NULL;

        if (!new_params) {
            new_params = tail = copied_param;
        } else {
            tail->next = copied_param;
            tail = copied_param;
        }

        params = params->next;
    }

    return new_params;
}

// Helper function for safely duplicating strings
char *safe_strdup(const char *str) {
    if (!str)
        return NULL;
    char *copy = strdup(str);
    if (!copy) {
        fatal_error("Memory allocation failed for string duplication.\n");
    }
    return copy;
}

// Helper function for copying ASTCaseNode linked lists
ASTCaseNode *copy_ast_case_node(ASTCaseNode *case_node) {
    if (!case_node)
        return NULL;

    ASTCaseNode *new_head = NULL, *new_tail = NULL;

    while (case_node) {
        ASTCaseNode *new_case = malloc(sizeof(ASTCaseNode));
        if (!new_case) {
            fatal_error("Memory allocation failed for ASTCaseNode.\n");
        }

        new_case->condition = copy_ast_node(case_node->condition);
        new_case->body = copy_ast_node(case_node->body);
        new_case->next = NULL;

        if (!new_head) {
            new_head = new_tail = new_case;
        } else {
            new_tail->next = new_case;
            new_tail = new_case;
        }

        case_node = case_node->next;
    }

    return new_head;
}

ASTNode *copy_ast_node(ASTNode *node) {
    if (!node) {
        return NULL;
    }

    debug_print_int("Copying ASTNode of type %d at %p\n", node->type,
                    (void *)node);

    ASTNode *new_node = calloc(1, sizeof(ASTNode));
    if (!new_node) {
        fatal_error("Memory allocation failed in `copy_ast_node`\n");
    }

    // Copy type first & validate it
    new_node->type = node->type;
    if (new_node->type < AST_VAR_DECLARATION ||
        new_node->type > AST_VARIABLE_REFERENCE) {
        fatal_error("Invalid node type %d encountered in copy_ast_node\n",
                    new_node->type);
        free(new_node);
        return NULL;
    }

    switch (node->type) {
    case AST_ASSIGNMENT:
        new_node->assignment.lhs = copy_ast_node(node->assignment.lhs);
        new_node->assignment.rhs = copy_ast_node(node->assignment.rhs);
        break;

    case AST_FUNCTION_DECLARATION:
        new_node->function_declaration.name =
            safe_strdup(node->function_declaration.name);
        new_node->function_declaration.parameters =
            copy_function_parameters(node->function_declaration.parameters);
        new_node->function_declaration.body =
            copy_ast_node(node->function_declaration.body);
        break;

    case AST_FUNCTION_CALL:
        // Deep copy function reference
        new_node->function_call.function_ref =
            copy_ast_node(node->function_call.function_ref);
        if (!new_node->function_call.function_ref &&
            node->function_call.function_ref != NULL) {
            // Handle memory allocation failure
            free(new_node);
            return NULL;
        }

        // Deep copy function arguments
        new_node->function_call.arguments =
            copy_ast_node(node->function_call.arguments);
        if (!new_node->function_call.arguments &&
            node->function_call.arguments != NULL) {
            // Handle memory allocation failure
            free(new_node->function_call.function_ref);
            free(new_node);
            return NULL;
        }
        break;

    case AST_FUNCTION_RETURN:
        new_node->function_return.return_data =
            copy_ast_node(node->function_return.return_data);
        break;

    case AST_LITERAL:
        new_node->literal.type = node->literal.type;
        if (node->literal.type == LITERAL_STRING) {
            new_node->literal.value.string =
                safe_strdup(node->literal.value.string);
        } else {
            new_node->literal.value =
                node->literal.value; // Copy union directly
        }
        break;

    case AST_CONDITIONAL:
        new_node->conditional.condition =
            copy_ast_node(node->conditional.condition);
        new_node->conditional.body = copy_ast_node(node->conditional.body);
        new_node->conditional.else_branch =
            copy_ast_node(node->conditional.else_branch);
        break;

    case AST_UNARY_OP:
        new_node->unary_op.operator= safe_strdup(node->unary_op.operator);
        new_node->unary_op.operand = copy_ast_node(node->unary_op.operand);
        break;

    case AST_BINARY_OP:
        new_node->binary_op.operator= safe_strdup(node->binary_op.operator);
        new_node->binary_op.left = copy_ast_node(node->binary_op.left);
        new_node->binary_op.right = copy_ast_node(node->binary_op.right);
        break;

    case AST_WHILE_LOOP:
        new_node->while_loop.condition =
            copy_ast_node(node->while_loop.condition);
        new_node->while_loop.body = copy_ast_node(node->while_loop.body);
        new_node->while_loop.re_evaluate_condition =
            node->while_loop.re_evaluate_condition;
        break;

    case AST_FOR_LOOP:
        new_node->for_loop.loop_variable =
            safe_strdup(node->for_loop.loop_variable);
        new_node->for_loop.start_expr =
            copy_ast_node(node->for_loop.start_expr);
        new_node->for_loop.end_expr = copy_ast_node(node->for_loop.end_expr);
        new_node->for_loop.inclusive = node->for_loop.inclusive;
        new_node->for_loop.step_expr = copy_ast_node(node->for_loop.step_expr);
        new_node->for_loop.body = copy_ast_node(node->for_loop.body);
        break;

    case AST_SWITCH:
        new_node->switch_case.expression =
            copy_ast_node(node->switch_case.expression);
        new_node->switch_case.cases =
            copy_ast_case_node(node->switch_case.cases);
        break;

    case AST_VAR_DECLARATION:
        new_node->var_declaration.variable_name =
            safe_strdup(node->var_declaration.variable_name);
        new_node->var_declaration.initializer =
            copy_ast_node(node->var_declaration.initializer);
        break;

    case AST_CONST_DECLARATION:
        new_node->const_declaration.constant_name =
            safe_strdup(node->const_declaration.constant_name);
        new_node->const_declaration.initializer =
            copy_ast_node(node->const_declaration.initializer);
        break;

    case AST_TERNARY:
        new_node->ternary.condition = copy_ast_node(node->ternary.condition);
        new_node->ternary.true_expr = copy_ast_node(node->ternary.true_expr);
        new_node->ternary.false_expr = copy_ast_node(node->ternary.false_expr);
        break;

    case AST_TRY:
        new_node->try_block.try_block =
            copy_ast_node(node->try_block.try_block);
        new_node->try_block.catch_blocks =
            copy_catch_node(node->try_block.catch_blocks);
        new_node->try_block.finally_block =
            copy_ast_node(node->try_block.finally_block);
        break;

    case AST_BREAK:
    case AST_CATCH:
    case AST_FINALLY:
        // Nothing to copy
        break;

    case AST_ARRAY_LITERAL:
        new_node->array_literal.count = node->array_literal.count;
        new_node->array_literal.elements =
            malloc(sizeof(ASTNode *) * node->array_literal.count);
        if (!new_node->array_literal.elements) {
            fatal_error(
                "Memory allocation failed for array literal elements.\n");
        }
        for (size_t i = 0; i < node->array_literal.count; i++) {
            new_node->array_literal.elements[i] =
                copy_ast_node(node->array_literal.elements[i]);
        }
        break;

    case AST_ARRAY_OPERATION:
        new_node->array_operation.operator=
            safe_strdup(node->array_operation.operator);
        new_node->array_operation.array =
            copy_ast_node(node->array_operation.array);
        new_node->array_operation.operand =
            copy_ast_node(node->array_operation.operand);
        break;

    case AST_ARRAY_INDEX_ACCESS:
        new_node->array_index_access.array =
            copy_ast_node(node->array_index_access.array);
        new_node->array_index_access.index =
            copy_ast_node(node->array_index_access.index);
        break;

    case AST_ARRAY_SLICE_ACCESS:
        new_node->array_slice_access.array =
            copy_ast_node(node->array_slice_access.array);
        new_node->array_slice_access.start =
            copy_ast_node(node->array_slice_access.start);
        new_node->array_slice_access.end =
            copy_ast_node(node->array_slice_access.end);
        new_node->array_slice_access.step =
            copy_ast_node(node->array_slice_access.step);
        break;

    case AST_VARIABLE_REFERENCE:
        new_node->variable_name = safe_strdup(node->variable_name);
        break;

    default:
        fatal_error("Unknown ASTNodeType encountered in `copy_ast_node`.\n");
    }

    new_node->next = copy_ast_node(node->next);

    return new_node;
}

ASTCatchNode *copy_catch_node(ASTCatchNode *catch_node) {
    if (!catch_node)
        return NULL;

    ASTCatchNode *new_catch = malloc(sizeof(ASTCatchNode));
    if (!new_catch) {
        fatal_error("Memory allocation failed for ASTCatchNode.\n");
    }

    // Duplicate error variable name if it exists
    if (catch_node->error_variable) {
        new_catch->error_variable = strdup(catch_node->error_variable);
        if (!new_catch->error_variable) {
            free(new_catch);
            fatal_error("Memory allocation failed for catch error variable.\n");
        }
    } else {
        new_catch->error_variable = NULL;
    }

    // Recursively copy the catch body
    new_catch->body = copy_ast_node(catch_node->body);

    // Recursively copy the next catch node
    new_catch->next = copy_catch_node(catch_node->next);

    return new_catch;
}

void add_function(Environment *env, Function func) {
    // Ensure the function name is unique
    if (get_function(env, func.name)) {
        fatal_error("Function `%s` is already defined.\n", func.name);
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

    // Deep copy function details
    Function *stored_func = &env->functions[env->function_count++];
    stored_func->parameters = copy_function_parameters(func.parameters);
    stored_func->body = copy_ast_node(func.body);
    stored_func->is_builtin = func.is_builtin;

    stored_func->name = strdup(func.name);
    if (!stored_func->name) {
        fatal_error("Memory allocation failed for function name.\n");
    }

    debug_print_int("Function `%s` added successfully.\n", stored_func->name);
}

/**
 * Function to retrieve a function by name, traversing the environment chain.
 */
Function *get_function(Environment *env, const char *name) {
    Environment *current_env = env;
    while (current_env) {
        for (size_t i = 0; i < current_env->function_count; i++) {
            if (strcmp(current_env->functions[i].name, name) == 0) {
                return &current_env->functions[i];
            }
        }
        current_env = current_env->parent;
    }
    return NULL;
}

// A helper to wrap `LiteralValue` in `InterpretResult`
InterpretResult make_result(LiteralValue val, bool did_return, bool did_break) {
    InterpretResult r;
    r.value = val;
    r.did_return = did_return;
    r.did_break = did_break;
    r.is_error = false;
    return r;
}

bool is_valid_int(const char *str, INT_SIZE *out) {
    char *endptr;
    errno = 0;
    long long val = strtoll(str, &endptr, 10);
    if (errno != 0 || *endptr != '\0') {
        return false;
    }
    *out = (INT_SIZE)val;
    return true;
}

bool is_valid_float(const char *str, FLOAT_SIZE *out) {
    char *endptr;
    errno = 0;
    long double val = strtold(str, &endptr);
    if (errno != 0 || *endptr != '\0') {
        return false;
    }
    *out = (FLOAT_SIZE)val;
    return true;
}

// Type checking helpers
bool is_numeric_type(LiteralType type) {
    return type == TYPE_INTEGER || type == TYPE_FLOAT;
}

bool is_boolean_type(LiteralType type) { return type == TYPE_BOOLEAN; }

// Convert LiteralType to string for error messages
const char *literal_type_to_string(LiteralType type) {
    switch (type) {
    case TYPE_INTEGER:
        return "integer";
    case TYPE_FLOAT:
        return "float";
    case TYPE_BOOLEAN:
        return "boolean";
    case TYPE_STRING:
        return "string";
    case TYPE_FUNCTION:
        return "function";
    case TYPE_ERROR:
        return "error";
    default:
        return "unknown";
    }
}
