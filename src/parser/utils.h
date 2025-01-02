#ifndef PARSER_UTILS_H
#define PARSER_UTILS_H

#include "../debug/debug.h"
#include "../shared/ast_types.h"
#include <stdio.h>

void free_ast(ASTNode *node);

// Print indentation based on depth
static void print_indentation(int depth);

// Print AST nodes
void print_ast(ASTNode *node, int depth);

#endif
