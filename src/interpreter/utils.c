#include "utils.h"

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

ASTNode *copy_ast_node(ASTNode *node) {
    if (!node) {
        return NULL;
    }

    debug_print_int("Copying ASTNode of type %d at %p\n", node->type,
                    (void *)node);

    ASTNode *new_node = malloc(sizeof(ASTNode));
    if (!new_node) {
        fatal_error("Memory allocation failed in `copy_ast_node`\n");
    }

    // Initialize the new node to zero to prevent dangling pointers
    memset(new_node, 0, sizeof(ASTNode));

    // Copy the node type
    new_node->type = node->type;

    // Deep copy based on node type
    switch (node->type) {
    case AST_ASSIGNMENT:
        if (node->assignment.variable_name) {
            new_node->assignment.variable_name =
                strdup(node->assignment.variable_name);
            if (!new_node->assignment.variable_name) {
                fatal_error("Memory allocation failed for variable name in "
                            "assignment.\n");
            }
        } else {
            new_node->assignment.variable_name = NULL;
        }
        new_node->assignment.value = copy_ast_node(node->assignment.value);
        break;

    case AST_FUNCTION_DECLARATION:
        if (node->function_declaration.name) {
            new_node->function_declaration.name =
                strdup(node->function_declaration.name);
            if (!new_node->function_declaration.name) {
                fatal_error("Memory allocation failed for function name in "
                            "declaration.\n");
            }
        } else {
            new_node->function_declaration.name = NULL;
        }
        new_node->function_declaration.parameters =
            copy_function_parameters(node->function_declaration.parameters);
        new_node->function_declaration.body =
            copy_ast_node(node->function_declaration.body);
        break;

    case AST_FUNCTION_CALL:
        if (node->function_call.name) {
            new_node->function_call.name = strdup(node->function_call.name);
            if (!new_node->function_call.name) {
                fatal_error(
                    "Memory allocation failed for function name in call.\n");
            }
        } else {
            new_node->function_call.name = NULL;
        }
        new_node->function_call.arguments =
            copy_ast_node(node->function_call.arguments);
        break;

    case AST_FUNCTION_RETURN:
        new_node->function_return.return_data =
            copy_ast_node(node->function_return.return_data);
        break;

    case AST_LITERAL:
        new_node->literal.type = node->literal.type;
        switch (node->literal.type) {
        case LITERAL_STRING:
            if (node->literal.value.string) {
                new_node->literal.value.string =
                    strdup(node->literal.value.string);
                if (!new_node->literal.value.string) {
                    fatal_error(
                        "Memory allocation failed for string literal.\n");
                }
            } else {
                new_node->literal.value.string = NULL;
            }
            break;
        case LITERAL_FLOAT:
            new_node->literal.value.floating_point =
                node->literal.value.floating_point;
            break;
        case LITERAL_INTEGER:
            new_node->literal.value.integer = node->literal.value.integer;
            break;
        case LITERAL_BOOLEAN:
            new_node->literal.value.boolean = node->literal.value.boolean;
            break;
        default:
            fatal_error("Unknown literal type during copy.\n");
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
        if (node->unary_op.operator) {
            new_node->unary_op.operator= strdup(node->unary_op.operator);
            if (!new_node->unary_op.operator) {
                fatal_error("Memory allocation failed for unary operator.\n");
            }
        } else {
            new_node->unary_op.operator= NULL;
        }
        new_node->unary_op.operand = copy_ast_node(node->unary_op.operand);
        break;

    case AST_BINARY_OP:
        if (node->binary_op.operator) {
            new_node->binary_op.operator= strdup(node->binary_op.operator);
            if (!new_node->binary_op.operator) {
                fatal_error("Memory allocation failed for binary operator.\n");
            }
        } else {
            new_node->binary_op.operator= NULL;
        }
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
        if (node->for_loop.loop_variable) {
            new_node->for_loop.loop_variable =
                strdup(node->for_loop.loop_variable);
            if (!new_node->for_loop.loop_variable) {
                fatal_error("Memory allocation failed for loop variable in "
                            "for-loop.\n");
            }
        } else {
            new_node->for_loop.loop_variable = NULL;
        }
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
        ASTCaseNode *current_case = node->switch_case.cases;
        ASTCaseNode *new_case_head = NULL;
        ASTCaseNode *new_case_tail = NULL;
        while (current_case) {
            ASTCaseNode *copied_case = malloc(sizeof(ASTCaseNode));
            if (!copied_case) {
                fatal_error("Memory allocation failed for ASTCaseNode.\n");
            }
            copied_case->condition = copy_ast_node(current_case->condition);
            copied_case->body = copy_ast_node(current_case->body);
            copied_case->next = NULL;
            if (!new_case_head) {
                new_case_head = new_case_tail = copied_case;
            } else {
                new_case_tail->next = copied_case;
                new_case_tail = copied_case;
            }
            current_case = current_case->next;
        }
        new_node->switch_case.cases = new_case_head;
        break;

    case AST_VARIABLE:
    case AST_CONSTANT:
        if (node->variable_name) {
            new_node->variable_name = strdup(node->variable_name);
            if (!new_node->variable_name) {
                fatal_error(
                    "Memory allocation failed for variable/constant name.\n");
            }
        } else {
            new_node->variable_name = NULL;
        }
        break;

    case AST_BREAK:
        // No fields to copy
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
