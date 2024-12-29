#ifndef UTILS_H
#define UTILS_H

#include "interpreter.h"
#include <stdarg.h>
#include <stdio.h>

// Initialize the environment
void init_environment(Environment *env);

// Free the environment
void free_environment(Environment *env);

#endif
