#ifndef DEBUG_H
#define DEBUG_H

#include <stdbool.h>

extern bool debug_flag;

typedef enum
{
    GENERAL,
    TOKENIZER,
    PARSER,
    INTERPRETER
} DebugMode;

void parse_cli_args(int argc, char *argv[]);
void debug_print_basic(const char *message, ...);
void debug_print_tok(const char *message, ...);
void debug_print_par(const char *message, ...);
void debug_print_int(const char *message, ...);

#endif
