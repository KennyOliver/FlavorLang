# Compiler and Flags
CC = clang
CFLAGS = -fsanitize=address -fsanitize=undefined -g -Wall -Wextra -pedantic
LDFLAGS = -fsanitize=address -fsanitize=undefined

# Directories
SRC_DIRS = . shared lexer parser interpreter debug
OBJ_DIR = obj
BIN = flavor

# Collect source and header files
SRCS = $(foreach dir, $(SRC_DIRS), $(wildcard $(dir)/*.c))
OBJS = $(SRCS:%.c=$(OBJ_DIR)/%.o)

# Default target
all: $(BIN)

# Build the executable
$(BIN): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Compile source files to object files
$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(OBJ_DIR)/lexer $(OBJ_DIR)/parser $(OBJ_DIR)/interpreter $(OBJ_DIR)/debug
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up generated files
clean:
	rm -rf $(OBJ_DIR) $(BIN)

# For testing purposes
.PHONY: all clean
