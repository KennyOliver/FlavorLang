#ifndef AST_NODE_UTILS_H
#define AST_NODE_UTILS_H

#include <stdlib.h>

typedef struct ASTNode
{
    char *type;            // Node type (e.g., operator, identifier, etc.)
    void *value;           // Node value (specific to the type)
    struct ASTNode *left;  // Left child (if applicable)
    struct ASTNode *right; // Right child (if applicable)
} ASTNode;

// Function prototypes
ASTNode *create_node(const char *type, void *value, ASTNode *left, ASTNode *right);
void free_node(ASTNode *node);
ASTNode *create_literal_node(const char *type, void *value);
ASTNode *create_binary_op_node(const char *type, ASTNode *left, ASTNode *right);

#endif
