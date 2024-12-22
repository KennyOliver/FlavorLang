# 🌶️ **FlavorLang** &mdash; _Cook 'n' Code_

**FlavorLang** is a language designed around cooking. From adding ingredients to cooking & serving, you can create programs as flavorful as your imagination. With a unique set of keywords, FlavorLang combines utility, creativity, and fun!

---

## ⚡ Quick Start

**Get up and running with FlavorLang in just a few steps!**

### 1. Install the Compiler

Clone the repository and build the project.

```bash
git clone https://github.com/your-repo/FlavorLang.git
cd FlavorLang
make
```

### 2. Write Your First Recipe

Create a file named **`recipe.flv`** with the following content.

```py
scran "Welcome to FlavorLang!";
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

## 🚀 **Execution Flags & Behaviors**

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
| `prep`    | Define a function            | Prepares a reusable block of logic.                                                         | ❌           |
| `serve`   | Return statement             | Returns a value and stops function execution.                                               | ❌           |
| `try`     | Try block                    | Executes code that might fail.                                                              | ❌           |
| `crumbs`  | Catch block                  | Handles errors during execution.                                                            | ❌           |
| `burn`    | Force exit or raise an error | Stops execution immediately with a message.                                                 | ❌           |
| `scran`   | Print or output              | Outputs a value or message immediately.                                                     | ✅           |
| `taste`   | Input from console           | Reads user input.                                                                           | ❌           |
| `plate`   | Write to file                | Writes data to a file.                                                                      | ❌           |
| `garnish` | Append to file               | Appends data to a file.                                                                     | ❌           |
| `gather`  | Read from file               | Reads data from a file.                                                                     | ❌           |
| `book`    | Import `.flv` file           | Imports logic from another `.flv` file.                                                     | ❌           |

---

## Extended Backus-Naur Form (EBNF) of FlavorLang Syntax

```ebnf
program       ::= statement* ;
statement     ::= variable_declaration | print_statement | if_statement | loop_statement | function_definition ;
variable_declaration ::= "let" IDENTIFIER "=" expression ";" ;
print_statement      ::= "scran" expression ("," expression)* ";" ;
if_statement         ::= "if" condition ":" block ("elif" condition ":" block)* ("else" ":" block)? ;
loop_statement       ::= "while" condition ":" block
                       | "for" IDENTIFIER "in" range [ "by" step ] ":" block ;
function_definition  ::= "prep" IDENTIFIER "with" parameter_list ":" block ;
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

## 🍳 **Syntax Examples**

Below are examples showcasing the unique (& fun) syntax of FlavorLang. They give a taste of the cooking-inspired syntax.

### 1. 👋 **Hello World**

The simplest program to print "Hello world!".

```py
scran "Hello world!";
```

### 2. 🍲 Defining Variables

Use `let` to declare and initialize variables.

```py
let name = "Chef";
let age = 25;

scran "Name:", name;
scran "Age:", age;
```

### 3. 🔄 Conditional Logic

Use `if`, `elif`, and `else` to control program flow.

```py
let oven_temperature = 200;

if oven_temperature > 180:
    scran "The oven is hot!";
elif oven_temperature == 180:
    scran "The oven is just right!";
else:
    scran "The oven is too cold!";
```

### 4. 🔁 For Loop

Use `for` to iterate a block of code.

```py
for i in 1..5:
    scran "Mixing... Step", i;
```

### 5. 🔄 While Loop

Use `while` for condition-based repetition.

```py
let flour_added = 0;

while flour_added < 3:
    scran "Adding flour...";
    let flour_added = flour_added + 1;

scran "All flour has been added!";
```

### 6. 📦 Functions with Return

Use `prep` to define functions and `serve` to return values.
Note that `burn` **takes precedence** over `serve`, stopping execution immediately.

```py
prep bake_cake(temperature):
    if temperature < 180:
        scran "Temperature is too low to bake!";
        burn "Cake burned!";  # Stops function execution immediately
    else:
        scran "Baking cake at", temperature, "degrees!";
        serve "Cake is ready!";

let result = bake_cake(200);
scran result;
```

### 7. 🛠️ Error Handling

Use `try` and `crumbs` to handle errors.

```py
try:
    burn "This recipe failed!";
    scran "This won't run!";
crumbs:
    scran "Caught an error: Recipe needs improvement.";
```

### 8. 📄 File Operations

- `Plate`: Write to a file.
- `Garnish`: Append to a file.
- `Gather`: Read from a file.

```py
plate "output.txt" with "Freshly baked cake ready to serve!";
garnish "output.txt" with "\nDon't forget the toppings!";

let data = gather "output.txt";
scran "File Contents:";
scran data;
```

### 9. 🔎 Switch-Case Logic

Use `when` and `is` for multiple condition matching.

```py
let dessert = "cake";

when dessert:
    is "cake":
        scran "Bake the cake!";
    is "pie":
        scran "Prepare the pie!";
    else:
        scran "Dessert not on the menu.";
```

### 10. 📥 User Input

Use `taste` to accept input from the user.

```py
scran "What's your favorite dessert?";
let favorite = taste;

scran "You chose:", favorite;
```

---

## Why FlavorLang?

- **Unique & Fun**:
  Express your programs like recipes!
- **Flexible Execution**:
  File extensions and flags allow customized program behavior.
- **Readable Syntax**:
  Keywords like add, mix, cook, and serve make code approachable and enjoyable.
- **Debug-Friendly**:
  Easily trace and test your code step-by-step with --chef mode.

---

## Tokenizer

The FlavorLang tokenizer is responsible for breaking down source code into its fundamental components, known as tokens. These tokens serve as the input for the parser and play a crucial role in interpreting and executing FlavorLang programs.

### Tokenizer Overview

| Token Type | Examples                       | Description                                         |
| ---------- | ------------------------------ | --------------------------------------------------- |
| KEYWORD    | `scran`, `prep`, `serve`, `if` | Reserved keywords in the language.                  |
| IDENTIFIER | `cake`, `temperature`          | Variable or function names.                         |
| NUMBER     | `42`, `200`, `3.14`            | Integer or floating-point numbers.                  |
| STRING     | `"Hello World!"`               | String literals.                                    |
| SYMBOL     | `=`, `:`, `,`, `+`, `-`        | Operators, colons, parentheses, etc.                |
| NEWLINE    | `\n`                           | Marks the end of a line.                            |
| WHITESPACE | ` `, `\t`                      | Spaces or tabs (can be ignored).                    |
| COMMENT    | `#`                            | This is a comment Lines starting with # (optional). |
| EOF        | End of input                   | Signals the end of the program.                     |

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

### Debugging Tokens

- The tokenizer includes a debugging flag `--debug` to print all generated tokens.
- Each token’s type, lexeme, and line number are displayed for easier inspection of the tokenization process.

### Example Output:

```
[Line 1] Token Type: 1 | Lexeme: for
[Line 1] Token Type: 2 | Lexeme: x
[Line 1] Token Type: 3 | Lexeme: in
...
```

### Error Handling

- The tokenizer raises errors for:
- Unexpected characters.
- Unterminated string literals.
- Memory allocation failures.
- Each error includes the line number to assist with debugging.

### Future Enhancements

- Support for additional operators and delimiters.
- Improved error recovery to handle malformed inputs gracefully.
- Optimizations for performance with large files.
