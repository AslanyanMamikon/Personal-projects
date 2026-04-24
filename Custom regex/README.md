# Regex Engine

A custom regular expression engine written in C++ that compiles patterns directly into a Deterministic Finite Automaton (DFA) using the classic **followpos algorithm** from the Dragon Book (Aho, Lam, Sethi, Ullman).

## How It Works

The engine avoids the NFA intermediate representation entirely. Instead, it takes a three-step approach:

```
Regex Pattern  →  AST  →  followpos sets  →  DFA  →  Match
```

1. **Parse** — A recursive-descent parser reads the pattern and builds an Abstract Syntax Tree (AST) with nodes for `CHAR`, `OR`, `CONCAT`, `STAR`, and a synthetic `END` marker.
2. **Annotate** — Each leaf node in the AST is assigned `firstpos`, `lastpos`, and `followpos` sets that encode which positions can follow which in a valid match.
3. **Build DFA** — States are subsets of AST leaf positions. Transitions are derived from `followpos`. A state is accepting if it contains the `END` marker node.
4. **Match** — The compiled DFA is walked character by character. O(n) in input length, no backtracking.

## Supported Syntax

| Syntax | Meaning | Example |
|--------|---------|---------|
| `a` | Literal character | `abc` matches `"abc"` |
| `\|` | Alternation | `a\|b` matches `"a"` or `"b"` |
| `*` | Kleene star (zero or more) | `a*` matches `""`, `"a"`, `"aa"`, … |
| `(…)` | Grouping | `(ab)*` matches `""`, `"ab"`, `"abab"`, … |

Operators can be freely combined: `a(b|c)*d`, `(a|b)*c`, etc.

## Project Structure

```
.
├── ASTNode.h / .cpp      # AST node: type, char, nullable, firstpos, lastpos, followpos
├── DFAState.h / .cpp     # DFA state: position set, accept flag, transition map
├── Regex.h / .cpp        # Parser + followpos algorithm + DFA construction + Match
├── RegexStream.h / .cpp  # Character-by-character stream over the pattern string
└── main.cpp              # Test suite
```

## Building

Requires a C++17-compatible compiler.

```bash
g++ -std=c++17 -o regex_test ASTNode.cpp DFAState.cpp RegexStream.cpp Regex.cpp main.cpp
./regex_test
```

## Usage

```cpp
#include "Regex.h"

Regex regex;

if (regex.Compile("a(b|c)*d")) {
    regex.Match("abbcd");  // true
    regex.Match("ad");     // true
    regex.Match("ab");     // false
}

// Reuse the same object for multiple patterns
regex.Compile("(a|b)*c");
regex.Match("abc");        // true
```

`Compile` can be called multiple times on the same object — it cleans up the previous DFA before building a new one.

## Algorithm Notes

- **followpos** is computed during tree construction, not as a post-pass, so each node factory method (`makeConcat`, `makeOr`, `makeStar`) sets its own sets immediately.
- **DFA state deduplication** uses a `std::map<std::set<ASTNode*>, DFAState*>` for O(log n) lookup instead of a linear scan.
- **Initial state** is tracked by pointer so the engine is safe to reuse across multiple `Compile` calls, even though `DFAState` uses a static global counter for indices.

## References

- Aho, Lam, Sethi, Ullman — *Compilers: Principles, Techniques, and Tools* (2nd ed.), §3.9
