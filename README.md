# 🌶️ **FlavorLang** &mdash; _Cook 'n' Code_

**FlavorLang** is a language designed around cooking. From adding ingredients to cooking & serving, you can create programs as flavorful as your imagination. With a unique set of keywords, FlavorLang combines utility, creativity, and fun!

---

## 📖 Table of Contents

1. [Quick Start](#quick-start)

2. [Execution Flags & Behaviors](#execution-flags--behaviors)

3. [Syntax Keywords](#syntax-keywords)

4. [Data Types](#data-types)

5. [Syntax Examples](#syntax-examples)

   - [Hello World](#hello-world)
   - [Defining Variables](#defining-variables)
   - [Conditional Logic](#conditional-logic)
   - [For Loop](#for-loop)
   - [While Loop](#while-loop)
   - [Functions with Return](#functions-with-return)
   - [Error Handling](#error-handling)
   - [File Operations](#file-operations)
   - [Switch-Case Logic](#switch-case-logic)
   - [User Input](#user-input)
   - [Raise an Error](#raise-error)
   - [Use Booleans](#use-booleans)

6. [Extended Backus-Naur Form (EBNF)](#extended-backus-naur-form-ebnf-of-flavorlang-syntax)

7. [Why FlavorLang?](#why-flavorlang)

8. [Debugging](#debugging)

   - [Overview](#debugging-overview)
   - [Example Script](#debugging-example-script)
   - [Debug Output Breakdown](#debugging-debug-output-breakdown)
   - [Output With and Without Debugging](#debugging-output-with-and-without-debugging)

9. [Lexer](#lexer)

   - [Overview](#lexer-overview)
   - [How the Lexer Works](#how-the-lexer-works)
   - [Debugging Tokens](#debugging-tokens)
   - [Error Handling](#lexer-error-handling)
   - [Future Enhancements](#future-enhancements)

10. [Parser](#parser)

    - [Key Structures](#key-structures)
    - [Main Parsing Functions](#main-parsing-functions)
    - [Supporting Functions](#supporting-functions)
    - [Error Handling](#error-handling)
    - [Workflow Example](#workflow-example)

11. [Interpreter](#interpreter)

    - [Main Interpreter Functions](#Main-Interpreter-Functions)
    - [Summary of Steps](#Summary-of-Steps)
    - [Example Execution Flow](#Example-Execution-Flow)

---

## ⚡ Quick Start <a id="quick-start"></a>

**Get up and running with FlavorLang in just a few steps!**

### 1. Install the Compiler

Clone the repository and build the project.

```bash
$ git clone https://github.com/KennyOliver/FlavorLang.git
$ cd FlavorLang
$ make
```

### 2. Write Your First Recipe

Create a file named **`recipe.flv`** with the following content.

```py
show "Welcome to FlavorLang!";
```

### 3. Run the Program

Use the flavor command to execute your program.

```bash
$ ./flavor recipe.flv
```

You should see:

```
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

| Keyword   | Usage                        | Description                                                                                 | Implemented? |
| --------- | ---------------------------- | ------------------------------------------------------------------------------------------- | ------------ |
| `let`     | Define variables             | Declares and initializes variables.                                                         | ✅           |
| `if`      | Conditional logic            | Executes code only if a condition is true.                                                  | ✅           |
| `elif`    | Conditional logic fallback   | Executes only if a prior `if` condition is false.                                           | ✅           |
| `else`    | Conditional fallback         | Executes code if any prior `if`/`is` conditions are false.                                  | ✅           |
| `for`     | For-loop                     | Iterates over a range or sequence, executing a block of code for each step.                 | ❌           |
| `in`      | Range declaration            | Specifies the range or sequence to iterate over.                                            | ❌           |
| `by`      | Optional step specifier      | Defines the step interval for iteration; defaults to `1`/`-1` (range dependent) if omitted. | ❌           |
| `while`   | While-loop                   | Repeatedly runs code while a condition is true.                                             | ✅           |
| `check`   | Switch-case equivalent       | Matches a value to multiple cases.                                                          | ✅           |
| `is`      | Case clause                  | Defines a case inside `check`.                                                              | ✅           |
| `break`   | Exit control flow            | Stops execution of further cases in `check` and exits the current flow.                     | ✅           |
| `create`  | Define a function            | createares a reusable block of logic.                                                       | ✅           |
| `deliver` | Return statement             | Returns a value and stops function execution.                                               | ✅           |
| `try`     | Try block                    | Executes code that might fail.                                                              | ❌           |
| `crumbs`  | Catch block                  | Handles errors during execution.                                                            | ❌           |
| `burn`    | Force exit or raise an error | Stops execution immediately with a message.                                                 | ✅           |
| `scran`   | Print or output              | Outputs a value or message immediately.                                                     | ✅           |
| `taste`   | Input from console           | Reads user input.                                                                           | ✅           |
| `plate`   | Write to file                | Writes data to a file.                                                                      | ❌           |
| `garnish` | Append to file               | Appends data to a file.                                                                     | ❌           |
| `gather`  | Read from file               | Reads data from a file.                                                                     | ❌           |
| `recipe`  | Import `.flv` file           | Imports logic from another `.flv` file.                                                     | ❌           |

---

## Data Types

| Data Type | Syntax Example    | Capacity/Range                                                                                     | Implemented? |
| --------- | ----------------- | -------------------------------------------------------------------------------------------------- | ------------ |
| `string`  | `"Hello, world!"` | Size depends on system memory and encoding (e.g., UTF-8). Null-terminated, variable length.        | ✅           |
| `float`   | `3.14`            | `float` (32-bit): ±3.4E±38. `double` (64-bit): ±1.8E±308.                                          | ✅           |
| `integer` | `42`              | Typically −2,147,483,648 to 2,147,483,647 (32-bit). Larger range for 64-bit.                       | ✅           |
| `boolean` | `True` / `False`  | `1` for `True`, `0` for `False`. Typically stored as 1 byte, though this can vary by architecture. | ✅           |

### Explanation

- **string**: In C, strings are null-terminated arrays of characters. The length is limited by system memory and encoding. For example, in UTF-8 encoding, a string can take varying amounts of space per character depending on the character set.
- **float**: The `float` type is a 32-bit floating-point number, typically with a range of ±3.4E±38. If you're using `double` (64-bit), it has a range of ±1.8E±308. In the `LiteralNode` struct, we are using `double` for floating-point literals, which offers higher precision.
- **integer**: The typical range for an `integer` (32-bit) is from −2,147,483,648 to 2,147,483,647. For larger integers, the range increases with a 64-bit integer type (e.g., ±9.2E18).
- **boolean**: Booleans are typically stored as `1` (`True`) and `0` (`False`). While they are logically 1-bit, they are typically stored in 1 byte, though this can vary based on the system's architecture.

---

## 🍳 Syntax Examples <a id="syntax-examples"></a>

Below are examples showcasing the unique (& fun) syntax of FlavorLang. They give a taste of the cooking-inspired syntax.

<details>
<summary>Expand to see examples</summary>

### 1. 👋 Hello World <a id="hello-world"></a>

The simplest program to print "Hello world!".

```py
show "Hello world!";
```

### 2. 🍲 Defining Variables <a id="defining-variables"></a>

Use `let` to declare and initialize variables.

```py
let name = "Chef";
let age = 25;

show "Name:", name;
show "Age:", age;
```

### 3. 🔄 Conditional Logic <a id="conditional-logic"></a>

Use `if`, `elif`, and `else` to control program flow.

```py
let oven_temperature = 200;

if oven_temperature > 180 {
	show "The oven is hot!";
} elif oven_temperature == 180 {
	show "The oven is just right!";
} else {
	show "The oven is too cold!";
}
```

### 4. 🔁 For Loop <a id="for-loop"></a>

Use `for` to iterate a block of code.

```py
for i in 1..5 {
  show "Mixing... Step", i;
}
```

### 5. 🔄 While Loop <a id="while-loop"></a>

Use `while` for condition-based repetition.

```py
let flour_added = 0;

while flour_added < 3 {
	show "Adding flour...";
	let flour_added = flour_added + 1;
}

show "All flour has been added!";
```

### 6. 📦 Functions with Return <a id="functions-with-return"></a>

Use `create` to define functions and `deliver` to return values.
Note that `burn` **takes precedence** over `deliver`, stopping execution immediately.

```py
create bake_cake(temperature) {
	if temperature < 180 {
		show "Temperature is too low to bake!";
		burn "Cake burned!";  # stops function execution immediately
  } else {
		show "Baking cake at", temperature, "degrees!";
		deliver "Cake is ready!";
  }
}

let result = bake_cake(200);
show result;
```

### 7. 🛠️ Error Handling <a id="error-handling"></a>

Use `try` and `rescue` to handle errors.

```py
try {
	burn "This recipe failed!";
	show "This won't run!";
} rescue {
	show "Caught an error: Recipe needs improvement.";
}
```

### 8. 📄 File Operations <a id="file-operations"></a>

- `Plate`: Write to a file.
- `Garnish`: Append to a file.
- `Gather`: Read from a file.

```py
plate "output.txt" with "Freshly baked cake ready to deliver!";
garnish "output.txt" with "\nDon't forget the toppings!";

let data = gather "output.txt";
show "File Contents:";
show data;
```

### 9. 🔎 Switch-Case Logic <a id="switch-case-logic"></a>

The `check`-`is` syntax in FlavorLang provides an intuitive way to match multiple conditions with beginner-friendly behavior inspired by Python & Go.

- `check` evaluates a value, is matches cases, and else handles unmatched values.
- **Fallthrough**: Cases fall through by default **unless explicitly stopped** with `break`. This ensures clarity and flexibility for beginners.

#### Example

```py
let dessert = "cake";

check dessert {
	is "cake":
		show "Bake the cake!";
	is "pie":
	is "cookie":
		show "Prepare the oven!";
		break;
	else:
		show "Dessert not on the menu.";
}
```

#### Output

```
Bake the cake!
Dessert not on the menu.
```

### 10. 📥 User Input <a id="user-input"></a>

Use `taste` to accept input from the user.

```py
show "What's your favorite dessert?";
let favorite = taste;

show "You chose:", favorite;
```

### 11. ⛔️ Raise an Error <a id="raise-error"></a>

Use `burn` to raise an error and halt execution.

```py
let time = 20;

show "Before error.";

if time > 15 {
	burn "Too late!", "The food got burnt!";
}

show "After error?";
```

### 12. Use Booleans <a id="use-booleans"></a>

Booleans in FlavorLang, `True` and `False`, can be used to create flags, evaluate conditionals, and more.

```py
let stop_loop = False;
let count = 10;

show "Preparing for launch!";

while stop_loop != True {
    show count;

    if count > 0 {
        count = count - 1;
    } else {
        stop_loop = True;
    }
}

show "Blast off!";
```

</details>

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

print_statement      ::= "show" expression ("," expression)* ";"   // Multiple arguments require commas
                       | "show" expression ;  // Single argument allows no brackets

if_statement         ::= "if" condition block
                       ("elif" condition block)*
                       ("else" block)? ;

loop_statement       ::= "while" condition block
                       | "for" IDENTIFIER "in" range [ "by" step ] block ;

function_definition  ::= "create" IDENTIFIER "with" parameter_list block ;

error_handling       ::= "try" block "rescue" block ;

file_operation       ::= "plate" STRING "with" expression
                       | "garnish" STRING "with" expression
                       | "gather" STRING ;

switch_case          ::= "check" expression block case_clause* [ "else" block ] ;

case_clause          ::= "is" expression block
                       | "is" expression block "break" ;

user_input           ::= "taste" ;

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

## Why FlavorLang? <a id="why-flavorlang"></a>

- **Unique & Fun**: Express your programs like recipes!
- **Flexible Execution**: File extensions and flags allow customized program behavior.
- **Readable Syntax**: Keywords like add, mix, cook, and deliver make code approachable and enjoyable.
- **Debug-Friendly**: Easily trace and test your code step-by-step with `--chef` mode.

---

## Debugging

### Overview <a id="debugging-overview"></a>

In this section, `test3.flv` is used as an example to demonstrate how the --debug flag works in the Flavor interpreter. The --debug flag provides step-by-step insights into the tokenization, parsing, and execution of the script. This helps developers debug their code and understand the internal processing of statements like if, elif, else, and show.

### Example Script <a id="debugging-example-script"></a>

```
let oven_temperature = 200;

if oven_temperature > 180 {
  show "The oven is hot!";
} elif oven_temperature == 180 {
  show "The oven is just right!";
} else {
  show "The oven is too cold!";
}
```

- This script assigns a value to the variable oven_temperature and checks its value using conditional statements.
- Based on the condition:
- If the temperature is greater than 180: it shows “The oven is hot!”.
- If the temperature equals 180: it shows “The oven is just right!”.
- Otherwise, it shows “The oven is too cold!”.

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
[DEBUG PRS] Parsing token in block: type=`0`, lexeme=`show`
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

#### `show` in Debug Mode

The show statement is executed as part of the conditional blocks:

##### 1. Tokenization

```
[DEBUG TOK] 4     Type: `0`  Lex: `show`
[DEBUG TOK]       Type: `3`  Lex: `The oven is hot!`
```

##### 2. Parsing

```
[DEBUG PRS] Parsing token in block: type=`0`, lexeme=`show`
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

Only the final output of the script is shown.

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

## Lexer <a id="lexer"></a>

The FlavorLang lexer is responsible for breaking the input source code into fundamental components, called **tokens**. These tokens then serve as the input for the parser, enabling FlavorLang to interpret and execute your code.

### Overview <a id="lexer-overview"></a>

Below are the **core** token types recognized by the lexer:

| Token Type     | Examples                          | Description                                                      |
| -------------- | --------------------------------- | ---------------------------------------------------------------- |
| **KEYWORD**    | `show`, `create`, `deliver`, `if` | Reserved language keywords (e.g., `let`, `while`, `burn`, etc.). |
| **IDENTIFIER** | `cake`, `temperature`             | Names for variables, functions, or parameters.                   |
| **NUMBER**     | `42`, `200`, `3.14`               | Integer or floating-point numeric literals.                      |
| **STRING**     | `"Hello World!"`                  | Text enclosed in double quotes.                                  |
| **OPERATOR**   | `=`, `==`, `+`, `-`, `>=`         | Single or multi-character operators used in expressions.         |
| **DELIMITER**  | `,`, `:`, `(`, `)`, `;`, `{`, `}` | Punctuation symbols that mark statement boundaries or grouping.  |
| **COMMENT**    | `#`                               | Everything from `#` to the end of a line is ignored.             |
| **EOF**        | End of input                      | Signifies that there are no more tokens to read.                 |

### How the Lexer Works

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

#### Tokenizing Key Constructs

- **Comments**: Start at `#` and continue until `\n`. The lexer ignores them entirely.
- **Numbers**: If digits are encountered, they may form either an `INTEGER` or `FLOAT` if a decimal point is found.
- **Strings**: Start and end with `"`. Unterminated strings trigger an error.
- **Identifiers/Keywords**: Any valid identifier start (letter or `_`) followed by letters/digits forms an identifier, and it’s cross-checked against a keywords list to decide if it’s `KEYWORD`.
- **Operators**: Single (`+`, `-`, `=`) or multi-character (`==`, `>=`, `<=`) operators.
- **Delimiters**: For punctuation like `,`, `(`, `)`, `;`, the lexer directly appends a token.

### Debugging Tokens <a id="debugging-tokens"></a>

- Use the `--debug` flag to print all generated tokens (with their types, lexemes, and line numbers). This helps diagnose lexical issues quickly.

#### Example Debug Output

```
[Line 1] Token Type: KEYWORD | Lexeme: let
[Line 1] Token Type: IDENTIFIER | Lexeme: x
[Line 1] Token Type: OPERATOR | Lexeme: =
[Line 1] Token Type: INTEGER | Lexeme: 10
...
```

### Error Handling <a id="lexer-error-handling"></a>

- **Unexpected Characters**: If the lexer encounters an unrecognized symbol, it raises an error and terminates.
- **Unterminated Strings**: If a `"` is opened and never closed, the lexer reports an error with the line number.
- **Memory Allocation Failures**: If resizing or token creation fails, the lexer exits.

### Future Enhancements <a id="future-enhancements"></a>

- **Additional Operators**: E.g., `!=`, `++`, or typed operators if the language evolves.
- **Better Error Recovery**: Instead of immediate exit, attempt to skip malformed input.
- **Performance Tweaks**: For very large .flv files, more efficient reading and token building.

---

## Parser

After the lexer produces tokens, the **parser** converts them into an **Abstract Syntax Tree (AST)** that represents the logical structure of the program.

### Key Structures

- **`ASTNode`**: The building block of the AST, representing statements, expressions, loops, etc.
- **`ParserState`**: Tracks current token index, plus optional flags (e.g. `in_function_body`) for controlling parse flow.
- **`Token`**: The lexical units from the lexer.

### Main Parsing Functions

1. **`parse_program`**

   - Initializes parser state, loops until `TOKEN_EOF`, and delegates each statement to the correct parse function.

2. **`parse_variable_declaration`**

   - Parses `let x = <expression>;`
   - Produces an `AST_ASSIGNMENT` node with `variable_name` and the parsed `value`.

3. **`parse_variable_assignment`**

   - Parses direct assignments like `x = 20;`.

4. **`parse_print_statement`**

   - Reads `show` and then one or more expressions (split by `,`) until a `;`.
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

### Error Handling

- **`parser_error(...)`**: Raises a fatal error if a token is unexpected or a semicolon is missing, etc.
- **`expect_token(...)`**: Ensures the next token is exactly what we want, or raises an error.

### Example

```flv
let x = 10;

if x > 5:
    show "Big";
```

- **Lexer**: Transforms this into tokens:

  [`TOKEN_KEYWORD(let)`, `TOKEN_IDENTIFIER(x)`, `TOKEN_OPERATOR(=)`, `TOKEN_INTEGER(10)`, `TOKEN_DELIMITER(;)`, `TOKEN_KEYWORD(if)`, ...]

- **Parser**: Produces an AST where:
  - `AST_ASSIGNMENT` (`x = 10`)
  - `AST_CONDITIONA` (`if x > 5`) → body: `AST_PRINT("Big")`

---

## Interpreter

The interpreter takes the AST and executes it.
In FlavorLang, this means:

1. Evaluating Expressions
2. Assigning / Retrieving Variables
3. Managing Conditionals & Loops
4. Handling Function Calls
5. Printing & Error Handling

### Main Interpreter Functions

- interpret_node(...): The primary function that returns an `InterpretResult`, containing a LiteralValue plus a did_return flag indicating if a function return (deliver) occurred.
- `interpret_assignment(...)`: Assigns values to variables in the environment.
- `interpret_binary_op(...)`: Applies arithmetic or comparison operators to numeric or string values.
- `interpret_conditional(...)`: Runs `if`/`elif`/`else` logic, short-circuiting if a `deliver` statement is hit.
- `interpret_while_loop(...)`: Evaluates a loop’s condition repeatedly, stopping if `did_return` is set or condition is false.
- `interpret_function_call(...)`: Creates a local environment for function parameters, executes the function body, and handles the final return value.

### Flow Control with InterpretResult

- `interpret_node(...)` always returns an InterpretResult:
- `.value` = The `LiteralValue` result of the node.
- `.did_return` = true if a deliver statement was encountered, letting parent code know to halt further statements.
- This approach ensures something like:

```flv
create factorial(n) {
    if n <= 1:
        deliver 1;
    else:
        deliver n * factorial(n - 1);
}

let result = factorial(3);
```

…stops interpreting once `deliver 1;` is returned in the base case.

### Example Execution Flow

For x = 5 + 3:

1. interpret_node(AST_ASSIGNMENT) → calls interpret_assignment().
2. interpret_node(RHS: AST_BINARY_OP) → calls interpret_binary_op().
3. interpret_node(left=5) returns 5; similarly right=3 returns 3.
4. After 5 + 3 = 8, store x = 8 in the environment.

### Error Handling

- Checks for undefined variables, invalid operator usage, division by zero, etc.
- On error, prints a message and calls exit(1).
