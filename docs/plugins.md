# Creating &amp; Using Plugins in FlavorLang

Plugins extend the capabilities of FlavorLang by allowing developers (like you) to write custom functions in C. This tutorial walks you through creating, compiling, and using plugins in your FlavorLang scripts.

---

# Table of Contents

1. [Writing Your First Plugin](#writing-your-first-plugin)
   - [Plugin API Overview](#plugin-api-overview)
   - [Example Plugin](#example-plugin)
2. [Compiling Your Plugin](#compiling-your-plugin)
   - [Using `--make-plugin`](#using---make-plugin)
   - [Manual Compilation](#manual-compilation)
3. [Using Your Plugin in FlavorLang](#using-your-plugin-in-flavorlang)
   - [Importing the Plugin](#importing-the-plugin)
   - [Calling Plugin Functions](#calling-plugin-functions)
4. [Running the Script](#running-the-script)
5. [Best Practices](#best-practices)
6. [License](#license)

---

## Writing Your First Plugin

### Plugin API Overview

Plugins interact with FlavorLang using the API defined in `fl_plugin.h`. Key components of the API include:

- **InterpretResult:** Returned by plugin functions, containing the result value &amp; metadata.
- **ASTNode:** Represents the Abstract Syntax Tree (AST) node passed to the plugin function.
- **Helper Functions:**
  - `create_default_value()`: Creates a default `LiteralValue`.
  - `make_result()`: Builds an `InterpretResult` from a `LiteralValue`.

### Example Plugin

Here's a plugin with two functions:

1. `printSomething`: Prints a string passed as an argument.
2. `addNumbers`: Adds two integers passed as arguments.

**example_plugin.c**

```c
#include "fl_plugin.h"
#include <stdio.h>

// Function to print a message
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

    // Extract & validate the arguments as integers
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
```

## Compiling Your Plugin

### Using `--make-plugin`

FlavorLang provides a `--make-plugin` flag to simplify plugin compilation:

```bash
$ flavor example_plugin.c --make-plugin
```

This command creates a shared library file (`example_plugin.so` on Linux/macOS or `example_plugin.dll` on Windows &mdash; currently only Linux/macOS is supported).

### Manual Compilation

To compile the plugin manually, use the following commands:

**Linux/macOS**

```bash
gcc -shared -o example_plugin.so -fPIC example_plugin.c
```

**Windows**

```bash
cl /LD example_plugin.c /Fpexample_plugin.dll
```

Ensure fl_plugin.h is in the include path during compilation.

## Using Your Plugin in FlavorLang

### Importing the Plugin

Use the `cimport` function to load your plugin in a FlavorLang script:

```js
cimport("example_plugin.so", "printSomething");
cimport("example_plugin.so", "addNumbers");
```

### Calling Plugin Functions

Once imported, plugin functions can be called like native FlavorLang functions:

**Example Script: `test_plugin.flv`**

```js
cimport("example_plugin.so", "printSomething");
cimport("example_plugin.so", "addNumbers");

printSomething("Hello, Plugin!");

let result = addNumbers(10, 20);
serve("Addition Result:", result);
```

## Running the Script

Execute the script using the `flavor` command:

```bash
$ flavor test_plugin.flv
```

Output:

```
Plugin says: Hello, Plugin!
Adding 10 + 20 = 30
Addition Result: 30
```

## Best Practices

- **Argument Validation:** Always check the number &amp; type of arguments passed to your plugin functions to prevent runtime errors.
- **Cross-Platform Support:** Use platform-specific flags &amp; file extensions when compiling plugins for different operating systems.
- **Error Messaging:** Provide clear &amp; informative error messages when argument validation fails.

---

## License

This project is licensed under the Apache 2.0 License &mdash; see the [LICENSE](../LICENSE) file for details.

&copy; 2024-2025 Kenneth Oliver. All rights reserved.
