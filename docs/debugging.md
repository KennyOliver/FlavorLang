# Debugging

---

## Table of Contents

- [Overview](#debugging-overview)
- [Example Script](#debugging-example-script)
- [Debug Output Breakdown](#debugging-debug-output-breakdown)
- [Output With and Without Debugging](#debugging-output-with-and-without-debugging)

---

## Overview <a id="debugging-overview"></a>

In this section, `test3.flv` is used as an example to demonstrate how the `--debug` flag works in the Flavor interpreter. The `--debug` flag provides step-by-step insights into the tokenization, parsing, and execution of the script. This helps developers debug their code and understand the internal processing of statements like `if`, `elif`, `else`, and `serve`.

## Example Script <a id="debugging-example-script"></a>

```py
let oven_temperature = 200;

if oven_temperature > 180 {
  serve("The oven is hot!");
} elif oven_temperature == 180 {
  serve("The oven is just right!");
} else {
  serve("The oven is too cold!");
}
```

- This script assigns a value to the variable `oven_temperature` and checks its value using conditional statements.
- Based on the condition:
- If the `temperature` is greater than `180`: it serves `"The oven is hot!"`.
- If the `temperature` equals `180`: it serves `"The oven is just right!"`.
- Otherwise, it serves `"The oven is too cold!"`.

In this case, `test3.flv` will be executed with the `--debug` flag to illustrate how the interpreter tokenizes, parses, and executes the script step by step.

## Debug Output Breakdown <a id="debugging-debug-output-breakdown"></a>

This detailed output helps track variable values, condition evaluations, and the flow of execution, which can be extremely useful for understanding how the interpreter processes and executes your script.

### 1. Tokenization

The lexer splits the script into tokens. Each line in the script generates tokens for variable declarations, conditions, and outputs.

Example:

```
[DEBUG TOK] 1     Type: `0`  Lex: `let`
[DEBUG TOK]       Type: `1`  Lex: `oven_temperature`
[DEBUG TOK]       Type: `4`  Lex: `=`
[DEBUG TOK]       Type: `2`  Lex: `200`
[DEBUG TOK]       Type: `5`  Lex: `;`
```

- **Type**: Describes the kind of token (e.g., keyword, operator, literal).
- **Lex**: The content of the token.

### 2. Parsing

The parser constructs a logical structure (AST) from the tokens, identifying blocks and conditions.

#### Example

```
[DEBUG PRS] Starting to parse block
[DEBUG PRS] Parsing token in block: type=`0`, lexeme=`if`
[DEBUG PRS] Parsing token in block: type=`0`, lexeme=`serve`
```

Logs indicate the parsing of each token into structured blocks for execution.

### 3. Interpretation

The interpreter executes the script step by step:

- Assigns values to variables.
- Evaluates conditional statements.
- Executes corresponding branches.

#### Example

```
[DEBUG INT] Matched: `AST_CONDITIONAL`
[DEBUG INT] `interpret_conditional()` called
[DEBUG INT] Evaluating `if`/`elif` branch
[DEBUG INT] Binary operation `200.000000 > 180.000000`
[DEBUG INT] Condition evaluated to: `1.000000`
[DEBUG INT] Condition is true, executing branch body
```

### `serve` in Debug Mode

The `serve` statement is executed as part of the conditional blocks:

#### 1. Tokenization

```
[DEBUG TOK] 4     Type: `0`  Lex: `serve`
[DEBUG TOK]       Type: `3`  Lex: `The oven is hot!`
```

#### 2. Parsing

```
[DEBUG PRS] Parsing token in block: type=`0`, lexeme=`serve`
```

#### 3. Interpretation

This:

```
[DEBUG INT] Matched: `AST_PRINT`
[DEBUG INT] `interpret_print()`
```

Displays this message:

```
The oven is hot!
```

## Output With and Without Debugging <a id="debugging-output-with-and-without-debugging"></a>

### With Debugging (`--debug`)

Full debug logs for each stage are printed.

```bash
$ ./flavor tests/test3.flv --debug
[DEBUG TOK] ...
[DEBUG INT] ...
The oven is hot!
```

### Without Debugging

Only the final output of the script is served.

```bash
$ ./flavor tests/test3.flv
The oven is hot!
```

### Running with Debugging

Run this to enable debugging for `test3.flv`.

```bash
$ ./flavor tests/test3.flv --debug
```
