# FlavorLang Language Design Specification

## Overview

FlavorLang is a programming language designed with a cooking-inspired syntax, combining intuitive keywords with robust programming concepts. This document specifies the language's design, syntax, and grammar.

## Language Design Principles

1. **Intuitive Syntax**: Culinary-inspired keywords make the language approachable
2. **Clear Semantics**: Each construct has unambiguous meaning
3. **Predictable Behavior**: Operations produce consistent results
4. **Strong Type System**: Type safety without excessive verbosity
5. **Error Handling**: Comprehensive error management capabilities

## Core Language Elements

### 1. Keywords

| Category                 | Keyword   | Description                    | Example                     |
| ------------------------ | --------- | ------------------------------ | --------------------------- |
| **Variable Declaration** | `let`     | Mutable variable declaration   | `let x = 5;`                |
|                          | `const`   | Immutable constant declaration | `const PI = 3.14;`          |
| **Control Flow**         | `if`      | Conditional execution          | `if condition { ... }`      |
|                          | `elif`    | Alternative condition          | `elif condition { ... }`    |
|                          | `else`    | Default condition              | `else { ... }`              |
|                          | `for`     | Loop iteration                 | `for i in range { ... }`    |
|                          | `while`   | Conditional loop               | `while condition { ... }`   |
|                          | `break`   | Exit loop or switch            | `break;`                    |
| **Pattern Matching**     | `check`   | Pattern matching construct     | `check value { ... }`       |
|                          | `is`      | Pattern case                   | `is pattern:`               |
| **Functions**            | `create`  | Function declaration           | `create func() { ... }`     |
|                          | `deliver` | Return value                   | `deliver result;`           |
| **Error Handling**       | `try`     | Exception handling             | `try { ... }`               |
|                          | `rescue`  | Error catching                 | `rescue { ... }`            |
|                          | `finish`  | Cleanup block                  | `finish { ... }`            |
|                          | `burn`    | Raise error                    | `burn "Error message";`     |
| **I/O Operations**       | `serve`   | Output                         | `serve("message");`         |
|                          | `sample`  | Input                          | `let input = sample();`     |
|                          | `plate`   | File write                     | `plate_file(path, data);`   |
|                          | `garnish` | File append                    | `garnish_file(path, data);` |
|                          | `taste`   | File read                      | `taste_file(path);`         |

### 2. Data Types

| Type      | Description        | Internal Representation | Range/Precision    |
| --------- | ------------------ | ----------------------- | ------------------ |
| `integer` | Whole numbers      | 64-bit signed           | ±9.2e18            |
| `float`   | Decimal numbers    | 64/128-bit              | ±1.7e±308 (64-bit) |
| `string`  | Text sequence      | UTF-8                   | Memory limited     |
| `boolean` | Truth values       | 1 byte                  | `True`/`False`     |
| `array`   | Ordered collection | Dynamic                 | Memory limited     |

### 3. Operators

| Category   | Operators                           | Associativity |
| ---------- | ----------------------------------- | ------------- |
| Arithmetic | `+`, `-`, `*`, `/`, `**`, `//`, `%` | Left to right |
| Comparison | `==`, `!=`, `<`, `<=`, `>`, `>=`    | Left to right |
| Logical    | `&&`, `\|\|`                        | Left to right |
| Range      | `..`, `..=`                         | Left to right |
| Assignment | `=`                                 | Right to left |

### 4. Array Operations

| Operation    | Syntax             | Description            |
| ------------ | ------------------ | ---------------------- |
| Access       | `array[index]`     | Get element at index   |
| Append       | `array[^+]`        | Add to end             |
| Prepend      | `array[+^]`        | Add to start           |
| Remove Last  | `array[^-]`        | Remove from end        |
| Remove First | `array[-^]`        | Remove from start      |
| Slice        | `array[start:end]` | Get subset             |
| Reverse      | `array[::-1]`      | Reverse elements       |
| Step Slice   | `array[::step]`    | Get elements with step |

## Grammar Specification

### Extended Backus-Naur Form (EBNF)

```ebnf
program              ::= statement* ;

statement            ::= declaration
                      | control_flow
                      | function_definition
                      | expression_statement
                      | error_handling ;

declaration          ::= variable_declaration
                      | constant_declaration ;

variable_declaration ::= "let" IDENTIFIER "=" expression ";" ;
constant_declaration ::= "const" IDENTIFIER "=" expression ";" ;

control_flow         ::= if_statement
                      | loop_statement
                      | switch_statement ;

if_statement         ::= "if" expression block
                      ("elif" expression block)*
                      ("else" block)? ;

loop_statement       ::= while_loop | for_loop ;

while_loop           ::= "while" expression block ;

for_loop             ::= "for" IDENTIFIER "in" range block ;

range                ::= expression range_operator expression ;

range_operator       ::= ".." | "..=" ;

function_definition  ::= "create" IDENTIFIER "(" parameter_list ")" block ;

parameter_list       ::= (IDENTIFIER ("," IDENTIFIER)*)? ;

block                ::= "{" statement* "}" ;

expression_statement ::= expression ";" ;

expression           ::= literal
                      | IDENTIFIER
                      | binary_expression
                      | function_call
                      | array_expression ;

literal              ::= NUMBER | STRING | BOOLEAN ;

binary_expression    ::= expression operator expression ;

operator             ::= "+" | "-" | "*" | "/" | "%" | "=="
                      | "!=" | "<" | "<=" | ">" | ">=" ;

function_call        ::= IDENTIFIER "(" argument_list ")" ;

argument_list        ::= (expression ("," expression)*)? ;

array_expression     ::= "[" argument_list "]" ;

error_handling       ::= try_block rescue_block finish_block? ;

try_block           ::= "try" block ;

rescue_block        ::= "rescue" block ;

finish_block        ::= "finish" block ;
```

---

## License

This project is licensed under the Apache 2.0 License &mdash; see the [LICENSE](../LICENSE) file for details.

&copy; 2024-2025 Kenneth Oliver. All rights reserved.
