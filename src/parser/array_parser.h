#ifndef ARRAY_PARSER_H
#define ARRAY_PARSER_H

#include "../shared/ast_types.h"
#include "../shared/token_types.h"
#include "parser.h"
#include "parser_state.h"
#include "utils.h"

ASTNode *parse_array_literal(ParserState *state);
ASTNode *parse_index_access(ASTNode *array, ParserState *state);

#endif
