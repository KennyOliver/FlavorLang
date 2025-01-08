#include "array_parser.h"

ASTNode *parse_array_literal(ParserState *state) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) {
        parser_error("Memory allocation failed for array literal",
                     get_current_token(state));
    }
    node->type = AST_ARRAY_LITERAL;
    node->array_literal.elements = NULL;
    node->array_literal.count = 0;
    node->next = NULL;

    expect_token(state, TOKEN_SQ_BRACKET_OPEN,
                 "Expected `[` to start array literal");

    // Initialize dynamic array for elements
    size_t capacity = 4; // initial capacity
    node->array_literal.elements = malloc(sizeof(ASTNode *) * capacity);
    if (!node->array_literal.elements) {
        parser_error("Memory allocation failed for array elements",
                     get_current_token(state));
    }

    // Handle empty array
    if (get_current_token(state)->type == TOKEN_SQ_BRACKET_CLOSE) {
        expect_token(state, TOKEN_SQ_BRACKET_CLOSE,
                     "Expected `]` to close array literal");
        return node;
    }

    while (get_current_token(state)->type != TOKEN_SQ_BRACKET_CLOSE) {
        // Parse the next expression
        ASTNode *element = parse_expression(state);
        if (!element) {
            parser_error("Failed to parse array element",
                         get_current_token(state));
        }

        // Add the element to the array
        if (node->array_literal.count >= capacity) {
            capacity *= 2;
            ASTNode **new_elements = realloc(node->array_literal.elements,
                                             sizeof(ASTNode *) * capacity);
            if (!new_elements) {
                parser_error(
                    "Memory allocation failed while resizing array elements",
                    get_current_token(state));
            }
            node->array_literal.elements = new_elements;
        }
        node->array_literal.elements[node->array_literal.count++] = element;

        // Check for comma `,` separator
        if (get_current_token(state)->type == TOKEN_DELIMITER &&
            strcmp(get_current_token(state)->lexeme, ",") == 0) {
            advance_token(state); // consume comma `,`
            // Allow trailing comma before closing bracket
            if (get_current_token(state)->type == TOKEN_SQ_BRACKET_CLOSE) {
                break;
            }
        } else {
            // No more elements
            break;
        }
    }

    expect_token(state, TOKEN_SQ_BRACKET_CLOSE,
                 "Expected `]` to close array literal");

    return node;
}

ASTNode *parse_index_access(ASTNode *array, ParserState *state) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) {
        parser_error("Memory allocation failed for index access",
                     get_current_token(state));
    }

    // Ensure we start with `[`
    expect_token(state, TOKEN_SQ_BRACKET_OPEN,
                 "Expected `[` for array indexing or slicing");

    Token *current = get_current_token(state);

    // Check if it's a slice (contains `:`)
    bool is_slice = false;
    size_t temp_token = state->current_token;

    // Iterate through tokens until `]` to detect a `:` for slicing
    while (state->tokens[temp_token].type != TOKEN_SQ_BRACKET_CLOSE &&
           state->tokens[temp_token].type != TOKEN_EOF) {
        if (state->tokens[temp_token].type == TOKEN_COLON) {
            is_slice = true;
            break;
        }
        temp_token++;
    }

    if (is_slice) {
        // It's a slice
        node->type = AST_ARRAY_SLICE_ACCESS;
        node->array_slice_access.array = array;
        node->array_slice_access.start = NULL;
        node->array_slice_access.end = NULL;
        node->array_slice_access.step = NULL;

        // Parse start expression (optional)
        if (get_current_token(state)->type != TOKEN_COLON) {
            node->array_slice_access.start = parse_expression(state);
        }

        // Expect and consume first `:`
        expect_token(state, TOKEN_COLON, "Expected `:` in slice expression");

        // Parse end expression (optional)
        if (get_current_token(state)->type != TOKEN_COLON &&
            get_current_token(state)->type != TOKEN_SQ_BRACKET_CLOSE) {
            node->array_slice_access.end = parse_expression(state);
        }

        // Check for optional step
        if (get_current_token(state)->type == TOKEN_COLON) {
            advance_token(state); // consume second `:`
            if (get_current_token(state)->type != TOKEN_SQ_BRACKET_CLOSE) {
                node->array_slice_access.step = parse_expression(state);
            } else {
                // Handle cases like `[::]`
                node->array_slice_access.step = NULL;
            }
        }

        // Expect and consume `]`
        expect_token(state, TOKEN_SQ_BRACKET_CLOSE,
                     "Expected `]` to close slice expression");
        node->next = NULL;
    } else if (is_array_operator(current)) {
        // It's an array operator like `^+`, `+^`, `^-`, `-^`
        node->type = AST_ARRAY_OPERATION;
        node->array_operation.operator= strdup(current->lexeme);
        if (!node->array_operation.operator) {
            parser_error("Memory allocation failed for array operator",
                         current);
        }

        node->array_operation.array = array;
        node->next = NULL;
        advance_token(state); // consume the operator

        // Expect and consume `]`
        expect_token(state, TOKEN_SQ_BRACKET_CLOSE,
                     "Expected `]` after array operator");
    } else {
        // It's a simple index
        node->type = AST_ARRAY_INDEX_ACCESS;
        node->array_index_access.array = array;
        node->array_index_access.index = parse_expression(state);

        // Expect and consume `]`
        expect_token(state, TOKEN_SQ_BRACKET_CLOSE,
                     "Expected `]` to close index expression");
        node->next = NULL;
    }

    return node;
}

// Helper function to check if a token is an array operator
bool is_array_operator(Token *token) {
    if (token->type != TOKEN_ARRAY_OP) {
        return false;
    }

    return strcmp(token->lexeme, "^+") == 0 ||
           strcmp(token->lexeme, "+^") == 0 ||
           strcmp(token->lexeme, "^-") == 0 || strcmp(token->lexeme, "-^") == 0;
}
