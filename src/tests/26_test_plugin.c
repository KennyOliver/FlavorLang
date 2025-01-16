#include "../interpreter/interpreter_types.h"
#include "../interpreter/utils.h"
#include "../shared/ast_types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

InterpretResult printSomething(struct ASTNode *args, Environment *env) {
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
