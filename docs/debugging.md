# FlavorLang Debugging Guide

## Overview

The FlavorLang interpreter provides comprehensive debugging capabilities through the `--debug` flag. This guide explains how to use debugging features effectively and understand the output at each stage of program execution.

---

## Table of Contents

1. [Using the Debug Flag](#using-the-debug-flag)
2. [Debugging Stages](#debugging-stages)
3. [Example Debug Session](#example-debug-session)
4. [Understanding Debug Output](#understanding-debug-output)
5. [Common Debug Patterns](#common-debug-patterns)
6. [Tips for Effective Debugging](#tips-for-effective-debugging)
7. [Reference](#reference)

---

## Using the Debug Flag

To enable debugging, append `--debug` to your FlavorLang command:

```bash
flavor your_script.flv --debug
```

## Debugging Stages

The debugging output is divided into three main stages:

### 1. Tokenization Stage `[DEBUG TOK]`

Shows how source code is broken into tokens. Each token includes:

- Line number
- Token type (numeric ID)
- Lexeme (actual content)

Example:

```
[DEBUG TOK] 1 Type: 0 Lex: `let`
[DEBUG TOK] Type: 1 Lex: `stop_loop`
[DEBUG TOK] Type: 6 Lex: `=`
```

### 2. Parsing Stage `[DEBUG PRS]`

Shows AST (Abstract Syntax Tree) construction:

- Block structure
- Expression parsing
- Statement organization

Example:

```
[DEBUG PRS] Current Token: Type=`0`, Lexeme=`let`
[DEBUG PRS] Starting variable declaration parse
[DEBUG PRS] Starting to parse block
```

### 3. Interpretation Stage `[DEBUG INT]`

Shows program execution:

- Variable operations
- Function calls
- Control flow
- Expression evaluation

Example:

```
[DEBUG INT] Added variable `count` with is_constant=0
[DEBUG INT] Looking up variable: `count`
[DEBUG INT] Variable found: `count` with value `10`
```

## Example Debug Session

Let's examine a countdown program that demonstrates various language features:

```javascript
let stop_loop = False;
let count = 10;
serve("Preparing for launch!");
while stop_loop != True {
    serve(count);
    if count > 0 {
        count = count - 1;
    } else {
        stop_loop = True;
    }
}
serve("Blast off!");
```

Normal output:

```
Preparing for launch!
10
9
8
7
6
5
4
3
2
1
0
Blast off!
```

### Understanding Debug Output

Key debug messages from each stage:

1. **Tokenization**:

```bash
[DEBUG TOK] 1 Type: 0 Lex: `let`              # Variable declaration
[DEBUG TOK] Type: 1 Lex: `stop_loop`          # Variable name
[DEBUG TOK] Type: 6 Lex: `=`                  # Assignment operator
[DEBUG TOK] Type: 2 Lex: `False`              # Boolean literal
```

2. **Parsing**:

```bash
[DEBUG PRS] Starting variable declaration parse
[DEBUG PRS] Starting to parse block
[DEBUG PRS] Parsing token in block: type=`8`, lexeme=`serve`
```

3. **Interpretation**:

```bash
[DEBUG INT] Added variable `stop_loop` with is_constant=0
[DEBUG INT] Variable found: `count` with value `10`
[DEBUG INT] Operator: `>`
```

## Common Debug Patterns

1. **Variable Operations**

```bash
[DEBUG INT] Added variable `count` with is_constant=0
[DEBUG INT] Looking up variable: `count`
[DEBUG INT] Variable found: `count` with value `10`
```

2. **Control Flow**

```bash
[DEBUG INT] Matched: `AST_CONDITIONAL`
[DEBUG INT] `interpret_conditional()` called
[DEBUG INT] Operator: `>`
```

3. **Function Calls**

```bash
[DEBUG INT] Starting function call interpretation
[DEBUG INT] Looking up variable: `serve`
[DEBUG INT] builtin_output() called
```

## Tips for Effective Debugging

1. **Read Token Types**

   - 0: Keywords (`let`, `if`, `while`)
   - 1: Identifiers (variable names)
   - 2: Boolean literals (`True`, `False`)
   - 3: Floating-point number literals
   - 4: Integer number literals
   - 5: String literals
   - 6: Operators (`+`, `-`, `==`, etc.)
   - 7: Delimiters (e.g., `;`, `,`)
   - 8: Function names
   - 9: Function parameter identifiers
   - 10: Open parentheses (`(`)
   - 11: Close parentheses (`)`)
   - 12: Open braces (`{`)
   - 13: Close braces (`}`)
   - 14: Colon (`:`)
   - 15: Array operations (e.g., access)
   - 16: Open square brackets (`[`)
   - 17: Close square brackets (`]`)
   - 18: End of file

2. **Track Variable State**

   - Watch for variable declarations (`Added variable`)
   - Monitor value changes (`Variable found: with value`)
   - Check condition evaluations

3. **Follow Control Flow**
   - Look for `AST_CONDITIONAL` and `AST_WHILE_LOOP`
   - Track block entry/exit
   - Monitor condition evaluations

## Reference

### Token Types

| ID  | Meaning               | Example              |
| --- | --------------------- | -------------------- |
| 0   | Keyword               | `let`, `if`, `while` |
| 1   | Identifier            | Variable names       |
| 2   | Boolean               | `True`, `False`      |
| 3   | Floating-point Number | `3.14`, `2.718`      |
| 4   | Integer Number        | `10`, `42`           |
| 5   | String                | `"Hello"`            |
| 6   | Operator              | `=`, `+`, `-`, `>`   |
| 7   | Delimiter             | `;`, `,`             |
| 8   | Function Name         | `serve`              |
| 9   | Function Parameter    | `x`, `y`             |
| 10  | Open Parenthesis      | `(`                  |
| 11  | Close Parenthesis     | `)`                  |
| 12  | Open Brace            | `{`                  |
| 13  | Close Brace           | `}`                  |
| 14  | Colon                 | `:`                  |
| 15  | Array Operation       | `arr[0]`             |
| 16  | Open Square Bracket   | `[`                  |
| 17  | Close Square Bracket  | `]`                  |
| 18  | End of File           | (None)               |

### Common Debug Messages

| Message Pattern          | Meaning                   |
| ------------------------ | ------------------------- |
| `Added variable`         | New variable created      |
| `Looking up variable`    | Variable access           |
| `Starting function call` | Function invocation       |
| `Operator:`              | Operation being performed |
| `Matched: AST_`          | Node type being processed |

---

## License

This project is licensed under the Apache 2.0 License &mdash; see the [LICENSE](../LICENSE) file for details.

&copy; 2024-2025 Kenneth Oliver. All rights reserved.
