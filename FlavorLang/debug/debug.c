#include "debug.h";
#include <stdio.h>
#include <string.h>

int debug_flag = 0;

void parse_args(int argc, char *argv[])
{
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "--debug") == 0)
        {
            debug_flag = 1;
        }
    }
}
