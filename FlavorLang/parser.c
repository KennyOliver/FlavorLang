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
        fprintf(stderr, "Parser Error: %s (found '%s' on line %d)",
                error_message,
                get_current(tokens)->lexeme,
                get_current(tokens)->line);
        exit(1);
    }

    to_next(tokens);
}

// Parse a `let` variable declaration
// let <identifier> = <string>;
ASTNode *parse_variable_declaration(Token *tokens)
{
    expect(tokens, TOKEN_KEYWORD, "Expected `let` keyword");
    Token *name = get_current(tokens);
    expect(tokens, TOKEN_IDENTIFIER, "Expected variable name");

    expect(tokens, TOKEN_OPERATOR, "Expected `=` after variable name");
    Token *value = get_current(tokens);
    expect(tokens, TOKEN_STRING, "Expected string literal after `=`");

    expect(tokens, TOKEN_DELIMITER, "Expected `;` after variable declaration");

    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = AST_ASSIGNMENT;
    node->value = strdup(value->lexeme);
    node->next = NULL;

    printf("Parsed variable declaration: `%s = %s`\n", name->lexeme, value->lexeme);

    return node;
}

// Parse a `scran` print statement
// scran <string>;
ASTNode *parse_print_statement(Token *tokens)
{
    expect(tokens, TOKEN_KEYWORD, "Expected `scran` keyword");
    Token *value = get_current(tokens);
    expect(tokens, TOKEN_STRING, "Expected string literal after `scran`");

    expect(tokens, TOKEN_DELIMITER, "Expected `;` after `scran` statement");

    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = AST_PRINT_STATEMENT;
    node->value = strdup(value->lexeme);
    node->next = NULL;

    printf("Parsed print statement: `scran %s`\n", value->lexeme);
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
        ASTNode *temp = node->next;
        node = node->next;
        free(temp->value);
        free(temp);
    }
}
