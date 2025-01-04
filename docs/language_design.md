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
| `try`     | Try block                    | Executes code that might fail.                                                              | ❌           |
| `crumbs`  | Catch block                  | Handles errors during execution.                                                            | ❌           |
| `burn`    | Force exit or raise an error | Stops execution immediately with a message.                                                 | ✅           |
| `serve`   | Print or output              | Outputs a value or message immediately.                                                     | ✅           |
| `sample`  | Input from console           | Reads user input.                                                                           | ✅           |
| `plate`   | Write to file                | Writes data to a file.                                                                      | ✅           |
| `garnish` | Append to file               | Appends data to a file.                                                                     | ✅           |
| `taste`   | Read from file               | Reads data from a file.                                                                     | ✅           |
| `recipe`  | Import `.flv` file           | Imports logic from another `.flv` file.                                                     | ❌           |

---

## Data Types

| Data Type | Capacity/Range                                                       |
| --------- | -------------------------------------------------------------------- |
| `string`  | Dependent on system memory & encoding (e.g., UTF-8).                 |
| `float`   | (64-bit to 128-bit): Platform-dependent, typically up to ±1.1E±4932. |
| `integer` | (64-bit): ±9e18 (quintillion).                                       |
| `boolean` | `1` for `True`, `0` for `False`. Typically stored as 1 byte.         |

### Explanation

- **string**:

  In C, the language used to make FlavorLang, strings are null-terminated arrays of characters. The length is limited by system memory and encoding. For example, in UTF-8 encoding, a string can take varying amounts of space per character depending on the character set.

- **float**:

  The `FLOAT_SIZE` type is a `long double`, offering a precision and range larger than the standard `double`. Its range is platform-dependent but typically up to ±1.1E±4932 on 128-bit implementations.

- **integer**:

  The `INT_SIZE` type is a `long long int` with a range from −9,223,372,036,854,775,808 to 9,223,372,036,854,775,807, suitable for large integer values.

- **boolean**:

  Booleans are typically stored as `1` (`True`) and `0` (`False`). While logically they are 1-bit, they are typically stored in 1 byte for practical reasons.

---

## Extended Backus-Naur Form (EBNF) of FlavorLang's Syntax <a id="ebnf"></a>

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
                       | random_statement
                       | type_casting
                       | return_statement
                       | raise_error ;

variable_declaration ::= "let" IDENTIFIER "=" expression ";" ;

print_statement      ::= "serve" expression ("," expression)* ";" ;

if_statement         ::= "if" condition block
                       ("elif" condition block)*
                       ("else" block)? ;

loop_statement       ::= "while" condition block
                       | "for" IDENTIFIER "in" range [ "by" step ] block ;

function_definition  ::= "create" IDENTIFIER parameter_list block ;

error_handling       ::= "try" block "rescue" block ;

file_operation       ::= "plate" STRING "," expression ";"
                       | "garnish" STRING "," expression ";"
                       | "taste" STRING ";" ;

switch_case          ::= "check" expression block case_clause* [ "else" block ] ;

case_clause          ::= "is" expression ":" block
                       | "is" expression ":" block "break" ";" ;

user_input           ::= "sample" "(" ")" ";" ;

random_statement     ::= "random" "(" [expression ["," expression]] ")" ;

type_casting         ::= "string" "(" expression ")"
                       | "int" "(" expression ")"
                       | "float" "(" expression ")" ;

return_statement     ::= "deliver" expression ";" ;

raise_error          ::= "burn" expression ("," expression)* ";" ;

block                ::= "{" statement+ "}" ;

condition            ::= expression comparison_operator expression ;

expression           ::= NUMBER
                       | STRING
                       | IDENTIFIER
                       | boolean
                       | math_expression
                       | function_call
                       | random_statement ;

math_expression      ::= "(" expression math_operator expression ")" ;

function_call        ::= IDENTIFIER "(" [expression ("," expression)*] ")" ;

boolean              ::= "True" | "False" ;

comparison_operator  ::= "==" | "!=" | "<" | "<=" | ">" | ">=" ;

math_operator        ::= "+" | "-" | "*" | "**" | "/" | "//" | "%" ;

logical_operator     ::= "&&" | "||" ;

bitwise_operator     ::= "~" ;

assignment_operator  ::= "=" ;

range_operator       ::= ".." | "..=" ;

parameter_list       ::= "(" [IDENTIFIER ("," IDENTIFIER)*] ")" ;

range                ::= expression range_operator expression ;

step                 ::= expression ;
```

---

## License

This project is licensed under the Apache 2.0 License &mdash; see the [LICENSE](../LICENSE) file for details.

&copy; 2024-2025 Kenneth Oliver. All rights reserved.
