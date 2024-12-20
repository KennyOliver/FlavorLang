#ifndef DEBUG_H
#define DEBUG_H

extern int debug_flag;

void parse_cli_args(int argc, char *argv[]);

void debug_print(const char *message);

#endif
