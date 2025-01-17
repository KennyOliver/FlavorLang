#include "fl_plugin.h"
#include <stdio.h>

InterpretResult printSomething(struct ASTNode *node, Environment *env) {
    (void)env; // unused
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

// Function to add two numbers
InterpretResult addNumbers(struct ASTNode *node, Environment *env) {
    (void)env; // unused

    // Extract arguments from the function call node
    ASTNode *args = node->function_call.arguments;

    // Validate the number of arguments
    if (args == NULL || args->next == NULL) {
        fprintf(stderr, "Error: addNumbers requires 2 arguments.\n");
        LiteralValue error_value = create_default_value();
        return make_result(error_value, false, false);
    }

    // Extract and validate the arguments as integers
    INT_SIZE num1 =
        (args->type == AST_LITERAL && args->literal.type == LITERAL_INTEGER)
            ? args->literal.value.integer
            : 0;
    INT_SIZE num2 = (args->next->type == AST_LITERAL &&
                     args->next->literal.type == LITERAL_INTEGER)
                        ? args->next->literal.value.integer
                        : 0;

    INT_SIZE result = num1 + num2;

    printf("Adding " INT_FORMAT " + " INT_FORMAT " = " INT_FORMAT "\n", num1,
           num2, result);

    // Return the result
    LiteralValue result_value;
    result_value.type = LITERAL_INTEGER;
    result_value.data.integer = result;

    return make_result(result_value, false, false);
}
