# 🌶️ **FlavorLang** &mdash; _Cook 'n' Code_

**FlavorLang** is a language designed around the art of cooking. From adding ingredients to cooking & serving, you can create programs as flavorful as your imagination. With a unique set of keywords, FlavorLang combines utility, creativity, and fun!

---

## 🚀 **Execution Flags & Behaviors**

| **Flag**     | **Behavior** | **Description**                     |
| ------------ | ------------ | ----------------------------------- |
| **`--chef`** | Debug mode   | Prints step-by-step execution logs. |

<!-- | **`--cook`**  | Optimized execution | Focuses on performance.             |
| **`--taste`** | Interactive mode    | Opens a REPL shell.                 | -->

### Example Usage

```bash
flavor recipe.flv         # Default execution
flavor recipe.flv --chef  # Debug mode
```

<!-- flavor recipe.flv --cook   # Optimized run
flavor recipe.flv --taste  # Interactive mode -->

## Syntax Keywords

| Keyword   | Usage                        | Description                                           |
| --------- | ---------------------------- | ----------------------------------------------------- |
| `recipe`  | Import file                  | Import logic from another file.                       |
| `prep`    | Define a function            | Prepares a reusable block of logic                    |
| `serve`   | Return statement             | Returns a value and stops function.                   |
| `burn`    | Force exit or raise an error | Stops execution immediately with a message.           |
| `scran`   | Print or output              | Ouputs a value or message immediately.                |
| `taste`   | Input from console           | Reads user input.                                     |
| `plate`   | Write to file                | Writes data to a file.                                |
| `garnish` | Append to file               | Appends data to a file.                               |
| `gather`  | Read from file               | Reads data from a file.                               |
| `try`     | Try block                    | Executes code that might fail.                        |
| `crumbs`  | Catch block                  | Handles errors during execution.                      |
| `for`     | For-loop                     | Iterates a block for a set number of times.           |
| `while`   | While-loop                   | Repeatedly runs code while a condition is true.       |
| `when`    | Switch-case equivalent       | matches a value to multiple cases.                    |
| `is`      | Case clause                  | define a case inside `when`.                          |
| `if`      | Conditional logic            | Executes code only if a condition is true.            |
| `elif`    | Conditional logic fallback   | Executes only if a prior `if` condition is false.     |
| `else`    | Conditional fallback         | Executes code if any prior `if` conditions are false. |
| `food`    | Define variables             | Declares and initializes variables.                   |

## 🧑‍🍳 Syntax Examples

### 1. Adding and Mixing Ingredients

```
add ingredients "flour" "water" "yeast";
mix well;
```

### 2. Conditional Cooking

```
if oven_temperature > 180:
    cook "cake" for 25 minutes;
else:
    wait 5 minutes;
```

### 3. Loops (While Mixing)

```
add "flour";
while mixing:
    add "sugar";
    stir;
```

### 4. Functions (Recipes)

```
recipe make_pizza with "toppings":
    add "dough" "sauce" toppings;
    temp 180 "c";
    cook for 15 minutes;
    serve;

make_pizza with "cheese" "pepperoni";
```

Output:

```
[Step 1] Adding ingredients: flour, water, yeast.
[Step 2] Mixing...
[Step 3] Baking for 25 minutes at 180°C.
[Step 4] Serving: pizza with cheese, pepperoni.
```

## 🌟 Why FlavorLang?

- 🍴 **Unique & Fun**:
  Express your programs like recipes!
- 🔄 **Flexible Execution**:
  File extensions and flags allow customized program behavior.
- 🧩 **Readable Syntax**:
  Keywords like add, mix, cook, and serve make code approachable and enjoyable.
- 🛠️ **Debug-Friendly**:
  Easily trace and test your code step-by-step with --chef mode.
