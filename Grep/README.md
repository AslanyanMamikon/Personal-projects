# grep

A command-line search tool implemented in C++ that replicates core `grep` functionality. Searches for a pattern in one or more files and prints matching lines, with support for extended regular expressions, wildcard file expansion, and ANSI color highlighting.

## Features

- Literal and extended regular expression (ERE) search
- Case-insensitive matching
- Inverted matching (print non-matching lines)
- Line number display
- Match count reporting
- Wildcard file expansion (e.g. `*.txt`)
- Multi-pattern search via a pattern file
- ANSI color highlighting of matched text in the terminal
- CRLF-safe pattern file parsing (Windows line endings handled)

## Requirements

- C++17 or later
- A compiler with `std::filesystem` support (GCC 8+, Clang 7+, MSVC 2017+)

## Building

```bash
g++ -std=c++17 -o grep main.cpp Grep.cpp
```

## Usage

```
grep [options] pattern filename
grep [options] -f patternfile filename
```

Wildcards are supported in the filename argument:

```
grep pattern "*.txt"
```

## Options

| Option | Description |
|--------|-------------|
| `-i` | Case-insensitive search |
| `-v` | Invert match — print lines that do **not** match |
| `-n` | Prefix each matching line with its line number |
| `-c` | Print only the total count of matching lines |
| `-h` | Hide the filename prefix in output |
| `-E` | Treat pattern as an extended regular expression (ERE) |
| `-f file` | Read patterns from a file, one per line |

## Examples

**Basic search**
```bash
./grep Hello test.txt
# test.txt : Hello world
# test.txt : Hello there
```

**Case-insensitive search**
```bash
./grep -i hello test.txt
# test.txt : Hello world
# test.txt : Hello there
```

**Show line numbers**
```bash
./grep -n Hello test.txt
# test.txt : 1 : Hello world
# test.txt : 12 : Hello there
```

**Count matches**
```bash
./grep -c Hello test.txt
# 3
```

**Invert match**
```bash
./grep -v Hello test.txt
# test.txt : helo
# test.txt : This is an error message
# ...
```

**Extended regular expressions**
```bash
# Anchor
./grep -E "^Hello" test.txt

# Alternation
./grep -E "(Hello|helo)" test.txt

# One or more
./grep -E "hel+" test.txt

# Character class
./grep -E "^[A-Z]" test.txt
```

**Wildcard file expansion**
```bash
./grep Hello "test*.txt"
# ./test.txt : Hello world
# ./test.txt : Hello there
```

**Pattern file**
```bash
# patterns.txt contains one pattern per line
./grep -f patterns.txt test.txt
```

**Combine flags**
```bash
./grep -i -n -E "^hello" test.txt
```

## Project Structure

```
.
├── main.cpp              # Entry point, argument parsing and orchestration
├── Grep.h                # Grep class declaration
├── Grep.cpp              # Grep class implementation
└── Helper_functions.h    # Argument parsing and usage helpers

```

## How It Works

The `Grep` class owns the filename, pattern, and all option flags. On each run it:

1. Expands the filename argument against the filesystem if it contains a wildcard
2. Compiles the pattern into a `std::regex` — in basic mode the pattern is treated as a literal string (all metacharacters escaped); in `-E` mode the pattern is passed through as-is to the ECMAScript/ERE engine
3. Reads each file line by line, tests each line against the compiled regex, and prints or counts matches
4. Wraps matched text in ANSI escape codes for terminal highlighting
