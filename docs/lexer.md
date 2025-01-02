# Lexer

The FlavorLang lexer is responsible for breaking the input source code into fundamental components, called **tokens**. These tokens then serve as the input for the parser, enabling FlavorLang to interpret and execute your code.

---

## Table of Contents

- [Overview](#lexer-overview)
- [How the Lexer Works](#how-the-lexer-works)
- [Debugging Tokens](#debugging-tokens)
- [Error Handling](#lexer-error-handling)
- [Future Enhancements](#future-enhancements)

---

## Overview <a id="lexer-overview"></a>

Below are the **core** token types recognized by the lexer:

| Token Type     | Examples                           | Description                                                      |
| -------------- | ---------------------------------- | ---------------------------------------------------------------- |
| **KEYWORD**    | `serve`, `create`, `deliver`, `if` | Reserved language keywords (e.g., `let`, `while`, `burn`, etc.). |
| **IDENTIFIER** | `cake`, `temperature`              | Names for variables, functions, or parameters.                   |
| **NUMBER**     | `42`, `200`, `3.14`                | Integer or floating-point numeric literals.                      |
| **STRING**     | `"Hello World!"`                   | Text enclosed in double quotes.                                  |
| **OPERATOR**   | `=`, `==`, `+`, `-`, `>=`          | Single or multi-character operators used in expressions.         |
| **DELIMITER**  | `,`, `:`, `(`, `)`, `;`, `{`, `}`  | Punctuation symbols that mark statement boundaries or grouping.  |
| **COMMENT**    | `#`                                | Everything from `#` to the end of a line is ignored.             |
| **EOF**        | End of input                       | Signifies that there are no more tokens to read.                 |

## How the Lexer Works

1. **Input & Initialization**

   - The lexer reads the entire file into a string.
   - It creates a token buffer (dynamic array) that grows as needed.

2. **Character Classification**

   - **Whitespace** is skipped, incrementing `line` if `\n`.
   - **Comments** begin with `#` and continue until end of line.
   - **Numbers** are sequences of digits (`0-9`), optionally including one decimal point.
   - **Strings** are enclosed in double quotes (`"`), supporting multiple characters.
   - **Identifiers or Keywords** are alphanumeric sequences that either match a keyword (e.g., `let`) or become `IDENTIFIER`.
   - **Operators** (e.g. `=`, `==`, `>=`) may be single or multi-character.
   - **Delimiters** (e.g. `,`, `;`, `(`) are tokenized in a straightforward manner.

3. **Token Construction**

   - Each recognized piece of text is turned into a token:
     - **Type**: The token kind (keyword, number, operator, etc.).
     - **Lexeme**: The exact substring from the source code.
     - **Line Number**: The line on which the token appears.

4. **End of File**
   - After scanning all characters, the lexer appends a `TOKEN_EOF` to signify there are no more tokens.

### Tokenizing Key Constructs

- **Comments**: Start at `#` and continue until `\n`. The lexer ignores them entirely.
- **Numbers**: If digits are encountered, they may form either an `INTEGER` or `FLOAT` if a decimal point is found.
- **Strings**: Start and end with `"`. Unterminated strings trigger an error.
- **Identifiers/Keywords**: Any valid identifier start (letter or `_`) followed by letters/digits forms an identifier, and it’s cross-checked against a keywords list to decide if it’s `KEYWORD`.
- **Operators**: Single (`+`, `-`, `=`) or multi-character (`==`, `>=`, `<=`) operators.
- **Delimiters**: For punctuation like `,`, `(`, `)`, `;`, the lexer directly appends a token.

## Debugging Tokens <a id="debugging-tokens"></a>

- Use the `--debug` flag to print all generated tokens (with their types, lexemes, and line numbers). This helps diagnose lexical issues quickly.

### Example Debug Output

```
[Line 1] Token Type: KEYWORD | Lexeme: let
[Line 1] Token Type: IDENTIFIER | Lexeme: x
[Line 1] Token Type: OPERATOR | Lexeme: =
[Line 1] Token Type: INTEGER | Lexeme: 10
...
```

## Error Handling <a id="lexer-error-handling"></a>

- **Unexpected Characters**: If the lexer encounters an unrecognized symbol, it raises an error and terminates.
- **Unterminated Strings**: If a `"` is opened and never closed, the lexer reports an error with the line number.
- **Memory Allocation Failures**: If resizing or token creation fails, the lexer exits.

## Future Enhancements <a id="future-enhancements"></a>

- **Additional Operators**: E.g., `!=`, `++`, or typed operators if the language evolves.
- **Better Error Recovery**: Instead of immediate exit, attempt to skip malformed input.
- **Performance Tweaks**: For very large .flv files, more efficient reading and token building.

---

## License

This project is licensed under the Apache 2.0 License &mdash; see the [LICENSE](../LICENSE) file for details.

&copy; 2024-2025 Kenneth Oliver. All rights reserved.
