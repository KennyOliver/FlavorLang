# Compiler and Flags
CC = clang

# Check if running in GitHub Actions
ifeq ($(GITHUB_ACTIONS),true)
    # CI build flags (no sanitizers)
    CFLAGS = -g -Wall -Wextra -pedantic
    LDFLAGS =
else
    # Local build flags (with sanitizers)
    CFLAGS = -fsanitize=address -fsanitize=undefined -g -Wall -Wextra -pedantic
    LDFLAGS = -fsanitize=address -fsanitize=undefined
    ASAN_OPTIONS = halt_on_error=0:log_path=asan_log
endif

LDFLAGS += -lm
CFLAGS += -I. -Iplugins

# Directories
SRC_DIRS = . shared lexer parser interpreter debug
OBJ_DIR = obj
BIN = flavor

# Collect source and header files
SRCS = $(foreach dir, $(SRC_DIRS), $(wildcard $(dir)/*.c))
OBJS = $(SRCS:%.c=$(OBJ_DIR)/%.o)

# Header tarball for embedding
HEADERS_TAR = headers.tar.gz
HEADER_FILES = $(wildcard shared/*.h interpreter/*.h lexer/*.h parser/*.h debug/*.h)
FL_PLUGIN_H = plugins/fl_plugin.h

# Generated C file for the embedded headers
HEADERS_DATA_C = headers_data.c

# Default target
all: $(HEADERS_TAR) $(HEADERS_DATA_C) $(BIN)

# Build the executable
$(BIN): $(OBJS) $(HEADERS_DATA_C)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Compile source files to object files
$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(OBJ_DIR)/lexer $(OBJ_DIR)/parser $(OBJ_DIR)/interpreter $(OBJ_DIR)/debug
	$(CC) $(CFLAGS) -c $< -o $@

# Create the tarball of headers
$(HEADERS_TAR): $(HEADER_FILES) $(FL_PLUGIN_H)
	@mkdir -p include
	cp -r plugins/fl_plugin.h shared interpreter lexer parser debug include/
	tar -czf $@ -C include .
	rm -rf include

# Generate C file from the tarball
$(HEADERS_DATA_C): $(HEADERS_TAR)
	xxd -i $< > $@

# Clean up generated files
clean:
	rm -rf $(OBJ_DIR) $(BIN) $(HEADERS_TAR) $(HEADERS_DATA_C)

# For testing purposes
.PHONY: all clean
