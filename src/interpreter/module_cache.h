#ifndef MODULE_CACHE_H
#define MODULE_CACHE_H

#include "../lexer/lexer.h"
#include "../parser/parser.h"
#include "interpreter_types.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Simple struct to hold cached module exports
typedef struct ModuleCacheEntry {
    char *module_path;       // key
    Environment *export_env; // pointer to an environment holding exports
    struct ModuleCacheEntry *next;
} ModuleCacheEntry;

static ModuleCacheEntry *moduleCacheHead = NULL;

ModuleCacheEntry *lookup_module_cache(const char *module_path);
void store_module_cache(const char *module_path, Environment *export_env);

#endif
