#include "parser.h"
#include <string.h>

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
        else if (strcmp(token->lexeme, "scran") == 0)
        {
            new_node = parse_print_statement(state);
        }
        else if (strcmp(token->lexeme, "if") == 0)
        {
            new_node = parse_conditional_block(state);
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
    expect_token(state, TOKEN_KEYWORD, "Expected 'let' keyword");

    // Parse variable name
    Token *name = get_current_token(state);
    expect_token(state, TOKEN_IDENTIFIER, "Expected variable name");

    // Expect '=' operator
    expect_token(state, TOKEN_OPERATOR, "Expected '=' after variable name");

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

ASTNode *parse_print_statement(ParserState *state)
{
    expect_token(state, TOKEN_KEYWORD, "Expected 'scran' keyword");

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
            parser_error("Too many arguments in scran statement", get_current_token(state));
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

    expect_token(state, TOKEN_DELIMITER, "Expected ';' after scran statement");
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

        node->type = AST_ASSIGNMENT;
        node->variable_name = strdup(current->lexeme);
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
    ASTNode *head = NULL;
    ASTNode *tail = NULL;

    while (get_current_token(state)->type != TOKEN_EOF)
    {
        Token *current = get_current_token(state);

        // Check for block end conditions
        if (current->type == TOKEN_DELIMITER && strcmp(current->lexeme, ";") == 0)
        {
            advance_token(state);
            break;
        }

        if (current->type == TOKEN_KEYWORD &&
            (strcmp(current->lexeme, "elif") == 0 || strcmp(current->lexeme, "else") == 0))
        {
            break;
        }

        // Parse statement
        ASTNode *statement = NULL;
        if (current->type == TOKEN_KEYWORD)
        {
            if (strcmp(current->lexeme, "scran") == 0)
            {
                statement = parse_print_statement(state);
            }
            else if (strcmp(current->lexeme, "if") == 0)
            {
                statement = parse_conditional_block(state);
            }
            else
            {
                parser_error("Unexpected keyword in block", current);
            }
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

void free_ast(ASTNode *node)
{
    while (node)
    {
        ASTNode *next = node->next;

        switch (node->type)
        {
        case AST_PRINT:
            for (size_t i = 0; i < node->to_print.arg_count; i++)
            {
                free_ast(node->to_print.arguments[i]);
            }
            free(node->to_print.arguments);
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

        case AST_FUNCTION_CALL:
            fprintf(stderr, "Error: `AST_FUNCTION_CALL` not implemented yet.\n");

        default:
            fprintf(stderr, "Error: Unknown ASTNode type.\n");
            exit(1);
        }

        free(node);
        node = next;
    }
}
