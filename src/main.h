#ifndef MAIN_H
#define MAIN_H

#include "debug/debug.h"
#include "interpreter/interpreter.h"
#include "lexer/lexer.h"
#include "parser/parser.h"
#include <dlfcn.h>
#include <libgen.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

// Structure to hold command-line options
typedef struct {
    bool minify;
    char *filename;
    bool make_plugin;
} Options;

void write_header_to_disk(const char *header_name, const char *content,
                          const char *output_dir);
void extract_embedded_headers(const char *output_dir);
void print_usage(const char *prog_name);
void parse_cli_args(int argc, char *argv[], Options *options);
char *generate_minified_filename(const char *input_filename);
void minify_tokens(Token *tokens, const char *output_file);
void print_logo(void);
void print_about(void);

// Logo
const char *FLAVOR_LOGO[] = {
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m \033[0m\033[42m "
    "\033[0m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\n",
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[42m \033[0m\033[42m "
    "\033[0m\033[42m \033[0m\033[42m \033[0m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\n",
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m \033[0m\033[42m \033[0m\033[42m "
    "\033[0m\033[42m \033[0m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\n",
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[42m "
    "\033[0m\033[42m \033[0m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\n",
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m \033[0m\033[42m \033[0m\033[42m "
    "\033[0m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\n",
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m \033[0m\033[42m \033[0m \033[0m \033[0m "
    "\033[0m\033[42m \033[0m\033[42m \033[0m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\n",
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m "
    "\033[0m\033[42m \033[0m\033[42m \033[0m\033[42m \033[0m\033[42m "
    "\033[0m\033[42m \033[0m\033[42m \033[0m\033[42m \033[0m\033[42m \033[0m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\n",
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[41m \033[0m\033[41m "
    "\033[0m\033[41m \033[0m\033[41m \033[0m\033[42m \033[0m\033[42m "
    "\033[0m\033[42m \033[0m\033[42m \033[0m\033[42m \033[0m\033[42m "
    "\033[0m\033[42m \033[0m\033[42m \033[0m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\n",
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[41m \033[0m\033[41m "
    "\033[0m\033[41m \033[0m\033[41m \033[0m\033[41m \033[0m\033[42m "
    "\033[0m\033[42m \033[0m\033[42m \033[0m\033[42m \033[0m\033[42m "
    "\033[0m\033[42m \033[0m\033[42m \033[0m\033[42m \033[0m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\n",
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[41m \033[0m\033[41m \033[0m\033[41m "
    "\033[0m\033[41m \033[0m\033[41m \033[0m\033[41m \033[42m \033[0m\033[42m "
    "\033[0m\033[42m \033[0m\033[42m \033[0m\033[41m \033[0m\033[41m "
    "\033[0m\033[42m \033[0m\033[42m \033[0m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\n",
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[41m \033[0m\033[41m \033[0m\033[41m \033[0m\033[41m "
    "\033[0m\033[41m \033[0m\033[41m \033[0m\033[41m \033[0m\033[41m \033[42m "
    "\033[42m \033[0m\033[41m \033[0m\033[41m \033[0m\033[41m \033[0m\033[41m "
    "\033[0m\033[41m \033[0m\033[41m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\n",
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[41m "
    "\033[0m\033[41m \033[0m\033[41m \033[0m\033[41m \033[0m\033[41m "
    "\033[0m\033[41m \033[0m\033[41m \033[0m\033[41m \033[0m\033[41m "
    "\033[0m\033[41m \033[0m\033[41m \033[0m\033[41m \033[0m\033[41m "
    "\033[0m\033[41m \033[0m\033[41m \033[0m\033[41m \033[0m\033[41m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\n",
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[41m "
    "\033[0m\033[41m \033[0m\033[47m \033[0m\033[47m \033[0m\033[47m "
    "\033[0m\033[41m \033[0m\033[41m \033[0m\033[41m \033[0m\033[41m "
    "\033[0m\033[41m \033[0m\033[41m \033[0m\033[47m \033[0m\033[47m "
    "\033[0m\033[47m \033[0m\033[41m \033[0m\033[41m \033[0m\033[41m "
    "\033[0m\033[41m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\n",
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[41m "
    "\033[0m\033[47m \033[0m\033[47m \033[0m\033[47m \033[0m\033[37m "
    "\033[0m\033[47m \033[0m\033[41m \033[0m\033[41m \033[0m\033[41m "
    "\033[0m\033[41m \033[0m\033[47m \033[0m\033[47m \033[0m\033[47m "
    "\033[0m\033[37m \033[0m\033[47m \033[0m\033[41m \033[0m\033[41m "
    "\033[0m\033[41m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\n",
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[41m "
    "\033[0m\033[47m \033[0m\033[47m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[41m \033[0m\033[41m \033[0m\033[41m "
    "\033[0m\033[41m \033[0m\033[47m \033[0m\033[47m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[41m \033[0m\033[41m "
    "\033[0m\033[41m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\n",
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[41m "
    "\033[0m\033[47m \033[0m\033[47m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[47m \033[0m\033[41m \033[0m\033[41m \033[0m\033[41m "
    "\033[0m\033[41m \033[0m\033[47m \033[0m\033[47m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[47m \033[0m\033[41m \033[0m\033[41m "
    "\033[0m\033[41m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\n",
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[41m \033[0m\033[41m "
    "\033[0m\033[41m \033[0m\033[41m \033[0m\033[41m \033[0m\033[41m "
    "\033[0m\033[41m \033[0m\033[41m \033[0m\033[41m \033[0m\033[41m "
    "\033[0m\033[41m \033[0m\033[41m \033[0m\033[41m \033[0m\033[41m "
    "\033[0m\033[41m \033[0m\033[41m \033[0m\033[41m \033[0m\033[41m "
    "\033[0m\033[41m \033[0m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\n",
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[41m \033[0m\033[41m "
    "\033[0m\033[41m \033[0m\033[41m \033[0m\033[41m \033[0m\033[41m "
    "\033[0m\033[37m \033[0m\033[41m \033[0m\033[41m \033[0m\033[41m "
    "\033[0m\033[37m \033[0m\033[41m \033[0m\033[41m \033[0m\033[41m "
    "\033[0m\033[41m \033[0m\033[41m \033[0m\033[41m \033[0m\033[41m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\n",
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[41m \033[0m\033[41m \033[0m\033[41m "
    "\033[0m\033[41m \033[0m\033[41m \033[0m\033[41m \033[0m\033[41m "
    "\033[0m\033[41m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[41m \033[0m\033[41m \033[0m\033[41m \033[0m\033[41m "
    "\033[0m\033[41m \033[0m\033[41m \033[0m\033[41m \033[0m\033[41m \033[0m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\n",
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[41m \033[0m\033[41m \033[37m \033[0m\033[41m \033[0m\033[41m "
    "\033[0m\033[41m \033[0m\033[41m \033[0m\033[41m \033[0m\033[41m "
    "\033[0m\033[41m \033[0m\033[41m \033[0m\033[41m \033[0m\033[41m "
    "\033[0m\033[41m \033[0m\033[41m \033[0m\033[41m \033[0m\033[41m "
    "\033[0m\033[41m \033[0m\033[41m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\n",
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[41m "
    "\033[0m\033[41m \033[37m \033[0m\033[41m \033[0m\033[41m \033[0m\033[41m "
    "\033[0m\033[41m \033[0m\033[41m \033[0m\033[41m \033[0m\033[41m "
    "\033[0m\033[41m \033[0m\033[41m \033[0m\033[41m \033[0m\033[41m "
    "\033[0m\033[41m \033[0m\033[41m \033[0m\033[41m \033[0m\033[41m "
    "\033[0m\033[41m \033[0m\033[41m \033[0m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\n",
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[41m "
    "\033[0m\033[41m \033[0m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[41m \033[0m\033[41m \033[0m\033[41m \033[37m \033[0m\033[41m "
    "\033[0m\033[41m \033[0m\033[41m \033[0m\033[41m \033[0m\033[41m "
    "\033[0m\033[41m \033[0m\033[41m \033[0m\033[41m \033[0m\033[41m "
    "\033[0m\033[41m \033[0m\033[41m \033[0m\033[41m \033[0m\033[41m "
    "\033[0m\033[41m \033[0m\033[41m \033[0m\033[41m \033[0m\033[41m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\n",
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[41m \033[37m "
    "\033[37m \033[0m\033[41m \033[0m\033[41m \033[0m\033[41m \033[0m \033[0m "
    "\033[0m \033[0m\033[41m \033[37m \033[37m \033[0m\033[41m \033[0m\033[41m "
    "\033[0m\033[41m \033[0m\033[41m \033[0m\033[41m \033[0m\033[41m "
    "\033[0m\033[41m \033[0m\033[41m \033[0m\033[41m \033[0m\033[41m "
    "\033[0m\033[41m \033[0m\033[41m \033[0m\033[41m \033[0m\033[41m "
    "\033[0m\033[41m \033[0m\033[41m \033[0m\033[41m \033[0m\033[41m "
    "\033[0m\033[41m \033[0m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\n",
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[41m "
    "\033[0m\033[41m \033[0m\033[41m \033[0m\033[41m \033[0m\033[41m "
    "\033[0m\033[41m \033[37m \033[0m\033[41m \033[0m\033[41m \033[0m\033[41m "
    "\033[0m\033[41m \033[0m\033[41m \033[0m\033[41m \033[0m\033[41m "
    "\033[0m\033[41m \033[0m\033[41m \033[0m\033[41m \033[0m\033[41m "
    "\033[0m\033[41m \033[0m\033[41m \033[0m\033[41m \033[0m\033[41m "
    "\033[0m\033[41m \033[0m\033[41m \033[0m\033[41m \033[0m\033[41m "
    "\033[0m\033[41m \033[0m\033[41m \033[0m\033[41m \033[0m\033[41m \033[0m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\n",
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[41m \033[0m\033[41m \033[0m\033[41m \033[0m\033[41m "
    "\033[0m\033[41m \033[0m\033[41m \033[0m\033[41m \033[0m\033[41m "
    "\033[0m\033[41m \033[0m\033[41m \033[0m\033[41m \033[0m\033[41m "
    "\033[0m\033[41m \033[0m\033[41m \033[0m\033[41m \033[0m\033[41m "
    "\033[0m\033[41m \033[0m\033[41m \033[0m\033[41m \033[0m\033[41m "
    "\033[0m\033[41m \033[0m\033[41m \033[0m\033[41m \033[0m\033[41m "
    "\033[0m\033[41m \033[0m\033[41m \033[0m\033[41m \033[0m\033[37m \033[0m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\n",
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[41m \033[0m\033[41m \033[0m\033[41m "
    "\033[0m\033[41m \033[0m\033[41m \033[0m\033[41m \033[0m\033[41m "
    "\033[0m\033[41m \033[0m\033[41m \033[0m\033[41m \033[0m\033[41m "
    "\033[0m\033[41m \033[0m\033[41m \033[0m\033[41m \033[0m\033[41m "
    "\033[0m\033[41m \033[0m\033[41m \033[0m\033[41m \033[0m\033[41m "
    "\033[0m\033[41m \033[0m\033[41m \033[0m\033[41m \033[0m\033[41m "
    "\033[0m\033[41m \033[0m\033[37m \033[0m\033[37m \033[0m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\n",
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[41m "
    "\033[0m\033[41m \033[0m\033[41m \033[0m\033[41m \033[0m\033[41m "
    "\033[0m\033[41m \033[0m\033[41m \033[0m\033[41m \033[0m\033[41m "
    "\033[0m\033[41m \033[0m\033[41m \033[0m\033[41m \033[0m\033[41m "
    "\033[0m\033[41m \033[0m\033[41m \033[0m\033[41m \033[0m\033[41m "
    "\033[0m\033[41m \033[0m\033[41m \033[0m\033[41m \033[0m\033[41m "
    "\033[0m\033[37m \033[0m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\n",
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m \033[0m "
    "\033[0m\033[41m \033[0m\033[41m \033[0m\033[41m \033[0m\033[41m "
    "\033[0m\033[41m \033[0m\033[41m \033[0m\033[41m \033[0m\033[41m "
    "\033[0m\033[41m \033[0m\033[41m \033[0m\033[41m \033[0m\033[41m "
    "\033[0m\033[41m \033[0m\033[41m \033[0m\033[41m \033[0m\033[41m "
    "\033[0m\033[41m \033[0m \033[0m \033[0m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\n",
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[41m "
    "\033[0m\033[41m \033[0m\033[41m \033[0m\033[41m \033[0m\033[41m "
    "\033[0m\033[41m \033[0m\033[41m \033[0m\033[41m \033[0m\033[41m "
    "\033[0m\033[41m \033[0m\033[41m \033[0m\033[41m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\n",
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[41m \033[0m\033[41m \033[0m\033[41m "
    "\033[0m\033[41m \033[0m\033[41m \033[0m\033[41m \033[0m\033[41m "
    "\033[0m\033[41m \033[0m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\033[37m "
    "\033[0m\033[37m \033[0m\033[37m \033[0m\033[37m \033[0m\n",
    NULL};

#endif
