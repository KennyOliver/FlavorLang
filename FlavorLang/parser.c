#include "parser.h"

// Global index for tracking current token position
size_t current_token = 0;

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

    // Token *value = get_current(tokens);
    // expect(tokens, TOKEN_STRING, "Expected string literal after `scran`");

    // node->next = NULL;

    // Parse arguments separated by `,` until `;` (delimiter) or EOF is reached
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

ASTNode *parse_conditionals(Token *tokens)
{
    expect(tokens, TOKEN_KEYWORD, "Expected `if` keyword");

    // Allocate memory for the AST_CONDITIONAL node
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node)
    {
        fprintf(stderr, "Error: Memory allocation failed for ASTNode\n");
        exit(1);
    }

    node->type = AST_CONDITIONAL;

    expect(tokens, TOKEN_DELIMITER, "Expected `:` after conditional");

    expect(tokens, TOKEN_DELIMITER, "Expected `;` after `if` conditional");

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
