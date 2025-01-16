#ifndef FL_PLUGIN_H
#define FL_PLUGIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../interpreter/interpreter_types.h" // internal types
#include "../interpreter/utils.h"
#include "../shared/ast_types.h" // for ASTNode, etc.

typedef LiteralType FL_LiteralType;
typedef LiteralValue FL_LiteralValue;
typedef InterpretResult FL_InterpretResult;

/* The prototype for a FlavorLang plugin function. */
typedef FL_InterpretResult (*FlavorLangCFunc)(ASTNode *args, Environment *env);

/* Helper functions that must be exported by the FlavorLang host:
   plugins can call these via dlsym.
*/
// FL_InterpretResult fl_make_result(FL_LiteralValue val, int did_return,
//                                   int did_break);
// FL_LiteralValue fl_create_default_value(void);

int fl_arg_count(ASTNode *args);
const char *fl_arg_as_string(ASTNode *args, int index);

#ifdef __cplusplus
}
#endif

#endif

/**
 * Reasoning for C wrapper
 *
 * If this header file is included in a C++ project, without it:
 * 1. Function names like fl_arg_count might get mangled into something like
 *     _Z12fl_arg_countP13FL_ArgList to allow C++-specific features like
 *    overloading.
 * 2. This would make it impossible for the C code to call these functions
 *    correctly.
 */
