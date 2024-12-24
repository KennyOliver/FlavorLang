#include "parser.h"
#include <string.h>
#include "../debug/debug.h"

ASTNode *parse_program(Token *tokens)
{
    ParserState *state = create_parser_state(tokens);
    ASTNode *head = NULL;
    ASTNode *current = NULL;

    while (get_current_token(state)->type != TOKEN_EOF)
    {
        ASTNode *new_node = NULL;
        Token *token = get_current_token(state);

        // Match statements based on token type
        if (strcmp(token->lexeme, "let") == 0)
        {
            new_node = parse_variable_declaration(state);
        }
        else if (token->type == TOKEN_IDENTIFIER)
        {
            new_node = parse_variable_assignment(state);
        }
        else if (strcmp(token->lexeme, "show") == 0)
        {
            new_node = parse_print_statement(state);
        }
        else if (strcmp(token->lexeme, "burn") == 0)
        {
            new_node = parse_raise_error(state);
        }
        else if (strcmp(token->lexeme, "taste") == 0)
        {
            new_node = parse_input(state);
        }
        else if (strcmp(token->lexeme, "if") == 0)
        {
            new_node = parse_conditional_block(state);
        }
        else if (strcmp(token->lexeme, "while") == 0)
        {
            new_node = parse_while_block(state);
        }
        else if (strcmp(token->lexeme, "check") == 0)
        {
            new_node = parse_switch_block(state);
        }
        else if (strcmp(token->lexeme, "is") == 0)
        {
            new_node = parse_block(state);
        }
        else if (strcmp(token->lexeme, "create") == 0)
        {
            new_node = parse_function(state);
        }
        else
        {
            parser_error("Unexpected token at start of statement", token);
        }

        // Add node to AST
        if (!head)
        {
            head = new_node;
            current = new_node;
        }
        else
        {
            current->next = new_node;
            current = new_node;
        }
    }

    free_parser_state(state);
    return head;
}

ASTNode *parse_variable_declaration(ParserState *state)
{
    expect_token(state, TOKEN_KEYWORD, "Expected `let` keyword");

    // Parse variable name
    Token *name = get_current_token(state);
    expect_token(state, TOKEN_IDENTIFIER, "Expected variable name");

    // Expect '=' operator
    expect_token(state, TOKEN_OPERATOR, "Expected `=` after variable name");

    // Create AST node
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node)
    {
        parser_error("Memory allocation failed", get_current_token(state));
    }

    node->type = AST_ASSIGNMENT;
    node->assignment.variable_name = strdup(name->lexeme);

    // Parse the value as an expression (which can handle both simple literals
    // and complex expressions with operators)
    node->assignment.value = parse_expression(state);

    node->next = NULL;

    expect_token(state, TOKEN_DELIMITER, "Expected ';' after variable declaration");

    return node;
}

ASTNode *parse_variable_assignment(ParserState *state)
{
    debug_print_par("Starting variable assignment parse\n");

    // Parse variable name
    Token *name = get_current_token(state);
    debug_print_par("Variable assignment name: `%s`\n", name->lexeme);

    advance_token(state);

    // Expect `=` operator
    expect_token(state, TOKEN_OPERATOR, "Expected `=` after variable name");

    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node)
    {
        parser_error("Memory allocation failed", get_current_token(state));
    }

    node->type = AST_ASSIGNMENT;
    // node->assignment.variable_name = strdup(name->lexeme);
    node->assignment.variable_name = name->lexeme;
    node->assignment.value = parse_expression(state);
    node->next = NULL;

    expect_token(state, TOKEN_DELIMITER, "Expected `;` after variable declaration");

    return node;
}

ASTNode *helper_print(ParserState *state)
{
    // Create print node
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node)
    {
        parser_error("Memory allocation failed", get_current_token(state));
    }

    node->type = AST_PRINT;
    node->to_print.arguments = malloc(sizeof(ASTNode *) * MAX_ARGUMENTS);
    if (!node->to_print.arguments)
    {
        free(node);
        parser_error("Memory allocation failed for arguments array", get_current_token(state));
    }
    node->to_print.arg_count = 0;

    // Parse arguments until we hit a semicolon
    while (get_current_token(state)->type != TOKEN_DELIMITER &&
           get_current_token(state)->type != TOKEN_EOF)
    {

        if (node->to_print.arg_count >= MAX_ARGUMENTS)
        {
            parser_error("Too many arguments in show statement", get_current_token(state));
        }

        // Parse the argument
        ASTNode *arg = parse_expression(state);
        node->to_print.arguments[node->to_print.arg_count++] = arg;

        // Check for comma separator
        Token *current = get_current_token(state);
        if (current->type == TOKEN_DELIMITER && strcmp(current->lexeme, ",") == 0)
        {
            advance_token(state); // consume the comma
        }
    }

    expect_token(state, TOKEN_DELIMITER, "Expected ';' after show statement");
    node->next = NULL;
    return node;
}

ASTNode *parse_print_statement(ParserState *state)
{
    expect_token(state, TOKEN_KEYWORD, "Expected 'show' keyword");

    return helper_print(state);
}

ASTNode *parse_raise_error(ParserState *state)
{
    expect_token(state, TOKEN_KEYWORD, "Expected `burn` keyword");

    ASTNode *node = helper_print(state);

    node->type = AST_ERROR;

    return node;
}

ASTNode *parse_input(ParserState *state)
{
    expect_token(state, TOKEN_KEYWORD, "Expected `taste` keyword");

    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node)
    {
        parser_error("Memory allocation failed", get_current_token(state));
    }

    node->type = AST_INPUT;

    expect_token(state, TOKEN_DELIMITER, "Expected ';' after show statement");
    node->next = NULL;
    return node;
}

ASTNode *parse_identifier(ParserState *state)
{
    Token *current = get_current_token(state);
    if (current->type != TOKEN_IDENTIFIER)
    {
        parser_error("Expected identifier", current);
    }

    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node)
    {
        parser_error("Memory allocation failed", current);
    }

    node->type = AST_ASSIGNMENT;
    node->variable_name = strdup(current->lexeme);
    node->next = NULL;

    advance_token(state);
    return node;
}

ASTNode *parse_literal_or_identifier(ParserState *state)
{
    Token *current = get_current_token(state);

    if (current->type == TOKEN_NUMBER || current->type == TOKEN_STRING)
    {
        ASTNode *node = malloc(sizeof(ASTNode));
        if (!node)
        {
            parser_error("Memory allocation failed", current);
        }

        node->type = AST_LITERAL;
        node->literal.type = (current->type == TOKEN_NUMBER) ? LITERAL_NUMBER : LITERAL_STRING;

        if (node->literal.type == LITERAL_NUMBER)
        {
            node->literal.value.number = atof(current->lexeme);
        }
        else
        {
            node->literal.value.string = strdup(current->lexeme);
        }

        node->next = NULL;

        advance_token(state);
        return node;
    }
    else if (current->type == TOKEN_IDENTIFIER)
    {
        ASTNode *node = malloc(sizeof(ASTNode));
        if (!node)
        {
            parser_error("Memory allocation failed", current);
        }

        node->type = AST_VARIABLE;
        node->variable_name = strdup(current->lexeme);
        node->next = NULL;

        advance_token(state);
        return node;
    }
    else if (current->type == TOKEN_KEYWORD && strcmp(current->lexeme, "taste") == 0)
    {
        ASTNode *node = malloc(sizeof(ASTNode));
        if (!node)
        {
            parser_error("Memory allocation failed", current);
        }

        node->type = AST_INPUT;
        node->next = NULL;

        advance_token(state);
        return node;
    }

    parser_error("Expected literal or identifier", current);
    return NULL; // unreachable due to parser_error, but keeps compiler happy
}

ASTNode *parse_term(ParserState *state)
{
    return parse_literal_or_identifier(state);
}

ASTNode *parse_expression(ParserState *state)
{
    ASTNode *node = parse_term(state);

    // Check for binary operators
    while (get_current_token(state)->type == TOKEN_OPERATOR)
    {
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

ASTNode *parse_block(ParserState *state)
{
    debug_print_par("Starting to parse block\n");
    ASTNode *head = NULL;
    ASTNode *tail = NULL;

    while (get_current_token(state)->type != TOKEN_EOF)
    {
        Token *current = get_current_token(state);
        debug_print_par("Parsing token in block: type=`%d`, lexeme=`%s`\n",
                        current->type,
                        current->lexeme);

        // Check for block end conditions
        // Handle semicolons between statements without breaking the block
        if (current->type == TOKEN_DELIMITER && strcmp(current->lexeme, ";") == 0)
        {
            advance_token(state);
            continue; // continue to next statement instead of breaking
        }

        // Handle `is` in check blocks
        if (current->type == TOKEN_KEYWORD)
        {
            if ((strcmp(current->lexeme, "elif") == 0 ||
                 strcmp(current->lexeme, "else") == 0) ||
                (!state->in_switch_block && strcmp(current->lexeme, "is") == 0))
            {
                break;
            }
        }

        // Parse statement
        ASTNode *statement = NULL;
        if (current->type == TOKEN_KEYWORD)
        {
            if (strcmp(current->lexeme, "show") == 0)
            {
                statement = parse_print_statement(state);
            }
            else if (strcmp(current->lexeme, "burn") == 0)
            {
                statement = parse_raise_error(state);
            }
            else if (strcmp(current->lexeme, "if") == 0)
            {
                statement = parse_conditional_block(state);
            }
            else if (strcmp(current->lexeme, "while") == 0)
            {
                statement = parse_while_block(state);
            }
            else if (strcmp(current->lexeme, "check") == 0)
            {
                state->in_switch_block = true;
                statement = parse_switch_block(state);
                state->in_switch_block = false;
            }
            else if (strcmp(current->lexeme, "break") == 0)
            {
                statement = parse_break_statement(state);
            }
            else if (strcmp(current->lexeme, "let") == 0)
            {
                statement = parse_variable_declaration(state);
            }
            else if (state->in_switch_block && strcmp(current->lexeme, "is") == 0)
            {
                break;
            }
            else
            {
                parser_error("Unexpected keyword in block", current);
            }
        }
        else if (current->type == TOKEN_IDENTIFIER)
        {
            statement = parse_variable_assignment(state);
        }
        else if (current->type == TOKEN_EOF)
        {
            break;
        }
        else
        {
            parser_error("Unexpected token in block", current);
        }

        // Add statement to list
        if (!head)
        {
            head = statement;
            tail = statement;
        }
        else
        {
            tail->next = statement;
            tail = statement;
        }
    }

    return head;
}

ASTNode *parse_conditional_block(ParserState *state)
{
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node)
    {
        parser_error("Memory allocation failed", get_current_token(state));
    }

    node->type = AST_CONDITIONAL;
    Token *current = get_current_token(state);

    // Handle if/elif/else
    if (strcmp(current->lexeme, "if") == 0 || strcmp(current->lexeme, "elif") == 0)
    {
        advance_token(state);
        node->conditional.condition = parse_expression(state);

        // Expect colon
        if (get_current_token(state)->type != TOKEN_DELIMITER ||
            strcmp(get_current_token(state)->lexeme, ":") != 0)
        {
            parser_error("Expected ':' after condition", get_current_token(state));
        }
        advance_token(state);
    }
    else if (strcmp(current->lexeme, "else") == 0)
    {
        advance_token(state);
        node->conditional.condition = NULL;

        // Expect colon
        if (get_current_token(state)->type != TOKEN_DELIMITER ||
            strcmp(get_current_token(state)->lexeme, ":") != 0)
        {
            parser_error("Expected ':' after else", get_current_token(state));
        }
        advance_token(state);
    }
    else
    {
        parser_error("Expected 'if', 'elif', or 'else'", current);
    }

    // Parse body
    node->conditional.body = parse_block(state);

    // Check for elif/else
    current = get_current_token(state);
    if (current->type == TOKEN_KEYWORD)
    {
        if (strcmp(current->lexeme, "elif") == 0 || strcmp(current->lexeme, "else") == 0)
        {
            node->conditional.else_branch = parse_conditional_block(state);
        }
        else
        {
            node->conditional.else_branch = NULL;
        }
    }
    else
    {
        node->conditional.else_branch = NULL;
    }

    node->next = NULL;
    return node;
}

ASTNode *parse_while_block(ParserState *state)
{
    // Allocate memory for the ASTNode
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node)
    {
        parser_error("Memory allocation failed", get_current_token(state));
    }

    // Set the node type to loop
    node->type = AST_LOOP;

    // Ensure the current token is 'while'
    Token *current = get_current_token(state);
    if (strcmp(current->lexeme, "while") != 0)
    {
        parser_error("Expected `while` keyword", current);
    }
    advance_token(state);

    // Parse the condition expression
    node->loop.condition = parse_expression(state);

    // Expect a colon after the condition
    current = get_current_token(state);
    if (current->type != TOKEN_DELIMITER || strcmp(current->lexeme, ":") != 0)
    {
        parser_error("Expected `:` after while condition", current);
    }
    advance_token(state);

    // Parse the loop body (indented block)
    node->loop.body = parse_block(state);

    // Add code to re-evaluate the condition after each iteration
    node->loop.re_evaluate_condition = 1; // Flag to indicate re-evaluation

    node->next = NULL;
    return node;
}

ASTNode *parse_break_statement(ParserState *state)
{
    expect_token(state, TOKEN_KEYWORD, "Expected `break` keyword");
    expect_token(state, TOKEN_DELIMITER, "Expected `;` after break");

    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node)
    {
        parser_error("Memory allocation failed", get_current_token(state));
    }

    node->type = AST_BREAK;
    node->next = NULL;
    return node;
}

ASTNode *parse_switch_block(ParserState *state)
{
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node)
    {
        parser_error("Memory allocation failed", get_current_token(state));
    }

    node->type = AST_SWITCH;

    // Handle `check` keyword
    expect_token(state, TOKEN_KEYWORD, "Expected `check` keyword");

    // Parse switch expression
    node->switch_case.expression = parse_expression(state);

    // Expect colon
    expect_token(state, TOKEN_DELIMITER, "Expected `:`");

    // Parse cases
    node->switch_case.cases = NULL;
    ASTCaseNode *last_case = NULL;

    Token *current;
    while ((current = get_current_token(state)))
    {
        // Skip any delimiters between cases
        if (current->type == TOKEN_DELIMITER)
        {
            advance_token(state);
            continue;
        }

        if (current->type != TOKEN_KEYWORD)
        {
            break; // exit if not a keyword (`is`/`else`)
        }

        if (strcmp(current->lexeme, "is") == 0)
        {
            advance_token(state); // consume `is`

            ASTCaseNode *case_node = malloc(sizeof(ASTCaseNode));
            if (!case_node)
            {
                parser_error("Memory allocation failed", current);
            }

            // Parse case value
            case_node->condition = parse_expression(state);

            // Expect colon
            expect_token(state, TOKEN_DELIMITER, "Expected `:` after case value");

            // Parse case body
            case_node->body = parse_block(state);
            case_node->next = NULL;

            // Add to cases list
            if (!node->switch_case.cases)
            {
                node->switch_case.cases = case_node;
            }
            else
            {
                last_case->next = case_node;
            }
            last_case = case_node;
        }
        else if (strcmp(current->lexeme, "else") == 0)
        {
            advance_token(state); // consume `else`

            ASTCaseNode *default_case = malloc(sizeof(ASTCaseNode));
            if (!default_case)
            {
                parser_error("Memory allocation failed", current);
            }

            default_case->condition = NULL; // no condition for `else

            // Expect colon
            expect_token(state, TOKEN_DELIMITER, "Expected `:` after else");

            // Parse else body
            default_case->body = parse_block(state);
            default_case->next = NULL;

            // Add as last case
            if (!node->switch_case.cases)
            {
                node->switch_case.cases = default_case;
            }
            else
            {
                last_case->next = default_case;
            }
            break; // else is always the last case
        }
        else
        {
            break; // Exit on any other keyword
        }
    }

    node->next = NULL;
    return node;
}

ASTNode *parse_parameter_list(ParserState *state)
{
    ASTNode *head = NULL;
    ASTNode *tail = NULL;

    while (1)
    {
        // Parse parameter name
        Token *name = get_current_token(state);
        expect_token(state, TOKEN_IDENTIFIER, "Expected parameter name");

        // Create parameter node
        ASTNode *param_node = malloc(sizeof(ASTNode));
        if (!param_node)
        {
            parser_error("Memory allocation failed", get_current_token(state));
        }
        param_node->type = AST_VARIABLE;
        param_node->variable_name = strdup(name->lexeme);
        param_node->next = NULL;

        // Add parameter to linked list
        if (!head)
        {
            head = param_node;
            tail = param_node;
        }
        else
        {
            tail->next = param_node;
            tail = param_node;
        }

        // Check for comma (indicates another parameter)
        if (get_current_token(state)->type == TOKEN_DELIMITER &&
            strcmp(get_current_token(state)->lexeme, ",") == 0)
        {
            advance_token(state); // consume `,`
        }
        else
        {
            break;
        }
    }

    return head;
}

ASTNode *parse_function_declaration(ParserState *state)
{
    expect_token(state, TOKEN_KEYWORD, "Expected `create` keyword");

    // Parse function name
    Token *name = get_current_token(state);
    excpet_token(state, TOKEN_IDENTIFIER, "Expected function name");

    // Create function node
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node)
    {
        parser_error("Memory allocation failed", get_current_token(state));
    }
    node->type = AST_FUNCTION_CALL;
    node->function_call.name = strdup(name->lexeme);

    // Parse parameters (if any)
    node->function_call.parameters = NULL;
    if (get_current_token(state)->type == TOKEN_DELIMITER &&
        strcmp(get_current_token(state)->lexeme, "(") == 0)
    {
        advance_token(state); // consume `(`
        node->function_call.parameters = parse_parameter_list(state);
        expect_token(state, TOKEN_DELIMITER, "Expected `(` after parameter list");
    }

    // Expect colon
    expect_token(state, TOKEN_DELIMITER, "Expected `:` after function signature declaration");

    // Parse function body
    node->function_call.body = parse_block(state);

    node->next = NULL;
    return node;
}

ASTNode *parse_argument_list(ParserState *state)
{
    ASTNode *head = NULL;
    ASTNode *tail = NULL;

    while (1)
    {
        // Parse arguments as expressions
        ASTNode *arg = parse_expression(state);

        // Add argument to linked list
        if (!head)
        {
            head = arg;
            tail = arg;
        }
        else
        {
            tail->next = arg;
            tail = arg;
        }

        // Check for comma (indicates another argument)
        if (get_current_token(state)->type == TOKEN_DELIMITER &&
            strcmp(get_current_token(state)->lexeme, ",") == 0)
        {
            advance_token(state); // consume `,`
        }
        else
        {
            break;
        }
    }

    return head;
}

ASTNode *parse_function_call(ParserState *state)
{
    // Parse function name
    Token *name = get_current_token(state);
    excpet_token(state, TOKEN_IDENTIFIER, "Expected function name");

    // Create function call node
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node)
    {
        parser_error("Memory allocation failed", get_current_token(state));
    }
    node->type = AST_FUNCTION_CALL;
    node->function_call.name = strdup(name->lexeme);

    // Parse arguments (if any)
    node->function_call.parameters = NULL;
    if (get_current_token(state)->type == TOKEN_DELIMITER &&
        strcmp(get_current_token(state)->lexeme, "(") == 0)
    {
        advance_token(state); // consume `(`
        node->function_call.parameters = parse_argument_list(state);
        expect_token(state, TOKEN_DELIMITER, "Expected `(` after parameter list");
    }

    node->next = NULL;
    return node;
}

void free_ast(ASTNode *node)
{
    while (node)
    {
        ASTNode *next = node->next;

        switch (node->type)
        {
        case AST_PRINT:
        case AST_ERROR:
            for (size_t i = 0; i < node->to_print.arg_count; i++)
            {
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
            if (node->literal.type == LITERAL_STRING)
            {
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

        case AST_LOOP:
            free_ast(node->loop.condition);
            free_ast(node->loop.body);
            break;

        case AST_VARIABLE:
            free(node->variable_name);
            break;

        case AST_FUNCTION_CALL:
            // Handle function call cleanup if needed
            break;

        case AST_BREAK:
            // No cleanup needed!
            break;

        case AST_SWITCH:
            if (node->switch_case.expression)
            {
                free_ast(node->switch_case.expression);
            }

            if (node->switch_case.cases)
            {
                ASTCaseNode *case_node = node->switch_case.cases;

                while (case_node)
                {
                    ASTCaseNode *next = case_node->next;

                    // Free the condition ASTNode if it exists
                    if (case_node->condition)
                    {
                        free_ast(case_node->condition);
                    }

                    // Free the body
                    if (case_node->body)
                    {
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
