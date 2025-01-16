#include "fl_plugin.h"
#include <stdio.h>

InterpretResult printSomething(ASTNode *args, Environment *env) {
    const char *msg = "No message";
    if (fl_arg_count(args) > 0) {
        const char *tmp = fl_arg_as_string(args, 0);
        if (tmp) {
            msg = tmp;
        }
    }
    printf("Plugin says: %s\n", msg);

    LiteralValue lv = create_default_value();
    return make_result(lv, 0, 0);
}
