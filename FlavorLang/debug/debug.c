#include "debug.h";
#include <stdio.h>
#include <string.h>
#include <time.h>

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

void get_timestamp(char *buffer, size_t size)
{
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(buffer, size, "%H:%M:%S", t);
}

void debug_print(const char *message)
{
    if (debug_flag)
    {
        char timestamp[20];
        get_timestamp(timestamp, sizeof(timestamp));
        printf("[DEBUG | %s]\n", timestamp, message);
    }
}
