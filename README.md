# 🌶️ **FlavorLang** &mdash; _Cook 'n' Code_

**FlavorLang** is a language designed around cooking. From adding ingredients to cooking & serving, you can create programs as flavorful as your imagination. With a unique set of keywords, FlavorLang combines utility, creativity, and fun!

---

## 🚀 **Execution Flags & Behaviors**

```bash
flavor recipe.flv          # Default execution
flavor recipe.flv --debug  # Debug mode
```

---

## Syntax Keywords

| Keyword   | Usage                        | Description                                                                                 |
| --------- | ---------------------------- | ------------------------------------------------------------------------------------------- |
| `let`     | Define variables             | Declares and initializes variables.                                                         |
| `if`      | Conditional logic            | Executes code only if a condition is true.                                                  |
| `elif`    | Conditional logic fallback   | Executes only if a prior `if` condition is false.                                           |
| `else`    | Conditional fallback         | Executes code if any prior `if`/`is` conditions are false.                                  |
| `for`     | For-loop                     | Iterates over a range or sequence, executing a block of code for each step.                 |
| `in`      | Range declaration            | Specifies the range or sequence to iterate over.                                            |
| `by`      | Optional step specifier      | Defines the step interval for iteration; defaults to `1`/`-1` (range dependent) if omitted. |
| `while`   | While-loop                   | Repeatedly runs code while a condition is true.                                             |
| `when`    | Switch-case equivalent       | Matches a value to multiple cases.                                                          |
| `is`      | Case clause                  | Defines a case inside `when`.                                                               |
| `prep`    | Define a function            | Prepares a reusable block of logic.                                                         |
| `serve`   | Return statement             | Returns a value and stops function execution.                                               |
| `try`     | Try block                    | Executes code that might fail.                                                              |
| `crumbs`  | Catch block                  | Handles errors during execution.                                                            |
| `burn`    | Force exit or raise an error | Stops execution immediately with a message.                                                 |
| `scran`   | Print or output              | Outputs a value or message immediately.                                                     |
| `taste`   | Input from console           | Reads user input.                                                                           |
| `plate`   | Write to file                | Writes data to a file.                                                                      |
| `garnish` | Append to file               | Appends data to a file.                                                                     |
| `gather`  | Read from file               | Reads data from a file.                                                                     |
| `book`    | Import `.flv` file           | Imports logic from another `.flv` file.                                                     |

---

## Extended Backus-Naur Form (EBNF) of FlavorLang Syntax

```ebnf
program       ::= statement* ;
statement     ::= variable_declaration | print_statement | if_statement | loop_statement | function_definition ;
variable_declaration ::= "let" IDENTIFIER "=" expression ";" ;
print_statement      ::= "scran" expression ("," expression)* ";" ;
if_statement         ::= "if" condition ":" block ("elif" condition ":" block)* ("else" ":" block)? ;
loop_statement       ::= "while" condition ":" block
                       | "for" "let" IDENTIFIER "=" expression "to" expression ":" block ;
function_definition  ::= "prep" IDENTIFIER "with" parameter_list ":" block ;
block                ::= statement+ ;
condition            ::= expression comparison_operator expression ;
expression           ::= NUMBER | STRING | IDENTIFIER | (expression math_operator expression) ;
comparison_operator  ::= "==" | "!=" | "<" | "<=" | ">" | ">=" ;
math_operator        ::= "+" | "-" | "*" | "/" ;
parameter_list       ::= IDENTIFIER ("," IDENTIFIER)* ;
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
for let i = 1 to 5:
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
prep bake_cake with let temperature:
    if temperature < 180:
        scran "Temperature is too low to bake!";
        burn "Cake burned!";  # Stops function execution immediately
    else:
        scran "Baking cake at", temperature, "degrees!";
        serve "Cake is ready!";

let result = bake_cake with 200;
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
