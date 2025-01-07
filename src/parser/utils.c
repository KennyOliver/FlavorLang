#include "utils.h"

void free_ast(ASTNode *node) {
    while (node) {
        ASTNode *next = node->next;

        switch (node->type) {
        case AST_ASSIGNMENT:
            // free(node->assignment.variable_name);
            free_ast(node->assignment.lhs);
            free_ast(node->assignment.rhs);
            break;

        case AST_LITERAL:
            if (node->literal.type == LITERAL_STRING) {
                free(node->literal.value.string);
            }
            break;

        case AST_CONDITIONAL:
            free_ast(node->conditional.condition);
            free_ast(node->conditional.body);
            free_ast(node->conditional.else_branch);
            break;

        case AST_UNARY_OP:
            free(node->unary_op.operator);
            free_ast(node->unary_op.operand);
            break;

        case AST_BINARY_OP:
            free(node->binary_op.operator);
            free_ast(node->binary_op.left);
            free_ast(node->binary_op.right);
            break;

        case AST_WHILE_LOOP:
            free_ast(node->while_loop.condition);
            free_ast(node->while_loop.body);
            break;

        case AST_FOR_LOOP:
            free(node->for_loop.loop_variable);
            free_ast(node->for_loop.start_expr);
            free_ast(node->for_loop.end_expr);
            free_ast(node->for_loop.step_expr);
            free_ast(node->for_loop.body);
            break;

        case AST_VAR_DECLARATION:
        case AST_CONST_DECLARATION:
            free(node->variable_name);
            break;

        case AST_FUNCTION_DECLARATION:
            free(node->function_declaration.name);
            free_ast(node->function_declaration.body);
            {
                ASTFunctionParameter *param =
                    node->function_declaration.parameters;
                while (param) {
                    ASTFunctionParameter *next = param->next;
                    free(param->parameter_name);
                    free(param);
                    param = next;
                }
            }
            break;

        case AST_FUNCTION_CALL:
            free(node->function_call.name);
            free_ast(node->function_call.arguments);
            break;

        case AST_FUNCTION_RETURN:
            free_ast(node->function_return.return_data);
            break;

        case AST_TERNARY:
            free_ast(node->ternary.condition);
            free_ast(node->ternary.true_expr);
            free_ast(node->ternary.false_expr);
            break;

        case AST_SWITCH:
            free_ast(node->switch_case.expression);
            {
                ASTCaseNode *case_node = node->switch_case.cases;
                while (case_node) {
                    ASTCaseNode *next = case_node->next;
                    free_ast(case_node->condition);
                    free_ast(case_node->body);
                    free(case_node);
                    case_node = next;
                }
            }
            break;

        case AST_TRY:
            free_ast(node->try_block.try_block);
            {
                ASTCatchNode *catch_node = node->try_block.catch_blocks;
                while (catch_node) {
                    ASTCatchNode *next_catch = catch_node->next;
                    if (catch_node->error_variable) {
                        free(catch_node->error_variable);
                    }
                    free_ast(catch_node->body);
                    free(catch_node);
                    catch_node = next_catch;
                }
            }

            if (node->try_block.finally_block) {
                free_ast(node->try_block.finally_block);
            }
            break;

        case AST_ARRAY_LITERAL:
            for (size_t i = 0; i < node->array_literal.count; i++) {
                free_ast(node->array_literal.elements[i]);
            }
            free(node->array_literal.elements);
            break;

        case AST_ARRAY_OPERATION:
            free(node->array_operation.operator);
            free_ast(node->array_operation.array);
            break;

        case AST_ARRAY_INDEX_ACCESS:
            free_ast(node->array_index_access.array);
            free_ast(node->array_index_access.index);
            break;

        case AST_ARRAY_SLICE_ACCESS:
            free_ast(node->array_slice_access.array);
            free_ast(node->array_slice_access.start);
            free_ast(node->array_slice_access.end);
            free_ast(node->array_slice_access.step);
            break;

        // Already handled by `AST_TRY`
        case AST_CATCH:
        case AST_FINALLY:
        case AST_VARIABLE_REFERENCE:
            // No dynamic memory to free
            break;

        default:
            fprintf(stderr, "Unknown ASTNode type `%d` in free_ast.\n",
                    node->type);
            exit(1);
        }

        free(node);
        node = next;
    }
}

// Print indentation based on depth
void print_indent(int depth) {
    for (int i = 0; i < depth; i++) {
        printf("  ");
    }
}

// Print AST nodes
void print_ast(ASTNode *node, int depth) {
    while (node != NULL) {
        print_indent(depth);
        switch (node->type) {
        case AST_VAR_DECLARATION:
            printf("Variable Declaration: %s\n",
                   node->var_declaration.variable_name);
            if (node->var_declaration.initializer) {
                print_indent(depth + 1);
                printf("Initializer:\n");
                print_ast(node->var_declaration.initializer, depth + 2);
            }
            break;

        case AST_CONST_DECLARATION:
            printf("Constant Declaration: %s\n",
                   node->const_declaration.constant_name);
            if (node->const_declaration.initializer) {
                print_indent(depth + 1);
                printf("Initializer:\n");
                print_ast(node->const_declaration.initializer, depth + 2);
            }
            break;

        case AST_ASSIGNMENT:
            printf("Assignment:\n");
            print_indent(depth + 1);
            printf("LHS:\n");
            print_ast(node->assignment.lhs, depth + 2);
            print_indent(depth + 1);
            printf("RHS:\n");
            print_ast(node->assignment.rhs, depth + 2);
            break;

        case AST_FUNCTION_DECLARATION:
            printf("Function Declaration: %s\n",
                   node->function_declaration.name);
            print_indent(depth + 1);
            printf("Parameters:\n");
            {
                ASTFunctionParameter *param =
                    node->function_declaration.parameters;
                while (param != NULL) {
                    print_indent(depth + 2);
                    printf("- %s\n", param->parameter_name);
                    param = param->next;
                }
            }
            print_indent(depth + 1);
            printf("Body:\n");
            print_ast(node->function_declaration.body, depth + 2);
            break;

        case AST_FUNCTION_CALL:
            printf("Function Call: %s\n", node->function_call.name);
            print_indent(depth + 1);
            printf("Arguments:\n");
            print_ast(node->function_call.arguments, depth + 2);
            break;

        case AST_FUNCTION_RETURN:
            printf("Function Return:\n");
            print_ast(node->function_return.return_data, depth + 1);
            break;

        case AST_LITERAL:
            printf("Literal: ");
            switch (node->literal.type) {
            case LITERAL_STRING:
                printf("String (%s)\n", node->literal.value.string);
                break;
            case LITERAL_FLOAT:
                printf("Float (%Lf)\n", node->literal.value.floating_point);
                break;
            case LITERAL_INTEGER:
                printf("Integer (%lld)\n",
                       (long long)node->literal.value.integer);
                break;
            case LITERAL_BOOLEAN:
                printf("Boolean (%s)\n",
                       node->literal.value.boolean ? "true" : "false");
                break;
            default:
                printf("Unknown Literal Type\n");
            }
            break;

        case AST_CONDITIONAL:
            printf("Conditional:\n");
            print_indent(depth + 1);
            printf("Condition:\n");
            print_ast(node->conditional.condition, depth + 2);
            print_indent(depth + 1);
            printf("Body:\n");
            print_ast(node->conditional.body, depth + 2);
            if (node->conditional.else_branch) {
                print_indent(depth + 1);
                printf("Else Branch:\n");
                print_ast(node->conditional.else_branch, depth + 2);
            }
            break;

        case AST_UNARY_OP:
            printf("Unary Operation: %s\n", node->unary_op.operator);
            print_indent(depth + 1);
            printf("Operand:\n");
            print_ast(node->unary_op.operand, depth + 2);
            break;

        case AST_BINARY_OP:
            printf("Binary Operation: %s\n", node->binary_op.operator);
            print_indent(depth + 1);
            printf("Left:\n");
            print_ast(node->binary_op.left, depth + 2);
            print_indent(depth + 1);
            printf("Right:\n");
            print_ast(node->binary_op.right, depth + 2);
            break;

        case AST_WHILE_LOOP:
            printf("While Loop:\n");
            print_indent(depth + 1);
            printf("Condition:\n");
            print_ast(node->while_loop.condition, depth + 2);
            print_indent(depth + 1);
            printf("Body:\n");
            print_ast(node->while_loop.body, depth + 2);
            printf("Re-evaluate Condition: %s\n",
                   node->while_loop.re_evaluate_condition ? "true" : "false");
            break;

        case AST_FOR_LOOP:
            printf("For Loop:\n");
            print_indent(depth + 1);
            printf("Loop Variable: %s\n", node->for_loop.loop_variable);
            print_indent(depth + 1);
            printf("Start Expression:\n");
            print_ast(node->for_loop.start_expr, depth + 2);
            print_indent(depth + 1);
            printf("End Expression:\n");
            print_ast(node->for_loop.end_expr, depth + 2);
            print_indent(depth + 1);
            printf("Inclusive: %s\n",
                   node->for_loop.inclusive ? "true" : "false");
            if (node->for_loop.step_expr) {
                print_indent(depth + 1);
                printf("Step Expression:\n");
                print_ast(node->for_loop.step_expr, depth + 2);
            }
            print_indent(depth + 1);
            printf("Body:\n");
            print_ast(node->for_loop.body, depth + 2);
            break;

        case AST_SWITCH:
            printf("Switch Statement:\n");
            print_indent(depth + 1);
            printf("Expression:\n");
            print_ast(node->switch_case.expression, depth + 2);
            print_indent(depth + 1);
            printf("Cases:\n");
            {
                ASTCaseNode *case_node = node->switch_case.cases;
                while (case_node != NULL) {
                    print_indent(depth + 2);
                    printf("Case:\n");
                    if (case_node->condition) {
                        print_indent(depth + 3);
                        printf("Condition:\n");
                        print_ast(case_node->condition, depth + 4);
                    } else {
                        print_indent(depth + 3);
                        printf("Condition: None (Default)\n");
                    }
                    print_indent(depth + 3);
                    printf("Body:\n");
                    print_ast(case_node->body, depth + 4);
                    case_node = case_node->next;
                }
            }
            break;

        case AST_BREAK:
            printf("Break Statement\n");
            break;

        case AST_TERNARY:
            printf("Ternary Operation:\n");
            print_indent(depth + 1);
            printf("Condition:\n");
            print_ast(node->ternary.condition, depth + 2);
            print_indent(depth + 1);
            printf("True Expression:\n");
            print_ast(node->ternary.true_expr, depth + 2);
            print_indent(depth + 1);
            printf("False Expression:\n");
            print_ast(node->ternary.false_expr, depth + 2);
            break;

        case AST_TRY:
            printf("Try Block:\n");
            print_indent(depth + 1);
            printf("Try Block Content:\n");
            print_ast(node->try_block.try_block, depth + 2);
            // Handle catch blocks
            if (node->try_block.catch_blocks) {
                ASTCatchNode *catch_node = node->try_block.catch_blocks;
                while (catch_node != NULL) {
                    print_indent(depth + 1);
                    printf("Catch Block (Error Variable: %s):\n",
                           catch_node->error_variable
                               ? catch_node->error_variable
                               : "None");
                    print_ast(catch_node->body, depth + 2);
                    catch_node = catch_node->next;
                }
            }
            // Handle finally block
            if (node->try_block.finally_block) {
                print_indent(depth + 1);
                printf("Finally Block:\n");
                print_ast(node->try_block.finally_block, depth + 2);
            }
            break;

        case AST_ARRAY_LITERAL:
            printf("Array Literal: [\n");
            for (size_t i = 0; i < node->array_literal.count; i++) {
                print_ast(node->array_literal.elements[i], depth + 1);
            }
            print_indent(depth);
            printf("]\n");
            break;

        case AST_ARRAY_OPERATION:
            printf("Array Operation: %s\n", node->array_operation.operator);
            print_indent(depth + 1);
            printf("Array:\n");
            print_ast(node->array_operation.array, depth + 2);
            break;

        case AST_ARRAY_INDEX_ACCESS:
            printf("Array Index Access:\n");
            print_indent(depth + 1);
            printf("Array:\n");
            print_ast(node->array_index_access.array, depth + 2);
            print_indent(depth + 1);
            printf("Index:\n");
            print_ast(node->array_index_access.index, depth + 2);
            break;

        case AST_ARRAY_SLICE_ACCESS:
            printf("Array Slice Access:\n");
            print_indent(depth + 1);
            printf("Array:\n");
            print_ast(node->array_slice_access.array, depth + 2);
            if (node->array_slice_access.start) {
                print_indent(depth + 1);
                printf("Start:\n");
                print_ast(node->array_slice_access.start, depth + 2);
            } else {
                print_indent(depth + 1);
                printf("Start: NULL\n");
            }
            if (node->array_slice_access.end) {
                print_indent(depth + 1);
                printf("End:\n");
                print_ast(node->array_slice_access.end, depth + 2);
            } else {
                print_indent(depth + 1);
                printf("End: NULL\n");
            }
            if (node->array_slice_access.step) {
                print_indent(depth + 1);
                printf("Step:\n");
                print_ast(node->array_slice_access.step, depth + 2);
            } else {
                print_indent(depth + 1);
                printf("Step: NULL\n");
            }
            break;

        case AST_VARIABLE_REFERENCE:
            printf("Variable Reference: %s\n", node->variable_name);
            break;

        default:
            printf("Unknown AST Node Type: %d\n", node->type);
            break;
        }

        // Move to the next node in the linked list, if any
        node = node->next;
    }
}
