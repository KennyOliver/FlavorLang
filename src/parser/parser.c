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

    debug_print_par("Current Token: Type=`%d`, Lexeme=`%s`\n", token->type,
                    token->lexeme);
    if (match_token(state, "let"))
        return parse_variable_declaration(state);
    if (match_token(state, "const"))
        return parse_constant_declaration(state);
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
    if (match_token(state, "break"))
        return parse_break_statement(state);
    if (match_token(state, "deliver"))
        return parse_function_return(state);
    if (match_token(state, "try"))
        return parse_try_block(state);

    // Handle function calls
    if (token->type == TOKEN_FUNCTION_NAME) {
        ASTNode *node = parse_function_call(state);
        expect_token(state, TOKEN_DELIMITER,
                     "Expected `;` after function call");
        return node;
    }

    // Handle variable assignments
    if (token->type == TOKEN_IDENTIFIER) {
        Token *next_token = peek_next_token(state);
        if (next_token) {
            debug_print_par("Peek Next Token: Type=`%d`, Lexeme=`%s`\n",
                            next_token->type, next_token->lexeme);
            if (next_token->type == TOKEN_OPERATOR &&
                strcmp(next_token->lexeme, "=") == 0) {
                return parse_variable_assignment(state);
            }
        }
        return parse_expression_statement(state);
    }

    // If none of the above, raise an error
    parser_error("Unexpected token at start of statement", token);
    return NULL;
}

ASTNode *parse_expression_statement(ParserState *state) {
    // Parse the expression (which can handle binary ops, variables, etc.)
    ASTNode *expr_node = parse_expression(state);
    expect_token(state, TOKEN_DELIMITER,
                 "Expected `;` after expression statement");
    return expr_node;
}

ASTNode *parse_declaration(ParserState *state, ASTNodeType type) {
    if (type == AST_CONST_DECLARATION) {
        debug_print_par("Starting constant declaration parse\n");
        expect_token(state, TOKEN_KEYWORD, "Expected `const` keyword");
    } else {
        debug_print_par("Starting variable declaration parse\n");
        expect_token(state, TOKEN_KEYWORD, "Expected `let` keyword");
    }

    // Parse variable/constant name
    Token *name = get_current_token(state);
    if (type == AST_CONST_DECLARATION) {
        expect_token(state, TOKEN_IDENTIFIER, "Expected constant name");
        expect_token(state, TOKEN_OPERATOR, "Expected `=` after constant name");
    } else {
        expect_token(state, TOKEN_IDENTIFIER, "Expected variable name");
        expect_token(state, TOKEN_OPERATOR, "Expected `=` after variable name");
    }

    // Create AST node
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) {
        parser_error("Memory allocation failed", get_current_token(state));
    }
    node->type = type;

    // Create a deep copy of the variable/constant name
    if (name && name->lexeme) {
        node->assignment.variable_name = strdup(name->lexeme);
        if (!node->assignment.variable_name) {
            parser_error(AST_CONST_DECLARATION
                             ? "Memory allocation failed for constant name"
                             : "Memory allocation failed for variable name",
                         name);
        }
    } else {
        parser_error(AST_CONST_DECLARATION ? "Invalid constant name token"
                                           : "Invalid variable name token",
                     name);
    }

    node->assignment.value = parse_expression(state);
    node->next = NULL;

    expect_token(state, TOKEN_DELIMITER,
                 AST_CONST_DECLARATION
                     ? "Expected `;` after constant declaration"
                     : "Expected `;` after variable declaration");

    return node;
}

ASTNode *parse_variable_declaration(ParserState *state) {
    return parse_declaration(state, AST_ASSIGNMENT);
}

ASTNode *parse_constant_declaration(ParserState *state) {
    return parse_declaration(state, AST_CONST_DECLARATION);
}

ASTNode *parse_variable_assignment(ParserState *state) {
    debug_print_par("Starting variable assignment parse\n");

    // Parse variable name
    Token *name = get_current_token(state);
    debug_print_par("Variable assignment name: `%s`\n", name->lexeme);
    advance_token(state); // consume variable name

    // Expect `=` operator
    Token *op_token = get_current_token(state);
    debug_print_par("Expected operator `=`, found: `%s`\n", op_token->lexeme);
    expect_token(state, TOKEN_OPERATOR, "Expected `=` after variable name");

    // Parse the expression on the right-hand side
    ASTNode *value_node = parse_expression(state);
    debug_print_par("Parsed expression for assignment\n");

    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) {
        parser_error("Memory allocation failed", get_current_token(state));
    }

    node->type = AST_ASSIGNMENT;
    node->assignment.variable_name = strdup(name->lexeme);
    node->assignment.value = value_node;
    node->next = NULL;

    // Expect `;` delimiter
    Token *delimiter = get_current_token(state);
    if (delimiter->type != TOKEN_DELIMITER ||
        strcmp(delimiter->lexeme, ";") != 0) {
        debug_print_par("Expected `;` after assignment, found: `%s`\n",
                        delimiter->lexeme);
    }
    expect_token(state, TOKEN_DELIMITER,
                 "Expected `;` after variable assignment");
    debug_print_par("Consumed `;` after variable assignment\n");

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
    } else if (current->type == TOKEN_FUNCTION_NAME ||
               current->type == TOKEN_IDENTIFIER) {
        // Check if `(` indicates a call
        Token *next = peek_next_token(state);
        if (next && next->type == TOKEN_PAREN_OPEN) {
            // It's a call
            ASTNode *node = parse_function_call(state);
            return node;
        } else {
            // It's just a variable (or a function reference used as a variable)
            ASTNode *node = create_variable_node(current->lexeme);
            advance_token(state);
            return node;
        }
    } else if (current->type == TOKEN_PAREN_OPEN) {
        advance_token(state); // consume `(`
        ASTNode *node = parse_operator_expression(state);
        expect_token(state, TOKEN_PAREN_CLOSE, "Expected `)` after expression");
        return node;
    }

    parser_error("Expected literal or identifier", current);
    return NULL; // unreachable
}

ASTNode *parse_expression(ParserState *state) {
    // Delegate to operator parser
    return parse_operator_expression(state);
}

ASTNode *parse_function_return(ParserState *state) {
    expect_token(state, TOKEN_KEYWORD, "Expected `deliver` keyword");

    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) {
        parser_error("Memory allocation failed", get_current_token(state));
    }

    node->type = AST_FUNCTION_RETURN;
    node->function_return.return_data = parse_expression(state);

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

        // Handle semicolons between statements without breaking the block
        if (current->type == TOKEN_DELIMITER &&
            strcmp(current->lexeme, ";") == 0) {
            advance_token(state);
            continue;
        }

        // Handle `break` and `deliver` keywords
        if (current->type == TOKEN_KEYWORD) {
            if (strcmp(current->lexeme, "break") == 0) {
                ASTNode *break_node = parse_break_statement(state);
                if (head) {
                    tail->next = break_node;
                    tail = break_node;
                } else {
                    head = tail = break_node;
                }
                break; // Exit block after `break`
            }

            if (strcmp(current->lexeme, "deliver") == 0) {
                ASTNode *return_node = parse_function_return(state);
                if (head) {
                    tail->next = return_node;
                    tail = return_node;
                } else {
                    head = tail = return_node;
                }
                break; // Exit block after `deliver`
            }

            // Note: Do NOT handle `is` here anymore
        }

        // Use `parse_statement` for standard statements
        ASTNode *statement = parse_statement(state);
        if (!statement) {
            parser_error("Failed to parse statement in block", current);
            break;
        }

        // Add statement to the linked list
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

ASTNode *parse_case_body(ParserState *state) {
    ASTNode *head = NULL;
    ASTNode *tail = NULL;

    while (1) {
        Token *current = get_current_token(state);

        // Stop parsing the body if any of `is`, `else`, or `}` get encountered
        if ((current->type == TOKEN_KEYWORD &&
             (strcmp(current->lexeme, "is") == 0 ||
              strcmp(current->lexeme, "else") == 0)) ||
            current->type == TOKEN_BRACE_CLOSE || current->type == TOKEN_EOF) {
            break;
        }

        // Parse a statement
        ASTNode *stmt = parse_statement(state);
        if (!stmt) {
            parser_error("Failed to parse statement in case body", current);
        }

        // Append the statement to the body linked list
        if (!head) {
            head = tail = stmt;
        } else {
            tail->next = stmt;
            tail = stmt;
        }
    }

    return head;
}

ASTNode *parse_switch_block(ParserState *state) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) {
        parser_error("Memory allocation failed", get_current_token(state));
    }

    node->type = AST_SWITCH;
    node->switch_case.expression = NULL;
    node->switch_case.cases = NULL;
    node->next = NULL;

    // Handle `check` keyword
    expect_token(state, TOKEN_KEYWORD, "Expected `check` keyword");

    // Parse switch expression
    node->switch_case.expression = parse_expression(state);

    // Expect `{`
    expect_token(state, TOKEN_BRACE_OPEN,
                 "Expected `{` after check expression");

    // Initialize cases list
    ASTCaseNode *last_case = NULL;

    while (1) {
        Token *current = get_current_token(state);

        // Skip any delimiters (like newlines or semicolons) between cases
        if (current->type == TOKEN_DELIMITER) {
            advance_token(state);
            continue;
        }

        // Handle `is` clauses
        if (current->type == TOKEN_KEYWORD &&
            strcmp(current->lexeme, "is") == 0) {
            advance_token(state); // consume `is`

            // Parse condition expression
            ASTNode *condition = parse_expression(state);
            expect_token(state, TOKEN_COLON, "Expected `:` after case value");

            // Create a new case node
            ASTCaseNode *case_node = malloc(sizeof(ASTCaseNode));
            if (!case_node) {
                parser_error("Memory allocation failed for ASTCaseNode",
                             current);
            }
            case_node->condition = condition;
            case_node->body = parse_case_body(state); // Parse unique body
            case_node->next = NULL;

            // Append to the cases list
            if (!node->switch_case.cases) {
                node->switch_case.cases = case_node;
            } else {
                last_case->next = case_node;
            }
            last_case = case_node;
        }
        // Handle `else` clause
        else if (current->type == TOKEN_KEYWORD &&
                 strcmp(current->lexeme, "else") == 0) {
            advance_token(state); // consume `else`

            expect_token(state, TOKEN_COLON, "Expected `:` after `else`");

            // Create the default case node
            ASTCaseNode *default_case = malloc(sizeof(ASTCaseNode));
            if (!default_case) {
                parser_error("Memory allocation failed for default ASTCaseNode",
                             current);
            }
            default_case->condition = NULL; // No condition for `else`
            default_case->body = parse_case_body(state); // Parse unique body
            default_case->next = NULL;

            // Append to the cases list
            if (!node->switch_case.cases) {
                node->switch_case.cases = default_case;
            } else {
                last_case->next = default_case;
            }
            last_case = default_case;

            break; // `else` is always the last case
        } else {
            // No more cases
            break;
        }
    }

    // Expect the closing `}` of the entire `check` block
    expect_token(state, TOKEN_BRACE_CLOSE, "Expected `}` to close check block");

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
    node->function_declaration.name = strdup(name->lexeme);
    node->function_declaration.parameters = NULL;
    node->function_declaration.body = NULL;
    node->next = NULL;

    advance_token(state); // Move past the function name

    // Parse parameters
    if (get_current_token(state)->type == TOKEN_PAREN_OPEN) {
        advance_token(state); // Consume `(`

        // Check if the parameter list is empty
        if (get_current_token(state)->type == TOKEN_PAREN_CLOSE) {
            node->function_declaration.parameters = NULL; // No parameters
        } else {
            node->function_declaration.parameters = parse_parameter_list(state);
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
        node->function_declaration.body =
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

ASTNode *parse_function_call(ParserState *state) {
    Token *name = get_current_token(state);

    if (name->type != TOKEN_FUNCTION_NAME && name->type != TOKEN_IDENTIFIER) {
        parser_error("Expected a function reference or identifier", name);
    }

    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) {
        parser_error("Memory allocation failed", get_current_token(state));
    }
    node->type = AST_FUNCTION_CALL;
    debug_print_par("AST_FUNCTION_CALL: %s\n", name->lexeme);

    node->function_call.name = strdup(name->lexeme);
    node->function_call.arguments = NULL;

    advance_token(state);

    // Parse arguments (if any)
    expect_token(state, TOKEN_PAREN_OPEN, "Expected `(` after function name");
    node->function_call.arguments = parse_argument_list(state);
    expect_token(state, TOKEN_PAREN_CLOSE, "Expected `)` after argument list");

    node->next = NULL;
    return node;
}

ASTNode *parse_try_block(ParserState *state) {
    expect_token(state, TOKEN_KEYWORD, "Expected `try` keyword");
    expect_token(state, TOKEN_BRACE_OPEN, "Expected `{` to start try block");

    // Parse `try` block statements
    ASTNode *try_body = parse_block(state);
    expect_token(state, TOKEN_BRACE_CLOSE, "Expected `}` to end try block");

    // Initialize try structure
    ASTTry try_block = {0};
    try_block.try_block = try_body;
    try_block.catch_blocks = NULL;  // initialize `rescue` (catch) blocks
    try_block.finally_block = NULL; // initialize finish block

    // Parse optional `catch` blocks
    while (match_token(state, "rescue")) {
        ASTCatchNode *catch = parse_catch_block(state);
        if (!try_block.catch_blocks) {
            try_block.catch_blocks = catch;
        } else {
            // For future: Handle multiple `rescue` (catch) blocks
            ASTCatchNode *last = try_block.catch_blocks;
            while (last->next) {
                last = last->next;
            }
            last->next = catch;
        }
    }

    // Parse optional `finish` block
    if (match_token(state, "finish")) {
        ASTNode *finally_body = parse_finally_block(state);
        try_block.finally_block = finally_body;
    }

    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) {
        parser_error("Memory allocation failed for `try` block",
                     get_current_token(state));
    }
    node->type = AST_TRY;
    node->try_block = try_block;
    node->next = NULL;

    return node;
}

ASTCatchNode *parse_catch_block(ParserState *state) {
    expect_token(state, TOKEN_KEYWORD, "Expected `rescue` keyword");

    // Optional: Parse error object variable
    char *error_var = NULL;
    Token *current = get_current_token(state);
    if (current->type == TOKEN_PAREN_OPEN) {
        advance_token(state); // consume `(`
        Token *var_token = get_current_token(state);
        if (var_token->type == TOKEN_IDENTIFIER) {
            error_var = strdup(var_token->lexeme);
            if (!error_var) {
                parser_error("Memory allocation failed for error variable name",
                             var_token);
            }
            advance_token(state); // consume variable name
        }
        expect_token(state, TOKEN_PAREN_CLOSE,
                     "Expected `)` after error variable");
    }

    expect_token(state, TOKEN_BRACE_OPEN, "Expected `{` to start rescue block");

    // Parse `rescue` (catch) block statements
    ASTNode *catch_body = parse_block(state);
    expect_token(state, TOKEN_BRACE_CLOSE, "Expected `}` to end rescue block");

    ASTCatchNode *catch_node = malloc(sizeof(ASTCatchNode));
    if (!catch_node) {
        parser_error("Memory allocation failed for rescue block", current);
    }
    catch_node->error_variable = error_var;
    catch_node->body = catch_body;
    catch_node->next = NULL;

    return catch_node;
}

ASTNode *parse_finally_block(ParserState *state) {
    expect_token(state, TOKEN_KEYWORD, "Expected `finish` keyword");
    expect_token(state, TOKEN_BRACE_OPEN, "Expected `{` to start finish block");
    ASTNode *finally_body = parse_block(state);
    expect_token(state, TOKEN_BRACE_CLOSE, "Expected `}` to end finish block");
    return finally_body;
}
