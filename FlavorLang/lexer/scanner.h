#ifndef SCANNER_H
#define SCANNERH

#include "../shared/token_types.h"
#include <stdbool.h>

static void handle_comment(const char *source, size_t *pos);
static void handle_number(const char *source, size_t *pos, size_t length,
                          Token **tokens, size_t *token_count, size_t *capacity,
                          int line);
static void handle_string(const char *source, size_t *pos, size_t length,
                          Token **tokens, size_t *token_count, size_t *capacity,
                          int line);
static void handle_identifier_or_keyword(const char *source, size_t *pos,
                                         size_t length, Token **tokens,
                                         size_t *token_count, size_t *capacity,
                                         int line);
static void handle_operator(const char *source, size_t *pos, size_t length,
                            Token **tokens, size_t *token_count,
                            size_t *capacity, int line);

#endif
