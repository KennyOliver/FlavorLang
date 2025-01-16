# ==================================================
# Makefile for building a FlavorLang C plugin
# ==================================================

PLUGIN_SRC = test_plugin.c  # Name of plugin source file
PLUGIN_SO = test_plugin.so  # Name of output shared object

# Set compiler here (defaults to gcc)
CC = gcc

# Compiler flags for creating a position‐independent code
CFLAGS += -fPIC -Wall -Wextra -O2

# Linker flags: `-shared` tells the linker to create a shared object
# For macOS, `-undefined dynamic_lookup` is needed
# so that unresolved symbols are resolved at runtime
LDFLAGS += -shared -undefined dynamic_lookup

# Tell linker to export all symbols so that plugins
# loaded later using dlopen/dlsym can resolve them
LDFLAGS += -rdynamic

# The default target builds the plugin shared object
all: $(PLUGIN_SO)

$(PLUGIN_SO): $(PLUGIN_SRC)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

# Clean rule to remove generated plugin file
clean:
	rm -f $(PLUGIN_SO)

.PHONY: all clean
