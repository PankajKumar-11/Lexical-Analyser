# ToyLang Lexical Analyzer & Predictive Parser

A complete **Lexical Analyzer** and **LL(1) Predictive Parser** for ToyLang — a compact educational programming language — built with **C + LEX (Flex)**.

- **Lexer (Part 1)** reads `.toy` source files and emits a classified token stream (keywords, identifiers, literals, operators, delimiters).
- **Parser (Part 2)** implements a non-recursive, explicit-stack LL(1) predictive parser that validates expressions and implements panic-mode error recovery using synchronizing sets.

---

## Project Structure

| File | Purpose |
|------|---------|
| `token.h` | `TokenType` enum + `Token` struct definition |
| `toylang.l` | Flex specification — regex rules + C actions + error handling |
| `parser.h` | `NonTerminal` enum + `StackSymbol` definitions |
| `parser.c` | Predictive parsing explicitly simulated stack logic + panic mode recovery |
| `main.c` | Entry point: calls parser and triggers the lexer sequence |
| `Makefile` | Build automation (`make` environment target) |
| `tests/` | Directory containing `.toy` sample files for Lexer and Parser testing |

## Prerequisites

- **Flex** ≥ 2.6 — scanner generator
- **GCC** (or Clang) ≥ 9 — C compiler
- **GNU Make** ≥ 4.0
- **Platform**: Linux / macOS / Windows via WSL

### Install on Ubuntu/WSL

```bash
sudo apt update && sudo apt install flex gcc make
```

## Build (Windows cmd / PowerShell)

Since `flex` is not natively available on Windows without WSL/MinGW-MSYS, the generated `lex.yy.c` has been included in the repository. You can compile the project natively bypassing `make` by calling `gcc` directly:

```cmd
gcc -Wall -std=c99 -D_POSIX_C_SOURCE=200809L -Isrc -o toylang_lexer.exe lex.yy.c src/main.c src/parser.c
```

*(If you are on Linux or macOS, you can still just run `make`)*

## Run

To execute the parser against a `.toy` source file, use the generated executable pointing to one of the test files:

**Run a Valid Expression:**
```cmd
.\toylang_lexer.exe tests\test_valid.toy
```

**Run an Invalid Expression (Error Recovery Synch test):**
```cmd
.\toylang_lexer.exe tests\test_error.toy
```

## Token Categories

| Category | Examples | Regex |
|----------|----------|-------|
| Keywords | `if`, `else`, `while`, `int`, `float`, `bool`, `return`, `true`, `false` | Literal strings |
| Identifiers | `x`, `myVar`, `_count` | `[a-zA-Z_][a-zA-Z0-9_]*` |
| Integer Literal | `0`, `42`, `1000` | `[0-9]+` |
| Float Literal | `3.14`, `0.5` | `[0-9]+\.[0-9]+` |
| Arithmetic Ops | `+` `-` `*` `/` `%` | Single char |
| Relational Ops | `==` `!=` `<` `>` `<=` `>=` | 2-char before 1-char |
| Logical Ops | `&&` `\|\|` `!` | 2-char before 1-char |
| Assignment | `=` | After `==` |
| Delimiters | `(` `)` `{` `}` `;` `,` | Single char |
| Comments | `// ...` and `/* ... */` | Consumed silently |

## Parser Logic & Error Handling

- **Grammar Validation**: Matches arithmetic (`+`, `-`, `*`, `/`, `%`) and logical expressions (`&&`, `||`, `!`, `==`, `<`, etc.)
- **Error Recovery**: The parser utilizes Panic Mode synchronization. If an unexpected token is encountered, it skips tokens until it finds a synchronizing element in the `FOLLOW` set (like `EOF`), pops the broken production from the stack, and safely continues parsing the rest of the file.

---

**Version 1.0 · March 2026**
