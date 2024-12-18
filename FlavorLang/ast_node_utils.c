#include "ast_node_utils.h"
#include <string.h> // For strdup

ASTNode *create_node(const char *type, void *value, ASTNode *left, ASTNode *right)
{
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    if (!node)
        return NULL;
    node->type = strdup(type); // Copy string
    node->value = value;
    node->left = left;
    node->right = right;
    return node;
}

void free_node(ASTNode *node)
{
    if (!node)
        return;
    free(node->type);
    free(node->value); // Free if dynamically allocated
    free_node(node->left);
    free_node(node->right);
    free(node);
}

ASTNode *create_literal_node(const char *type, void *value)
{
    return create_node(type, value, NULL, NULL);
}

ASTNode *create_binary_op_node(const char *type, ASTNode *left, ASTNode *right)
{
    return create_node(type, NULL, left, right);
}
