# FlavorLang Tutorial: A Beginner's Guide

Welcome to FlavorLang! This tutorial will guide you through the fundamentals of this cooking-inspired programming language. We'll start with the basics and gradually move on towards more advanced features.

---

## Table of Contents

1. [Getting Started](#getting-started)
2. [Variables and Constants](#variables-and-constants)
3. [Basic Output and Input](#basic-output-and-input)
4. [Data Types](#data-types)
5. [Control Flow](#control-flow)
6. [Functions](#functions)
7. [Error Handling](#error-handling)
8. [Working with Arrays](#working-with-arrays)
9. [File Operations](#file-operations)
10. [Standard Library Functions](#standard-library-functions)

## Getting Started

FlavorLang is a programming language that uses cooking-inspired syntax to make programming more approachable &amp; fun. Files use the `.flv` extension.

Let's start with the classic "Hello, World!" program:

```js
serve("Hello, world!");
```

## Variables and Constants

### Variables

Use `let` to declare variables that can be modified:

```js
let name = "Chef";
let age = 25;
serve("Hello,", name);  # Output: Hello, Chef
```

### Constants

Use `const` for values that shouldn't change:

```js
const PI = 3.14159;
const RESTAURANT_NAME = "Flavor House";
```

## Basic Output and Input

### Output with `serve`

`serve` is FlavorLang's print function. It has some special features:

```py
serve("Basic output");               # Adds newline by default
serve("No newline", False);          # Doesn't add newline
serve("Multiple", "values", "work"); # Spaces between values
```

### Input with `sample`

Use `sample()` to get user input:

```js
serve("What's your favorite dish?");
let favorite = sample();
serve("Ah,", favorite, "is delicious!");
```

## Data Types

FlavorLang supports these basic data types:

- Strings: `"Hello"`
- Integers: `42`
- Floats: `3.14`
- Booleans: `True`, `False`

Type conversion functions:

```py
let num_str = "123";
let num = int(num_str);      # String to integer
let float_num = float("3.14"); # String to float
let str_num = string(42);    # Number to string
```

## Control Flow

### If Statements

```js
let temperature = 180;

if temperature > 180 {
    serve("Too hot!");
} elif temperature == 180 {
    serve("Perfect temperature!");
} else {
    serve("Too cold!");
}
```

### Loops

#### For Loops

```py
# Count up
for i in 1..5 {
    serve("Step", i);
}

# Count down
for i in 10..=1 by -2 {
    serve(i);  # Counts: 10, 8, 6, 4, 2
}
```

#### While Loops

```js
let ingredients = 3;

while ingredients > 0 {
    serve("Adding ingredient...");
    ingredients = ingredients - 1;
}
```

### Switch-Case (`check`-`is`)

```js
const dish = "pasta";

check dish {
    is "pasta":
        serve("Italian cuisine!");
        break;
    is "sushi":
        serve("Japanese cuisine!");
        break;
    else:
        serve("Unknown cuisine!");
}
```

## Functions

Functions in FlavorLang are created using the `create` keyword and can return values using `deliver`:

```py
create add_ingredients(a, b) {
    deliver a + b;  # Must use deliver for returning values
}

create mix_ingredients(ingredients) {
    serve("Mixing:", ingredients);
    # No deliver = void return (implicit)
}

# Using the functions
let result = add_ingredients(5, 3);
serve(result);  # Output: 8

mix_ingredients("flour and water");
```

## Error Handling

Use `try`, `rescue`, and optionally `finish` for error handling:

```ts
try {
    let number = int("not a number");
    serve("This won't run!");
} rescue {
    serve("Oops! Couldn't convert to number.");
} finish {
    serve("This always runs!");
}
```

Raise errors using `burn`:

```js
create check_temperature(temp) {
    if temp > 200 {
        burn("Temperature too high!", temp);
    }
    serve("Temperature is OK:", temp);
}
```

## Working with Arrays

Arrays in FlavorLang support various operations:

```py
# Creating arrays
let ingredients = ["flour", "sugar", "eggs"];

# Accessing elements
serve(ingredients[0]);  # Output: flour

# Array operations
ingredients[^+] = "milk";    # Append
ingredients[+^] = "butter";  # Prepend
serve(ingredients[-^]);      # Remove and return first element
serve(ingredients[^-]);      # Remove and return last element

# Slicing
serve(ingredients[1:3]);     # Elements from index 1 to 2
serve(ingredients[::-1]);    # Reverse array
serve(ingredients[::2]);     # Every second element

# 2D arrays
let recipe_matrix = [
    ["Cake", "60 min"],
    ["Cookies", "25 min"]
];
serve(recipe_matrix[0][1]);  # Output: 60 min
```

## File Operations

FlavorLang provides three main file operations:

```py
# Writing to a file
plate_file("recipe.txt", "Flour: 2 cups\nSugar: 1 cup");

# Appending to a file
garnish_file("recipe.txt", "\nEggs: 2");

# Reading from a file
let recipe = taste_file("recipe.txt");
serve(recipe);
```

## Standard Library Functions

FlavorLang includes several useful built-in functions:

```py
# Random numbers
let random_float = random();       # 0.0 to 1.0
let random_int = random(10);       # 0 to 10
let random_range = random(5, 10);  # 5 to 10

# Time operations
let timestamp = get_time();        # Unix timestamp

# Array/String length
let len = length("Hello");         # 5

# Delay execution
sleep(1000);                       # Pause for 1 second
```

---

This tutorial covers the main features of FlavorLang. You can now start creating your own programs using these cooking-inspired programming concepts! Remember that like cooking, programming gets better with practice, so don't be afraid to experiment with different combinations of these features.

---

## License

This project is licensed under the Apache 2.0 License &mdash; see the [LICENSE](../LICENSE) file for details.

&copy; 2024-2025 Kenneth Oliver. All rights reserved.
