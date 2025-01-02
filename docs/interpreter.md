# Interpreter

## Table of Contents

- [Overview](#overview)
- [Main Interpreter Functions](#main-interpreter-functions)
- [Flow Control with `InterpretResult`](#flow-control)
- [Summary of Steps](#summary-of-steps)
- [Example Execution Flow](#example-execution-flow)
- [Error Handling](#error-handling)

---

## Overview

The interpreter takes the AST and executes it.
In FlavorLang, this means:

1. Evaluating Expressions
2. Assigning / Retrieving Variables
3. Managing Conditionals & Loops
4. Handling Function Calls
5. Printing & Error Handling

## Main Interpreter Functions

- interpret_node(...): The primary function that returns an `InterpretResult`, containing a LiteralValue plus a did_return flag indicating if a function return (deliver) occurred.
- `interpret_assignment(...)`: Assigns values to variables in the environment.
- `interpret_binary_op(...)`: Applies arithmetic or comparison operators to numeric or string values.
- `interpret_conditional(...)`: Runs `if`/`elif`/`else` logic, short-circuiting if a `deliver` statement is hit.
- `interpret_while_loop(...)`: Evaluates a loop’s condition repeatedly, stopping if `did_return` is set or condition is false.
- `interpret_function_call(...)`: Creates a local environment for function parameters, executes the function body, and handles the final return value.

## Flow Control with `InterpretResult` <a id="flow-control"></a>

- `interpret_node(...)` always returns an `InterpretResult`:
- `.value` = The `LiteralValue` result of the node.
- `.did_return` = true if a deliver statement was encountered, letting parent code know to halt further statements.
- This approach ensures something like the following stops interpreting once `deliver 1;` is returned in the base case:

```flv
create factorial(n) {
   if n <= 1 {
      deliver 1;
   } else {
      deliver n * factorial(n - 1);
   }
}

let result = factorial(3);
```

## Example Execution Flow

For `x = 5 + 3`:

1. `interpret_node(AST_VARIABLE)` → calls `interpret_assignment()`.
2. `interpret_node(RHS: AST_BINARY_OP)` → calls `interpret_binary_op()`.
3. `interpret_node(left=5)` returns `5`; similarly `right=3` returns `3`.
4. After `5 + 3 = 8`, store `x = 8` in the environment.

## Error Handling

- Checks for undefined variables, invalid operator usage, division by zero, etc.
- On error, prints a message and calls `exit(1)`.

---

## License

This project is licensed under the Apache 2.0 License &mdash; see the [LICENSE](../LICENSE) file for details.

&copy; 2024-2025 Kenneth Oliver. All rights reserved.
