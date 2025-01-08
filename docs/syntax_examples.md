# FlavorLang Syntax Examples

These are examples showcasing the unique (& fun) syntax of FlavorLang. They give a taste of the cooking-inspired syntax.

---

## Table of Contents

1. [Hello World](#1)
2. [Defining Variables](#2)
3. [Conditional Logic](#3)
4. [For Loop](#4)
5. [While Loop](#5)
6. [Functions with Return](#6)
7. [Error Handling](#7)
8. [Switch-Case Logic](#8)
9. [User Input](#9)
10. [Use Booleans](#10)
11. [File Operations](#11)
12. [Raise an Error](#12)
13. [Casting](#13)
14. [Random Number Generation & Standard Library Functions](#14)
15. [Get Current Year with `get_time()`](#15)
16. [Ternary Operator](#16)
17. [Nested Function Application](#17)
18. [Basic Arrays & Operations](#18)
19. [2D Arrays](#19)

---

### 1. 👋 Hello World <a id="1"></a>

The simplest program to print "Hello world!".

```py
serve("Hello world!");
```

### 2. 🍲 Defining Variables <a id="2"></a>

Use `let` to declare and initialize variables.

```py
const name = "Hello, " + "world!" + " :)";
const age = 25;

serve(name);
serve("Age:", age);
```

### 3. 🔄 Conditional Logic <a id="3"></a>

Use `if`, `elif`, and `else` to control program flow.

```py
const oven_temperature = 200;

if oven_temperature > 180 {
    serve("The oven is hot!");
} elif oven_temperature == 180 {
    serve("The oven is just right!");
} else {
    serve("The oven is too cold!");
}
```

### 4. 🔁 For Loop <a id="4"></a>

Use `for` to iterate a block of code.

```py
for i in 1..5 {
    serve("Mixing... Step", i);
}

for j in 10..=1 by -3 {
    serve(j);
}
```

### 5. 🔄 While Loop <a id="5"></a>

Use `while` for condition-based repetition.

```py
let flour_added = 0;

while flour_added < 3 {
    serve("Adding flour...");
    let flour_added = flour_added + 1;
}

serve("All flour has been added!");
```

### 6. 📦 Functions with Return <a id="6"></a>

Use `create` to define functions and `deliver` to return values.
Note that `burn` **takes precedence** over `deliver`, stopping execution immediately.

```py
create bake_cake(temperature) {
    if temperature < 180 {
        serve("Temperature is too low to bake!");
        burn("Cake burned!");  # stops function execution immediately
  } else {
        serve("Baking cake at", temperature, "degrees!");
        deliver "Cake is ready!";
  }
}

const result = bake_cake(200);
serve(result);
```

### 7. 🛠️ Error Handling <a id="7"></a>

Use `try` and `rescue` to handle errors.

```py
try {
    serve("This will run!");
    burn("This recipe failed!");
    serve("This won't run!");
} rescue {
    serve("Caught an error: Recipe needs improvement.");
}

try {
    int("abc");
    serve("This won't run!");
} rescue {
    serve("Runtime error: Can't cast string `abc` to int.");
} finish {
    serve("This always executes!");
}
```

### 8. 🔎 Switch-Case Logic <a id="8"></a>

The `check`-`is` syntax in FlavorLang provides an intuitive way to match multiple conditions with beginner-friendly behavior inspired by Python & Go.

- `check` evaluates a value, is matches cases, and else handles unmatched values.
- **Fallthrough**: Cases fall through by default **unless explicitly stopped** with `break`. This ensures clarity and flexibility for beginners.

#### Example

```py
const dessert = "cake";

check dessert {
    is "cake":
        serve("Bake the cake!");
    is "pie":
    is "cookie":
        serve("Prepare the oven!");
        break;
    else:
        serve("Dessert not on the menu.");
}
```

#### Output

```
Bake the cake!
Dessert not on the menu.
```

### 9. 📥 User Input <a id="9"></a>

Use `sample` to accept input from the user.

```py
serve("What's your favorite dessert?");
let favorite = sample();

serve("You chose:", favorite);
```

### 🔵 10. Use Booleans <a id="10"></a>

Booleans in FlavorLang, `True` and `False`, can be used to create flags, evaluate conditionals, and more.

```py
let stop_loop = False;
let count = 10;

serve("Preparing for launch!");

while stop_loop != True {
    serve(count);

    if count > 0 {
        count = count - 1;
    } else {
        stop_loop = True;
    }
}

serve("Blast off!");
```

### 11. 📄 File Operations <a id="11"></a>

- `Plate`: Write to a file.
- `Garnish`: Append to a file.
- `taste`: Read from a file.

```py
const filepath = "./tests/output.txt";

plate_file(filepath, "Freshly baked cake ready to deliver!");
garnish_file(filepath, "\nDon't forget the toppings!");

const data = taste_file(filepath);
serve("File Contents:");
serve(data);
```

### 12. ⛔️ Raise an Error <a id="12"></a>

Use `burn` to raise an error and halt execution.

```py
const time = 20;

serve("Before error.");

if time > 15 {
    burn("Too late!", "The food got burnt!");
}

serve("After error?");
```

### 13. 🔀 Casting <a id="13"></a>

```py
const a = 1;
serve(a + a);
serve(string(a) + string(a));

const b = "True";
const c = "False";
serve(b == c);
serve(b != c);

for _ in 1..=2 {
    serve("Enter a number:");
    let user_input = int(sample());
    let positive = user_input >= 0;
    serve("Positive?", positive);
}

serve(float("+8"));
```

### 🎲 14. Random Number Generation & Standard Library Functions <a id="14"></a>

```py
# Using `sample()` to get user input
serve("Enter your favorite number:");
const favorite = sample();
serve("Your favorite is:", favorite);

# Using `random()` with different argument counts
const num1 = random();              # Generates between 0.0 and 1.0
const num2 = random(5.0);           # Generates between 0.0 and 5.0
const num3 = random(10.0, 20.0);    # Generates between 10.0 and 20.0
serve("Random numbers:", num1, num2, num3);

for i in 1..=10 {
    serve(i, "\t->", random());
}
# Using `burn()` to raise an error
burn("This is a fatal error with code:", 1001);
```

### 🗓️ 15. Get Current Year with `get_time()` <a id="15"></a>

```py
create get_year_from_unix(timestamp) {
    let temp_timestamp = timestamp;
    let year = 1970;

    let is_leap = ((year % 4 == 0) && ((year % 100 != 0) || (year % 400 == 0)));
    let seconds_in_year = 365 * 24 * 60 * 60;
    if is_leap {
        seconds_in_year = 366 * 24 * 60 * 60;
    }

    while (temp_timestamp >= seconds_in_year) {
        temp_timestamp = temp_timestamp - seconds_in_year;
        year = year + 1;

        is_leap = ((year % 4 == 0) && ((year % 100 != 0) || (year % 400 == 0)));
        seconds_in_year = 365 * 24 * 60 * 60;
        if is_leap {
            seconds_in_year = 366 * 24 * 60 * 60;
        }
    }

    deliver year;
}

serve(get_year_from_unix(get_time()));
```

### 🧮 16. Ternary Operator <a id="16"></a>

```py
create check_even_odd(num) {
    deliver num % 2 == 0 ? "Even" : "Odd";
}

for _ in 1..=5 {
    serve("Enter a number:");
    let input = int(sample());
    serve(check_even_odd(input));
}
```

### 17. 🪺 Nested Function Application <a id="17"></a>

```py
create times_2(x) {
    deliver x * 2;
}

create times_3(x) {
    deliver x * 3;
}

create test(num, func_a, func_b) {
    deliver func_b(func_a(func_b(num)));
}

let a = test(10, times_2, times_3); # 180
serve(a);
```

### 18. Basic Arrays & Operations <a id="18"></a>

```py
# Initialize test array
let array = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10];
serve("Initial array:", array);

# Element Access
serve("Accessing 6th element (index 5):", array[5]);

# Append
array[^+] = 11;
serve("Array after appending 11:", array);

# Prepend
array[+^] = 0;
serve("Array after prepending 0:", array);

# Remove Last Element
serve("Removing last element:", array[^-]);
serve("Array after removing last element:", array);

# Remove First Element
serve("Removing first element:", array[-^]);
serve("Array after removing first element:", array);

# Slicing
serve("Slice from index 0 to 5:", array[0:5]);
serve("Reversed array:", array[::-1]);
serve("Slice from index 3 to end:", array[3:]);
serve("Slice from start to index 8:", array[:8]);
serve("Slice from index 3 to 8, skipping every 2nd element:", array[3:8:2]);
serve("Slice from index 8 to 3, skipping every 2nd element (reverse):", array[8:3:-2]);
serve("Slice from index 3 to start (reversed):", array[3::-1]);
serve("Slice from end towards index 8 (exclusive, reversed):", array[:8:-1]);
serve("Slice entire array, skipping every 2nd element:", array[::2]);
serve("Reverse array, skipping every 3rd element:", array[::-3]);

# Test completed
serve("Array operation tests complete!");

# Initialize const (immutable) array
const const_array = [10, 20, 30, 40, 50];
serve("Initial const array:", const_array);

# Attempt mutation
try {
    const_array[^+] = 60; # Trying to append to const array
    serve("This should not be shown; const array was mutated!");
} rescue {
    serve("Caught error: Cannot mutate a const array.");
}

# Confirm immutability
serve("Const array remains unchanged:", const_array);

# Try to remove element
try {
    const_array[^-]; # Attempting to remove the last element
    serve("This should not be shown; const array was mutated!");
} rescue {
    serve("Caught error: Cannot mutate a const array.");
}

# Final confirmation
serve("Const array is still:", const_array);

# Test completed
serve("Immutable array tests complete!");
```

### 19. 2D Arrays <a id="19"></a>

```py
# Test for array length
let array = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10];
serve("Initial array:", array);
serve("Length of array:", length(array));

# Test compound slicing
serve("Original array:", array);
serve("Double slice (first 5, then reverse):", (array[0:5])[::-1]);
serve("Triple slice (first 5, skip every second, then reverse):", ((array[0:5])[::2])[::-1]);

# Test declaring and using 2D arrays
let array2D = [
    [1, 2, 3],
    [4, 5, 6],
    [7, 8, 9]
];
serve("2D array:", array2D);

# Access individual elements
serve("Element at (2, 2):", array2D[1][1]);
serve("Element at (3, 3):", array2D[2][2]);

# Modify elements in a 2D array
array2D[0][0] = 10;
serve("2D array after modifying (1, 1) to 10:", array2D);

# Test slicing on rows
serve("First row:", array2D[0]);
serve("Last row reversed:", array2D[2][::-1]);

# Test slicing on columns (manual transpose-like slicing)
let first_column = [array2D[0][0], array2D[1][0], array2D[2][0]];
serve("First column:", first_column);

let second_column_reversed = [array2D[2][1], array2D[1][1], array2D[0][1]];
serve("Second column reversed:", second_column_reversed);

# Test length of 2D array and its rows
serve("Number of rows:", length(array2D));
serve("Number of elements in first row:", length(array2D[0]));

# Immutable 2D arrays
const const_2D_array = [
    [10, 20, 30],
    [40, 50, 60],
    [70, 80, 90]
];
serve("Immutable 2D array:", const_2D_array);

# Attempt mutation on immutable 2D array
try {
    const_2D_array[0][0] = 100; # Trying to modify element
    serve("This should not be shown; const 2D array was mutated!");
} rescue {
    serve("Caught error: Cannot mutate a const 2D array.");
}

# Confirm immutability
serve("Const 2D array remains unchanged:", const_2D_array);

# Final confirmation
serve("Immutable 2D array tests complete!");

# Test length of a mixed array
let mixed_array = [1, "two", [3, 4], [5, 6]];
serve("Mixed array:", mixed_array);
serve("Length of mixed array:", length(mixed_array));

# Compound operations on mixed array
serve("First element of nested array (3, 4):", mixed_array[2][0]);
serve("Length of nested array:", length(mixed_array[2]));

# Test completed
serve("All array operation tests complete!");
```

---

## License

This project is licensed under the Apache 2.0 License &mdash; see the [LICENSE](../LICENSE) file for details.

&copy; 2024-2025 Kenneth Oliver. All rights reserved.
