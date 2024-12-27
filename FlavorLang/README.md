# `/FlavorLang`

## Structure

| File       | Purpose                                |
| ---------- | -------------------------------------- |
| `common.h` | Shared constancts, macros, and structs |
| `lexer.h`  | Function declarations for lexer        |
| `lexer.c`  | Implementation of lexer logic          |
| `main.c`   | Entry point & lexer testing            |
| `test.flv` | Sample FlavorLang code                 |

```bash
FlavorLang/
├── common.h       # Shared constants and structs
├── lexer.h    # Function declarations
├── lexer.c    # lexer implementation
├── main.c         # Entry point for testing
└── test.flv       # Sample FlavorLang code
```

## Lexer Workflow

1. **Input**: `test.flv` as a string.
2. **Scan**: Read said string char by char.
3. **Recognize tokens**:

- Skip whitespace & comments.
- Recognize keywords, strings, numbers, operators, and delimiters.
- Record the line number for error reporting.

4. **Output**: An array of tokens.
