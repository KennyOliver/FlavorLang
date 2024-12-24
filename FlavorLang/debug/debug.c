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

void debug_print(DebugMode debug_mode, const char *format, ...)
{
    if (debug_flag)
    {
        const char *color;
        const char *mode_name;

        switch (debug_mode)
        {
        case TOKENIZER:
            color = "\033[38;5;202m";
            mode_name = "TOK";
            break;
        case PARSER:
            color = "\033[38;5;46m";
            mode_name = "PRS";
            break;
        case INTERPRETER:
            color = "\033[38;5;93m";
            mode_name = "INT";
            break;
        default:
            color = "\033[38;5;255m"; // White for unknown
            mode_name = "UNK";
        }

        // Print the debug header with color and mode
        if (strcmp(mode_name, "UNK") == 0)
        {
            printf("%s[DEBUG]%s ", color, "\033[0m");
        }
        else
        {
            printf("%s[DEBUG | %s]%s ", color, mode_name, "\033[0m");
        }

        // Handle variable arguments
        va_list args;
        va_start(args, format);

        vprintf(format, args);

        // End variable argument processing
        va_end(args);
    }
}

void debug_print_basic(const char *format, ...)
{
    if (debug_flag)
    {
        va_list args;
        va_start(args, format);

        debug_print(GENERAL, format, args);

        va_end(args);
    }
}

void debug_print_tok(const char *format, ...)
{
    if (debug_flag)
    {
        va_list args;
        va_start(args, format);

        debug_print(TOKENIZER, format, args);

        va_end(args);
    }
}

void debug_print_par(const char *format, ...)
{
    if (debug_flag)
    {
        va_list args;
        va_start(args, format);

        debug_print(PARSER, format, args);

        va_end(args);
    }
}

void debug_print_int(const char *format, ...)
{
    if (debug_flag)
    {
        va_list args;
        va_start(args, format);

        debug_print(INTERPRETER, format, args);

        va_end(args);
    }
}
