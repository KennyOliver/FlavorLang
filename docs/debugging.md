# FlavorLang Debugging Guide

## Overview

The FlavorLang interpreter provides comprehensive debugging capabilities through the `--debug` flag. This guide explains how to use debugging features effectively and understand the output at each stage of program execution.

---

## 📖 Table of Contents

1. [Using the Debug Flag](#using-the-debug-flag)
2. [Debugging Stages](#debugging-stages)
   - [Tokenization Stage `[DEBUG TOK]`](#1-tokenization-stage-debug-tok)
   - [Parsing Stage `[DEBUG PRS]`](#2-parsing-stage-debug-prs)
   - [Interpretation Stage `[DEBUG INT]`](#3-interpretation-stage-debug-int)
3. [Example Debug Session](#example-debug-session)
   - [Debug Output Explained](#debug-output-explained)
4. [Common Debug Patterns](#common-debug-patterns)
   - [Variable Assignment](#1-variable-assignment)
   - [Conditional Evaluation](#2-conditional-evaluation)
   - [Function Calls](#3-function-calls)
5. [Debug Output Format](#debug-output-format)
6. [Tips for Effective Debugging](#tips-for-effective-debugging)
   - [Incremental Debugging](#incremental-debugging)
   - [Understanding Token Types](#understanding-token-types)
   - [Following Control Flow](#following-control-flow)
   - [Variable State Tracking](#variable-state-tracking)
7. [Reference](#reference)
   - [Debug Flag Options](#debug-flag-options)
   - [Common Debug Messages](#common-debug-messages)
8. [License](#license)

---

## Using the Debug Flag

To enable debugging, append `--debug` to your FlavorLang command:

```bash
flavor your_script.flv --debug
```

## Debugging Stages

The debugging output is divided into three main stages, each providing different insights into program execution:

### 1. Tokenization Stage `[DEBUG TOK]`

Shows how the source code is broken down into individual tokens:

- Token Type
- Lexeme (actual content)
- Line number

### 2. Parsing Stage `[DEBUG PRS]`

Demonstrates how tokens are organized into an Abstract Syntax Tree (AST):

- Block structure
- Expression parsing
- Statement organization

### 3. Interpretation Stage `[DEBUG INT]`

Shows the actual execution of the code:

- Variable assignments
- Condition evaluations
- Function calls
- Control flow decisions

## Example Debug Session

Let's examine a simple program with debugging enabled:

```js
let temperature = 180;

if temperature >= 170 {
    serve("Ready to bake!");
}
```

### Debug Output Explained

```bash
# Tokenization
[DEBUG TOK] 1     Type: `0`  Lex: `let`           # Variable declaration keyword
[DEBUG TOK]       Type: `1`  Lex: `temperature`   # Variable identifier
[DEBUG TOK]       Type: `4`  Lex: `=`            # Assignment operator
[DEBUG TOK]       Type: `2`  Lex: `180`          # Numeric literal
[DEBUG TOK]       Type: `5`  Lex: `;`            # Statement terminator

# Parsing
[DEBUG PRS] Starting to parse block
[DEBUG PRS] Parsing variable declaration
[DEBUG PRS] Parsing conditional statement

# Interpretation
[DEBUG INT] Assigning value to variable 'temperature'
[DEBUG INT] Evaluating condition: temperature >= 170
[DEBUG INT] Condition result: true
[DEBUG INT] Executing conditional block
Ready to bake!
```

## Common Debug Patterns

### 1. Variable Assignment

```bash
[DEBUG TOK] Type: `0` Lex: `let`
[DEBUG INT] Variable assignment: name='x' value=42
```

### 2. Conditional Evaluation

```bash
[DEBUG INT] Evaluating condition
[DEBUG INT] Left operand: 180
[DEBUG INT] Operator: >=
[DEBUG INT] Right operand: 170
[DEBUG INT] Result: true
```

### 3. Function Calls

```bash
[DEBUG INT] Function call: 'serve'
[DEBUG INT] Arguments: ["Ready to bake!"]
```

## Debug Output Format

Each debug line follows this format:

```
[DEBUG XXX] <line_number> <message>
```

Where:

- `XXX`: Stage identifier (TOK/PRS/INT)
- `line_number`: Source code line number (when applicable)
- `message`: Debug information

## Tips for Effective Debugging

1. **Incremental Debugging**

   - Debug small code sections first
   - Add complexity gradually

2. **Understanding Token Types**

   ```
   0: Keywords
   1: Identifiers
   2: Numbers
   3: Strings
   4: Operators
   5: Punctuation
   ```

3. **Following Control Flow**

   - Watch for `[DEBUG INT]` messages showing condition evaluations
   - Track block entry/exit points

4. **Variable State Tracking**
   - Monitor variable assignments and modifications
   - Check value transformations

## Reference

### Debug Flag Options

- `--debug`: Full debug output
- `--debug-tokens`: Tokenization only (in the future)
- `--debug-parser`: Parsing only (in the future)
- `--debug-interpreter`: Interpretation only (in the future)

### Common Debug Messages

| Message Pattern           | Meaning                          |
| ------------------------- | -------------------------------- |
| `Starting to parse block` | Beginning of a new code block    |
| `Evaluating condition`    | Conditional statement check      |
| `Variable assignment`     | Value being assigned to variable |
| `Function call`           | Function invocation              |
| `Executing block`         | Entering a code block            |

---

## License

This project is licensed under the Apache 2.0 License &mdash; see the [LICENSE](../LICENSE) file for details.

&copy; 2024-2025 Kenneth Oliver. All rights reserved.
