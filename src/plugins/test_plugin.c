#include "fl_plugin.h"
#include <stdio.h>

InterpretResult printSomething(struct ASTNode *node, Environment *env) {
    (void)env;
    // Get the arguments list from the function call AST node
    ASTNode *args = node->function_call.arguments;
    char *msg = "No message";
    if (args != NULL && args->type == AST_LITERAL &&
        args->literal.type == LITERAL_STRING) {
        msg = args->literal.value.string;
    }

    printf("Plugin says: %s\n", msg);

    // Return a default value (0)
    LiteralValue lv = create_default_value();
    return make_result(lv, false, false);
}
