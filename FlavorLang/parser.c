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

ASTNode *parse_variable_declaration(Token *tokens);

ASTNode *parse(Token *tokens)
{
    ASTNode *head = NULL; // start of AST
    ASTNode *current_node = NULL;

    while (get_current(tokens)->type != TOKEN_EOF)
    {
        ASTNode *new_node = NULL;

        // Match statements based on token type
        char *lexeme = get_current(tokens)->lexeme;
        if (strcmp(get_current(tokens)->lexeme, "let") == 0)
        {
            new_node = parse_variable_declaration(tokens);
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
