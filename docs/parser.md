# Parser

After the lexer produces tokens, the **parser** converts them into an **Abstract Syntax Tree (AST)** that represents the logical structure of the program.

---

## Table of Contents

- [Key Structures](#key-structures)
- [Main Parsing Functions](#main-parsing-functions)
- [Error Handling](#error-handling)
- [Workflow Example](#workflow-example)

---

## Key Structures

- **`ASTNode`**: The building block of the AST, representing statements, expressions, loops, etc.
- **`ParserState`**: Tracks current token index, plus optional flags (e.g. `in_function_body`) for controlling parse flow.
- **`Token`**: The lexical units from the lexer.

## Main Parsing Functions

1. **`parse_program`**

   - Initializes parser state, loops until `TOKEN_EOF`, and delegates each statement to the correct parse function.

2. **`parse_variable_declaration`**

   - Parses `let x = <expression>;`
   - Produces an `AST_ASSIGNMENT` node with `variable_name` and the parsed `value`.

3. **`parse_variable_assignment`**

   - Parses direct assignments like `x = 20;`.

4. **`parse_print_statement`**

   - Reads `serve` and then one or more expressions (split by `,`) until a `;`.
   - Produces an `AST_PRINT` node containing arguments.

5. **`parse_expression`**

   - Recursively parses numeric or string expressions (including binary operators).
   - Results in `AST_BINARY_OP` nodes (like `x + 5`).

6. **`parse_conditional_block`**

   - Handles `if`, `elif`, `else`.
   - Creates `AST_CONDITIONAL` with a condition and body, plus chained else branches.

7. **`parse_while_block`**

   - For `while <condition>:`, builds an `AST_LOOP` node referencing the loop body.

8. **`parse_block`**
   - Repeatedly parses statements until a block terminator (like `}` or an `else`) is reached.
   - Builds a linked list of statements.

## Error Handling

- **`parser_error(...)`**: Raises a fatal error if a token is unexpected or a semicolon is missing, etc.
- **`expect_token(...)`**: Ensures the next token is exactly what we want, or raises an error.

## Workflow Example

```flv
let x = 10;

if x > 5 {
   serve("Big");
}
```

- **Lexer**: Transforms this into tokens:

  [`TOKEN_KEYWORD(let)`, `TOKEN_IDENTIFIER(x)`, `TOKEN_OPERATOR(=)`, `TOKEN_INTEGER(10)`, `TOKEN_DELIMITER(;)`, `TOKEN_KEYWORD(if)`, ...]

- **Parser**: Produces an AST where:
  - `AST_ASSIGNMENT` (`x = 10`)
  - `AST_CONDITIONA` (`if x > 5`) → body: `AST_PRINT("Big")`

---

## License

This project is licensed under the Apache 2.0 License &mdash; see the [LICENSE](../LICENSE) file for details.

&copy; 2024-2025 Kenneth Oliver. All rights reserved.
