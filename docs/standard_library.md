# FlavorLang Standard Library Reference

## Overview

The FlavorLang Standard Library provides a comprehensive set of built-in functions for common programming tasks. This document details each function's syntax, behavior, and usage.

---

## 📖 Table of Contents

1. [Core Functions](#core-functions)
   - [Type Conversion](#type-conversion)
     - [`string(value) → string`](#stringvalue--string)
     - [`float(value) → float`](#floatvalue--float)
     - [`int(value) → int`](#intvalue--int)
   - [Input/Output](#inputoutput)
     - [`sample(prompt) → string`](#sampleprompt--string)
     - [`serve(*args, newline=True)`](#serveargs-newlinetrue)
   - [Error Handling](#error-handling)
     - [`burn(*messages)`](#burnmessages)
   - [Random Number Generation](#random-number-generation)
     - [`random() → float`](#random--float)
     - [`random(max) → int`](#randommax--int)
     - [`random(min, max) → int`](#randommin-max--int)
   - [Math Functions](#math-functions)
     - [`floor(value) → int`](#floorvalue--int)
     - [`ceil(value) → int`](#ceilvalue--int)
     - [`round(value) → int`](#roundvalue--int)
     - [`abs(value) → numeric`](#absvalue--numeric)
   - [Time Operations](#time-operations)
     - [`get_time() → int`](#get_time--int)
   - [File Operations](#file-operations)
     - [`taste_file(path) → string`](#taste_filepath--string)
     - [`plate_file(path, content)`](#plate_filepath-content)
     - [`garnish_file(path, content)`](#garnish_filepath-content)
   - [Collection Operations](#collection-operations)
     - [`length(collection) → int`](#lengthcollection--int)
   - [System Operations](#system-operations)
     - [`sleep(milliseconds)`](#sleepmilliseconds)
2. [Best Practices](#best-practices)
3. [License](#license)

---

## Core Functions

### Type Conversion

#### `string(value) → string`

Converts a value to its string representation.

**Parameters:**

- `value`: any

**Returns:**

- `string`: string representation of the value

**Examples:**

```python
string(42);    # "42"
string(3.14);  # "3.14"
string(True);  # "True"
```

#### `float(value) → float`

Converts a value to a floating-point number.

**Parameters:**

- `value`: string or numeric value

**Returns:**

- `float`: floating-point representation
- Raises error if conversion fails

**Examples:**

```python
float("3.14");  # 3.14
float("42");    # 42.0
float("-0.5");  # -0.5
```

#### `int(value) → int`

Converts a value to an integer.

**Parameters:**

- `value`: string or numeric value

**Returns:**

- `int`: Integer representation
- Raises error if conversion fails

**Examples:**

```python
int("42");   # 42
int(3.14);   # 3
int("-17");  # -17
```

### Input/Output

#### `sample(prompt) → string`

Reads a line of input from the terminal with an optional prompt.

**Parameters:**

- `prompt`: any value

**Returns:**

- `string`: User input line

**Examples:**

```py
let name = sample();  # Waits for user input
```

#### `serve(*args, newline=True)`

Outputs values to the terminal.

**Parameters:**

- `*args`: Values to output
- `newline`: Boolean flag for newline (optional)

**Examples:**

```py
serve("Hello");                 # Hello\n
serve("No", "newline", False);  # No newline
```

### Error Handling

#### `burn(*messages)`

Raises an error with specified message(s).

**Parameters:**

- `*messages`: Error message components

**Examples:**

```python
burn("Invalid input:", value);
```

### Random Number Generation

#### `random() → float`

Generates a random float between 0 and 1.

**Returns:**

- `float`: Random value in [0.0, 1.0]

#### `random(max) → int`

Generates a random integer between 0 and max.

**Parameters:**

- `max`: Upper bound (exclusive)

**Returns:**

- `int`: Random value in [0, max)

#### `random(min, max) → int`

Generates a random integer in specified range.

**Parameters:**

- `min`: Lower bound (inclusive)
- `max`: Upper bound (exclusive)

**Returns:**

- `int`: Random value in [min, max)

**Examples:**

```py
random();       # 0.0 to 1.0
random(10);     # 0 to 9
random(5, 10);  # 5 to 9
```

### Math Functions

#### `floor(value) → int`

Rounds a floating-point number down to the nearest integer.

##### Parameters

- **value:** A floating-point number.

##### Returns

- **int:** The largest integer less than or equal to value.

##### Examples

```py
serve(floor(3.8));  # Output: 3
serve(floor(-2.4)); # Output: -3
```

#### `ceil(value) → int`

Rounds a floating-point number up to the nearest integer.

##### Parameters

- **value:** A floating-point number.

##### Returns

- **int:** The smallest integer greater than or equal to value.

##### Examples

```py
serve(ceil(3.2));   # Output: 4
serve(ceil(-2.8));  # Output: -2
```

#### `round(value) → int`

Rounds a floating-point number to the nearest integer.

##### Parameters

- **value:** A floating-point number.

##### Returns

- **int:** The nearest integer to value.

##### Examples

```
serve(round(3.5));   # Output: 4
serve(round(-2.5));  # Output: -2
```

#### `abs(value) → numeric`

Returns the absolute value of a number.

##### Parameters

- **numeric:** An integer or floating-point number.

##### Returns

- **numeric:** The absolute value of the input.

##### Examples

```py
serve(abs(-42));   # Output: 42
serve(abs(-3.8));  # Output: 3.8
```

###

### Time Operations

#### `get_time() → int`

Returns current UNIX timestamp.

**Returns:**

- `int`: Seconds since Unix epoch (1st January 1970 at 00:00:00 UTC)

**Examples:**

```js
let now = get_time();
```

### File Operations

#### `taste_file(path) → string`

Reads entire file content.

**Parameters:**

- `path`: File path

**Returns:**

- `string`: File contents
- Raises error if file not found

#### `plate_file(path, content)`

Writes content to file (overwrites).

**Parameters:**

- `path`: File path
- `content`: Data to write

#### `garnish_file(path, content)`

Appends content to file.

**Parameters:**

- `path`: File path
- `content`: Data to append

### Collection Operations

#### `length(collection) → int`

Returns collection length.

**Parameters:**

- `collection`: string or Array

**Returns:**

- `int`: Number of elements

**Examples:**

```py
length([1,2,3]); # 3
length("hello"); # 5
```

### System Operations

#### `sleep(milliseconds)`

Pauses execution.

**Parameters:**

- `milliseconds`: Delay duration

**Examples:**

```py
sleep(1000);  # Pause for 1 second
```

## Best Practices

1. Always check file operations for errors
2. Use type conversion functions defensively
3. Handle random number generation edge cases
4. Consider performance implications of sleep
5. Validate user input from `sample()`

---

## License

This project is licensed under the Apache 2.0 License &mdash; see the [LICENSE](../LICENSE) file for details.

&copy; 2024-2025 Kenneth Oliver. All rights reserved.
