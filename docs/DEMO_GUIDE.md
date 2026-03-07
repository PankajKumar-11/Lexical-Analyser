# ToyLang Lexer — Demo & Presentation Guide

## Quick Demo Script (5 minutes)

### Step 1: Show the Big Picture (30 sec)
Explain what a lexer does:
> "A lexer is the FIRST phase of a compiler. It reads raw source code character by character
> and groups them into meaningful units called **tokens** — like how your eyes group letters
> into words when reading a sentence."

```
Source Code  →  [ LEXER ]  →  Token Stream  →  [ PARSER ]  →  ...
"int x = 42;"    ↓
              TOK_INT    'int'
              TOK_IDENT  'x'
              TOK_ASSIGN '='
              TOK_INT_LIT '42'
              TOK_SEMI   ';'
```

---

### Step 2: Show the Source Files (1 min)
Open these files and briefly explain:
- **`token.h`** — "These are all the token types our lexer can recognize"
- **`toylang.l`** — "This is the Flex file — regex rules that define each token"
- **`main.c`** — "This is the driver that reads a file and prints all tokens"

---

### Step 3: Live Demo — Run 3 Test Cases (3 min)

**Demo 1 — Clean program (show it works):**
```bash
./toylang_lexer test_keywords.toy
```
> "See how every keyword like `if`, `while`, `int` gets its own token type,
> while variable names like `flag` are classified as `TOK_IDENT`."

**Demo 2 — Error recovery (most impressive):**
```bash
./toylang_lexer test_errors.toy
```
> "Here we have 5 invalid characters: @, #, $, ^, ~. The lexer reports ALL 5 errors
> with line numbers, but doesn't crash — it recovers and continues tokenizing valid code."

**Demo 3 — Edge cases (show correctness):**
```bash
./toylang_lexer test_edge_cases.toy
```
Point out:
> - `iffy` → `TOK_IDENT` (not `TOK_IF` + `TOK_IDENT`) — "longest match wins"
> - `3.14` → `TOK_FLOAT_LIT` (not `INT.INT`) — "float rule before int rule"
> - `==` → `TOK_EQ` (not two `=`) — "2-char ops have priority"
> - Multi-line comment gone — "comments produce zero tokens"

---

## Key Talking Points

### How does it work internally?
1. Flex reads `toylang.l` (our regex rules)
2. Generates a **DFA** (Deterministic Finite Automaton) in `lex.yy.c`
3. The DFA scans input character-by-character using state transitions
4. When a pattern matches → emits the corresponding token

### Why rule order matters (great exam question!)
| Problem | Wrong Order | Correct Order |
|---------|-------------|---------------|
| `if` matched as identifier | IDENT before keywords | Keywords FIRST |
| `3.14` split into `3` + `.` + `14` | INT_LIT before FLOAT_LIT | FLOAT_LIT FIRST |
| `==` becomes two `=` | `=` before `==` | `==` FIRST |

### Error recovery strategy
> "The catch-all rule `.` matches ANY single unrecognized character.
> It prints an error to stderr, increments the error counter, consumes
> that one bad character, and keeps scanning. This means ALL errors
> in a file are reported in one run."

---

## Test Files Available

| File | What it Tests | Expected Errors |
|------|---------------|-----------------|
| `sample.toy` | All token categories | 1 (`@`) |
| `test_keywords.toy` | All 10 keywords | 0 |
| `test_operators.toy` | All operators + delimiters | 0 |
| `test_errors.toy` | Error recovery (5 bad chars) | 5 (`@#$^~`) |
| `test_edge_cases.toy` | Rule priority, longest match | 0 |

## Docker Command (for running on Windows)

```powershell
docker run --rm -v "${PWD}:/src" -w /src gcc:latest sh -c "apt-get update -qq && apt-get install -y -qq flex > /dev/null 2>&1 && make && ./toylang_lexer <filename>.toy"
```
