#include "debug.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
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

void get_timestamp(char *buffer, size_t size)
{
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(buffer, size, "%H:%M:%S", t);
}

void debug_print(const char *format, ...)
{
    if (debug_flag)
    {
        char timestamp[20];
        get_timestamp(timestamp, sizeof(timestamp));

        printf("[DEBUG %s] ", timestamp);

        // Handle variable arguments
        va_list args;
        va_start(args, format);

        vprintf(format, args);

        // End variable argument processing
        va_end(args);

        printf("\n");
    }
}
