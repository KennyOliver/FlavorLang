# Standard Library Functions

| **Function**   | **Description**                                                    |
| -------------- | ------------------------------------------------------------------ |
| `string`       | Casts a value to a string.                                         |
| `float`        | Casts a value to a float.                                          |
| `int`          | Casts a value to an integer.                                       |
| `sample`       | Reads input from the terminal.                                     |
| `serve`        | Outputs data to the terminal.                                      |
| `burn`         | Raises an error. If uncaught, it terminates the program.           |
| `random`       | Generates a random number:                                         |
|                | - 0 args: a random float between `0` and `1`.                      |
|                | - 1 arg: a random integer between `0` and the given max value.     |
|                | - 2 args: a random integer between the given min and max values.   |
| `get_time`     | Returns the current UNIX timestamp.                                |
| `taste_file`   | Reads the content of a file.                                       |
| `plate_file`   | Writes data to a file (overwrites the file if it already exists).  |
| `garnish_file` | Appends data to a file.                                            |
| `length`       | Returns the length of a string or array.                           |
| `sleep`        | Pauses program execution for the specified number of milliseconds. |
