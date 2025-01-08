#ifndef DEBUG_H
#define DEBUG_H

#include <stdbool.h>

extern bool debug_flag;

typedef enum { GENERAL, LEXER, PARSER, INTERPRETER } DebugMode;

void debug_print_basic(const char *message, ...);
void debug_print_lex(const char *message, ...);
void debug_print_par(const char *message, ...);
void debug_print_int(const char *message, ...);

#endif
