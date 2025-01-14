# Brainf Interpreter

---

## [`brainf.flv`](../src/tests/examples/brainf.flv)

```
# ==================================================
# 🧠 Brainf Interpreter in FlavorLang
# ==================================================

create brainf_interpreter(code) {
    # Step 1: Preprocess the code to map brackets
    let code_length = length(code);
    let loop_stack = [];

    # Preallocate arrays for loop mappings with a size equal to code_length
    let loop_start_to_end = [];
    let loop_end_to_start = [];
    for i in 0..code_length {
        loop_start_to_end[^+] = -1;
        loop_end_to_start[^+] = -1;
    }

    for i in 0..(code_length - 1) {
        let char = code[i];
        if char == "[" {
            loop_stack[^+] = i;
        } elif char == "]" {
            let stack_length = length(loop_stack);
            if stack_length > 0 {
                let start_index = loop_stack[length(loop_stack) - 1];
                let new_length = length(loop_stack) - 1;
                loop_stack = loop_stack[:new_length]; # Remove last element
                loop_start_to_end[start_index] = i;
                loop_end_to_start[i] = start_index;
            } else {
                serve("Error: Unmatched ']' at position", i);
                deliver "";  # Halt execution by returning an empty string
            }
        }
    }

    # Check for any unmatched '['
    if length(loop_stack) > 0 {
        serve("Error: Unmatched '[' at positions:", loop_stack);
        deliver "";  # Halt execution by returning an empty string
    }

    # Step 2: Initialize the data tape
    let tape_size = 30000;
    let data_tape = [];
    for i in 0..tape_size {
        data_tape[^+] = 0;
    }
    let data_ptr = 0;

    # Step 3: Initialize program pointer
    let program_ptr = 0;

    # Step 4: Execute the Brainf code
    while program_ptr < code_length {
        let command = code[program_ptr];

        if command == ">" {
            data_ptr = data_ptr + 1;
            if data_ptr >= tape_size {
                data_ptr = 0; # Wrap around if exceeding tape size
            }
        } elif command == "<" {
            data_ptr = data_ptr - 1;
            if data_ptr < 0 {
                data_ptr = tape_size - 1; # Wrap around if below zero
            }
        } elif command == "+" {
            data_tape[data_ptr] = data_tape[data_ptr] + 1;
            if data_tape[data_ptr] > 255 {
                data_tape[data_ptr] = 0; # Wrap around byte value
            }
        } elif command == "-" {
            data_tape[data_ptr] = data_tape[data_ptr] - 1;
            if data_tape[data_ptr] < 0 {
                data_tape[data_ptr] = 255; # Wrap around byte value
            }
        } elif command == "." {
            let ascii_val = data_tape[data_ptr];
            let character = chr(ascii_val);
            serve(character, False);
        } elif command == "," {
            serve("Input a character:");
            let input_char = sample();
            let ascii_val = ord(input_char);
            data_tape[data_ptr] = ascii_val;
        } elif command == "[" {
            if data_tape[data_ptr] == 0 {
                # Jump to the command after the matching ']'
                let jump_to = loop_start_to_end[program_ptr];
                program_ptr = jump_to;
            }
        } elif command == "]" {
            if data_tape[data_ptr] != 0 {
                # Jump back to the command after the matching '['
                let jump_to = loop_end_to_start[program_ptr];
                program_ptr = jump_to;
            }
        }

        # Increment program pointer
        program_ptr = program_ptr + 1;
    }

    deliver "Execution Finished";  # Return a confirmation value
}

# Helper function to convert ASCII value to single-character string
create chr(ascii_val) {
    # Implement a lookup table for ASCII values 0-127
    let ascii_chars = [
        "\x00", "\x01", "\x02", "\x03", "\x04", "\x05", "\x06", "\x07",
        "\x08", "\x09", "\x0A", "\x0B", "\x0C", "\x0D", "\x0E", "\x0F",
        "\x10", "\x11", "\x12", "\x13", "\x14", "\x15", "\x16", "\x17",
        "\x18", "\x19", "\x1A", "\x1B", "\x1C", "\x1D", "\x1E", "\x1F",
        " ", "!", "\"", "#", "$", "%", "&", "'", "(", ")", "*", "+", ",", "-", ".", "/",
        "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", ":", ";", "<", "=", ">", "?",
        "@", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O",
        "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "[", "\\", "]", "^", "_",
        "`", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o",
        "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", "{", "|", "}", "~", "\x7F"
    ];

    if ascii_val >= 0 && ascii_val < length(ascii_chars) {
        deliver ascii_chars[ascii_val];
    } else {
        deliver "?"; # Placeholder for undefined ASCII values
    }
}

# Helper function to convert single-character string to ASCII value
create ord(char_str) {
    let ascii_chars = [
        "\x00", "\x01", "\x02", "\x03", "\x04", "\x05", "\x06", "\x07",
        "\x08", "\x09", "\x0A", "\x0B", "\x0C", "\x0D", "\x0E", "\x0F",
        "\x10", "\x11", "\x12", "\x13", "\x14", "\x15", "\x16", "\x17",
        "\x18", "\x19", "\x1A", "\x1B", "\x1C", "\x1D", "\x1E", "\x1F",
        " ", "!", "\"", "#", "$", "%", "&", "'", "(", ")", "*", "+", ",", "-", ".", "/",
        "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", ":", ";", "<", "=", ">", "?",
        "@", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O",
        "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "[", "\\", "]", "^", "_",
        "`", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o",
        "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", "{", "|", "}", "~", "\x7F"
    ];

    let ascii_val = -1;
    for i in 0..(length(ascii_chars) - 1) {
        let current_char = ascii_chars[i];
        let is_equal = char_str_equals(char_str, current_char);
        if is_equal == True {
            ascii_val = i;
            break;
        }
    }

    if ascii_val != -1 {
        deliver ascii_val;
    } else {
        deliver 63; # ASCII for '?'
    }
}

# Helper function to compare two single-character strings
create char_str_equals(a, b) {
    if a == b {
        deliver True;
    } else {
        deliver False;
    }
}


# ==================================================
# 🔄 Example Usage
# ==================================================

# Example Brainf code: "Hello World!"
let hello_world_bf = "++++++++++[>+++++++>++++++++++>+++>+<<<<-]>++.>+.+++++++..+++.>++.<<+++++++++++++++.>.+++.------.--------.>+.>.";

serve("Executing Brainf Interpreter for 'Hello World!':");
brainf_interpreter(hello_world_bf);
serve("Brainf Execution Complete!");
```

---

## License

This project is licensed under the Apache 2.0 License &mdash; see the [LICENSE](../LICENSE) file for details.

&copy; 2024-2025 Kenneth Oliver. All rights reserved.
