#include "interpreter.h"

// Helper function to create a default LiteralValue (zero number)
LiteralValue create_default_value(void) {
    LiteralValue value;
    memset(&value, 0, sizeof(value));
    value.type = TYPE_INTEGER;
    value.data.integer = 0;
    return value;
}

InterpretResult interpret_node(ASTNode *node, Environment *env) {
    if (!node) {
        return make_result(create_default_value(), false, false);
    }

    debug_print_int("`interpret_node()` called\n");

    InterpretResult result = {0};

    switch (node->type) {
    case AST_LITERAL:
        debug_print_int("\tMatched: `AST_LITERAL`\n");
        result = interpret_literal(node);
        break;

    case AST_VAR_DECLARATION: {
        debug_print_int("\tMatched: `AST_VAR_DECLARATION`\n");
        result = interpret_var_declaration(node, env);
        break;
    }

    case AST_CONST_DECLARATION: {
        debug_print_int("\tMatched: `AST_CONST_DECLARATION`\n");
        result = interpret_const_declaration(node, env);
        break;
    }

    case AST_ASSIGNMENT:
        debug_print_int("\tMatched: `AST_ASSIGNMENT`\n");
        result = interpret_assignment(node, env);
        break;

    case AST_VARIABLE_REFERENCE:
        result = interpret_variable_reference(node, env);
        break;

    case AST_UNARY_OP:
        debug_print_int("\tMatched: `AST_UNARY_OP`\n");
        result = interpret_unary_op(node, env);
        break;

    case AST_BINARY_OP:
        debug_print_int("\tMatched: `AST_BINARY_OP`\n");
        result = interpret_binary_op(node, env);
        break;

    case AST_CONDITIONAL: {
        debug_print_int("\tMatched: `AST_CONDITIONAL`\n");
        result = interpret_conditional(node, env);
        break;
    }

    case AST_FUNCTION_CALL: {
        debug_print_int("\tMatched: `AST_FUNCTION_CALL`\n");
        // interpret_function_call(...) returns an InterpretResult
        result = interpret_function_call(node, env);
        break;
    }

    case AST_FUNCTION_DECLARATION: {
        debug_print_int("\tMatched: `AST_FUNCTION_DECLARATION`\n");
        interpret_function_declaration(node, env);
        // No direct return from a function declaration
        result = make_result(create_default_value(), false, false);
        break;
    }

    case AST_FUNCTION_RETURN: {
        // Interpret the return expression
        InterpretResult return_res =
            interpret_node(node->function_return.return_data, env);

        // Log the return value for debugging
        switch (return_res.value.type) {
        case TYPE_INTEGER:
            debug_print_int("Function returning integer: %lld\n",
                            return_res.value.data.integer);
            break;
        case TYPE_FLOAT:
            debug_print_int("Function returning float: %Lf\n",
                            return_res.value.data.floating_point);
            break;
        case TYPE_STRING:
            debug_print_int("Function returning string: %s\n",
                            return_res.value.data.string);
            break;
        // Handle other types as needed
        default:
            debug_print_int("Function returning type: %d\n",
                            return_res.value.type);
        }

        // Propagate errors if any
        if (return_res.is_error) {
            return return_res;
        }

        // Indicate that a return has occurred
        return_res.did_return = true;
        return return_res;
    }

    case AST_WHILE_LOOP: {
        debug_print_int("\tMatched: `AST_WHILE_LOOP`\n");
        result = interpret_while_loop(node, env);
        break;
    }

    case AST_FOR_LOOP: {
        debug_print_int("\tMatched: `AST_FOR_LOOP`\n");
        InterpretResult loop_res = interpret_for_loop(node, env);
        result = loop_res;
        break;
    }

    case AST_SWITCH: {
        debug_print_int("\tMatched: `AST_SWITCH`\n");
        result = interpret_switch(node, env);
        break;
    }

    case AST_BREAK:
        debug_print_int("\tMatched: `AST_BREAK`\n");
        result = make_result(create_default_value(), false, true);
        break;

    case AST_TERNARY:
        debug_print_int("\tMatched: `AST_TERNARY`\n");
        result = interpret_ternary(node, env);
        break;

    case AST_TRY:
        debug_print_int("\tMatched: `AST_TRY`\n");
        result = interpret_try(node, env);
        break;

    case AST_ARRAY_LITERAL:
        debug_print_int("\tMatched: `AST_ARRAY_LITERAL`\n");
        result = interpret_array_literal(node, env);
        break;

    case AST_ARRAY_OPERATION:
        debug_print_int("\tMatched: `AST_ARRAY_OPERATION`\n");
        result = interpret_array_operation(node, env);
        break;

    case AST_ARRAY_INDEX_ACCESS:
        debug_print_int("\tMatched: `AST_ARRAY_INDEX_ACCESS`\n");
        result = interpret_array_index_access(node, env);
        break;

    case AST_ARRAY_SLICE_ACCESS:
        debug_print_int("\tMatched: `AST_ARRAY_SLICE_ACCESS`\n");
        result = interpret_array_slice_access(node, env);
        break;

    default:
        return raise_error("Unsupported `ASTNode` type.\n");
    }

    return result;
}

void interpret_program(ASTNode *program, Environment *env) {
    ASTNode *current = program;
    while (current) {
        debug_print_int("Executing top-level statement\n");
        InterpretResult res = interpret_node(current, env);
        if (res.is_error) {
            fprintf(stderr, "Unhandled error: %s\n", res.value.data.string);
            break; // (or handle as needed in future)
        }
        current = current->next;
    }
}

InterpretResult interpret_literal(ASTNode *node) {
    LiteralValue value;
    debug_print_int("Interpreting literal value...\n");
    debug_print_int("Literal type: %d\n", node->literal.type);

    switch (node->literal.type) {
    case LITERAL_STRING:
        value.type = TYPE_STRING;
        value.data.string = strdup(node->literal.value.string);
        debug_print_int("Created string literal: `%s`\n", value.data.string);
        break;
    case LITERAL_FLOAT:
        value.type = TYPE_FLOAT;
        value.data.floating_point = node->literal.value.floating_point;
        debug_print_int("Created float literal: `%Lf`\n",
                        value.data.floating_point);
        break;
    case LITERAL_INTEGER:
        value.type = TYPE_INTEGER;
        value.data.integer = node->literal.value.integer;
        debug_print_int("Created integer literal: `%lld`\n",
                        value.data.integer);
        break;
    case LITERAL_BOOLEAN:
        value.type = TYPE_BOOLEAN;
        value.data.boolean = node->literal.value.boolean;
        debug_print_int("Created boolean literal: `%s`\n",
                        value.data.boolean ? "True" : "False");
        break;
    default:
        // Let `interpret_node` handle unsupported literals
        value.type = TYPE_ERROR;
        value.data.string = strdup("Unsupported literal type.\n");
        break;
    }

    return make_result(value, false, false);
}

InterpretResult interpret_variable_reference(ASTNode *node, Environment *env) {
    if (node->type != AST_VARIABLE_REFERENCE) {
        return raise_error("Expected AST_VARIABLE_REFERENCE node.\n");
    }

    Variable *var = get_variable(env, node->variable_name);
    if (!var) {
        return raise_error("Undefined variable `%s`.\n", node->variable_name);
    }

    return make_result(var->value, false, false);
}

InterpretResult interpret_var_declaration(ASTNode *node, Environment *env) {
    if (node->type != AST_VAR_DECLARATION) {
        return raise_error("Invalid node type for variable declaration.\n");
    }

    char *var_name = node->var_declaration.variable_name;
    ASTNode *init_expr = node->var_declaration.initializer;

    // Evaluate the initializer expression
    InterpretResult init_val_res = interpret_node(init_expr, env);
    if (init_val_res.is_error) {
        return init_val_res; // propagate the error
    }

    // If that variable already exists in Environment, possibly handle
    // re-declaration For now, just add a brand-new variable
    Variable new_var;
    new_var.variable_name = strdup(var_name);
    new_var.value = init_val_res.value;
    new_var.is_constant = false;

    // Add the variable to Environment
    InterpretResult add_res = add_variable(env, new_var);
    if (add_res.is_error) {
        return add_res;
    }

    return make_result(init_val_res.value, false, false);
}

InterpretResult interpret_const_declaration(ASTNode *node, Environment *env) {
    if (node->type != AST_CONST_DECLARATION) {
        return raise_error("Invalid node type for constant declaration.\n");
    }

    char *const_name = node->const_declaration.constant_name;
    ASTNode *init_expr = node->const_declaration.initializer;

    // Evaluate initializer
    InterpretResult init_val_res = interpret_node(init_expr, env);
    if (init_val_res.is_error) {
        return init_val_res;
    }

    // Add the constant to the environment
    Variable new_var;
    new_var.variable_name = strdup(const_name);
    new_var.value = init_val_res.value;
    new_var.is_constant = true;

    InterpretResult add_res = add_variable(env, new_var);
    if (add_res.is_error) {
        return add_res;
    }

    return make_result(init_val_res.value, false, false);
}

InterpretResult interpret_assignment(ASTNode *node, Environment *env) {
    if (node->type != AST_ASSIGNMENT) {
        return raise_error("Invalid node type for assignment.\n");
    }

    // Interpret the RHS expression
    InterpretResult rhs_val_res = interpret_node(node->assignment.rhs, env);
    if (rhs_val_res.is_error) {
        return rhs_val_res; // propagate the error
    }

    // Determine the LHS type
    ASTNode *lhs_node = node->assignment.lhs;

    switch (lhs_node->type) {
    case AST_VARIABLE_REFERENCE: {
        const char *var_name = lhs_node->variable_name;

        Variable *var = get_variable(env, var_name);
        if (!var) {
            // Variable not found; optionally handle declaration here
            Variable new_var;
            new_var.variable_name = strdup(var_name);
            if (!new_var.variable_name) {
                return raise_error(
                    "Memory allocation failed for variable name `%s`.\n",
                    var_name);
            }
            new_var.value = rhs_val_res.value;
            new_var.is_constant = false;

            InterpretResult add_res = add_variable(env, new_var);
            if (add_res.is_error) {
                free(new_var.variable_name); // clean up on error
                return add_res;
            }

            return add_res;
        }

        if (var->is_constant) {
            return raise_error("Cannot reassign to constant `%s`.\n", var_name);
        }

        // If assigning to a string, free the old string to prevent memory leaks
        if (var->value.type == TYPE_STRING && var->value.data.string) {
            free(var->value.data.string);
        }

        var->value = rhs_val_res.value;

        return rhs_val_res;
    }

    case AST_ARRAY_INDEX_ACCESS: {
        return interpret_array_index_assignment(lhs_node, env,
                                                rhs_val_res.value);
    }

    case AST_ARRAY_SLICE_ACCESS: {
        return interpret_array_slice_access(node, env);
    }

    case AST_ARRAY_OPERATION:
        return interpret_array_operation(node, env);

    default:
        return raise_error("Invalid LHS in assignment.\n");
    }
}

InterpretResult handle_string_concatenation(InterpretResult left,
                                            InterpretResult right) {
    LiteralValue lv_result;
    lv_result.type = TYPE_STRING;

    LiteralValue left_val = left.value;
    LiteralValue right_val = right.value;

    // Convert numbers to strings
    char num_str1[50] = {0};
    char num_str2[50] = {0};

    if (left_val.type == TYPE_FLOAT) {
        snprintf(num_str1, sizeof(num_str1), FLOAT_FORMAT_SPECIFIER,
                 left_val.data.floating_point);
    }
    if (right_val.type == TYPE_FLOAT) {
        snprintf(num_str2, sizeof(num_str2), FLOAT_FORMAT_SPECIFIER,
                 right_val.data.floating_point);
    }

    // Allocate memory for the concatenated string
    size_t new_size =
        strlen(num_str1) + strlen(num_str2) +
        strlen(left_val.type == TYPE_STRING ? left_val.data.string : "") +
        strlen(right_val.type == TYPE_STRING ? right_val.data.string : "") + 1;

    char *new_string = malloc(new_size);
    if (!new_string) {
        return raise_error(
            "Memory allocation failed for string concatenation.\n");
    }

    strcpy(new_string,
           left_val.type == TYPE_STRING ? left_val.data.string : num_str1);
    strcat(new_string,
           right_val.type == TYPE_STRING ? right_val.data.string : num_str2);
    lv_result.data.string = new_string;

    return make_result(lv_result, false, false);
}

// Helper function to handle numeric operations and comparisons
InterpretResult handle_numeric_operator(const char *op,
                                        InterpretResult left_res,
                                        InterpretResult right_res) {
    // Check for errors in operands
    if (left_res.is_error) {
        return left_res;
    }
    if (right_res.is_error) {
        return right_res;
    }

    LiteralValue left = left_res.value;
    LiteralValue right = right_res.value;

    // Ensure both operands are numeric
    if (!is_numeric_type(left.type) || !is_numeric_type(right.type)) {
        return raise_error("Operator `%s` requires numeric operands.\n", op);
    }

    // Determine if the result should be a float
    bool result_is_float =
        (left.type == TYPE_FLOAT || right.type == TYPE_FLOAT);

    // Fetch numeric values with coercion
    FLOAT_SIZE left_val = (left.type == TYPE_FLOAT)
                              ? left.data.floating_point
                              : (FLOAT_SIZE)left.data.integer;
    FLOAT_SIZE right_val = (right.type == TYPE_FLOAT)
                               ? right.data.floating_point
                               : (FLOAT_SIZE)right.data.integer;

    LiteralValue result;
    memset(&result, 0, sizeof(LiteralValue));

    // Handle operators
    if (strcmp(op, "+") == 0) {
        if (result_is_float) {
            result.type = TYPE_FLOAT;
            result.data.floating_point = left_val + right_val;
        } else {
            result.type = TYPE_INTEGER;
            result.data.integer = (INT_SIZE)(left_val + right_val);
        }
    } else if (strcmp(op, "*") == 0) {
        if (result_is_float) {
            result.type = TYPE_FLOAT;
            result.data.floating_point = left_val * right_val;
        } else {
            result.type = TYPE_INTEGER;
            result.data.integer = (INT_SIZE)(left_val * right_val);
        }
    } else if (strcmp(op, "-") == 0) {
        if (result_is_float) {
            result.type = TYPE_FLOAT;
            result.data.floating_point = left_val - right_val;
        } else {
            result.type = TYPE_INTEGER;
            result.data.integer = (INT_SIZE)(left_val - right_val);
        }
    } else if (strcmp(op, "/") == 0) {
        if (right_val == 0.0) {
            return raise_error("Division by zero.\n");
        }
        result.type = TYPE_FLOAT;
        result.data.floating_point = left_val / right_val;
    } else if (strcmp(op, "//") == 0) { // floor Division
        if (right_val == 0.0) {
            return raise_error("Floor division by zero.\n");
        }
        FLOAT_SIZE div_result = left_val / right_val;
        if (result_is_float) {
            result.type = TYPE_FLOAT;
            result.data.floating_point = floor(div_result);
        } else {
            result.type = TYPE_INTEGER;
            result.data.integer = (INT_SIZE)floor(div_result);
        }
    } else if (strcmp(op, "%") == 0) { // modulo
        if (right_val == 0.0) {
            return raise_error("Modulo by zero.\n");
        }
        if (result_is_float) {
            result.type = TYPE_FLOAT;
            result.data.floating_point = fmod(left_val, right_val);
        } else {
            result.type = TYPE_INTEGER;
            result.data.integer =
                (INT_SIZE)((INT_SIZE)left_val % (INT_SIZE)right_val);
        }
    } else if (strcmp(op, "**") == 0) { // exponentiation
        if (result_is_float) {
            result.type = TYPE_FLOAT;
            result.data.floating_point = pow(left_val, right_val);
        } else {
            result.type = TYPE_INTEGER;
            result.data.integer = (INT_SIZE)pow(left_val, right_val);
        }
    } else if (strcmp(op, "<") == 0) {
        result.type = TYPE_BOOLEAN;
        result.data.boolean = (left_val < right_val);
    } else if (strcmp(op, ">") == 0) {
        result.type = TYPE_BOOLEAN;
        result.data.boolean = (left_val > right_val);
    } else if (strcmp(op, "<=") == 0) {
        result.type = TYPE_BOOLEAN;
        result.data.boolean = (left_val <= right_val);
    } else if (strcmp(op, ">=") == 0) {
        result.type = TYPE_BOOLEAN;
        result.data.boolean = (left_val >= right_val);
    } else {
        return raise_error("Unknown operator `%s`.\n", op);
    }

    return make_result(result, false, false);
}

// Function to evaluate binary operators
InterpretResult evaluate_operator(const char *op, InterpretResult left_res,
                                  InterpretResult right_res) {
    debug_print_int("Operator: `%s`\n", op);

    // Handle string concatenation with "+" operator
    if (strcmp(op, "+") == 0 && (left_res.value.type == TYPE_STRING ||
                                 right_res.value.type == TYPE_STRING)) {
        return handle_string_concatenation(left_res, right_res);
    }

    // Handle logical AND and OR
    if (strcmp(op, "&&") == 0 || strcmp(op, "||") == 0) {
        // Ensure both operands are boolean
        if (!is_boolean_type(left_res.value.type) ||
            !is_boolean_type(right_res.value.type)) {
            return raise_error(
                "Logical operators `&&` and `||` require boolean operands.\n");
        }

        LiteralValue result;
        result.type = TYPE_BOOLEAN;

        if (strcmp(op, "&&") == 0) {
            result.data.boolean =
                left_res.value.data.boolean && right_res.value.data.boolean;
        } else { // op == "||"
            result.data.boolean =
                left_res.value.data.boolean || right_res.value.data.boolean;
        }

        return make_result(result, false, false);
    }

    // Handle Equality Operators `==` and `!=` Across All Types
    if (strcmp(op, "==") == 0 || strcmp(op, "!=") == 0) {
        bool comparison_result = false;

        // If types are the same, perform direct comparison
        if (left_res.value.type == right_res.value.type) {
            switch (left_res.value.type) {
            case TYPE_INTEGER:
                comparison_result = (left_res.value.data.integer ==
                                     right_res.value.data.integer);
                break;
            case TYPE_FLOAT:
                comparison_result = (left_res.value.data.floating_point ==
                                     right_res.value.data.floating_point);
                break;
            case TYPE_BOOLEAN:
                comparison_result = (left_res.value.data.boolean ==
                                     right_res.value.data.boolean);
                break;
            case TYPE_STRING:
                if (left_res.value.data.string == NULL ||
                    right_res.value.data.string == NULL) {
                    return raise_error("Cannot compare NULL strings.\n");
                }
                comparison_result = (strcmp(left_res.value.data.string,
                                            right_res.value.data.string) == 0);
                break;
            default:
                return raise_error("Equality operators `==` and `!=` are not "
                                   "supported for this type.\n");
            }
        }
        // Handle cross-type comparisons
        else {
            // For simplicity, handle numeric comparisons by coercing to float
            if (is_numeric_type(left_res.value.type) &&
                is_numeric_type(right_res.value.type)) {
                return handle_numeric_operator(op, left_res, right_res);
            }
            // Handle boolean and integer comparisons
            else if ((left_res.value.type == TYPE_BOOLEAN &&
                      is_numeric_type(right_res.value.type)) ||
                     (right_res.value.type == TYPE_BOOLEAN &&
                      is_numeric_type(left_res.value.type))) {
                // Coerce boolean to integer (false=0, true=1)
                LiteralValue coerced_left = left_res.value;
                LiteralValue coerced_right = right_res.value;

                if (left_res.value.type == TYPE_BOOLEAN) {
                    coerced_left.type = TYPE_INTEGER;
                    coerced_left.data.integer =
                        left_res.value.data.boolean ? 1 : 0;
                }
                if (right_res.value.type == TYPE_BOOLEAN) {
                    coerced_right.type = TYPE_INTEGER;
                    coerced_right.data.integer =
                        right_res.value.data.boolean ? 1 : 0;
                }

                return handle_numeric_operator(
                    op, make_result(coerced_left, false, false),
                    make_result(coerced_right, false, false));
            }
            // Handle string and other type comparisons if necessary
            else {
                return raise_error("Cannot compare different types.\n");
            }
        }

        // Apply `!=` logic if operator is "!="
        if (strcmp(op, "!=") == 0) {
            comparison_result = !comparison_result;
        }

        // Set the result
        LiteralValue result;
        result.type = TYPE_BOOLEAN;
        result.data.boolean = comparison_result;

        return make_result(result, false, false);
    }

    // Handle Arithmetic and Comparison Operators
    // List of operators that require numeric operands
    const char *numeric_operators[] = {
        "+", "*", "-", "/", "//", "%", "**", "<", ">", "<=", ">=",
    };
    size_t num_numeric_ops =
        sizeof(numeric_operators) / sizeof(numeric_operators[0]);

    bool is_numeric_op = false;
    for (size_t i = 0; i < num_numeric_ops; ++i) {
        if (strcmp(op, numeric_operators[i]) == 0) {
            is_numeric_op = true;
            break;
        }
    }

    if (is_numeric_op) {
        return handle_numeric_operator(op, left_res, right_res);
    }

    if (strcmp(op, "+") == 0) {
        return handle_string_concatenation(left_res, right_res);
    }

    // If operator is not recognized
    return raise_error("Unknown operator `%s`.\n", op);
}

InterpretResult interpret_binary_op(ASTNode *node, Environment *env) {
    if (node->type != AST_BINARY_OP) {
        return raise_error("Invalid node type for binary operation.\n");
    }

    // Interpret left operand
    InterpretResult left_res = interpret_node(node->binary_op.left, env);
    if (left_res.is_error) {
        return left_res;
    }

    // Interpret right operand
    InterpretResult right_res = interpret_node(node->binary_op.right, env);
    if (right_res.is_error) {
        return right_res;
    }

    // Evaluate the operator
    const char *op = node->binary_op.operator;
    InterpretResult op_res = evaluate_operator(op, left_res, right_res);
    if (op_res.is_error) {
        return op_res;
    }

    return make_result(op_res.value, false, false);
}

// Implementation of interpret_unary_op
InterpretResult interpret_unary_op(ASTNode *node, Environment *env) {
    if (node->type != AST_UNARY_OP) {
        return raise_error("Invalid node type for unary operation.\n");
    }

    // Interpret the operand
    InterpretResult operand_res = interpret_node(node->unary_op.operand, env);
    if (operand_res.is_error) {
        return operand_res;
    }

    // Evaluate the unary operator
    const char *op = node->unary_op.operator;
    InterpretResult op_res = evaluate_unary_operator(op, operand_res);
    if (op_res.is_error) {
        return op_res;
    }

    return make_result(op_res.value, false, false);
}

// Helper function to handle unary operators
InterpretResult evaluate_unary_operator(const char *op,
                                        InterpretResult operand_res) {
    debug_print_int("Unary Operator: `%s`\n", op);

    // Check for errors in operand
    if (operand_res.is_error) {
        return operand_res;
    }

    LiteralValue operand = operand_res.value;
    LiteralValue result;

    if (strcmp(op, "-") == 0) {
        // Arithmetic negation
        if (operand.type == TYPE_INTEGER) {
            result.type = TYPE_INTEGER;
            result.data.integer = -operand.data.integer;
        } else if (operand.type == TYPE_FLOAT) {
            result.type = TYPE_FLOAT;
            result.data.floating_point = -operand.data.floating_point;
        } else {
            return raise_error(
                "Unary `-` operator requires numeric operand.\n");
        }
    } else if (strcmp(op, "!") == 0) {
        // Logical NOT
        if (operand.type == TYPE_BOOLEAN) {
            result.type = TYPE_BOOLEAN;
            result.data.boolean = !operand.data.boolean;
        } else if (operand.type == TYPE_INTEGER) {
            // Treat 0 as false, non-zero as true
            result.type = TYPE_BOOLEAN;
            result.data.boolean = (operand.data.integer == 0) ? true : false;
        } else {
            return raise_error(
                "Unary `!` operator requires boolean or integer operand.\n");
        }
    } else {
        return raise_error("Unsupported unary operator `%s`.\n", op);
    }

    return make_result(result, false, false);
}

Variable *get_variable(Environment *env, const char *variable_name) {
    debug_print_int("Looking up variable: `%s`\n", variable_name);
    Environment *current_env = env;
    while (current_env) {
        for (size_t i = 0; i < current_env->variable_count; i++) {
            if (strcmp(current_env->variables[i].variable_name,
                       variable_name) == 0) {
                // Debugging information based on type
                switch (current_env->variables[i].value.type) {
                case TYPE_FLOAT:
                    debug_print_int(
                        "Variable found: `%s` with value `%Lf`\n",
                        variable_name,
                        current_env->variables[i].value.data.floating_point);
                    break;
                case TYPE_INTEGER:
                    debug_print_int(
                        "Variable found: `%s` with value `%lld`\n",
                        variable_name,
                        current_env->variables[i].value.data.integer);
                    break;
                case TYPE_STRING:
                    debug_print_int(
                        "Variable found: `%s` with value `%s`\n", variable_name,
                        current_env->variables[i].value.data.string);
                    break;
                case TYPE_FUNCTION:
                    debug_print_int(
                        "Variable found: `%s` with function reference `%s`\n",
                        variable_name,
                        current_env->variables[i]
                            .value.data.function_ptr->name);
                    break;
                case TYPE_ARRAY:
                    debug_print_int(
                        "Variable found: `%s` with array of %zu elements.\n",
                        variable_name,
                        current_env->variables[i].value.data.array.count);
                    break;
                default:
                    debug_print_int("Variable found: `%s` with unknown type.\n",
                                    variable_name);
                }
                return &current_env->variables[i];
            }
        }
        current_env = current_env->parent;
    }
    debug_print_int("Variable not found: `%s`\n", variable_name);
    return NULL;
}

InterpretResult add_variable(Environment *env, Variable var) {
    // Check if the variable already exists
    for (size_t i = 0; i < env->variable_count; i++) {
        if (strcmp(env->variables[i].variable_name, var.variable_name) == 0) {
            // If existing variable is a constant, prevent re-assignment
            if (env->variables[i].is_constant) {
                debug_print_int("Attempted to reassign to constant `%s`\n",
                                var.variable_name);
                return raise_error("Error: Cannot reassign to constant `%s`.\n",
                                   var.variable_name);
            }

            // Update the value of the existing variable
            if (env->variables[i].value.type == TYPE_STRING &&
                env->variables[i].value.data.string) {
                free(env->variables[i].value.data.string);
            }

            env->variables[i].value = var.value;
            // Do not modify is_constant when updating existing variable
            // env->variables[i].is_constant = var.is_constant;

            debug_print_int("Updated variable `%s` with is_constant=%d\n",
                            var.variable_name, env->variables[i].is_constant);

            return make_result(var.value, false, false);
        }
    }

    // Add a new variable
    if (env->variable_count == env->capacity) {
        // Resize the variables array if necessary
        size_t new_capacity = env->capacity ? env->capacity * 2 : 4;
        Variable *new_variables =
            realloc(env->variables, new_capacity * sizeof(Variable));
        if (!new_variables) {
            return raise_error(
                "Memory allocation failed while adding variable `%s`.\n",
                var.variable_name);
        }
        env->variables = new_variables;
        env->capacity = new_capacity;
    }

    // Copy variable name and value
    env->variables[env->variable_count].variable_name =
        strdup(var.variable_name);
    if (!env->variables[env->variable_count].variable_name) {
        return raise_error("Memory allocation failed for variable name `%s`.\n",
                           var.variable_name);
    }

    // Deep copy based on type
    if (var.value.type == TYPE_STRING) {
        env->variables[env->variable_count].value.type = TYPE_STRING;
        env->variables[env->variable_count].value.data.string =
            strdup(var.value.data.string);
        if (!env->variables[env->variable_count].value.data.string) {
            return raise_error(
                "Memory allocation failed for string variable `%s`.\n",
                var.variable_name);
        }
    } else if (var.value.type == TYPE_FUNCTION) {
        env->variables[env->variable_count].value.type = TYPE_FUNCTION;
        env->variables[env->variable_count].value.data.function_ptr =
            var.value.data.function_ptr;
    } else {
        env->variables[env->variable_count].value = var.value;
    }

    env->variables[env->variable_count].is_constant = var.is_constant;
    env->variable_count++;

    debug_print_int("Added variable `%s` with is_constant=%d\n",
                    var.variable_name, var.is_constant);

    return make_result(var.value, false, false);
}

InterpretResult allocate_variable(Environment *env, const char *name) {
    // Check if the variable already exists
    for (size_t i = 0; i < env->variable_count; i++) {
        if (strcmp(env->variables[i].variable_name, name) == 0) {
            return make_result(env->variables[i].value, false, false);
        }
    }

    // If the variable doesn't exist, allocate it in memory
    if (env->variable_count == env->capacity) {
        size_t new_capacity = env->capacity ? env->capacity * 2 : 4;
        Variable *new_variables =
            realloc(env->variables, new_capacity * sizeof(Variable));
        if (!new_variables) {
            return raise_error("Memory allocation failed.\n");
        }
        env->variables = new_variables;
        env->capacity = new_capacity;
    }

    env->variables[env->variable_count].variable_name = strdup(name);
    if (!env->variables[env->variable_count].variable_name) {
        return raise_error("Memory allocation failed for variable name `%s`.\n",
                           name);
    }

    // Initialize the variable with default value
    env->variables[env->variable_count].value = create_default_value();
    env->variables[env->variable_count].is_constant = false;
    env->variable_count++;

    Variable *var = &env->variables[env->variable_count - 1];
    return make_result(var->value, false, false);
}

InterpretResult interpret_conditional(ASTNode *node, Environment *env) {
    debug_print_int("`interpret_conditional()` called\n");
    if (!node) {
        // Error
        return raise_error("Invalid conditional node.");
    }

    bool condition_met = false;

    ASTNode *current_branch = node;

    while (current_branch) {
        if (current_branch->conditional.condition) {
            InterpretResult cond_res =
                interpret_node(current_branch->conditional.condition, env);

            if (cond_res.is_error) {
                // Propagate the error
                return cond_res;
            }

            // Check for valid condition types
            if (cond_res.value.type != TYPE_INTEGER &&
                cond_res.value.type != TYPE_BOOLEAN) {
                return raise_error(
                    "Condition expression must be boolean or integer.\n");
            }

            bool condition_true = false;
            if (cond_res.value.type == TYPE_BOOLEAN) {
                condition_true = cond_res.value.data.boolean;
            } else if (cond_res.value.type == TYPE_INTEGER) {
                condition_true = (cond_res.value.data.integer != 0);
            }

            if (condition_true) {
                // Interpret the conditional body
                ASTNode *cs = current_branch->conditional.body;
                while (cs) {
                    InterpretResult body_res = interpret_node(cs, env);

                    if (body_res.is_error || body_res.did_return ||
                        body_res.did_break) {
                        return body_res;
                    }
                    cs = cs->next;
                }
                condition_met = true;

                // Exit after the first true condition
                break;
            }
        } else {
            // Else branch
            if (!condition_met) {
                ASTNode *cs = current_branch->conditional.body;
                while (cs) {
                    InterpretResult body_res = interpret_node(cs, env);

                    if (body_res.is_error || body_res.did_return ||
                        body_res.did_break) {
                        return body_res;
                    }
                    cs = cs->next;
                }
            }

            // Exit after else
            break;
        }

        current_branch = current_branch->conditional.else_branch;
    }

    debug_print_int("`interpret_conditional()` completed\n");

    // Return a default value if no conditions met
    return make_result(create_default_value(), false, false);
}

InterpretResult interpret_while_loop(ASTNode *node, Environment *env) {
    ASTNode *condition = node->while_loop.condition;
    ASTNode *body = node->while_loop.body;

    while (1) {
        // Check condition
        InterpretResult cond_r = interpret_node(condition, env);
        if (cond_r.did_return || cond_r.did_break) {
            // Propagate break or return upwards
            return cond_r;
        }

        // Evaluate condition as boolean/integer
        bool condition_true = false;
        if (cond_r.value.type == TYPE_BOOLEAN) {
            condition_true = cond_r.value.data.boolean;
        } else if (cond_r.value.type == TYPE_INTEGER) {
            condition_true = (cond_r.value.data.integer != 0);
        }

        // Exit if condition is false
        if (!condition_true) {
            break;
        }

        // Interpret the loop body
        ASTNode *current = body;
        while (current) {
            InterpretResult body_r = interpret_node(current, env);

            // If there's a return or break, propagate it up
            if (body_r.did_return || body_r.did_break) {
                return body_r;
            }

            current = current->next;
        }
    }

    return make_result(create_default_value(), false, false);
}

InterpretResult interpret_for_loop(ASTNode *node, Environment *env) {
    if (node->type != AST_FOR_LOOP) {
        return raise_error(
            "`interpret_for_loop` called with non-`for`-loop `ASTNode`\n");
    }

    // Extract loop components
    char *loop_var = strdup(node->for_loop.loop_variable);
    if (!loop_var) {
        return raise_error("Memory allocation failed for loop variable\n");
    }
    ASTNode *start_expr = node->for_loop.start_expr;
    ASTNode *end_expr = node->for_loop.end_expr;
    bool inclusive = node->for_loop.inclusive;
    ASTNode *step_expr = node->for_loop.step_expr; // may be `NULL`
    ASTNode *body = node->for_loop.body;

    // Evaluate start and end expressions
    InterpretResult start_res = interpret_node(start_expr, env);
    if (start_res.is_error) {
        free(loop_var);
        return start_res;
    }

    InterpretResult end_res = interpret_node(end_expr, env);
    if (end_res.is_error) {
        free(loop_var);
        return end_res;
    }

    // Determine start & end as FLOAT_SIZEs for flexibility
    FLOAT_SIZE start_val, end_val;
    if (start_res.value.type == TYPE_FLOAT) {
        start_val = start_res.value.data.floating_point;
    } else if (start_res.value.type == TYPE_INTEGER) {
        start_val = (FLOAT_SIZE)start_res.value.data.integer;
    } else {
        free(loop_var);
        return raise_error("Start expression in `for` loop must be numeric\n");
    }

    if (end_res.value.type == TYPE_FLOAT) {
        end_val = end_res.value.data.floating_point;
    } else if (end_res.value.type == TYPE_INTEGER) {
        end_val = (FLOAT_SIZE)end_res.value.data.integer;
    } else {
        free(loop_var);
        return raise_error("End expression in `for` loop must be numeric\n");
    }

    // Evaluate step expression if present
    FLOAT_SIZE step = 1.0; // default
    if (step_expr) {
        InterpretResult step_res = interpret_node(step_expr, env);
        if (step_res.is_error) {
            free(loop_var);
            return step_res;
        }

        if (step_res.value.type == TYPE_FLOAT) {
            step = step_res.value.data.floating_point;
        } else if (step_res.value.type == TYPE_INTEGER) {
            step = (FLOAT_SIZE)step_res.value.data.integer;
        } else {
            free(loop_var);
            return raise_error(
                "Step expression in `for` loop must be numeric\n");
        }
    } else {
        // Determine implied step based on range direction
        if (start_val < end_val) {
            step = 1.0;
        } else {
            step = -1.0;
        }
    }

    // Validate step to prevent infinite loops
    if (step < 1e-9 && step > -1e-9) {
        free(loop_var);
        return raise_error("Step value cannot be zero in `for` loop\n");
    }

    // Assign or update loop variable in the environment
    InterpretResult var_res = allocate_variable(env, loop_var);
    if (var_res.is_error) {
        free(loop_var);
        return var_res; // Propagate the error
    }

    // Initialize the loop variable with the start value
    Variable *var = get_variable(env, loop_var);
    if (!var) {
        free(loop_var);
        return raise_error("Failed to allocate and retrieve variable `%s`.\n",
                           loop_var);
    }

    if (start_res.value.type == TYPE_FLOAT) {
        var->value.type = TYPE_FLOAT;
        var->value.data.floating_point = start_val;
    } else {
        var->value.type = TYPE_INTEGER;
        var->value.data.integer = (INT_SIZE)start_val;
    }

    // Determine loop direction
    bool is_ascending = step > 0.0;

    while (1) {
        // Re-fetch the variable pointer at the start of each iteration
        var = get_variable(env, loop_var);
        if (!var) {
            free(loop_var);
            return raise_error("Loop variable `%s` not found in environment\n",
                               loop_var);
        }

        FLOAT_SIZE current_val;
        if (var->value.type == TYPE_FLOAT) {
            current_val = var->value.data.floating_point;
        } else if (var->value.type == TYPE_INTEGER) {
            current_val = (FLOAT_SIZE)var->value.data.integer;
        } else {
            free(loop_var);
            return raise_error("Loop variable `%s` must be numeric\n",
                               loop_var);
        }

        // Check if the loop should continue
        bool condition_true = false;
        if (is_ascending) {
            condition_true =
                inclusive ? (current_val <= end_val) : (current_val < end_val);
        } else {
            condition_true =
                inclusive ? (current_val >= end_val) : (current_val > end_val);
        }

        if (!condition_true) {
            break;
        }

        // Interpret the loop body
        ASTNode *current = body;
        while (current) {
            InterpretResult body_res = interpret_node(current, env);
            if (body_res.did_return || body_res.did_break) {
                free(loop_var);
                return body_res;
            }
            current = current->next;
        }

        // Re-fetch the variable pointer after interpreting the loop body
        var = get_variable(env, loop_var);
        if (!var) {
            free(loop_var);
            return raise_error(
                "Loop variable `%s` not found in environment after loop body\n",
                loop_var);
        }

        // Update loop variable
        if (var->value.type == TYPE_FLOAT) {
            var->value.data.floating_point += step;
        } else if (var->value.type == TYPE_INTEGER) {
            var->value.data.integer += (INT_SIZE)step;
        }
    }

    // Clean up
    free(loop_var);

    // Loops do not return values
    return make_result(create_default_value(), false, false);
}

InterpretResult interpret_switch(ASTNode *node, Environment *env) {
    debug_print_int("`interpret_switch()`\n");

    // Evaluate the switch expression
    InterpretResult switch_r =
        interpret_node(node->switch_case.expression, env);
    LiteralValue switch_val = switch_r.value;
    debug_print_int("Switch expression evaluated\n");

    ASTCaseNode *current_case = node->switch_case.cases;

    while (current_case) {
        if (current_case->condition == NULL) {
            // `else` case
            debug_print_int("Executing `else` case\n");
            ASTNode *current_statement = current_case->body;
            while (current_statement) {
                if (current_statement->type == AST_BREAK) {
                    // Return InterpretResult with did_break = true
                    debug_print_int("Break encountered in else case\n");
                    return make_result(create_default_value(), false, true);
                }
                // Interpret the statement
                InterpretResult stmt_res =
                    interpret_node(current_statement, env);
                if (stmt_res.did_return || stmt_res.did_break) {
                    // Propagate the flags upwards
                    return stmt_res;
                }
                current_statement = current_statement->next;
            }
            // After else, no more cases to execute
            break;
        } else {
            // Evaluate the case condition
            InterpretResult case_r =
                interpret_node(current_case->condition, env);
            LiteralValue case_val = case_r.value;

            bool values_match = false;

            // Handle type comparison
            if (switch_val.type == case_val.type) {
                if (switch_val.type == TYPE_BOOLEAN) {
                    values_match =
                        (switch_val.data.boolean == case_val.data.boolean);
                } else if (switch_val.type == TYPE_FLOAT) {
                    values_match = (switch_val.data.floating_point ==
                                    case_val.data.floating_point);
                } else if (switch_val.type == TYPE_INTEGER) {
                    values_match =
                        (switch_val.data.integer == case_val.data.integer);
                } else if (switch_val.type == TYPE_STRING) {
                    values_match = (strcmp(switch_val.data.string,
                                           case_val.data.string) == 0);
                }
            }

            if (values_match) {
                debug_print_int("Match found, executing case body\n");

                ASTNode *current_statement = current_case->body;
                while (current_statement) {
                    if (current_statement->type == AST_BREAK) {
                        // Return InterpretResult with did_break = true
                        debug_print_int("Break encountered in matched case\n");
                        return make_result(create_default_value(), false, true);
                    }
                    // Interpret the statement
                    InterpretResult stmt_res =
                        interpret_node(current_statement, env);
                    if (stmt_res.did_return || stmt_res.did_break) {
                        // Propagate the flags upwards
                        return stmt_res;
                    }
                    current_statement = current_statement->next;
                }
                // After matched case, no more cases to execute
                break;
            }
        }

        current_case = current_case->next;
    }

    debug_print_int("Switch statement interpretation complete\n");

    // Return a default value, no break or return encountered
    return make_result(create_default_value(), false, false);
}

/**
 * Function to call a user-defined function
 */
InterpretResult call_user_defined_function(Function *func_ref,
                                           ASTNode *call_node,
                                           Environment *env) {
    debug_print_int("Calling user-defined function: `%s`\n", func_ref->name);

    // Create a new local environment with 'env' as its parent
    Environment local_env;
    init_environment_with_parent(&local_env, env);

    // Bind function parameters with arguments
    ASTFunctionParameter *param = func_ref->parameters;
    ASTNode *arg = call_node->function_call.arguments;

    while (param && arg) {
        InterpretResult arg_res = interpret_node(arg, env);
        if (arg_res.is_error) {
            free_environment(&local_env);
            return arg_res; // Propagate the error
        }

        LiteralValue arg_value = arg_res.value;

        // Bind the argument to the parameter in the local environment
        Variable param_var = {.variable_name = strdup(param->parameter_name),
                              .value = arg_value,
                              .is_constant = false};
        InterpretResult add_res = add_variable(&local_env, param_var);
        if (add_res.is_error) {
            free_environment(&local_env);
            return add_res;
        }

        param = param->next;
        arg = arg->next;
    }

    // Check for argument count mismatch
    if (param || arg) {
        free_environment(&local_env);
        return raise_error(
            "Error: Argument count mismatch when calling function `%s`\n",
            func_ref->name);
    }

    // Interpret the function body
    ASTNode *stmt = func_ref->body;
    InterpretResult func_res =
        make_result(create_default_value(), false, false);

    while (stmt) {
        InterpretResult r = interpret_node(stmt, &local_env);
        if (r.did_return) {
            func_res = r;
            break;
        }
        if (r.did_break) {
            free_environment(&local_env);
            return raise_error(
                "Error: 'break' statement outside of loop or switch.\n");
        }
        if (r.is_error) {
            free_environment(&local_env);
            return r;
        }
        stmt = stmt->next;
    }

    free_environment(&local_env);

    // If no explicit return, return default value (e.g., `0`)
    return func_res;
}

InterpretResult interpret_function_declaration(ASTNode *node,
                                               Environment *env) {
    debug_print_int("`interpret_function_declaration()` called\n");

    if (!node || !node->function_declaration.name) {
        fatal_error("Invalid function declaration\n");
    }

    // Initialize param_list as NULL
    ASTFunctionParameter *param_list = NULL;
    ASTFunctionParameter *param_tail = NULL; // keep track of the last parameter

    // Copy parameters
    ASTFunctionParameter *param = node->function_declaration.parameters;
    while (param) {
        ASTFunctionParameter *new_param = malloc(sizeof(ASTFunctionParameter));
        if (!new_param) {
            fatal_error(
                "Error: Memory allocation failed for function parameter\n");
        }

        new_param->parameter_name = strdup(param->parameter_name);
        if (!new_param->parameter_name) {
            free(new_param);
            fatal_error("Memory allocation failed for parameter name\n");
        }

        new_param->next = NULL;

        // Add to list
        if (!param_list) {
            param_list = new_param;
            param_tail = new_param;
        } else {
            param_tail->next = new_param;
            param_tail = new_param;
        }

        param = param->next;
    }

    Function func = {.name = strdup(node->function_declaration.name),
                     .parameters = param_list,
                     .body = node->function_declaration.body,
                     .is_builtin = false};

    add_function(env, func);

    // Also add the function as a variable holding its reference
    LiteralValue func_ref = {.type = TYPE_FUNCTION,
                             .data.function_ptr =
                                 &env->functions[env->function_count - 1]};

    Variable var = {.variable_name = strdup(func.name),
                    .value = func_ref,
                    .is_constant = false};

    add_variable(env, var);

    return make_result(create_default_value(), false, false);
}

InterpretResult interpret_function_call(ASTNode *node, Environment *env) {
    debug_print_int("Starting function call interpretation\n");

    if (!node || !node->function_call.name) {
        return raise_error("Invalid function call");
    }

    const char *func_name = node->function_call.name;

    // 1) Try looking up the function in the functions array (built-in or global
    // user-defined)
    Function *func = get_function(env, func_name);
    if (func) {
        if (func->is_builtin) {
            // Handle built-in functions
            if (strcmp(func->name, "sample") == 0) {
                return builtin_input(node, env);
            } else if (strcmp(func->name, "serve") == 0) {
                return builtin_output(node, env);
            } else if (strcmp(func->name, "burn") == 0) {
                return builtin_error(node, env);
            } else if (strcmp(func->name, "random") == 0) {
                return builtin_random(node, env);
            } else if (strcmp(func->name, "string") == 0 ||
                       strcmp(func->name, "int") == 0 ||
                       strcmp(func->name, "float") == 0) {
                return builtin_cast(node, env);
            } else if (strcmp(func->name, "get_time") == 0) {
                return builtin_time();
            } else if (strcmp(func->name, "taste_file") == 0) {
                return builtin_file_read(node, env);
            } else if (strcmp(func->name, "plate_file") == 0) {
                return builtin_file_write(node, env);
            } else if (strcmp(func->name, "garnish_file") == 0) {
                return builtin_file_append(node, env);
            } else {
                return raise_error("Unknown built-in function `%s`\n",
                                   func->name);
            }
        } else {
            // Handle user-defined functions in the functions array
            return call_user_defined_function(func, node, env);
        }
    }

    // 2) If not found in functions array, check if it's a variable holding
    // a function reference
    Variable *func_var = get_variable(env, func_name);
    if (func_var && func_var->value.type == TYPE_FUNCTION) {
        Function *func_ref = func_var->value.data.function_ptr;
        // Delegate to call_user_defined_function
        return call_user_defined_function(func_ref, node, env);
    }

    // 3) If not found in either, raise an error
    return raise_error("Undefined function `%s`\n", func_name);
}

InterpretResult interpret_ternary(ASTNode *node, Environment *env) {
    if (!node || node->type != AST_TERNARY) {
        return raise_error("Invalid ternary operation node.\n");
    }

    InterpretResult cond_res = interpret_node(node->ternary.condition, env);
    if (cond_res.is_error || cond_res.did_return || cond_res.did_break) {
        return cond_res;
    }

    bool is_true = false;
    if (cond_res.value.type == TYPE_BOOLEAN) {
        is_true = cond_res.value.data.boolean;
    } else if (cond_res.value.type == TYPE_INTEGER) {
        is_true = (cond_res.value.data.integer != 0);
    } else {
        return raise_error("Ternary condition must be boolean or integer.\n");
    }

    if (is_true) {
        InterpretResult true_res = interpret_node(node->ternary.true_expr, env);
        return true_res;
    } else {
        InterpretResult false_res =
            interpret_node(node->ternary.false_expr, env);
        return false_res;
    }
}

InterpretResult interpret_try(ASTNode *node, Environment *env) {
    if (!node || node->type != AST_TRY) {
        return raise_error("Invalid AST node for try block.");
    }

    InterpretResult result = make_result(create_default_value(), false, false);
    bool exception_occurred = false;
    LiteralValue exception_value = create_default_value();

    // Execute try block
    ASTNode *stmt = node->try_block.try_block;
    while (stmt) {
        InterpretResult res = interpret_node(stmt, env);
        if (res.is_error) {
            // An exception has been thrown
            exception_occurred = true;
            exception_value = res.value;
            break;
        }
        if (res.did_return) {
            // Propagate return up
            return res;
        }
        stmt = stmt->next;
    }

    // If exception occurred, handle rescue blocks
    if (exception_occurred) {
        ASTCatchNode *catch = node->try_block.catch_blocks;
        bool handled = false;

        while (catch && !handled) {
            Environment catch_env;
            init_environment(&catch_env);

            // Copy global functions to rescue environment
            for (size_t i = 0; i < env->function_count; i++) {
                Function *global_func = &env->functions[i];
                Function func_copy = {.name = strdup(global_func->name),
                                      .parameters = copy_function_parameters(
                                          global_func->parameters),
                                      .body = copy_ast_node(global_func->body),
                                      .is_builtin = global_func->is_builtin};
                add_function(&catch_env, func_copy);
            }

            // If there's an error variable, bind the exception to it
            if (catch->error_variable) {
                Variable error_var = {.variable_name =
                                          strdup(catch->error_variable),
                                      .value = exception_value,
                                      .is_constant = false};
                add_variable(&catch_env, error_var);
            }

            // Execute catch block
            ASTNode *catch_stmt = catch->body;
            while (catch_stmt) {
                InterpretResult res = interpret_node(catch_stmt, &catch_env);
                if (res.is_error) {
                    // Nested exception, propagate
                    free_environment(&catch_env);
                    return res;
                }
                if (res.did_return) {
                    // Propagate return up
                    free_environment(&catch_env);
                    return res;
                }
                catch_stmt = catch_stmt->next;
            }

            handled = true; // Currently handling only one catch block
            free_environment(&catch_env);
            catch = catch->next;
        }

        if (!handled) {
            // No rescue block handled the exception, propagate it
            result.value = exception_value;
            result.is_error = true;
            return result;
        }
    }

    // Execute finish block if it exists
    if (node->try_block.finally_block) {
        ASTNode *finish_stmt = node->try_block.finally_block;
        while (finish_stmt) {
            InterpretResult res = interpret_node(finish_stmt, env);
            if (res.is_error) {
                // If an exception occurs in finish, prioritize it
                return res;
            }
            if (res.did_return) {
                // Propagate return up
                return res;
            }
            finish_stmt = finish_stmt->next;
        }
    }

    // Normal execution
    return result;
}

/**
 * @brief Interprets array literals like `[1, 2, 3]`.
 *
 * @param node
 * @param env
 * @return InterpretResult
 */
InterpretResult interpret_array_literal(ASTNode *node, Environment *env) {
    if (node->type != AST_ARRAY_LITERAL) {
        return raise_error("Expected AST_ARRAY_LITERAL node.\n");
    }

    ArrayValue array;
    array.count = 0;
    array.capacity = node->array_literal.count > 4 ? node->array_literal.count
                                                   : 4; // initial capacity
    array.elements = malloc(array.capacity * sizeof(LiteralValue));
    if (!array.elements) {
        return raise_error("Memory allocation failed for array elements.\n");
    }

    for (size_t i = 0; i < node->array_literal.count; i++) {
        ASTNode *element_node = node->array_literal.elements[i];
        InterpretResult elem_res = interpret_node(element_node, env);
        if (elem_res.is_error) {
            // Free previously allocated elements if necessary
            // For simplicity, assume no deep copies needed here
            free(array.elements);
            return elem_res; // propagate the error
        }

        // Add the element to the array
        if (array.count == array.capacity) {
            size_t new_capacity = array.capacity * 2;
            LiteralValue *new_elements =
                realloc(array.elements, new_capacity * sizeof(LiteralValue));
            if (!new_elements) {
                free(array.elements);
                return raise_error(
                    "Memory allocation failed while expanding array.\n");
            }
            array.elements = new_elements;
            array.capacity = new_capacity;
        }

        array.elements[array.count++] = elem_res.value;
    }

    LiteralValue result;
    result.type = TYPE_ARRAY;
    result.data.array = array; // by value

    return make_result(result, false, false);
}

InterpretResult interpret_array_operation(ASTNode *node, Environment *env) {
    if (node->type != AST_ARRAY_OPERATION) {
        return raise_error("Expected AST_ARRAY_OPERATION node.\n");
    }

    const char *operator= node->array_operation.operator;
    ASTNode *array_node = node->array_operation.array;
    ASTNode *operand_node = node->array_operation.operand;

    if (array_node->type != AST_VARIABLE_REFERENCE) {
        return raise_error(
            "Array operation requires a variable reference as the array.\n");
    }

    const char *var_name = array_node->variable_name;

    // Retrieve variable from environment
    Variable *var = get_variable(env, var_name);
    if (!var) {
        return raise_error("Undefined variable `%s`.\n", var_name);
    }

    if (var->value.type != TYPE_ARRAY) {
        return raise_error("Array operation requires an array variable.\n");
    }

    // Check for `const`-ness
    if (var->is_constant) {
        return raise_error("Cannot mutate a constant array `%s`.\n", var_name);
    }

    // Access the ArrayValue by reference
    ArrayValue *array = &var->value.data.array;

    // For append & prepend, interpret the operand
    InterpretResult operand_res;
    if (strcmp(operator, "^+") == 0 || strcmp(operator, "+^") == 0) {
        operand_res = interpret_node(operand_node, env);
        if (operand_res.is_error) {
            return operand_res;
        }
    }

    // Perform operation based on operator
    if (strcmp(operator, "^+") == 0) { // Append
        if (array->count == array->capacity) {
            size_t new_capacity = array->capacity * 2;
            LiteralValue *new_elements =
                realloc(array->elements, new_capacity * sizeof(LiteralValue));
            if (!new_elements) {
                return raise_error(
                    "Memory allocation failed while expanding array.\n");
            }
            array->elements = new_elements;
            array->capacity = new_capacity;
        }
        array->elements[array->count++] = operand_res.value;
        // Return the modified array
        return make_result(var->value, false, false);
    } else if (strcmp(operator, "+^") == 0) { // Prepend
        if (array->count == array->capacity) {
            size_t new_capacity = array->capacity * 2;
            LiteralValue *new_elements =
                realloc(array->elements, new_capacity * sizeof(LiteralValue));
            if (!new_elements) {
                return raise_error(
                    "Memory allocation failed while expanding array.\n");
            }
            array->elements = new_elements;
            array->capacity = new_capacity;
        }

        // Shift elements to the right
        memmove(&array->elements[1], &array->elements[0],
                array->count * sizeof(LiteralValue));
        array->elements[0] = operand_res.value;
        array->count++;
        // Return the modified array
        return make_result(var->value, false, false);
    } else if (strcmp(operator, "^-") == 0) { // Remove Last Element
        if (array->count == 0) {
            return raise_error("Cannot remove from an empty array.\n");
        }
        LiteralValue removed = array->elements[array->count - 1];
        array->count--;
        // Return the removed element
        return make_result(removed, false, false);
    } else if (strcmp(operator, "-^") == 0) { // Remove First Element
        if (array->count == 0) {
            return raise_error("Cannot remove from an empty array.\n");
        }
        LiteralValue removed = array->elements[0];
        // Shift elements to the left
        memmove(&array->elements[0], &array->elements[1],
                (array->count - 1) * sizeof(LiteralValue));
        array->count--;
        // Return the removed element
        return make_result(removed, false, false);
    } else {
        return raise_error(
            "Unsupported array operation operator `%s`.\n", operator);
    }
}

/**
 * @brief Handles accessing array elements like `array[2]`.
 *
 * @param node
 * @param env
 * @return InterpretResult
 */
InterpretResult interpret_array_index_access(ASTNode *node, Environment *env) {
    if (node->type != AST_ARRAY_INDEX_ACCESS) {
        return raise_error("Expected AST_ARRAY_INDEX_ACCESS node.\n");
    }

    ASTNode *array_node = node->array_index_access.array;
    ASTNode *index_node = node->array_index_access.index;

    // Interpret the array
    InterpretResult array_res = interpret_node(array_node, env);
    if (array_res.is_error) {
        return array_res;
    }

    if (array_res.value.type != TYPE_ARRAY) {
        return raise_error("Index access requires an array operand.\n");
    }

    // Access the ArrayValue by reference
    ArrayValue *array = &array_res.value.data.array;

    // Interpret the index
    InterpretResult index_res = interpret_node(index_node, env);
    if (index_res.is_error) {
        return index_res;
    }

    // Index must be integer
    if (index_res.value.type != TYPE_INTEGER) {
        return raise_error("Array index must be an integer.\n");
    }

    INT_SIZE index = index_res.value.data.integer;

    // Handle negative indices (e.g., -1 refers to the last element)
    if (index < 0) {
        index = (INT_SIZE)array->count + index;
    }

    if (index < 0 || (size_t)index >= array->count) {
        return raise_error("Array index `%lld` out of bounds.\n", index);
    }

    // Access the element
    LiteralValue element = array->elements[index];
    return make_result(element, false, false);
}

InterpretResult interpret_array_index_assignment(ASTNode *node,
                                                 Environment *env,
                                                 LiteralValue new_value) {
    if (node->type != AST_ARRAY_INDEX_ACCESS) {
        return raise_error("Expected AST_ARRAY_INDEX_ACCESS node.\n");
    }

    ASTNode *array_node = node->array_index_access.array;
    ASTNode *index_node = node->array_index_access.index;

    // Interpret the array
    InterpretResult array_res = interpret_node(array_node, env);
    if (array_res.is_error) {
        return array_res;
    }

    if (array_res.value.type != TYPE_ARRAY) {
        return raise_error("Index access requires an array operand.\n");
    }

    // Retrieve the variable to check for `const`-ness
    if (array_node->type != AST_VARIABLE_REFERENCE) {
        return raise_error("Index assignment requires a variable reference.\n");
    }
    const char *var_name = array_node->variable_name;
    Variable *var = get_variable(env, var_name);
    if (!var) {
        return raise_error("Undefined variable `%s`.\n", var_name);
    }

    if (var->is_constant) {
        debug_print_int("Attempted to assign to const array `%s`\n", var_name);
        return raise_error("Cannot mutate a constant array `%s`.\n", var_name);
    }

    // Access the ArrayValue by reference
    ArrayValue *array = &var->value.data.array;

    // Interpret the index
    InterpretResult index_res = interpret_node(index_node, env);
    if (index_res.is_error) {
        return index_res;
    }

    // Index must be integer
    if (index_res.value.type != TYPE_INTEGER) {
        return raise_error("Array index must be an integer.\n");
    }

    INT_SIZE index = index_res.value.data.integer;

    // Handle negative indices (e.g., -1 refers to the last element)
    if (index < 0) {
        index = (INT_SIZE)array->count + index;
    }

    if (index < 0 || (size_t)index >= array->count) {
        return raise_error("Array index `%lld` out of bounds.\n", index);
    }

    // Assign the new value to the array element
    array->elements[index] = new_value;
    return make_result(new_value, false, false);
}

// Helper function to get the index of a variable in the environment
int get_variable_index(Environment *env, const char *variable_name) {
    for (size_t i = 0; i < env->variable_count; i++) {
        if (strcmp(env->variables[i].variable_name, variable_name) == 0) {
            return (int)i;
        }
    }

    // Not found
    return -1;
}

/**
 * @brief Handles slicing arrays like `array[1:4]`.
 *
 * @param node
 * @param env
 * @return InterpretResult
 */
InterpretResult interpret_array_slice_access(ASTNode *node, Environment *env) {
    if (node->type != AST_ARRAY_SLICE_ACCESS) {
        return raise_error("Expected AST_ARRAY_SLICE_ACCESS node.\n");
    }

    ASTNode *array_node = node->array_slice_access.array;
    ASTNode *start_node = node->array_slice_access.start;
    ASTNode *end_node = node->array_slice_access.end;
    ASTNode *step_node = node->array_slice_access.step;

    // Interpret the array
    InterpretResult array_res = interpret_node(array_node, env);
    if (array_res.is_error) {
        return array_res;
    }

    if (array_res.value.type != TYPE_ARRAY) {
        return raise_error("Slice access requires an array operand.\n");
    }

    // Access the ArrayValue by reference
    ArrayValue *array = &array_res.value.data.array;
    size_t array_count = array->count;

    debug_print_int("Array `%s` has %zu elements.\n", array_node->variable_name,
                    array_count);

    // Interpret the step first to determine default start and end
    FLOAT_SIZE step_val = 1.0; // default step
    if (step_node) {
        InterpretResult step_res = interpret_node(step_node, env);
        if (step_res.is_error) {
            return step_res;
        }
        if (step_res.value.type != TYPE_INTEGER &&
            step_res.value.type != TYPE_FLOAT) {
            return raise_error("Slice step must be integer or float.\n");
        }
        step_val = (step_res.value.type == TYPE_INTEGER)
                       ? (FLOAT_SIZE)step_res.value.data.integer
                       : step_res.value.data.floating_point;
    }

    if (step_val == 0.0) {
        return raise_error("Slice step cannot be zero.\n");
    }

    debug_print_int(
        "Slice parameters: start_node=%p, end_node=%p, step_val=%Lf\n",
        start_node, end_node, step_val);

    // Set default start and end based on step direction
    FLOAT_SIZE start_val;
    FLOAT_SIZE end_val;
    if (step_val > 0) {
        start_val = 0.0;                   // Default start for positive step
        end_val = (FLOAT_SIZE)array_count; // Default end for positive step
    } else {                               // step_val < 0
        start_val =
            (FLOAT_SIZE)(array_count - 1); // Default start for negative step
        end_val = -1.0;                    // Default end for negative step
    }

    // Interpret start if provided
    if (start_node) {
        InterpretResult start_res = interpret_node(start_node, env);
        if (start_res.is_error) {
            return start_res;
        }
        if (start_res.value.type != TYPE_INTEGER &&
            start_res.value.type != TYPE_FLOAT) {
            return raise_error("Slice start must be integer or float.\n");
        }
        start_val = (start_res.value.type == TYPE_INTEGER)
                        ? (FLOAT_SIZE)start_res.value.data.integer
                    : (step_val > 0)
                        ? floor(start_res.value.data.floating_point)
                        : ceil(start_res.value.data.floating_point);
    }

    // Interpret end if provided
    if (end_node) {
        InterpretResult end_res = interpret_node(end_node, env);
        if (end_res.is_error) {
            return end_res;
        }
        if (end_res.value.type != TYPE_INTEGER &&
            end_res.value.type != TYPE_FLOAT) {
            return raise_error("Slice end must be integer or float.\n");
        }
        end_val = (end_res.value.type == TYPE_INTEGER)
                      ? (FLOAT_SIZE)end_res.value.data.integer
                  : (step_val > 0) ? ceil(end_res.value.data.floating_point)
                                   : floor(end_res.value.data.floating_point);
    }

    debug_print_int("Interpreted slice values: start_val=%Lf, end_val=%Lf\n",
                    start_val, end_val);

    // Convert to integer indices
    INT_SIZE start_index = (INT_SIZE)floor(start_val);
    INT_SIZE end_index = (INT_SIZE)ceil(end_val);
    INT_SIZE step = (INT_SIZE)step_val;

    // Handle negative indices based on step direction
    if (step > 0) {
        if (start_index < 0)
            start_index += (INT_SIZE)array_count;
        if (end_index < 0)
            end_index += (INT_SIZE)array_count;
    } else { // step < 0
        if (start_index < 0)
            start_index += (INT_SIZE)array_count;
        // Do NOT adjust end_index for negative steps
    }

    debug_print_int(
        "Converted indices: start_index=%lld, end_index=%lld, step=%lld\n",
        start_index, end_index, step);

    // Clamp indices based on step direction
    if (step > 0) {
        if (start_index < 0)
            start_index = 0;
        if ((size_t)start_index > array_count)
            start_index = array_count;
        if (end_index < 0)
            end_index = 0;
        if ((size_t)end_index > array_count)
            end_index = array_count;
    } else { // step < 0
        if (start_index >= (INT_SIZE)array_count)
            start_index = (INT_SIZE)array_count - 1;
        if (start_index < 0)
            start_index = (INT_SIZE)array_count - 1;
        if (end_index < -1)
            end_index = -1;
        if (end_index >= (INT_SIZE)array_count)
            end_index = -1;
    }

    // Determine the number of elements in the slice
    size_t slice_count = 0;
    if (step > 0) {
        if (start_index >= end_index) {
            slice_count = 0;
        } else {
            slice_count = (size_t)((end_index - start_index + step - 1) / step);
        }
    } else { // step < 0
        if (start_index <= end_index) {
            slice_count = 0;
        } else {
            slice_count =
                (size_t)((start_index - end_index - step - 1) / (-step));
        }
    }

    debug_print_int("Calculated slice_count=%zu\n", slice_count);

    // Initialize the slice array
    ArrayValue slice;
    slice.count = 0;
    slice.capacity = slice_count > 4 ? slice_count : 4;
    slice.elements = malloc(slice.capacity * sizeof(LiteralValue));
    if (!slice.elements) {
        return raise_error("Memory allocation failed for array slice.\n");
    }

    // Populate the slice
    for (INT_SIZE i = start_index;
         (step > 0 && i < end_index) || (step < 0 && i > end_index);
         i += step) {
        debug_print_int("Loop iteration: i=%lld\n", i);

        if (i < 0 || (size_t)i >= array_count) {
            debug_print_int("Index out of bounds: i=%lld, array_count=%zu\n", i,
                            array_count);
            break;
        }
        slice.elements[slice.count++] = array->elements[i];
        if (slice.elements[slice.count - 1].type == TYPE_INTEGER) {
            debug_print_int("Added element: %lld (count=%zu)\n",
                            slice.elements[slice.count - 1].data.integer,
                            slice.count);
        } else {
            debug_print_int("Added element of non-integer type (count=%zu)\n",
                            slice.count);
        }
    }

    debug_print_int("Final slice count=%zu\n", slice.count);

    // Create the slice LiteralValue
    LiteralValue result;
    result.type = TYPE_ARRAY;
    result.data.array = slice; // by value

    return make_result(result, false, false);
}
