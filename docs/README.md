# ToyLang Lexical Analyzer

A complete **Lexical Analyzer** for ToyLang — a compact educational programming language — built with **C + LEX (Flex)**.

The lexer reads `.toy` source files and emits a classified token stream (keywords, identifiers, literals, operators, delimiters) with line/column numbers and full error recovery.

---

## Project Structure

| File | Purpose |
|------|---------|
| `token.h` | `TokenType` enum + `Token` struct definition |
| `toylang.l` | Flex specification — regex rules + C actions + error handling |
| `main.c` | Entry point: opens file, calls `yylex()` loop, prints tokens |
| `Makefile` | Build automation: `flex → gcc → link` |
| `sample.toy` | Test program covering all token categories + error case |

## Prerequisites

- **Flex** ≥ 2.6 — scanner generator
- **GCC** (or Clang) ≥ 9 — C compiler
- **GNU Make** ≥ 4.0
- **Platform**: Linux / macOS / Windows via WSL

### Install on Ubuntu/WSL

```bash
sudo apt update && sudo apt install flex gcc make
```

## Build

```bash
make
```

This runs:
1. `flex toylang.l` → generates `lex.yy.c`
2. `gcc -Wall -std=c99 -o toylang_lexer lex.yy.c main.c -lfl` → produces `toylang_lexer`

## Run

```bash
./toylang_lexer sample.toy
```

Or use the Makefile shortcut:

```bash
make run
```

## Clean

```bash
make clean
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

## Error Handling

Invalid characters produce:
```
LEXICAL_ERROR [line X, col Y]: unexpected character '<c>'
```
The lexer **continues scanning** after an error — all errors in a file are reported in one run.

---

**Version 1.0 · March 2026**
