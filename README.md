<h1 align="center">
<img src="./logo/logo_transparent.png" width="50%" align="center" />
</h1>

<h1 align="center">
   <b>🌶️<br />FlavorLang</b>
   <br />
   <i>"Cook 'n' Code"</i>
</h1>

<p align="center">
FlavorLang blends coding with culinary creativity! Write programs like recipes &mdash; add ingredients, cook, and serve ideas with unique keywords that make coding as fun and flavorful as your imagination.
</p>

<br />

---

> [!IMPORTANT]
> FlavorLang is currently in **Alpha**. While core functionality is largely stable, new features, syntax, and language designs are subject to change without prior notice.
>
> Please feel free to contribute by reporting any issues or suggestions in the repository's [Issues](https://github.com/KennyOliver/FlavorLang/issues/).

---

## 📖 Table of Contents

1. [🌶️ Why FlavorLang?](#why-flavorlang)

2. [⚡ Quick Start](#quick-start)

3. [🚀 Execution Flags & Behaviors](#execution-flags--behaviors)

4. [Syntax Keywords](#syntax-keywords)

5. [Data Types](#data-types)

6. [Syntax Examples](docs/syntax_examples.md)

7. [Extended Backus-Naur Form (EBNF)](#extended-backus-naur-form-ebnf-of-flavorlang-syntax)

8. [Debugging](#debugging)

   - [Overview](#debugging-overview)
   - [Example Script](#debugging-example-script)
   - [Debug Output Breakdown](#debugging-debug-output-breakdown)
   - [Output With and Without Debugging](#debugging-output-with-and-without-debugging)

9. [VS Code Syntax Highlighter Extension](#vs-code-syntax-highlighter-extension)

10. [Lexer](docs/lexer.md)

11. [Parser](docs/parser.md)

12. [Interpreter](docs/interpreter.md)

---

## 🌶️ Why FlavorLang? <a id="why-flavorlang"></a>

- **Unique & Fun**: Express your programs like recipes!
- **Flexible Execution**: File extensions and flags allow customized program behavior.
- **Readable Syntax**: Keywords like add, mix, cook, and deliver make code approachable and enjoyable.
- **Debug-Friendly**: Easily trace and test your code step-by-step with `--chef` mode.

---

## ⚡ Quick Start <a id="quick-start"></a>

**Get up and running with FlavorLang in just a few steps!**

### 1. Install the Compiler

```bash
$ git clone https://github.com/KennyOliver/FlavorLang.git
$ cd src
$ make
```

### 2. Write Your First Recipe

```py
# recipe.flv

serve("Welcome to FlavorLang!");
```

### 3. Run the Program

```
$ ./flavor recipe.flv

Welcome to FlavorLang!
```

### 4. Debug Your Code

Enable debug mode to inspect tokens and execution flow.

```bash
$ ./flavor recipe.flv --debug
```

This will print detailed information about the tokenization and parsing process.

---

## 🚀 Execution Flags & Behaviors <a id="execution-flags--behaviors"></a>

```bash
$ ./flavor recipe.flv          # Default execution
$ ./flavor recipe.flv --debug  # Debug mode
```

The `--debug` flag is really useful for understanding how FlavorLang is executing (tokenizing, parsing, and interpreting) your file.

---

## Syntax Keywords

<details>
<summary><b>Syntax Keywords table</b></summary>

| Keyword   | Usage                        | Description                                                                                 | Implemented? |
| --------- | ---------------------------- | ------------------------------------------------------------------------------------------- | ------------ |
| `let`     | Define variables             | Declares and initializes variables.                                                         | ✅           |
| `if`      | Conditional logic            | Executes code only if a condition is true.                                                  | ✅           |
| `elif`    | Conditional logic fallback   | Executes only if a prior `if` condition is false.                                           | ✅           |
| `else`    | Conditional fallback         | Executes code if any prior `if`/`is` conditions are false.                                  | ✅           |
| `for`     | For-loop                     | Iterates over a range or sequence, executing a block of code for each step.                 | ✅           |
| `in`      | Range declaration            | Specifies the range or sequence to iterate over.                                            | ✅           |
| `by`      | Optional step specifier      | Defines the step interval for iteration; defaults to `1`/`-1` (range dependent) if omitted. | ✅           |
| `while`   | While-loop                   | Repeatedly runs code while a condition is true.                                             | ✅           |
| `check`   | Switch-case equivalent       | Matches a value to multiple cases.                                                          | ✅           |
| `is`      | Case clause                  | Defines a case inside `check`.                                                              | ✅           |
| `break`   | Exit control flow            | Stops execution of further cases in `check` and exits the current flow.                     | ✅           |
| `create`  | Define a function            | createares a reusable block of logic.                                                       | ✅           |
| `deliver` | Return statement             | Returns a value and stops function execution.                                               | ✅           |
| `try`     | Try block                    | Executes code that might fail.                                                              | ❌           |
| `crumbs`  | Catch block                  | Handles errors during execution.                                                            | ❌           |
| `burn`    | Force exit or raise an error | Stops execution immediately with a message.                                                 | ✅           |
| `serve`   | Print or output              | Outputs a value or message immediately.                                                     | ✅           |
| `sample`  | Input from console           | Reads user input.                                                                           | ✅           |
| `plate`   | Write to file                | Writes data to a file.                                                                      | ✅           |
| `garnish` | Append to file               | Appends data to a file.                                                                     | ✅           |
| `taste`   | Read from file               | Reads data from a file.                                                                     | ✅           |
| `recipe`  | Import `.flv` file           | Imports logic from another `.flv` file.                                                     | ❌           |

</details>

---

## Data Types

| Data Type | Syntax Example    | Capacity/Range                                                                                     |
| --------- | ----------------- | -------------------------------------------------------------------------------------------------- |
| `string`  | `"Hello, world!"` | Size depends on system memory and encoding (e.g., UTF-8). Null-terminated, variable length.        |
| `float`   | `3.14`            | `FLOAT_SIZE` (long double, 64-bit to 128-bit): Platform-dependent, typically up to ±1.1E±4932.     |
| `integer` | `42`              | `INT_SIZE` (long long int, 64-bit): −9,223,372,036,854,775,808 to 9,223,372,036,854,775,807.       |
| `boolean` | `True` / `False`  | `1` for `True`, `0` for `False`. Typically stored as 1 byte, though this can vary by architecture. |

### Explanation

- **string**: In C, the language used to make FlavorLang, strings are null-terminated arrays of characters. The length is limited by system memory and encoding. For example, in UTF-8 encoding, a string can take varying amounts of space per character depending on the character set.
- **float**: The `FLOAT_SIZE` type is a `long double`, offering a precision and range larger than the standard `double`. Its range is platform-dependent but typically up to ±1.1E±4932 on 128-bit implementations.
- **integer**: The `INT_SIZE` type is a `long long int` with a range from −9,223,372,036,854,775,808 to 9,223,372,036,854,775,807, suitable for large integer values.
- **boolean**: Booleans are typically stored as `1` (`True`) and `0` (`False`). While logically they are 1-bit, they are typically stored in 1 byte for practical reasons.

---

## Extended Backus-Naur Form (EBNF) of FlavorLang Syntax <a id="extended-backus-naur-form-ebnf-of-flavorlang-syntax"></a>

```ebnf
program              ::= statement* ;

statement            ::= variable_declaration
                       | print_statement
                       | if_statement
                       | loop_statement
                       | function_definition
                       | error_handling
                       | file_operation
                       | switch_case
                       | user_input
                       | raise_error ;

variable_declaration ::= "let" IDENTIFIER "=" expression ";" ;

print_statement      ::= "serve" expression ("," expression)* ";"   // Multiple arguments require commas
                       | "serve" expression ;  // Single argument allows no brackets

if_statement         ::= "if" condition block
                       ("elif" condition block)*
                       ("else" block)? ;

loop_statement       ::= "while" condition block
                       | "for" IDENTIFIER "in" range [ "by" step ] block ;

function_definition  ::= "create" IDENTIFIER "with" parameter_list block ;

error_handling       ::= "try" block "rescue" block ;

file_operation       ::= "plate" STRING "with" expression
                       | "garnish" STRING "with" expression
                       | "taste" STRING ;

switch_case          ::= "check" expression block case_clause* [ "else" block ] ;

case_clause          ::= "is" expression block
                       | "is" expression block "break" ;

user_input           ::= "sample" ;

raise_error          ::= "burn" expression ("," expression)* ";" ;

block                ::= "{" statement+ "}" ;

condition            ::= expression comparison_operator expression ;

expression           ::= NUMBER | STRING | IDENTIFIER | boolean | "(" expression math_operator expression ")" ;

boolean              ::= "True" | "False" ;

comparison_operator  ::= "==" | "!=" | "<" | "<=" | ">" | ">=" ;

math_operator        ::= "+" | "-" | "*" | "/" ;

parameter_list       ::= IDENTIFIER ("," IDENTIFIER)* ;

range                ::= expression ".." expression
                       | expression "..=" expression ;

step                 ::= expression ;
```

---

## Debugging

### Overview <a id="debugging-overview"></a>

In this section, `test3.flv` is used as an example to demonstrate how the --debug flag works in the Flavor interpreter. The --debug flag provides step-by-step insights into the tokenization, parsing, and execution of the script. This helps developers debug their code and understand the internal processing of statements like `if`, `elif`, `else`, and `serve`.

### Example Script <a id="debugging-example-script"></a>

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

In this case, `test3.flv` will be executed with the --debug flag to illustrate how the interpreter tokenizes, parses, and executes the script step by step.

### Debug Output Breakdown <a id="debugging-debug-output-breakdown"></a>

This detailed output helps track variable values, condition evaluations, and the flow of execution, which can be extremely useful for understanding how the interpreter processes and executes your script.

#### 1. Tokenization

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

#### 2. Parsing

The parser constructs a logical structure (AST) from the tokens, identifying blocks and conditions.

##### Example

```
[DEBUG PRS] Starting to parse block
[DEBUG PRS] Parsing token in block: type=`0`, lexeme=`if`
[DEBUG PRS] Parsing token in block: type=`0`, lexeme=`serve`
```

Logs indicate the parsing of each token into structured blocks for execution.

#### 3. Interpretation

The interpreter executes the script step by step:

- Assigns values to variables.
- Evaluates conditional statements.
- Executes corresponding branches.

##### Example

```
[DEBUG INT] Matched: `AST_CONDITIONAL`
[DEBUG INT] `interpret_conditional()` called
[DEBUG INT] Evaluating `if`/`elif` branch
[DEBUG INT] Binary operation `200.000000 > 180.000000`
[DEBUG INT] Condition evaluated to: `1.000000`
[DEBUG INT] Condition is true, executing branch body
```

#### `serve` in Debug Mode

The `serve` statement is executed as part of the conditional blocks:

##### 1. Tokenization

```
[DEBUG TOK] 4     Type: `0`  Lex: `serve`
[DEBUG TOK]       Type: `3`  Lex: `The oven is hot!`
```

##### 2. Parsing

```
[DEBUG PRS] Parsing token in block: type=`0`, lexeme=`serve`
```

##### 3. Interpretation

This:

```
[DEBUG INT] Matched: `AST_PRINT`
[DEBUG INT] `interpret_print()`
```

Displays this message:

```
The oven is hot!
```

### Output With and Without Debugging <a id="debugging-output-with-and-without-debugging"></a>

#### With Debugging (--debug)

Full debug logs for each stage are printed.

```bash
$ ./flavor tests/test3.flv --debug
[DEBUG TOK] ...
[DEBUG INT] ...
The oven is hot!
```

#### Without Debugging

Only the final output of the script is served.

```bash
$ ./flavor tests/test3.flv
The oven is hot!
```

#### Running with Debugging

Run this to enable debugging for `test3.flv`.

```bash
$ ./flavor tests/test3.flv --debug
```

---

## VS Code Syntax Highlighter Extension

## Install the VS Code Syntax Highlighter for FlavorLang

### 1. Build the Extension

- Navigate to the `vscode-extension` folder and install dependencies:

  ```bash
  cd vscode-extension
  npm install
  ```

### 2. Package the Extension

- Use vsce (Visual Studio Code Extension Manager) to create the `.vsix` package:

  ```bash
  npx vsce package
  ```

This generates a `.vsix` file in the vscode-extension folder.

### 3. Install in VS Code

- Open VS Code.
- Press <kbd>Ctrl</kbd>+<kbd>Shift</kbd>+<kbd>P</kbd> (or <kbd>Cmd</kbd>+<kbd>Shift</kbd>+<kbd>P</kbd> on Mac) and select _`Extensions: Install from VSIX…`_.
- Select the generated `.vsix` file.

Open a `.flv` file in VS Code.
You should see custom syntax highlighting for FlavorLang!

---

## License

This project is licensed under the Apache 2.0 License &mdash; see the [LICENSE](./LICENSE) file for details.

&copy; 2024-2025 Kenneth Oliver. All rights reserved.
