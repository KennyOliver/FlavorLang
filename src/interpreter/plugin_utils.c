#include "plugin_utils.h"

/* Returns the number of arguments in the AST linked list. */
int fl_arg_count(ASTNode *args) {
    int count = 0;
    while (args != NULL) {
        count++;
        args = args->next;
    }
    return count;
}

/* Returns the string from the AST literal at index 'index'.
   (Assumes the argument is an AST literal with type LITERAL_STRING.)
*/
const char *fl_arg_as_string(ASTNode *args, int index) {
    int i = 0;
    while (args != NULL && i < index) {
        args = args->next;
        i++;
    }
    if (args != NULL && args->type == AST_LITERAL &&
        args->literal.type == LITERAL_STRING) {
        return args->literal.value.string;
    }
    return NULL;
}

/* Create a default literal value. In our case, we use an integer zero. */
FL_LiteralValue fl_create_default_value(void) {
    FL_LiteralValue val;
    memset(&val, 0, sizeof(val));
    val.type = TYPE_INTEGER;
    val.data.integer = 0;
    return val;
}

/* Create an interpreted result from a literal value and status flags. */
FL_InterpretResult fl_make_result(FL_LiteralValue val, int did_return,
                                  int did_break) {
    FL_InterpretResult res;
    res.value = val;
    res.did_return = did_return;
    res.did_break = did_break;
    res.is_error = 0;
    return res;
}
