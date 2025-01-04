# Language Design

This `docs/` page details the core design of FlavorLang's syntax, the various data types, and the EBNF of the language.

---

## Table of Contents

1. [Syntax Keywords](#syntax-keywords)

2. [Data Types](#data-types)

3. [Extended Backus-Naur Form (EBNF)](#ebnf)

---

## Syntax Keywords

| Keyword   | Usage                        | Description                                                                                 | Implemented? |
| --------- | ---------------------------- | ------------------------------------------------------------------------------------------- | ------------ |
| `let`     | Define variables             | Declares and initializes variables.                                                         | ✅           |
| `const`   | Define constants             | Declares and initializes constants.                                                         | ✅           |
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
| `create`  | Define a function            | Creates a reusable block of logic.                                                          | ✅           |
| `deliver` | Return statement             | Returns a value and stops function execution.                                               | ✅           |
| `try`     | Try block                    | Executes code that might fail.                                                              | ✅           |
| `rescue`  | Catch block                  | Handles errors during execution.                                                            | ✅           |
| `finish`  | Finally block                | Optional cleanup & always runs.                                                             | ✅           |
| `burn`    | Force exit or raise an error | Stops execution immediately with a message.                                                 | ✅           |
| `serve`   | Print or output              | Outputs a value or message immediately.                                                     | ✅           |
| `sample`  | Input from console           | Reads user input.                                                                           | ✅           |
| `plate`   | Write to file                | Writes data to a file.                                                                      | ✅           |
| `garnish` | Append to file               | Appends data to a file.                                                                     | ✅           |
| `taste`   | Read from file               | Reads data from a file.                                                                     | ✅           |

---

## Data Types

| Data Type | Capacity/Range                                                 |
| --------- | -------------------------------------------------------------- |
| `string`  | Dependent on system memory and encoding (e.g., UTF-8, UTF-16). |
| `float`   | 64-bit: `±1.7e±308` (double); 128-bit: `±1.1e±4932` (quad).    |
| `integer` | 64-bit: `±9e18` (quintillion).                                 |
| `boolean` | `1` for `True`, `0` for `False`.                               |

### Explanation

- **string**:

  In C, the language used to make FlavorLang, strings are null-terminated arrays of characters. The length is limited by system memory and encoding. For example, in UTF-8 encoding, a string can take varying amounts of space per character depending on the character set.

- **float**:

  The `FLOAT_SIZE` type is a `long double`, offering a precision and range larger than the standard `double`. Its range is platform-dependent but typically up to `±1.1e±4932` on 128-bit implementations.

- **integer**:

  The `INT_SIZE` type is a `long long int` with a range from `−9,223,372,036,854,775,808` to `9,223,372,036,854,775,807`, suitable for large integer values.

- **boolean**:

  Booleans are typically stored as `1` (`True`) and `0` (`False`). While logically they are 1-bit, they are typically stored in 1 byte for practical reasons.

---

## Extended Backus-Naur Form (EBNF) of FlavorLang's Syntax <a id="ebnf"></a>

```ebnf
program              ::= statement* ;

statement            ::= variable_declaration
                       | constant_declaration
                       | print_statement
                       | if_statement
                       | loop_statement
                       | function_definition
                       | function_call
                       | error_handling
                       | file_operation
                       | switch_case
                       | user_input
                       | random_statement
                       | type_casting
                       | return_statement
                       | raise_error
                       | array_operation
                       | comment ;

variable_declaration ::= "let" IDENTIFIER "=" expression ";" ;
constant_declaration ::= "const" IDENTIFIER "=" expression ";" ;

print_statement      ::= "serve" expression ("," expression)* ";" ;

if_statement         ::= "if" condition block
                       ("elif" condition block)*
                       ("else" block)? ;

loop_statement       ::= "while" condition block
                       | "for" IDENTIFIER "in" range [ "by" step ] block
                       | "for" "_" "in" range block ;

function_definition  ::= "create" IDENTIFIER parameter_list block ;

function_call        ::= IDENTIFIER "(" [expression ("," expression)*] ")" ;

error_handling       ::= "try" block "rescue" block ;

file_operation       ::= "plate_file" "(" STRING "," expression ")" ";"
                       | "garnish_file" "(" STRING "," expression ")" ";"
                       | "taste_file" "(" STRING ")" ";" ;

switch_case          ::= "check" expression "{" case_clause* [ "else" ":" block ] "}" ;

case_clause          ::= "is" expression ":" block
                       | "is" expression (":" block "break" ";")? ;

user_input           ::= "sample" "(" ")" ";" ;

random_statement     ::= "random" "(" [expression ["," expression]] ")" ";" ;

type_casting         ::= "string" "(" expression ")" ";"
                       | "int" "(" expression ")" ";"
                       | "float" "(" expression ")" ";" ;

return_statement     ::= "deliver" expression ";" ;

raise_error          ::= "burn" expression ("," expression)* ";" ;

array_operation      ::= IDENTIFIER "[" expression "]"                # Access
                       | IDENTIFIER "[" range "]"                     # Slicing
                       | IDENTIFIER "." function_call                 # Array methods
                       | "length" "(" IDENTIFIER ")"                  # Length
                       | "index_of" "(" IDENTIFIER "," expression ")" # Index lookup
                       | "contains" "(" IDENTIFIER "," expression ")" # Containment
                       | "append" "(" IDENTIFIER "," expression ")"   # Append
                       | "prepend" "(" IDENTIFIER "," expression ")"  # Prepend
                       | "insert" "(" IDENTIFIER "," expression "," expression ")" # Insert
                       | IDENTIFIER "+" IDENTIFIER ";" ;              # Concatenation

block                ::= "{" statement+ "}" ;

condition            ::= expression comparison_operator expression ;

expression           ::= NUMBER
                       | STRING
                       | IDENTIFIER
                       | boolean
                       | array_expression
                       | math_expression
                       | function_call
                       | random_statement ;

array_expression     ::= "[" [expression ("," expression)*] "]" ;

math_expression      ::= expression math_operator expression ;

boolean              ::= "True" | "False" ;

comparison_operator  ::= "==" | "!=" | "<" | "<=" | ">" | ">=" ;

math_operator        ::= "+" | "-" | "*" | "**" | "/" | "//" | "%" ;

logical_operator     ::= "&&" | "||" ;

range_operator       ::= ".." | "..=" ;

assignment_operator  ::= "=" ;

parameter_list       ::= "(" [IDENTIFIER ("," IDENTIFIER)*] ")" ;

range                ::= expression range_operator expression ;

step                 ::= expression ;

comment              ::= "#" .* ;
```

---

## License

This project is licensed under the Apache 2.0 License &mdash; see the [LICENSE](../LICENSE) file for details.

&copy; 2024-2025 Kenneth Oliver. All rights reserved.
