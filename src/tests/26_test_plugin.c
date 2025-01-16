#include "../interpreter/interpreter_types.h"
#include "../interpreter/utils.h"
#include "../shared/ast_types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// A very simple external function. It expects a function call node.
// For example, if the user writes: printSomething("Hello from plugin!")
// then 'args' is the AST node holding the literal "Hello from plugin!".
InterpretResult printSomething(struct ASTNode *args, Environment *env) {
    // (We ignore 'env' here; in a real plugin you might use it.)
    // Extract the first argument as a string.
    // (You should really check types and number of arguments.)
    char *msg = NULL;
    if (args != NULL && args->type == AST_LITERAL &&
        args->literal.type == LITERAL_STRING)
        msg = args->literal.value.string;
    else
        msg = "No message";

    printf("Plugin says: %s\n", msg);

    // Return a default value (for example integer 0)
    LiteralValue lv;
    lv.type = TYPE_INTEGER;
    lv.data.integer = 0;

    return make_result(lv, false, false);
}
