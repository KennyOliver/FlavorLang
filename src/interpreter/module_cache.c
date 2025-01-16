#include "module_cache.h"

ModuleCacheEntry *lookup_module_cache(const char *module_path) {
    ModuleCacheEntry *entry = moduleCacheHead;
    while (entry) {
        if (strcmp(entry->module_path, module_path) == 0) {
            return entry;
        }
        entry = entry->next;
    }
    return NULL;
}

void store_module_cache(const char *module_path, Environment *export_env) {
    ModuleCacheEntry *entry = calloc(1, sizeof(ModuleCacheEntry));
    if (!entry) {
        fatal_error("Memory allocation failed in store_module_cache.\n");
    }
    entry->module_path = strdup(module_path);
    entry->export_env =
        export_env; // might want to copy export table in the future
    entry->next = moduleCacheHead;
    moduleCacheHead = entry;
}
