#include "utils.h"

void free_ast(ASTNode *node) {
    while (node) {
        ASTNode *next = node->next;

        switch (node->type) {

        case AST_ASSIGNMENT:
            free(node->assignment.variable_name);
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

        case AST_CONST_DECLARATION:
        case AST_VAR_DECLARATION:
            free(node->variable_name);
            break;

        case AST_FUNCTION_DECLARATION:
            free(node->function_declaration.name);
            free_ast(node->function_declaration
                         .body); // free function body recursively

            // Free the function parameters
            ASTFunctionParameter *param = node->function_declaration.parameters;
            while (param) {
                ASTFunctionParameter *next = param->next;
                free(param->parameter_name);
                free(param);
                param = next;
            }

            break;

        case AST_FUNCTION_CALL:
            free(node->function_call.name);
            free_ast(node->function_call.arguments);
            break;

        case AST_FUNCTION_RETURN:
            free_ast(node->function_return.return_data);
            break;

        case AST_BREAK:
            // No cleanup needed!
            break;

        case AST_TERNARY:
            free_ast(node->ternary.condition);
            free_ast(node->ternary.true_expr);
            free_ast(node->ternary.false_expr);
            break;

        case AST_SWITCH: {
            if (node->switch_case.expression) {
                free_ast(node->switch_case.expression);
            }

            if (node->switch_case.cases) {
                ASTCaseNode *case_node = node->switch_case.cases;

                while (case_node) {
                    ASTCaseNode *next = case_node->next;

                    // Free the condition ASTNode if it exists
                    if (case_node->condition) {
                        free_ast(case_node->condition);
                    }

                    // Free the body
                    if (case_node->body) {
                        free_ast(case_node->body);
                    }

                    free(case_node);
                    case_node = next;
                }
            }

            break;
        }

        case AST_TRY:
            // Free try block
            if (node->try_block.try_block) {
                free_ast(node->try_block.try_block);
            }

            // Free all catch blocks
            if (node->try_block.catch_blocks) {
                ASTCatchNode *catch_node = node->try_block.catch_blocks;

                while (catch_node) {
                    ASTCatchNode *next_catch = catch_node->next;

                    // Free the error variable (if allocated)
                    if (catch_node->error_variable) {
                        free(catch_node->error_variable);
                    }

                    // Free the body of the catch block
                    if (catch_node->body) {
                        free_ast(catch_node->body);
                    }

                    free(catch_node);
                    catch_node = next_catch;
                }
            }

            // Free finally block
            if (node->try_block.finally_block) {
                free_ast(node->try_block.finally_block);
            }
            break;

        // Already handled by `AST_TRY`
        case AST_CATCH:
        case AST_FINALLY:
            break;

        default:
            fprintf(stderr,
                    "Error: Unknown `ASTNode` type `%d` in `free_ast`.\n",
                    node->type);
            exit(1);
        }

        free(node);
        node = next;
    }
}

// Print indentation based on depth
void print_indentation(int depth) {
    for (int i = 0; i < depth; i++) {
        printf("  ");
    }
}

// Print AST nodes
void print_ast(ASTNode *node, int depth) {
    if (!debug_flag) {
        return;
    }

    while (node != NULL) {
        print_indentation(depth);

        switch (node->type) {
        case AST_ASSIGNMENT:
            printf("Assignment:\n");
            print_indentation(depth + 1);
            printf("Variable: `%s`\n", node->assignment.variable_name);
            print_indentation(depth + 1);
            printf("Value:\n");
            print_ast(node->assignment.lhs, depth + 2);
            print_ast(node->assignment.rhs, depth + 2);
            break;

        case AST_FUNCTION_DECLARATION:
            printf("Function Declaration: `%s`\n",
                   node->function_declaration.name);
            // Print Parameters
            if (node->function_declaration.parameters != NULL) {
                print_indentation(depth + 1);
                printf("Parameters:\n");
                ASTFunctionParameter *param =
                    node->function_declaration.parameters;
                while (param != NULL) {
                    print_indentation(depth + 2);
                    printf("- `%s`\n", param->parameter_name);
                    param = param->next;
                }
            } else {
                print_indentation(depth + 1);
                printf("Parameters: None\n");
            }
            // Print Body
            if (node->function_declaration.body != NULL) {
                print_indentation(depth + 1);
                printf("Body:\n");
                print_ast(node->function_declaration.body, depth + 2);
            } else {
                print_indentation(depth + 1);
                printf("Body: None\n");
            }
            break;

        case AST_FUNCTION_CALL:
            printf("Function Call: `%s`\n", node->function_call.name);
            // Print Arguments
            if (node->function_call.arguments != NULL) {
                print_indentation(depth + 1);
                printf("Arguments:\n");
                print_ast(node->function_call.arguments, depth + 2);
            } else {
                print_indentation(depth + 1);
                printf("Arguments: None\n");
            }
            break;

        case AST_FUNCTION_RETURN:
            printf("Function Return:\n");
            if (node->function_return.return_data != NULL) {
                print_ast(node->function_return.return_data, depth + 1);
            } else {
                print_indentation(depth + 1);
                printf("Return Data: None\n");
            }
            break;

        case AST_LITERAL:
            printf("Literal: ");
            switch (node->literal.type) {
            case LITERAL_INTEGER:
                printf("`%lld`\n", node->literal.value.integer);
                break;
            case LITERAL_FLOAT:
                printf("`%Lf`\n", node->literal.value.floating_point);
                break;
            case LITERAL_STRING:
                printf("`\"%s\"`\n", node->literal.value.string);
                break;
            case LITERAL_BOOLEAN:
                printf(node->literal.value.boolean ? "True\n" : "False\n");
                break;
            default:
                printf("Unknown Literal Type\n");
            }
            break;

        case AST_CONDITIONAL:
            printf("Conditional Statement:\n");
            print_indentation(depth + 1);
            printf("Condition:\n");
            print_ast(node->conditional.condition, depth + 2);
            print_indentation(depth + 1);
            printf("Body:\n");
            print_ast(node->conditional.body, depth + 2);
            if (node->conditional.else_branch != NULL) {
                print_indentation(depth + 1);
                printf("Else Branch:\n");
                print_ast(node->conditional.else_branch, depth + 2);
            }
            break;

        case AST_UNARY_OP:
            printf("Unary Operation: `%s`\n", node->unary_op.operator);
            print_ast(node->unary_op.operand, depth + 1);
            break;

        case AST_BINARY_OP:
            printf("Binary Operation: `%s`\n", node->binary_op.operator);
            print_indentation(depth + 1);
            printf("Left Operand:\n");
            print_ast(node->binary_op.left, depth + 2);
            print_indentation(depth + 1);
            printf("Right Operand:\n");
            print_ast(node->binary_op.right, depth + 2);
            break;

        case AST_WHILE_LOOP:
            printf("While Loop:\n");
            print_indentation(depth + 1);
            printf("Condition:\n");
            print_ast(node->while_loop.condition, depth + 2);
            print_indentation(depth + 1);
            printf("Body:\n");
            print_ast(node->while_loop.body, depth + 2);
            break;

        case AST_FOR_LOOP:
            printf("For Loop:\n");
            print_indentation(depth + 1);
            printf("Loop Variable: `%s`\n", node->for_loop.loop_variable);
            print_indentation(depth + 1);
            printf("Start Expression:\n");
            print_ast(node->for_loop.start_expr, depth + 2);
            print_indentation(depth + 1);
            printf("End Expression:\n");
            print_ast(node->for_loop.end_expr, depth + 2);
            print_indentation(depth + 1);
            printf("Inclusive: `%s`\n",
                   node->for_loop.inclusive ? "True" : "False");
            if (node->for_loop.step_expr != NULL) {
                print_indentation(depth + 1);
                printf("Step Expression:\n");
                print_ast(node->for_loop.step_expr, depth + 2);
            } else {
                print_indentation(depth + 1);
                printf("Step Expression: None\n");
            }
            print_indentation(depth + 1);
            printf("Body:\n");
            print_ast(node->for_loop.body, depth + 2);
            break;

        case AST_SWITCH:
            printf("Switch Statement:\n");
            print_indentation(depth + 1);
            printf("Expression:\n");
            print_ast(node->switch_case.expression, depth + 2);
            print_indentation(depth + 1);
            printf("Cases:\n");
            ASTCaseNode *case_node = node->switch_case.cases;
            size_t case_num = 1;
            while (case_node != NULL) {
                print_indentation(depth + 2);
                printf("Case %zu:\n", case_num++);
                print_indentation(depth + 3);
                printf("Condition:\n");
                print_ast(case_node->condition, depth + 4);
                print_indentation(depth + 3);
                printf("Body:\n");
                print_ast(case_node->body, depth + 4);
                case_node = case_node->next;
            }
            break;

        case AST_BREAK:
            printf("Break Statement\n");
            break;

        case AST_TERNARY:
            printf("Ternary Operation:\n");
            print_indentation(depth + 1);
            printf("Condition:\n");
            print_ast(node->ternary.condition, depth + 2);
            print_indentation(depth + 1);
            printf("True Expression:\n");
            print_ast(node->ternary.true_expr, depth + 2);
            print_indentation(depth + 1);
            printf("False Expression:\n");
            print_ast(node->ternary.false_expr, depth + 2);
            break;

        case AST_VAR_DECLARATION:
            printf("Variable: `%s`\n", node->variable_name);
            break;

        case AST_CONST_DECLARATION:
            printf("Constant: `%s`\n", node->variable_name);
            break;

        case AST_ARRAY_LITERAL:
            printf("AST_ARRAY_LITERAL: [\n");
            for (size_t i = 0; i < node->array_literal.count; i++) {
                print_ast(node->array_literal.elements[i], depth + 1);
            }
            for (int i = 0; i < depth; i++)
                printf("  ");
            printf("]\n");
            break;
        case AST_ARRAY_OPERATION:
            printf("AST_ARRAY_OPERATION: %s\n", node->array_operation.operator);
            break;
        case AST_ARRAY_INDEX_ACCESS:
            printf("AST_INDEX_ACCESS:\n");
            print_ast(node->array_index_access.array, depth + 1);
            print_ast(node->array_index_access.index, depth + 1);
            break;
        case AST_ARRAY_SLICE_ACCESS:
            printf("AST_SLICE_ACCESS:\n");
            for (int i = 0; i < depth + 1; i++)
                printf("  ");
            printf("Array:\n");
            print_ast(node->array_slice_access.array, depth + 2);
            for (int i = 0; i < depth + 1; i++)
                printf("  ");
            printf("Start:\n");
            if (node->array_slice_access.start) {
                print_ast(node->array_slice_access.start, depth + 2);
            } else {
                for (int i = 0; i < depth + 2; i++)
                    printf("  ");
                printf("NULL\n");
            }
            for (int i = 0; i < depth + 1; i++)
                printf("  ");
            printf("End:\n");
            if (node->array_slice_access.end) {
                print_ast(node->array_slice_access.end, depth + 2);
            } else {
                for (int i = 0; i < depth + 2; i++)
                    printf("  ");
                printf("NULL\n");
            }
            for (int i = 0; i < depth + 1; i++)
                printf("  ");
            printf("Step:\n");
            if (node->array_slice_access.step) {
                print_ast(node->array_slice_access.step, depth + 2);
            } else {
                for (int i = 0; i < depth + 2; i++)
                    printf("  ");
                printf("NULL\n");
            }
            break;

        default:
            printf("Unknown Node Type: `%d`\n", node->type);
        }

        // Move to the next node in the linked list
        node = node->next;
    }
}
