#include "parser.h"
#include "../debug/debug.h"
#include <string.h>

ASTNode *parse_program(Token *tokens) {
    ParserState *state = create_parser_state(tokens);
    ASTNode *head = NULL;
    ASTNode *tail = NULL;

    while (get_current_token(state)->type != TOKEN_EOF) {
        ASTNode *stmt = parse_statement(state);
        if (!stmt)
            break;

        if (!head) {
            head = tail = stmt;
        } else {
            tail->next = stmt;
            tail = stmt;
        }
    }

    free_parser_state(state);
    return head;
}

ASTNode *parse_statement(ParserState *state) {
    Token *token = get_current_token(state);

    if (!token)
        return NULL;

    if (match_token(state, "let"))
        return parse_variable_declaration(state);
    if (token->type == TOKEN_IDENTIFIER) {
        Token *next_token = peek_next_token(state);
        if (next_token && next_token->type == TOKEN_OPERATOR &&
            strcmp(next_token->lexeme, "=") == 0) {
            return parse_variable_assignment(state);
        } else {
            return parse_expression_statement(state);
        }
    }
    if (match_token(state, "show"))
        return parse_print_statement(state);
    if (match_token(state, "burn"))
        return parse_raise_error(state);
    if (match_token(state, "taste"))
        return parse_input(state);
    if (match_token(state, "if"))
        return parse_conditional_block(state);
    if (match_token(state, "while"))
        return parse_while_loop(state);
    if (match_token(state, "for"))
        return parse_for_loop(state);
    if (match_token(state, "check"))
        return parse_switch_block(state);
    if (match_token(state, "create"))
        return parse_function_declaration(state);
    if (token->type == TOKEN_FUNCTION_NAME) {
        ASTNode *node = parse_function_call(state);
        expect_token(state, TOKEN_DELIMITER,
                     "Expected `;` after function call");
        return node;
    }

    parser_error("Unexpected token at start of statement", token);
    return NULL;
}

bool match_token(ParserState *state, const char *lexeme) {
    Token *token = get_current_token(state);
    return token && strcmp(token->lexeme, lexeme) == 0;
}

Token *peek_next_token(ParserState *state) {
    // Just look at the next token (but don’t advance)
    return &state->tokens[state->current_token + 1];
}

ASTNode *parse_expression_statement(ParserState *state) {
    // Parse the expression (which can handle binary ops, variables, etc.)
    ASTNode *expr_node = parse_expression(state);
    expect_token(state, TOKEN_DELIMITER,
                 "Expected `;` after expression statement");
    return expr_node;
}

ASTNode *parse_variable_declaration(ParserState *state) {
    debug_print_par("Starting variable declaration parse\n");
    expect_token(state, TOKEN_KEYWORD, "Expected `let` keyword");

    // Parse variable name
    Token *name = get_current_token(state);
    expect_token(state, TOKEN_IDENTIFIER, "Expected variable name");

    // Expect '=' operator
    expect_token(state, TOKEN_OPERATOR, "Expected `=` after variable name");

    // Create AST node
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) {
        parser_error("Memory allocation failed", get_current_token(state));
    }
    node->type = AST_ASSIGNMENT;

    // Create a deep copy of the variable name
    if (name && name->lexeme) {
        node->assignment.variable_name = strdup(name->lexeme);
        if (!node->assignment.variable_name) {
            parser_error("Memory allocation failed for variable name", name);
        }
    } else {
        parser_error("Invalid variable name token", name);
    }

    // Parse the value as an expression (which can handle both simple literals
    // and complex expressions with operators)
    node->assignment.value = parse_expression(state);
    node->next = NULL;

    expect_token(state, TOKEN_DELIMITER,
                 "Expected `;` after variable declaration");
    return node;
}

ASTNode *parse_variable_assignment(ParserState *state) {
    debug_print_par("Starting variable assignment parse\n");

    // Parse variable name
    Token *name = get_current_token(state);
    debug_print_par("Variable assignment name: `%s`\n", name->lexeme);

    advance_token(state);

    // Expect `=` operator
    expect_token(state, TOKEN_OPERATOR, "Expected `=` after variable name");

    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) {
        parser_error("Memory allocation failed", get_current_token(state));
    }

    node->type = AST_ASSIGNMENT;
    node->assignment.variable_name = strdup(name->lexeme);
    node->assignment.value = parse_expression(state);
    node->next = NULL;

    expect_token(state, TOKEN_DELIMITER,
                 "Expected `;` after variable declaration");

    return node;
}

ASTNode *helper_print(ParserState *state) {
    // Create print node
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) {
        parser_error("Memory allocation failed", get_current_token(state));
    }

    node->type = AST_PRINT;
    node->to_print.arguments = malloc(sizeof(ASTNode *) * MAX_ARGUMENTS);
    if (!node->to_print.arguments) {
        free(node);
        parser_error("Memory allocation failed for arguments array",
                     get_current_token(state));
    }
    node->to_print.arg_count = 0;

    expect_token(state, TOKEN_PAREN_OPEN, "Expected `(` before arguments list");

    // Handle empty argument list
    if (get_current_token(state)->type == TOKEN_PAREN_CLOSE) {
        advance_token(state); // consume the `)`
        expect_token(state, TOKEN_DELIMITER,
                     "Expected `;` after show statement");
        node->next = NULL;
        return node;
    }

    // Parse arguments
    while (get_current_token(state)->type != TOKEN_PAREN_CLOSE &&
           get_current_token(state)->type != TOKEN_EOF) {

        if (node->to_print.arg_count >= MAX_ARGUMENTS) {
            parser_error("Too many arguments in show statement",
                         get_current_token(state));
        }

        // Parse the argument
        ASTNode *arg = parse_expression(state);
        node->to_print.arguments[node->to_print.arg_count++] = arg;

        // Check for comma or closing parenthesis
        Token *current = get_current_token(state);
        debug_print_par("[DEBUG PRS] Current Token: Type=%d, Lexeme=`%s`\n",
                        current->type, current->lexeme);
        if (current->type == TOKEN_DELIMITER &&
            strcmp(current->lexeme, ",") == 0) {
            advance_token(state); // consume the comma
        } else if (current->type != TOKEN_PAREN_CLOSE) {
            parser_error("Expected `,` or `)` in arguments list",
                         get_current_token(state));
        }
    }

    expect_token(state, TOKEN_PAREN_CLOSE, "Expected `)` after arguments list");
    expect_token(state, TOKEN_DELIMITER, "Expected `;` after show statement");
    node->next = NULL;
    return node;
}

ASTNode *parse_print_statement(ParserState *state) {
    debug_print_par("Starting print statement parse\n");
    expect_token(state, TOKEN_KEYWORD, "Expected 'show' keyword");
    return helper_print(state);
}

ASTNode *parse_raise_error(ParserState *state) {
    expect_token(state, TOKEN_KEYWORD, "Expected `burn` keyword");
    ASTNode *node = helper_print(state);
    node->type = AST_ERROR;
    return node;
}

ASTNode *parse_input(ParserState *state) {
    expect_token(state, TOKEN_KEYWORD, "Expected `taste` keyword");

    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) {
        parser_error("Memory allocation failed", get_current_token(state));
    }

    node->type = AST_INPUT;

    expect_token(state, TOKEN_DELIMITER, "Expected ';' after show statement");
    node->next = NULL;
    return node;
}

ASTNode *parse_literal_or_identifier(ParserState *state) {
    Token *current = get_current_token(state);

    // Check for negative numbers
    if (current->type == TOKEN_OPERATOR && strcmp(current->lexeme, "-") == 0) {
        // Look ahead to next token
        advance_token(state);
        Token *next = get_current_token(state);

        // Check if it's a number
        if (next->type == TOKEN_INTEGER || next->type == TOKEN_FLOAT) {
            ASTNode *node = malloc(sizeof(ASTNode));
            if (!node) {
                parser_error("Memory allocation failed", current);
            }

            node->type = AST_LITERAL;

            if (next->type == TOKEN_FLOAT) {
                node->literal.type = LITERAL_FLOAT;
                node->literal.value.floating_point = -atof(next->lexeme);
            } else {
                node->literal.type = LITERAL_INTEGER;
                node->literal.value.integer = -atoi(next->lexeme);
            }

            node->next = NULL;
            advance_token(state);
            return node;
        } else {
            // If it's not followed by a number, rewind and treat as operator
            state->current_token--;
            current = get_current_token(state);
        }
    }

    // Handle literals and identifiers
    if (current->type == TOKEN_FLOAT || current->type == TOKEN_INTEGER ||
        current->type == TOKEN_STRING || current->type == TOKEN_BOOLEAN) {
        ASTNode *node = malloc(sizeof(ASTNode));
        if (!node) {
            parser_error("Memory allocation failed", current);
        }

        node->type = AST_LITERAL;

        if (current->type == TOKEN_FLOAT) {
            node->literal.type = LITERAL_FLOAT;
            node->literal.value.floating_point = atof(current->lexeme);
        } else if (current->type == TOKEN_INTEGER) {
            node->literal.type = LITERAL_INTEGER;
            node->literal.value.integer = atoi(current->lexeme);
        } else if (current->type == TOKEN_STRING) {
            node->literal.type = LITERAL_STRING;
            node->literal.value.string = strdup(current->lexeme);
        } else if (current->type == TOKEN_BOOLEAN) { // Handle boolean
            node->literal.type = LITERAL_BOOLEAN;
            if (strcmp(current->lexeme, "True") == 0) {
                node->literal.value.boolean = 1;
            } else {
                node->literal.value.boolean = 0;
            }
        }

        node->next = NULL;
        advance_token(state);
        return node;
    } else if (current->type == TOKEN_IDENTIFIER) {
        ASTNode *node = malloc(sizeof(ASTNode));
        if (!node) {
            parser_error("Memory allocation failed", current);
        }

        node->type = AST_VARIABLE;
        node->variable_name = strdup(current->lexeme);
        node->next = NULL;

        advance_token(state);
        return node;
    } else if (current->type == TOKEN_KEYWORD &&
               strcmp(current->lexeme, "taste") == 0) {
        ASTNode *node = malloc(sizeof(ASTNode));
        if (!node) {
            parser_error("Memory allocation failed", current);
        }

        node->type = AST_INPUT;
        node->next = NULL;

        advance_token(state);
        return node;
    } else if (current->type == TOKEN_FUNCTION_NAME) {
        ASTNode *node = parse_function_call(state);
        return node;
    }

    parser_error("Expected literal or identifier", current);
    return NULL;
}

ASTNode *parse_term(ParserState *state) {
    return parse_literal_or_identifier(state);
}

ASTNode *parse_expression(ParserState *state) {
    ASTNode *node = parse_term(state);

    // Check for binary operators
    while (get_current_token(state)->type == TOKEN_OPERATOR) {
        Token *operator= get_current_token(state);
        advance_token(state);

        ASTNode *rhs = parse_term(state);
        ASTNode *new_node = malloc(sizeof(ASTNode));

        new_node->type = AST_BINARY_OP;
        new_node->binary_op.operator= strdup(operator->lexeme);
        new_node->binary_op.left = node;
        new_node->binary_op.right = rhs;

        node = new_node;
    }

    return node;
}

ASTNode *parse_function_return(ParserState *state) {
    expect_token(state, TOKEN_KEYWORD, "Expected `deliver` keyword");

    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) {
        parser_error("Memory allocation failed", get_current_token(state));
    }

    node->type = AST_FUNCTION_RETURN;
    // node->function_call.return_data = parse_expression(state);
    node->assignment.value = parse_expression(state);

    node->next = NULL;
    expect_token(state, TOKEN_DELIMITER,
                 "Expected `;` after deliver statement");
    return node;
}

ASTNode *parse_block(ParserState *state) {
    debug_print_par("Starting to parse block\n");
    ASTNode *head = NULL;
    ASTNode *tail = NULL;

    while (get_current_token(state)->type != TOKEN_EOF) {
        Token *current = get_current_token(state);
        debug_print_par("Parsing token in block: type=`%d`, lexeme=`%s`\n",
                        current->type, current->lexeme);

        // Break if we hit end of block
        if (current->type == TOKEN_BRACE_CLOSE) {
            break;
        }

        // Check for block end conditions
        // Handle semicolons between statements without breaking the block
        if (current->type == TOKEN_DELIMITER &&
            strcmp(current->lexeme, ";") == 0) {
            advance_token(state);
            continue; // continue to next statement instead of breaking
        }

        // Handle `is` in check blocks
        if (current->type == TOKEN_KEYWORD) {
            if ((strcmp(current->lexeme, "elif") == 0 ||
                 strcmp(current->lexeme, "else") == 0) ||
                (!state->in_switch_block &&
                 strcmp(current->lexeme, "is") == 0)) {
                break;
            }
        }

        // Parse statement
        ASTNode *statement = NULL;
        if (current->type == TOKEN_KEYWORD) {
            if (strcmp(current->lexeme, "deliver") == 0) {
                statement = parse_function_return(state);
                if (head) {
                    tail->next = statement;
                    tail = statement;
                } else {
                    head = tail = statement;
                }
                break; // exit block after deliver
            } else if (strcmp(current->lexeme, "show") == 0) {
                statement = parse_print_statement(state);
            } else if (strcmp(current->lexeme, "burn") == 0) {
                statement = parse_raise_error(state);
            } else if (strcmp(current->lexeme, "if") == 0) {
                statement = parse_conditional_block(state);
            } else if (strcmp(current->lexeme, "while") == 0) {
                statement = parse_while_loop(state);
            } else if (strcmp(current->lexeme, "for") == 0) {
                statement = parse_for_loop(state);
            } else if (strcmp(current->lexeme, "check") == 0) {
                state->in_switch_block = true;
                statement = parse_switch_block(state);
                state->in_switch_block = false;
            } else if (strcmp(current->lexeme, "break") == 0) {
                statement = parse_break_statement(state);
            } else if (strcmp(current->lexeme, "let") == 0) {
                statement = parse_variable_declaration(state);
            } else if (strcmp(current->lexeme, "create") == 0) {
                statement = parse_function_declaration(state);
            } else if (state->in_switch_block &&
                       strcmp(current->lexeme, "is") == 0) {
                break;
            } else {
                parser_error("Unexpected keyword in block", current);
            }
        } else if (current->type == TOKEN_IDENTIFIER) {
            Token *next_token = peek_next_token(state);
            if (next_token && next_token->type == TOKEN_OPERATOR &&
                strcmp(next_token->lexeme, "=") == 0) {
                statement = parse_variable_assignment(state);
            } else {
                statement = parse_expression_statement(state);
            }
        } else if (current->type == TOKEN_PAREN_CLOSE) {
            state->in_function_body = false;
        } else if (current->type == TOKEN_EOF) {
            break;
        } else {
            parser_error("Unexpected token in block", current);
        }

        // Add statement to list
        if (!head) {
            head = statement;
            tail = statement;
        } else {
            tail->next = statement;
            tail = statement;
        }
    }

    return head;
}

ASTNode *parse_conditional_block(ParserState *state) {
    // Allocate the node
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) {
        parser_error("Memory allocation failed", get_current_token(state));
    }
    node->type = AST_CONDITIONAL;
    node->next = NULL;
    node->conditional.else_branch = NULL;

    // Expect either `if`, `elif`, or `else`
    Token *current = get_current_token(state);
    if (strcmp(current->lexeme, "if") == 0 ||
        strcmp(current->lexeme, "elif") == 0) {
        // Consume `if`/`elif`
        advance_token(state);

        // Parse condition expression
        node->conditional.condition = parse_expression(state);

        expect_token(state, TOKEN_BRACE_OPEN, "Expected `{` delimiter");
        node->conditional.body = parse_block(state);
        expect_token(state, TOKEN_BRACE_CLOSE, "Expected `}` delimiter");
    } else if (strcmp(current->lexeme, "else") == 0) {
        // Consume `else`
        advance_token(state);

        // No condition => node->conditional.condition = NULL
        node->conditional.condition = NULL;

        expect_token(state, TOKEN_BRACE_OPEN, "Expected `{` delimiter");
        node->conditional.body = parse_block(state);
        expect_token(state, TOKEN_BRACE_CLOSE, "Expected `}` delimiter");
    } else {
        parser_error("Expected `if`, `elif`, or `else`", current);
    }

    // Check if next token is `elif` or `else` to chain
    Token *next = get_current_token(state);
    if (next->type == TOKEN_KEYWORD && (strcmp(next->lexeme, "elif") == 0 ||
                                        strcmp(next->lexeme, "else") == 0)) {
        node->conditional.else_branch = parse_conditional_block(state);
    }

    return node;
}

ASTNode *parse_while_loop(ParserState *state) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) {
        parser_error("Memory allocation failed", get_current_token(state));
    }
    node->type = AST_WHILE_LOOP;

    expect_token(state, TOKEN_KEYWORD, "Expected `while` keyword");
    node->while_loop.condition = parse_expression(state);

    expect_token(state, TOKEN_BRACE_OPEN, "Expected `{` delimiter");
    node->while_loop.body = parse_block(state);
    expect_token(state, TOKEN_BRACE_CLOSE, "Expected `}` delimiter");

    // Optional re-evaluation logic
    node->while_loop.re_evaluate_condition = 1;
    node->next = NULL;
    return node;
}

ASTNode *parse_for_loop(ParserState *state) {
    debug_print_par("Parsing a `for` loop...\n");

    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) {
        parser_error("Memory allocation failed", get_current_token(state));
    }
    node->type = AST_FOR_LOOP;

    // Expect `for` keyword
    expect_token(state, TOKEN_KEYWORD, "Expected `for` keyword");
    debug_print_par("Found `for` keyword\n");

    // Parse loop variable
    Token *var_token = get_current_token(state);
    if (var_token->type != TOKEN_IDENTIFIER) {
        parser_error("Expected loop variable identifier", var_token);
    }
    char *loop_var = strdup(var_token->lexeme);
    if (!loop_var) {
        parser_error("Memory allocation failed for loop variable", var_token);
    }
    debug_print_par("Loop variable: %s\n", loop_var);
    advance_token(state); // Consume loop variable

    // Expect `in` keyword
    expect_token(state, TOKEN_KEYWORD, "Expected `in` keyword");
    debug_print_par("Found `in` keyword\n");

    // Parse start expression as a literal
    ASTNode *start_expr = parse_literal_or_identifier(state);
    if (!start_expr) {
        parser_error("Expected start expression in for loop",
                     get_current_token(state));
    }
    debug_print_par("Parsed start expression\n");

    // Parse `..` or `..=` operator
    Token *range_op = get_current_token(state);
    bool inclusive = false;
    if (range_op->type == TOKEN_OPERATOR &&
        strcmp(range_op->lexeme, "..") == 0) {
        inclusive = false;
    } else if (range_op->type == TOKEN_OPERATOR &&
               strcmp(range_op->lexeme, "..=") == 0) {
        inclusive = true;
    } else {
        parser_error("Expected `..` or `..=` operator in for loop", range_op);
    }
    debug_print_par("Found range operator: %s\n", range_op->lexeme);
    advance_token(state); // Consume range operator

    // Parse end expression as a literal
    ASTNode *end_expr = parse_literal_or_identifier(state);
    if (!end_expr) {
        parser_error("Expected end expression in for loop",
                     get_current_token(state));
    }
    debug_print_par("Parsed end expression\n");

    // Initialize step expression to NULL
    ASTNode *step_expr = NULL;

    // Check for optional `by` keyword
    Token *current = get_current_token(state);
    if (current->type == TOKEN_KEYWORD && strcmp(current->lexeme, "by") == 0) {
        debug_print_par("Found `by` keyword\n");
        advance_token(state); // Consume `by` keyword

        // Parse step expression as a full expression
        step_expr = parse_expression(state);
        if (!step_expr) {
            parser_error("Expected step expression after `by`",
                         get_current_token(state));
        }
        debug_print_par("Parsed step expression\n");
    }

    expect_token(state, TOKEN_BRACE_OPEN,
                 "Expected `{` delimiter to start loop body");
    debug_print_par("Found `{` to start loop body\n");

    // Parse loop body
    ASTNode *body = parse_block(state);
    if (!body) {
        parser_error("Expected loop body", get_current_token(state));
    }
    debug_print_par("Parsed loop body\n");

    expect_token(state, TOKEN_BRACE_CLOSE,
                 "Expected `}` delimiter to end loop body");
    debug_print_par("Found `}` to end loop body\n");

    // Assign parsed components to ASTForLoop
    node->for_loop.loop_variable = loop_var;
    node->for_loop.start_expr = start_expr;
    node->for_loop.end_expr = end_expr;
    node->for_loop.inclusive = inclusive;
    node->for_loop.step_expr = step_expr;
    node->for_loop.body = body;
    node->next = NULL;

    debug_print_par("Successfully parsed a `for` loop\n");
    return node;
}

ASTNode *parse_break_statement(ParserState *state) {
    expect_token(state, TOKEN_KEYWORD, "Expected `break` keyword");
    expect_token(state, TOKEN_DELIMITER, "Expected `;` after break");

    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) {
        parser_error("Memory allocation failed", get_current_token(state));
    }

    node->type = AST_BREAK;
    node->next = NULL;
    return node;
}

ASTNode *parse_switch_block(ParserState *state) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) {
        parser_error("Memory allocation failed", get_current_token(state));
    }

    node->type = AST_SWITCH;

    // Handle `check` keyword
    expect_token(state, TOKEN_KEYWORD, "Expected `check` keyword");

    // Parse switch expression
    node->switch_case.expression = parse_expression(state);

    // Expect `{` instead of `:`
    expect_token(state, TOKEN_BRACE_OPEN,
                 "Expected '{' after check expression");

    // Parse cases
    node->switch_case.cases = NULL;
    ASTCaseNode *last_case = NULL;

    Token *current;
    while ((current = get_current_token(state))) {
        // Skip any delimiters between cases
        if (current->type == TOKEN_DELIMITER) {
            advance_token(state);
            continue;
        }

        if (current->type != TOKEN_KEYWORD) {
            break; // exit if not a keyword (`is`/`else`)
        }

        if (strcmp(current->lexeme, "is") == 0) {
            advance_token(state); // consume `is`

            ASTCaseNode *case_node = malloc(sizeof(ASTCaseNode));
            if (!case_node) {
                parser_error("Memory allocation failed", current);
            }

            // Parse case value
            case_node->condition = parse_expression(state);
            expect_token(state, TOKEN_DELIMITER,
                         "Expected `:` after case value");

            // Parse case body
            case_node->body = parse_block(state);
            case_node->next = NULL;

            // Add to cases list
            if (!node->switch_case.cases) {
                node->switch_case.cases = case_node;
            } else {
                last_case->next = case_node;
            }
            last_case = case_node;
        } else if (strcmp(current->lexeme, "else") == 0) {
            advance_token(state); // consume `else`

            ASTCaseNode *default_case = malloc(sizeof(ASTCaseNode));
            if (!default_case) {
                parser_error("Memory allocation failed", current);
            }

            default_case->condition = NULL; // no condition for `else`

            expect_token(state, TOKEN_DELIMITER, "Expected `:` after else");

            // Parse `else` body
            default_case->body = parse_block(state);
            default_case->next = NULL;

            // Add as last case
            if (!node->switch_case.cases) {
                node->switch_case.cases = default_case;
            } else {
                last_case->next = default_case;
            }
            break; // `else` is always the last case
        } else {
            break; // exit on any other keyword
        }
    }

    expect_token(state, TOKEN_BRACE_CLOSE, "Expected '}' to close check block");

    node->next = NULL;
    return node;
}

ASTFunctionParameter *parse_parameter_list(ParserState *state) {
    ASTFunctionParameter *head = NULL;
    ASTFunctionParameter *tail = NULL;

    while (1) {
        // Parse parameter name
        Token *name = get_current_token(state);
        expect_token(state, TOKEN_IDENTIFIER, "Expected parameter name");

        // Create parameter node
        ASTFunctionParameter *param_node = malloc(sizeof(ASTFunctionParameter));
        if (!param_node) {
            parser_error("Memory allocation failed", get_current_token(state));
        }
        param_node->parameter_name = strdup(name->lexeme);
        param_node->next = NULL;

        // Add parameter to linked list
        if (!head) {
            head = param_node;
            tail = param_node;
        } else {
            tail->next = param_node;
            tail = param_node;
        }

        // Check for comma (indicates another parameter)
        if (get_current_token(state)->type == TOKEN_DELIMITER &&
            strcmp(get_current_token(state)->lexeme, ",") == 0) {
            advance_token(state); // consume `,`
        } else {
            break;
        }
    }

    return head;
}

ASTNode *parse_function_body(ParserState *state) {
    ASTNode *body = parse_block(state);

    return body;
}

ASTNode *parse_function_declaration(ParserState *state) {
    // Expect and consume the `create` keyword
    expect_token(state, TOKEN_KEYWORD, "Expected `create` keyword");

    // Parse function name
    Token *name = get_current_token(state);
    if (name->type != TOKEN_FUNCTION_NAME) {
        parser_error("Expected function name after `create`", name);
    }

    // Create the function declaration node
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) {
        parser_error("Memory allocation failed for function declaration node",
                     name);
    }

    node->type = AST_FUNCTION_DECLARATION;
    node->function_call.name = strdup(name->lexeme);
    node->function_call.parameters = NULL;
    node->function_call.body = NULL;
    node->function_call.return_data = NULL;
    node->next = NULL;

    advance_token(state); // Move past the function name

    // Parse parameters
    if (get_current_token(state)->type == TOKEN_PAREN_OPEN) {
        advance_token(state); // Consume `(`

        // Check if the parameter list is empty
        if (get_current_token(state)->type == TOKEN_PAREN_CLOSE) {
            node->function_call.parameters = NULL; // No parameters
        } else {
            node->function_call.parameters = parse_parameter_list(state);
        }

        expect_token(state, TOKEN_PAREN_CLOSE,
                     "Expected `)` after parameter list");
    } else {
        parser_error("Expected `(` for parameter list",
                     get_current_token(state));
    }

    // Parse function body
    if (get_current_token(state)->type == TOKEN_BRACE_OPEN) {
        advance_token(state); // Consume `(`
        state->in_function_body = true;
        node->function_call.body =
            parse_function_body(state); // Parse function body
        state->in_function_body = false;

        expect_token(state, TOKEN_BRACE_CLOSE,
                     "Expected `}` to close function body");
    } else {
        parser_error("Expected `{` to start function body",
                     get_current_token(state));
    }

    return node;
}

ASTNode *parse_argument_list(ParserState *state) {
    // Check for empty argument list first
    if (get_current_token(state)->type == TOKEN_PAREN_CLOSE) {
        return NULL; // return NULL for empty argument list
    }

    ASTNode *head = NULL;
    ASTNode *tail = NULL;

    while (1) {
        // Parse arguments as expressions
        ASTNode *arg = parse_expression(state);

        // Add argument to linked list
        if (!head) {
            head = arg;
            tail = arg;
        } else {
            tail->next = arg;
            tail = arg;
        }

        // Check for comma (indicates another argument)
        if (get_current_token(state)->type == TOKEN_DELIMITER &&
            strcmp(get_current_token(state)->lexeme, ",") == 0) {
            advance_token(state); // consume `,`
        } else {
            break;
        }
    }

    return head;
}

ASTNode *parse_function_call(ParserState *state) {
    // Parse function name
    Token *name = get_current_token(state);
    expect_token(state, TOKEN_FUNCTION_NAME, "Expected function name");

    // Create function call node
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) {
        parser_error("Memory allocation failed", get_current_token(state));
    }
    node->type = AST_FUNCTION_CALL;
    debug_print_par("AST_FUNCTION_CALL\n");
    node->function_call.name = strdup(name->lexeme);

    // Initialize both parameters and arguments to NULL
    node->function_call.parameters = NULL;
    node->function_call.arguments = NULL;

    // Parse arguments (if any)
    expect_token(state, TOKEN_PAREN_OPEN, "Expected '(' after function name");

    // Parse argument list (which can now be empty)
    node->function_call.arguments = parse_argument_list(state);

    expect_token(state, TOKEN_PAREN_CLOSE, "Expected ')' after argument list");

    node->next = NULL;
    return node;
}

void free_ast(ASTNode *node) {
    while (node) {
        ASTNode *next = node->next;

        switch (node->type) {
        case AST_PRINT:
        case AST_ERROR:
            for (size_t i = 0; i < node->to_print.arg_count; i++) {
                free_ast(node->to_print.arguments[i]);
            }
            free(node->to_print.arguments);
            break;

        case AST_INPUT:
            // No cleanup needed!
            break;

        case AST_ASSIGNMENT:
            free(node->assignment.variable_name);
            free_ast(node->assignment.value);
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

        case AST_VARIABLE:
            free(node->variable_name);
            break;

        case AST_FUNCTION_DECLARATION:
        case AST_FUNCTION_CALL:
            free(node->function_call.parameters);
            free(node->function_call.arguments);
            free(node->function_call.body);
            free(node->function_call.return_data);
            break;

        case AST_BREAK:
            // No cleanup needed!
            break;

        case AST_SWITCH:
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

        default:
            fprintf(stderr, "Error: Unknown `ASTNode` type in `free_ast`.\n");
            exit(1);
        }

        free(node);
        node = next;
    }
}
