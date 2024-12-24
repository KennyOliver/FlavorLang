#include "debug.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

bool debug_flag = false;

void parse_cli_args(int argc, char *argv[])
{
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "--debug") == 0)
        {
            debug_flag = true;
        }
    }
}

void debug_print(const char *format, ...)
{
    if (debug_flag)
    {
        printf("[DEBUG %s] ", "???");

        // Handle variable arguments
        va_list args;
        va_start(args, format);

        vprintf(format, args);

        // End variable argument processing
        va_end(args);

        printf("\n");
    }
}
