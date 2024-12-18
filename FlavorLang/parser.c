#include "parser.h"

// Global index for tracking current token position
size_t current_token = 0;

Token *get_current(Token *tokens);
Token *to_next(Token *tokens);
void expect(Token *tokens, TokenType expected, const char *error_message);
ASTNode *parse_variable_declaration(Token *tokens);
ASTNode *parse_print_statement(Token *tokens);
ASTNode *parse_identifier(Token *tokens);
ASTNode *parse_literal_or_identifier(Token *tokens);
ASTNode *parse_expression(Token *tokens);
ASTNode *parse_block(Token *tokens);
ASTNode *parse_conditional_block(Token *tokens);
ASTNode *parse(Token *tokens);
void free_ast(ASTNode *node);

Token *get_current(Token *tokens)
{
    return &tokens[current_token];
}

Token *to_next(Token *tokens)
{
    return &tokens[++current_token];
}

void expect(Token *tokens, TokenType expected, const char *error_message)
{
    if (get_current(tokens)->type != expected)
    {
        fprintf(stderr, "Parser Error: %s (found \"%s\" on line %d)",
                error_message,
                get_current(tokens)->lexeme,
                get_current(tokens)->line);
        exit(1);
    }

    to_next(tokens);
}

// Parse a `let` variable declaration
// let <identifier> = <string|number>;
ASTNode *parse_variable_declaration(Token *tokens)
{
    expect(tokens, TOKEN_KEYWORD, "Expected `let` keyword");

    // Parse variable name
    Token *name = get_current(tokens);
    expect(tokens, TOKEN_IDENTIFIER, "Expected variable name");

    // Expect '=' operator
    expect(tokens, TOKEN_OPERATOR, "Expected `=` after variable name");

    // Parse the value (string or number)
    Token *value = get_current(tokens);
    if (value->type != TOKEN_STRING && value->type != TOKEN_NUMBER)
    {
        fprintf(stderr, "Error: Expected string or number literal after `=`\n");
        exit(1);
    }

    // Advance the token pointer after parsing the value
    to_next(tokens);

    // Expect `;` after variable declaration
    expect(tokens, TOKEN_DELIMITER, "Expected `;` after variable declaration");

    // Allocate memory for the variable assignment AST node
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node)
    {
        fprintf(stderr, "Error: Memory allocation failed for ASTNode\n");
        exit(1);
    }
    node->type = AST_ASSIGNMENT;
    node->assignment.variable_name = strdup(name->lexeme);

    // Allocate memory for the value node
    ASTNode *value_node = malloc(sizeof(ASTNode));
    if (!value_node)
    {
        fprintf(stderr, "Error: Memory allocation failed for value_node\n");
        exit(1);
    }

    value_node->type = AST_LITERAL;
    // value_node->literal.type = LITERAL_STRING;
    // value_node->literal.value.string = strdup(value->lexeme);

    // Determine if value is string or number
    if (value->type == TOKEN_STRING)
    {
        value_node->literal.type = LITERAL_STRING;
        value_node->literal.value.string = strdup(value->lexeme);
    }
    else if (value->type == TOKEN_NUMBER)
    {
        value_node->literal.type = LITERAL_NUMBER;
        value_node->literal.value.number = atoi(value->lexeme); // convert lexeme to int
    }

    // Attach value node to assignment node
    node->assignment.value = value_node;
    node->next = NULL;

    // Debugging output
    if (value->type == TOKEN_STRING)
    {
        printf("Parsed variable declaration: `%s = \"%s\"`\n", name->lexeme, value->lexeme);
    }
    else
    {
        printf("Parsed variable declaration: `%s = %s`\n", name->lexeme, value->lexeme);
    }

    return node;
}

// Parse a `scran` print statement
// scran <string|identifier>;
// scran <string|identifier> <string|identifier> etc;
ASTNode *parse_print_statement(Token *tokens)
{
    expect(tokens, TOKEN_KEYWORD, "Expected `scran` keyword");

    // Allocate memory for the AST_PRINT node
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node)
    {
        fprintf(stderr, "Error: Memory allocation failed for ASTNode\n");
        exit(1);
    }

    node->type = AST_PRINT;
    node->to_print.arguments = malloc(sizeof(ASTNode *) * MAX_ARGUMENTS);
    if (!node->to_print.arguments)
    {
        fprintf(stderr, "Error: Memory allocation failed for arguments\n");
        exit(1);
    }
    node->to_print.arg_count = 0;

    while (get_current(tokens)->type != TOKEN_DELIMITER && get_current(tokens)->type != TOKEN_EOF)
    {
        if (node->to_print.arg_count >= MAX_ARGUMENTS)
        {
            fprintf(stderr, "Error: Too many arguments in `scran` statement (max %d)\n", MAX_ARGUMENTS);
            exit(1);
        }

        ASTNode *arg = malloc(sizeof(ASTNode));
        if (!arg)
        {
            fprintf(stderr, "Error: Memory allocation failed for argument node\n");
            exit(1);
        }

        // Handle string or identifier arguments
        if (get_current(tokens)->type == TOKEN_STRING)
        {
            arg->type = AST_LITERAL;
            arg->literal.type = LITERAL_STRING;
            arg->literal.value.string = strdup(get_current(tokens)->lexeme);
        }
        else if (get_current(tokens)->type == TOKEN_NUMBER)
        {
            arg->type = AST_LITERAL;
            arg->literal.type = LITERAL_NUMBER;
            arg->literal.value.number = atof(get_current(tokens)->lexeme); // convert lexeme to int
        }
        else if (get_current(tokens)->type == TOKEN_IDENTIFIER)
        {
            arg->type = AST_ASSIGNMENT;
            arg->variable_name = strdup(get_current(tokens)->lexeme);
        }
        else
        {
            fprintf(stderr, "Error: Unexpected token \"%s\" in `scran` statement\n", get_current(tokens)->lexeme);
            exit(1);
        }

        // Add argument to the `to_print.arguments` array
        node->to_print.arguments[node->to_print.arg_count++] = arg;
        to_next(tokens);

        // If the next token is `,` then skip it and continue parsing
        if (get_current(tokens)->type == TOKEN_DELIMITER && strcmp(get_current(tokens)->lexeme, ",") == 0)
        {
            to_next(tokens);
        }
    }

    // Expect `;` after parsing all arguments
    expect(tokens, TOKEN_DELIMITER, "Expected `;` after `scran` statement");

    printf("Parsed print statement with %zu arguments:\n", node->to_print.arg_count);

    // Debugging
    for (size_t i = 0; i < node->to_print.arg_count; i++)
    {
        ASTNode *arg = node->to_print.arguments[i];
        if (arg->type == AST_LITERAL)
        {
            switch (arg->literal.type)
            {
            case LITERAL_STRING:
                printf("\tArgument %zu: STRING = \"%s\"\n", i + 1, arg->literal.value.string);
                break;
            case LITERAL_NUMBER:
                printf("\tArgument %zu: NUMBER = \"%f\"\n", i + 1, arg->literal.value.number);
                break;
            default:
                printf("\tArgument %zu: UNKNOWN LITERAL TYPE\n", i + 1);
            }
        }
        else if (arg->type == AST_ASSIGNMENT)
        {
            printf("\tArgument %zu: VARIABLE = \"%s\"\n", i + 1, arg->variable_name);
        }
        else
        {
            printf("\tArgument %zu: UNKNOWN TYPE\n", i + 1);
        }
    }

    return node;
}

/**
 * Parse an identifier token (variable name).
 * @param tokens all the tokens from the tokenizer
 * @return ASTNode*
 */
ASTNode *parse_identifier(Token *tokens)
{
    Token *current = get_current(tokens);

    if (current->type != TOKEN_IDENTIFIER)
    {
        fprintf(stderr, "Error: Expected identifier, but got \"%s\"\n", current->lexeme);
        exit(1);
    }

    // Allocate and populate an ASTNode for the identifier
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node)
    {
        fprintf(stderr, "Error: Memory allocation failed for ASTNode\n");
        exit(1);
    }

    node->type = AST_ASSIGNMENT;
    node->variable_name = strdup(current->lexeme); // copy variable name
    node->next = NULL;

    to_next(tokens);

    return node;
}

/**
 * @brief Determines whetehr the current token is a literal (e.g., number, string) or an identifier.
 * Delegates to `parse_literal()` or `parse_identifier()` accordingly
 *
 * @param tokens
 * @return ASTNode*
 */
ASTNode *parse_literal_or_identifier(Token *tokens)
{
    Token *current = get_current(tokens);

    if (current->type == TOKEN_NUMBER || current->type == TOKEN_STRING)
    {
        ASTNode *node = malloc(sizeof(ASTNode));
        if (!node)
        {
            fprintf(stderr, "Error: Memory allocation failed for ASTNode\n");
            exit(1);
        }

        node->type = AST_LITERAL;
        node->literal.type = (current->type == TOKEN_NUMBER) ? LITERAL_NUMBER : LITERAL_STRING;

        if (node->literal.type == LITERAL_NUMBER)
        {
            node->literal.value.number = atof(current->lexeme); // convert string to number
        }
        else
        {
            node->literal.value.string = strdup(current->lexeme);
        }

        node->next = NULL;

        to_next(tokens);

        return node;
    }
    else if (current->type == TOKEN_IDENTIFIER)
    {
        return parse_identifier(tokens);
    }
    else
    {
        fprintf(stderr, "Error: Expected literal or identifier, but got \"%s\"\n", current->lexeme);
        exit(1);
    }
}

ASTNode *parse_expression(Token *tokens)
{
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node)
    {
        fprintf(stderr, "Error: Memory allocation failed for ASTNode\n");
        exit(1);
    }

    node->type = AST_BINARY_OP;

    // Parse left-hand side
    if (get_current(tokens)->type == TOKEN_IDENTIFIER)
    {
        node->binary_op.left = parse_identifier(tokens);
    }
    else
    {
        node->binary_op.left = parse_literal_or_identifier(tokens);
    }

    // Parse operator
    Token *operator= get_current(tokens);
    if (operator->type == TOKEN_OPERATOR)
    {
        node->binary_op.operator= strdup(operator->lexeme);
        to_next(tokens);
    }
    else
    {
        fprintf(stderr, "Error: Expected operator in condition, got '%s'\n", operator->lexeme);
        free(node);
        exit(1);
    }

    // Parse right-hand side
    if (get_current(tokens)->type == TOKEN_IDENTIFIER)
    {
        node->binary_op.right = parse_identifier(tokens);
    }
    else
    {
        node->binary_op.right = parse_literal_or_identifier(tokens);
    }

    return node;
}

ASTNode *parse_block(Token *tokens)
{
    ASTNode *head = NULL; // Head of the linked list
    ASTNode *tail = NULL; // Tail of the linked list

    while (get_current(tokens)->type != TOKEN_EOF)
    {
        Token *current = get_current(tokens);

        // Check if we've reached the end of the block
        if (current->type == TOKEN_DELIMITER && strcmp(current->lexeme, ";") == 0)
        {
            to_next(tokens); // Consume the semicolon
            break;
        }

        // Handle case where we hit an elif/else - this means we're done with this block
        if (current->type == TOKEN_KEYWORD &&
            (strcmp(current->lexeme, "elif") == 0 || strcmp(current->lexeme, "else") == 0))
        {
            break;
        }

        ASTNode *statement = NULL;

        // Parse statements based on token type
        if (current->type == TOKEN_KEYWORD)
        {
            if (strcmp(current->lexeme, "scran") == 0)
            {
                statement = parse_print_statement(tokens);
            }
            else if (strcmp(current->lexeme, "if") == 0)
            {
                statement = parse_conditional_block(tokens);
            }
            else
            {
                fprintf(stderr, "Error: Unexpected keyword '%s' in block\n",
                        current->lexeme ? current->lexeme : "null");
                exit(1);
            }
        }
        else if (current->type == TOKEN_EOF)
        {
            break;
        }
        else
        {
            fprintf(stderr, "Error: Unexpected token '%s' in block\n",
                    current->lexeme ? current->lexeme : "null");
            exit(1);
        }

        // Add the statement to the linked list
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

ASTNode *parse_conditional_block(Token *tokens)
{
    // Allocate memory for the AST_CONDITIONAL node
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node)
    {
        fprintf(stderr, "Error: Memory allocation failed for ASTNode\n");
        exit(1);
    }

    node->type = AST_CONDITIONAL;

    // Check if this is an 'if' or 'elif' block (which need conditions)
    // or an 'else' block (which doesn't)
    Token *current = get_current(tokens);
    if (strcmp(current->lexeme, "if") == 0 || strcmp(current->lexeme, "elif") == 0)
    {
        to_next(tokens); // consume 'if' or 'elif'

        // Parse the condition for 'if' or 'elif'
        node->conditional.condition = parse_expression(tokens);

        // Expect and consume the ':' delimiter
        if (get_current(tokens)->type != TOKEN_DELIMITER || strcmp(get_current(tokens)->lexeme, ":") != 0)
        {
            fprintf(stderr, "Error: Expected ':' after condition but got '%s'\n", get_current(tokens)->lexeme);
            exit(1);
        }
        to_next(tokens); // Consume the ':'
    }
    else if (strcmp(current->lexeme, "else") == 0)
    {
        to_next(tokens); // consume 'else'

        // No condition for else block
        node->conditional.condition = NULL;

        // Expect and consume the ':' delimiter
        if (get_current(tokens)->type != TOKEN_DELIMITER || strcmp(get_current(tokens)->lexeme, ":") != 0)
        {
            fprintf(stderr, "Error: Expected ':' after 'else' but got '%s'\n", get_current(tokens)->lexeme);
            exit(1);
        }
        to_next(tokens); // Consume the ':'
    }
    else
    {
        fprintf(stderr, "Error: Expected 'if', 'elif', or 'else' but got '%s'\n", current->lexeme);
        exit(1);
    }

    // Parse the body
    node->conditional.body = parse_block(tokens);

    // Check for `elif` or `else`
    current = get_current(tokens);
    if (current->type == TOKEN_KEYWORD)
    {
        if (strcmp(current->lexeme, "elif") == 0 || strcmp(current->lexeme, "else") == 0)
        {
            node->conditional.else_branch = parse_conditional_block(tokens);
        }
    }
    else
    {
        node->conditional.else_branch = NULL; // no `elif` or `else` present
    }

    printf("Parsed conditional statement\n");
    return node;
}

ASTNode *parse(Token *tokens)
{
    ASTNode *head = NULL; // start of AST
    ASTNode *current_node = NULL;

    while (get_current(tokens)->type != TOKEN_EOF)
    {
        ASTNode *new_node = NULL;

        // Match statements based on token type
        if (strcmp(get_current(tokens)->lexeme, "let") == 0)
        {
            new_node = parse_variable_declaration(tokens);
        }
        else if (strcmp(get_current(tokens)->lexeme, "scran") == 0)
        {
            new_node = parse_print_statement(tokens);
        }
        else if (strcmp(get_current(tokens)->lexeme, "if") == 0)
        {
            new_node = parse_conditional_block(tokens);
        }
        else
        {
            fprintf(stderr, "Parser Error: Unexpected token '%s' on line %d",
                    get_current(tokens)->lexeme,
                    get_current(tokens)->line);
            exit(1);
        }

        // Append new node to AST
        if (head == NULL)
        {
            head = new_node;
        }
        else
        {
            current_node->next = new_node;
        }

        current_node = new_node;
    }

    return head;
}

void free_ast(ASTNode *node)
{
    while (node)
    {
        ASTNode *temp = node->next; // save next node pointer

        // Free current node base on its type
        if (node->type == AST_PRINT)
        {
            for (size_t i = 0; i < node->to_print.arg_count; i++)
            {
                free_ast(node->to_print.arguments[i]); // free each argument node
            }
            free(node->to_print.arguments);
        }
        else if (node->type == AST_ASSIGNMENT)
        {
            free(node->assignment.variable_name); // free variable name
            free(node->assignment.value);         // free assigned value node
        }
        else if (node->type == AST_LITERAL)
        {
            if (node->literal.type == LITERAL_STRING)
            {
                free(node->literal.value.string); // free the string literal
            }
            // No need to free memory for LITERAL_NUMBER
        }

        free(node);  // free current node itself
        node = temp; // go to next node
    }
}
