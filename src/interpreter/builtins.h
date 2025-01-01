#ifndef BUILTINS_H
#define BUILTINS_H

#include "../debug/debug.h"
#include "../shared/ast_types.h"
#include "interpreter_types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

LiteralValue builtin_input();
LiteralValue builtin_random();

#endif
