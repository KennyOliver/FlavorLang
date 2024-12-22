# 🌶️ **FlavorLang** &mdash; _Cook 'n' Code_

**FlavorLang** is a language designed around cooking. From adding ingredients to cooking & serving, you can create programs as flavorful as your imagination. With a unique set of keywords, FlavorLang combines utility, creativity, and fun!

---

## 📖 Table of Contents

1. [Quick Start](#quick-start)
2. [Execution Flags & Behaviors](#execution-flags--behaviors)
3. [Syntax Keywords](#syntax-keywords)
4. [Syntax Examples](#syntax-examples)
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
5. [Extended Backus-Naur Form (EBNF)](#extended-backus-naur-form-ebnf-of-flavorlang-syntax)
6. [Why FlavorLang?](#why-flavorlang)
7. [Tokenizer](#tokenizer)
   - [Overview](#tokenizer-overview)
   - [How the Tokenizer Works](#how-the-tokenizer-works)
   - [Debugging Tokens](#debugging-tokens)
   - [Error Handling](#tokenizer-error-handling)
   - [Future Enhancements](#future-enhancements)
8. [Parser](#parser)
   - [Key Structures](#key-structures)
   - [Main Parsing Functions](#main-parsing-functions)
   - [Supporting Functions](#supporting-functions)
   - [Error Handling](#error-handling)
   - [Workflow Example](#workflow-example)

---

## ⚡ Quick Start <a id="quick-start"></a>

**Get up and running with FlavorLang in just a few steps!**

### 1. Install the Compiler

Clone the repository and build the project.

```bash
git clone https://github.com/KennyOliver/FlavorLang.git
cd FlavorLang
make
```

### 2. Write Your First Recipe

Create a file named **`recipe.flv`** with the following content.

```py
show "Welcome to FlavorLang!";
```

### 3. Run the Program

Use the flavor command to execute your program.

```bash
./flavor recipe.flv
```

You should see:

```
Welcome to FlavorLang!
```

### 4. Debug Your Code

Enable debug mode to inspect tokens and execution flow.

```bash
./flavor recipe.flv --debug
```

This will print detailed information about the tokenization and parsing process.

---

## 🚀 Execution Flags & Behaviors <a id="execution-flags--behaviors"></a>

```bash
flavor recipe.flv          # Default execution
flavor recipe.flv --debug  # Debug mode
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
| `when`    | Switch-case equivalent       | Matches a value to multiple cases.                                                          | ❌           |
| `is`      | Case clause                  | Defines a case inside `when`.                                                               | ❌           |
| `create`  | Define a function            | createares a reusable block of logic.                                                       | ❌           |
| `deliver` | Return statement             | Returns a value and stops function execution.                                               | ❌           |
| `try`     | Try block                    | Executes code that might fail.                                                              | ❌           |
| `crumbs`  | Catch block                  | Handles errors during execution.                                                            | ❌           |
| `burn`    | Force exit or raise an error | Stops execution immediately with a message.                                                 | ❌           |
| `show`    | Print or output              | Outputs a value or message immediately.                                                     | ✅           |
| `taste`   | Input from console           | Reads user input.                                                                           | ✅           |
| `plate`   | Write to file                | Writes data to a file.                                                                      | ❌           |
| `garnish` | Append to file               | Appends data to a file.                                                                     | ❌           |
| `gather`  | Read from file               | Reads data from a file.                                                                     | ❌           |
| `book`    | Import `.flv` file           | Imports logic from another `.flv` file.                                                     | ❌           |

---

## 🍳 Syntax Examples <a id="syntax-examples"></a>

Below are examples showcasing the unique (& fun) syntax of FlavorLang. They give a taste of the cooking-inspired syntax.

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

if oven_temperature > 180:
    show "The oven is hot!";
elif oven_temperature == 180:
    show "The oven is just right!";
else:
    show "The oven is too cold!";
```

### 4. 🔁 For Loop <a id="for-loop"></a>

Use `for` to iterate a block of code.

```py
for i in 1..5:
    show "Mixing... Step", i;
```

### 5. 🔄 While Loop <a id="while-loop"></a>

Use `while` for condition-based repetition.

```py
let flour_added = 0;

while flour_added < 3:
    show "Adding flour...";
    let flour_added = flour_added + 1;

show "All flour has been added!";
```

### 6. 📦 Functions with Return <a id="functions-with-return"></a>

Use `create` to define functions and `deliver` to return values.
Note that `burn` **takes precedence** over `deliver`, stopping execution immediately.

```py
create bake_cake(temperature):
    if temperature < 180:
        show "Temperature is too low to bake!";
        burn "Cake burned!";  # Stops function execution immediately
    else:
        show "Baking cake at", temperature, "degrees!";
        deliver "Cake is ready!";

let result = bake_cake(200);
show result;
```

### 7. 🛠️ Error Handling <a id="error-handling"></a>

Use `try` and `crumbs` to handle errors.

```py
try:
    burn "This recipe failed!";
    show "This won't run!";
crumbs:
    show "Caught an error: Recipe needs improvement.";
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

Use `when` and `is` for multiple condition matching.

```py
let dessert = "cake";

when dessert:
    is "cake":
        show "Bake the cake!";
    is "pie":
        show "createare the pie!";
    else:
        show "Dessert not on the menu.";
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

if time > 15:
    burn "Too late!", "The food got burnt!";

show "After error?";
```

---

## Extended Backus-Naur Form (EBNF) of FlavorLang Syntax <a id="extended-backus-naur-form-ebnf-of-flavorlang-syntax"></a>

```ebnf
program       ::= statement* ;
statement     ::= variable_declaration | print_statement | if_statement | loop_statement | function_definition ;
variable_declaration ::= "let" IDENTIFIER "=" expression ";" ;
print_statement      ::= "show" expression ("," expression)* ";" ;
if_statement         ::= "if" condition ":" block ("elif" condition ":" block)* ("else" ":" block)? ;
loop_statement       ::= "while" condition ":" block
                       | "for" IDENTIFIER "in" range [ "by" step ] ":" block ;
function_definition  ::= "create" IDENTIFIER "with" parameter_list ":" block ;
block                ::= statement+ ;
condition            ::= expression comparison_operator expression ;
expression           ::= NUMBER | STRING | IDENTIFIER | (expression math_operator expression) ;
comparison_operator  ::= "==" | "!=" | "<" | "<=" | ">" | ">=" ;
math_operator        ::= "+" | "-" | "*" | "/" ;
parameter_list       ::= IDENTIFIER ("," IDENTIFIER)* ;
range                ::= expression ".." expression ;
step                 ::= expression ;
```

---

## Why FlavorLang? <a id="why-flavorlang"></a>

- **Unique & Fun**: Express your programs like recipes!
- **Flexible Execution**: File extensions and flags allow customized program behavior.
- **Readable Syntax**: Keywords like add, mix, cook, and deliver make code approachable and enjoyable.
- **Debug-Friendly**: Easily trace and test your code step-by-step with --chef mode.

---

## Tokenizer <a id="tokenizer"></a>

The FlavorLang tokenizer is responsible for breaking down source code into its fundamental components, known as tokens. These tokens deliver as the input for the parser and play a crucial role in interpreting and executing FlavorLang programs.

### Overview <a id="tokenizer-overview"></a>

| Token Type | Examples                          | Description                                         |
| ---------- | --------------------------------- | --------------------------------------------------- |
| KEYWORD    | `show`, `create`, `deliver`, `if` | Redeliverd keywords in the language.                |
| IDENTIFIER | `cake`, `temperature`             | Variable or function names.                         |
| NUMBER     | `42`, `200`, `3.14`               | Integer or floating-point numbers.                  |
| STRING     | `"Hello World!"`                  | String literals.                                    |
| SYMBOL     | `=`, `:`, `,`, `+`, `-`           | Operators, colons, parentheses, etc.                |
| NEWLINE    | `\n`                              | Marks the end of a line.                            |
| WHITESPACE | ` `, `\t`                         | Spaces or tabs (can be ignored).                    |
| COMMENT    | `#`                               | This is a comment Lines starting with # (optional). |
| EOF        | End of input                      | Signals the end of the program.                     |

### How the Tokenizer Works

#### 1. Input and Initialization

    - The tokenizer reads the entire source code from a file or string input.
    - It initializes a token buffer to store the tokens and processes the input character by character.

#### 2. Character Classification

Each character in the source is categorized into one of the following types:

- **Whitespace**: Skipped, with line numbers updated for newlines.
- **Comments**: Ignored after the `#` symbol until the end of the line.
- **Numbers**: Sequences of digits are recognized as numeric literals.
- **Strings**: Text enclosed in double quotes `"` is identified as a string literal.
- **Identifiers or Keywords**: Alphanumeric sequences starting with a valid character are checked against a keyword list or treated as identifiers.
- **Operators**: Symbols like `=`, `+`, and `>=` are parsed as operators.
- **Delimiters**: Characters such as `,`, `:`, and `(` are directly tokenized.
- **_Unexpected characters trigger a syntax error._**

#### 3. Token Construction

- For each recognized character or sequence, a token is created with the following attributes:
- Type: The kind of token (e.g., TOKEN_NUMBER, TOKEN_IDENTIFIER, TOKEN_OPERATOR).
- Lexeme: The string representation of the token.
- Line Number: The line where the token was encountered.
- Tokens are stored in a dynamically allocated array, which grows as needed.

#### 4. End of File

- After processing all characters, a special `TOKEN_EOF` is appended to signal the end of the input.

### Tokenizing Key Constructs

#### 1. Comments

- Begin with `#` and continue until the end of the line.
- Ignored entirely by the tokenizer.

#### 2. Numbers

- Consist of one or more digits (0-9).
- **Example**: `123` → `TOKEN_NUMBER`

#### 3. Strings

- Enclosed in double quotes `"`and may span multiple characters.
- Unterminated strings result in a syntax error.
- **Example**: `"Hello"` → `TOKEN_STRING`

#### 4. Identifiers and Keywords

- **Identifiers**: Alphanumeric sequences used for variables and function names.
- **Keywords**: Reserved words like `for`, `if`, and `let`.
- The tokenizer determines if a sequence matches a keyword using the is_keyword function.
- **Example**: `for` → `TOKEN_KEYWORD`, `myVar` → `TOKEN_IDENTIFIER`

#### 5. Operators

- Single or multi-character symbols (=, ==, >=).
- Multi-character operators are recognized first to ensure proper tokenization.
- Example: `>=` → `TOKEN_OPERATOR`

#### 6. Delimiters

- Single-character symbols like `,`, `:`, and `(` are tokenized directly.
- Example: `,` → `TOKEN_DELIMITER`

### Debugging Tokens <a id="debugging-tokens"></a>

- The tokenizer includes a debugging flag `--debug` to print all generated tokens.
- Each token’s type, lexeme, and line number are displayed for easier inspection of the tokenization process.

#### Example Output:

```
[Line 1] Token Type: 1 | Lexeme: for
[Line 1] Token Type: 2 | Lexeme: x
[Line 1] Token Type: 3 | Lexeme: in
...
```

### Error Handling <a id="tokenizer-error-handling"></a>

- The tokenizer raises errors for:
- Unexpected characters.
- Unterminated string literals.
- Memory allocation failures.
- Each error includes the line number to assist with debugging.

### Future Enhancements <a id="future-enhancements"></a>

- Support for additional operators and delimiters.
- Improved error recovery to handle malformed inputs gracefully.
- Optimizations for performance with large files.

---

## Parser

The parser converts the tokenized input into an Abstract Syntax Tree (AST), which represents the logical structure of the program.

### Key Structures

- `ASTNode`: Represents a node in the syntax tree.
- `ParserState`: Tracks the current token and handles parsing state.
- `Token`: Represents individual units from the lexer (keywords, operators, literals, etc.).

### Main Parsing Functions

#### 1. `parse_program`

- **Purpose**: Entry point for the parser. Processes the entire token stream.
- **Steps**:
  - Initializes `ParserState`.
  - Iterates over tokens until `TOKEN_EOF` is reached.
  - Based on the token type, invokes specific parsing functions (e.g., `parse_variable_declaration`, `parse_conditional_block`).
  - Links parsed nodes into a linked list to form the AST.

#### 2. `parse_variable_declaration`

- **Purpose**: Parses `let` statements (e.g., `let x = 10;`).
- **Steps**:
  - Ensures the token sequence matches `let <identifier> = <expression>;`.
  - Creates an `AST_ASSIGNMENT` node, storing the variable name and its value.
  - Expects a semicolon at the end of the declaration.

#### 3. `parse_variable_assignment`

- **Purpose**: Parses assignments (e.g., `x = 20;`).
  - **Steps**:
  - Reads the identifier and ensures it’s followed by = and an expression.
  - Creates an `AST_ASSIGNMENT` node linking the variable and its new value.
  - Expects a semicolon at the end of the assignment.

#### 4. `parse_print_statement`

- **Purpose**: Parses `show` (print) statements (e.g., `show "Hello";`).
  - **Steps**:
  - Reads the show keyword and parses arguments until a semicolon.
  - Stores arguments in an `AST_PRINT` node.

#### 5. `parse_expression`

- **Purpose**: Parses mathematical or logical expressions.
- **Steps**:
  - Calls parse_term to handle individual terms (identifiers, literals).
  - Recursively parses binary operators to build a tree structure for expressions.
  - **Output**: An `AST_BINARY_OP` node with left and right subtrees.

#### 6. `parse_conditional_block`

- **Purpose**: Parses `if`, `elif`, and `else` blocks.
- **Steps**:
  - Reads the condition after `if` or `elif` and expects a colon.
  - Parses the block body and recursively handles `elif` or `else`.
  - Creates an `AST_CONDITIONAL` node with condition, body, and optional `else` branch.

#### 7. `parse_while_block`

- **Purpose**: Parses `while` loops.
- **Steps**:
  - Ensures the `while` keyword is followed by a condition and colon.
  - Parses the loop body and stores the condition in an `AST_LOOP` node.

#### 8. `parse_block`

- **Purpose**: Parses a series of statements in a block (e.g., an indented region).
- **Steps**:
  - Reads tokens until an end marker (e.g., `elif`, `else`, `EOF`).
  - Calls appropriate parsing functions for each statement.

### Supporting Functions

- `parse_literal_or_identifier`: Determines if a token is a literal (e.g., number, string) or identifier and creates the appropriate AST node.
- `parse_term`: A wrapper around `parse_literal_or_identifier`.
- `free_ast`: Recursively frees all memory associated with the AST.

### Error Handling

- parser_error: Reports unexpected tokens or syntax violations and terminates parsing.
- expect_token: Ensures the next token matches the expected type or lexeme.

### Workflow Example:

Given the code snippet:

```
let x = 10;

if x > 5:
    show "Big";
```

#### 1. Tokenized Input:

```
[TOKEN_KEYWORD(let), TOKEN_IDENTIFIER(x), TOKEN_OPERATOR(=), TOKEN_NUMBER(10), ...]
```

#### 2. Parser Output (AST):

```
AST_ASSIGNMENT -> AST_CONDITIONAL
-> (Condition: AST_BINARY_OP(x > 5))
-> (Body: AST_PRINT("Big"))
```

---

## Interpreter

The interpreter in FlavorLang is tasked with processing the abstract syntax tree (AST) produced by the parser. Through this process, it systematically evaluates different elements of FlavorLang, including variable assignments, complex expressions, loops, and conditionals.

### 1. Core Interpretation Flow

- `interpret()`function: The main entry point for interpreting an AST node. It recursively processes the node based on its type.
- Each node in the AST represents a different construct in the program (e.g., literals, assignments, binary operations, print statements).
- The interpreter evaluates these nodes and carries out the corresponding operations.

### 2. Key Components

- **Literal Values**:
  - The interpreter supports literal values (e.g., numbers, strings).
  - The `interpret_literal()`function extracts the value and returns a `LiteralValue`that contains the appropriate data (either a number or string).
- **Variables**:

  - The interpreter can read and modify variables.
  - The `interpret_variable()`function retrieves a variable’s value from the environment.
  - The `interpret_assignment()`function handles variable assignment, either updating existing variables or creating new ones in the environment.

- **Binary Operations**:

  - Binary operations (e.g., `+`, `-`, `*`, `/`) are handled by the `interpret_binary_op()`function.
  - The interpreter evaluates both operands and applies the operator to produce the result. Special cases include string concatenation and comparisons (`<`, `<=`, `>`, `>=`, `==`).

- **Print Statements**:

  - The `interpret_print()`function evaluates the arguments of the print statement and outputs them (either literals, variables, or expressions).

- **Conditionals**:

  - The `interpret_conditional()`function processes `if`/`else` branches, evaluating the condition and executing the appropriate body.
  - It continues evaluating `elif` or `else` branches if the condition fails, stopping once a condition evaluates as true.

- **Loops**:
  - The `interpret_while_loop()`function processes while loops by evaluating the loop’s condition and repeatedly executing the loop’s body as long as the condition is true.

### 3. Handling Environment

- The interpreter operates on an `Environment` structure, which stores variables and their values.
- The `get_variable()`function is used to look up a variable’s value in the environment.
- The `init_environment()`function initializes the environment, while `free_environment()`cleans up the allocated resources when done.

### 4. Helper Functions

- The interpreter uses helper functions to ensure safe memory management and smooth execution of the program, such as `create_default_value()`for initializing a zeroed `LiteralValue`, or expanding the environment’s variable storage as necessary.

### 5. Flow Control

- The `interpret_program()`function processes the entire program by iterating over each statement and interpreting it.
- Each type of AST node is handled by a different case in the switch statement in `interpret()`(e.g., `AST_LITERAL`, `AST_ASSIGNMENT`, `AST_BINARY_OP`, etc.).

### 6. Error Handling

- There are multiple checks for errors, such as undefined variables (`get_variable()`), division by zero (`interpret_binary_op()`), and invalid types (`interpret_literal()`).
- If the program encounters an error, it terminates with a descriptive message and exits (`exit(1)`).

---

### Summary of Steps in `interpret()`

1. **Check Node Type**: The `interpret()`function first checks the type of the AST node (e.g., literal, assignment, binary operation).
2. **Evaluate Node**: Depending on the node type, it calls the respective function to handle the logic (e.g., `interpret_literal()`for literals, `interpret_assignment()`for assignments).
3. **Return Value**: After performing the necessary operation (like evaluation or assignment), the function returns a `LiteralValue`representing the result of the operation.
4. **Error Handling**: In case of an unsupported operation or invalid state (e.g., dividing by zero), the interpreter exits with an error message.

---

### Example Execution Flow

Consider the following AST node for a simple assignment.

```
x = 5 + 3
```

The `interpret()` function will:

1. Encounter an `AST_ASSIGNMENT` node.
2. Call `interpret_assignment()` which will evaluate the right-hand side (`5 + 3`).
3. This triggers `interpret_binary_op()` which evaluates the operands `5` and `3`, applies the `+` operator, and returns `8`.
4. Then the interpreter updates the variable x in the environment with the new value (`8`).
