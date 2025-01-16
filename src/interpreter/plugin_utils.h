#ifndef PLUGIN_UTILS_H
#define PLUGIN_UTILS_H

#include "../interpreter/interpreter.h"
#include "../plugins/fl_plugin.h"
#include "../shared/ast_types.h"
#include <stdlib.h>
#include <string.h>

// Count the nodes in the linked list of arguments
int fl_arg_count(ASTNode *args);

// Return the string of the AST literal at index 'index'
const char *fl_arg_as_string(ASTNode *args, int index);

#endif
