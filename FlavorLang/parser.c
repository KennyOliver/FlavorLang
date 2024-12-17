#include "parser.h"

// Global index for tracking current token position
size_t current_token = 0;

Token *get_current(Token *tokens)
{
    return &tokens[current_token];
}

Token *get_next(Token *tokens)
{
    return &tokens[++current_token];
}

void expect(Token *tokens, TokenType expected, const char *error_message);

ASTNode *parse(Token *tokens)
{
    ASTNode *head = NULL; // start of AST
    ASTNode *current_node = NULL;

    return head;
}
