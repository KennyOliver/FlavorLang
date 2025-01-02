# FlavorLang Syntax Examples

These are examples showcasing the unique (& fun) syntax of FlavorLang. They give a taste of the cooking-inspired syntax.

---

## Table of Contents

- [Hello World](#1)
- [Defining Variables](#2)
- [Conditional Logic](#3)
- [For Loop](#4)
- [While Loop](#5)
- [Functions with Return](#6)
- [Error Handling](#7)
- [Switch-Case Logic](#8)
- [User Input](#9)
- [Use Booleans](#10)
- [File Operations](#11)
- [Raise an Error](#12)
- [Casting](#13)
- [Random Number Generation & Standard Library Functions](#14)
- [UNIX Timestamp to ISO-8601 Date](#15)

---

### 1. 👋 Hello World <a id="1"></a>

The simplest program to print "Hello world!".

```py
serve("Hello world!");
```

### 2. 🍲 Defining Variables <a id="2"></a>

Use `let` to declare and initialize variables.

```py
let name = "Chef";
let age = 25;

serve("Name:", name);
serve("Age:", age);
```

### 3. 🔄 Conditional Logic <a id="3"></a>

Use `if`, `elif`, and `else` to control program flow.

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

let result = bake_cake(200);
serve(result);
```

### 7. 🛠️ Error Handling <a id="7"></a>

Use `try` and `rescue` to handle errors.

```py
try {
    burn("This recipe failed!");
    serve("This won't run!");
} rescue {
    serve("Caught an error: Recipe needs improvement.");
}
```

### 8. 🔎 Switch-Case Logic <a id="8"></a>

The `check`-`is` syntax in FlavorLang provides an intuitive way to match multiple conditions with beginner-friendly behavior inspired by Python & Go.

- `check` evaluates a value, is matches cases, and else handles unmatched values.
- **Fallthrough**: Cases fall through by default **unless explicitly stopped** with `break`. This ensures clarity and flexibility for beginners.

#### Example

```py
let dessert = "cake";

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
let filepath = "./tests/output.txt";

plate_file(filepath, "Freshly baked cake ready to deliver!");
garnish_file(filepath, "\nDon't forget the toppings!");

let data = taste_file(filepath);
serve("File Contents:");
serve(data);
```

### 12. ⛔️ Raise an Error <a id="12"></a>

Use `burn` to raise an error and halt execution.

```py
let time = 20;

serve("Before error.");

if time > 15 {
    burn("Too late!", "The food got burnt!");
}

serve("After error?");
```

### 13. 🔀 Casting <a id="13"></a>

```py
let a = 1;
serve(a + a);
serve(string(a) + string(a));

let b = "True";
let c = "False";
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
let favorite = sample();
serve("Your favorite is:", favorite);

# Using `random()` with different argument counts
let num1 = random();              # Generates between 0.0 and 1.0
let num2 = random(5.0);           # Generates between 0.0 and 5.0
let num3 = random(10.0, 20.0);    # Generates between 10.0 and 20.0
serve("Random numbers:", num1, num2, num3);

for i in 1..=10 {
    serve(i, "\t->", random());
}

# Using `burn()` to raise an error
burn("This is a fatal error with code:", 1001);
```

### 🗓️ 15. Get Current Year <a id="15"></a>

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

---

## License

This project is licensed under the Apache 2.0 License &mdash; see the [LICENSE](../LICENSE) file for details.

&copy; 2024-2025 Kenneth Oliver. All rights reserved.
