#ifndef FL_PLUGIN_H
#define FL_PLUGIN_H

#ifdef __cplusplus
extern "C" {
#endif

// Relative paths aren't used since these reference header files
// from within the tarball that gets generated via the Makefile
#include "interpreter/interpreter_types.h" // internal types
#include "interpreter/utils.h"
#include "shared/ast_types.h" // for ASTNode, etc.

#ifdef __cplusplus
}
#endif

#endif
