# Personal Projects

A collection of C++ systems programming projects.

```
Personal Projects/
├── Allocators/
│   ├── Custom Allocator/
│   │   ├── Allocator.h
│   │   └── Allocator.cpp
│   └── Pool Allocator/
│       ├── Memory_pool.h
│       ├── Pool_allocator.h
│       └── main.cpp
├── Custom Regex/
│   ├── ASTNode.h
│   ├── ASTNode.cpp
│   ├── DFAState.h
│   ├── DFAState.cpp
│   ├── Regex.h
│   ├── Regex.cpp
│   ├── RegexStream.h
│   ├── RegexStream.cpp
│   └── main.cpp
└── Grep/
    ├── main.cpp
    ├── Grep.h
    ├── Grep.cpp
    └── Helper_functions.h
```

---

## [Allocators](Allocators/README.md)

### Custom Allocator

A minimal general-purpose allocator modelled after the C++ standard allocator interface (`std::allocator`).

**Features:**
- `allocate(n)` — allocates raw memory for `n` objects via `::operator new`
- `deallocate(p, n)` — releases memory via `::operator delete`
- `construct(p, args...)` — placement-new constructs an object in-place
- `destroy(p)` — explicitly calls the destructor
- `rebind<U>` — allows reuse of the allocator for a different type

**Usage:**
```cpp
Allocator<int> alloc;
int* p = alloc.allocate(5);
for (int i = 0; i < 5; i++) alloc.construct(p + i, i);
// ...
for (int i = 0; i < 5; i++) alloc.destroy(p + i);
alloc.deallocate(p, 5);
```

**Building:**
```bash
g++ -std=c++17 -o alloc_demo "Custom Allocator/Allocator.cpp" && ./alloc_demo
```

---

### Pool Allocator

A fixed-size memory pool that pre-allocates a contiguous block of memory and manages it via a singly-linked free list, enabling O(1) allocation and deallocation with zero fragmentation.

Consists of two components:
- **`MemoryPool<T>`** — the pool itself, managing raw block allocation
- **`PoolAllocator<T>`** — a thin STL-compatible adapter wrapping `MemoryPool<T>`

**Features:**
- Pre-allocates a pool of `N` blocks at construction time
- O(1) `allocate()` and `deallocate()` using an intrusive free list
- `construct` / `destroy` for explicit object lifetime management
- `newElement(args...)` / `deleteElement(p)` — convenience wrappers on `MemoryPool`
- `PoolAllocator<T>` exposes the standard allocator interface for STL compatibility

**Usage:**
```cpp
MemoryPool<int> pool(10);        // pre-allocate 10 slots
PoolAllocator<int> alloc(&pool);
int* p = alloc.allocate(1);
alloc.construct(p, 42);
std::cout << *p;                 // 42
alloc.destroy(p);
alloc.deallocate(p, 1);
```

**Design Notes:**
- `MemoryPool` stores the free list inline within unused blocks — zero metadata overhead per block.
- Block size is `max(sizeof(T), sizeof(Block*))` to ensure a free-list pointer always fits in a slot.
- `PoolAllocator` only supports single-element allocation (`n == 1`); requesting more throws `std::bad_alloc`.
- The pool does **not** grow — exhaustion prints a warning and returns `nullptr`. Size your pool accordingly.

**Building:**
```bash
g++ -std=c++17 -o pool_demo "Pool Allocator/main.cpp" && ./pool_demo
```

---

## [Regex Engine](Custom%20regex/README.md)

A custom regular expression engine written in C++ that compiles patterns directly into a Deterministic Finite Automaton (DFA) using the classic followpos algorithm from the Dragon Book (Aho, Lam, Sethi, Ullman).

**How It Works:**

```
Regex Pattern  →  AST  →  followpos sets  →  DFA  →  Match
```

1. **Parse** — A recursive-descent parser reads the pattern and builds an AST with nodes for `CHAR`, `OR`, `CONCAT`, `STAR`, and a synthetic `END` marker.
2. **Annotate** — Each leaf node is assigned `firstpos`, `lastpos`, and `followpos` sets encoding which positions can follow which in a valid match.
3. **Build DFA** — States are subsets of AST leaf positions. Transitions are derived from `followpos`. A state is accepting if it contains the `END` marker node.
4. **Match** — The compiled DFA is walked character by character — O(n) in input length, no backtracking.

**Supported Syntax:**

| Syntax | Meaning | Example |
|--------|---------|---------|
| `a` | Literal character | `abc` matches `"abc"` |
| `\|` | Alternation | `a\|b` matches `"a"` or `"b"` |
| `*` | Kleene star (zero or more) | `a*` matches `""`, `"a"`, `"aa"`, … |
| `(…)` | Grouping | `(ab)*` matches `""`, `"ab"`, `"abab"`, … |

Operators can be freely combined: `a(b|c)*d`, `(a|b)*c`, etc.

**Usage:**
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

**Algorithm Notes:**
- `followpos` is computed during tree construction, not as a post-pass, so each node factory method (`makeConcat`, `makeOr`, `makeStar`) sets its own sets immediately.
- DFA state deduplication uses a `std::map<std::set<ASTNode*>, DFAState*>` for O(log n) lookup instead of a linear scan.
- Initial state is tracked by pointer so the engine is safe to reuse across multiple `Compile` calls.

**Building:**
```bash
g++ -std=c++17 -o regex_test ASTNode.cpp DFAState.cpp RegexStream.cpp Regex.cpp main.cpp
./regex_test
```

**References:**
- Aho, Lam, Sethi, Ullman — *Compilers: Principles, Techniques, and Tools* (2nd ed.), §3.9

---

## [Grep](Grep/README.md)

A command-line search tool implemented in C++ that replicates core `grep` functionality. Searches for a pattern in one or more files and prints matching lines, with support for extended regular expressions, wildcard file expansion, and ANSI color highlighting.

**Features:**
- Literal and extended regular expression (ERE) search
- Case-insensitive matching
- Inverted matching (print non-matching lines)
- Line number display
- Match count reporting
- Wildcard file expansion (e.g. `*.txt`)
- Multi-pattern search via a pattern file
- ANSI color highlighting of matched text in the terminal
- CRLF-safe pattern file parsing (Windows line endings handled)

**Requirements:** C++17 or later, a compiler with `std::filesystem` support (GCC 8+, Clang 7+, MSVC 2017+).

**Building:**
```bash
g++ -std=c++17 -o grep main.cpp Grep.cpp
```

**Usage:**
```
grep [options] pattern filename
grep [options] -f patternfile filename
```

Wildcards are supported in the filename argument:
```
grep pattern "*.txt"
```

**Options:**

| Option | Description |
|--------|-------------|
| `-i` | Case-insensitive search |
| `-v` | Invert match — print lines that do not match |
| `-n` | Prefix each matching line with its line number |
| `-c` | Print only the total count of matching lines |
| `-h` | Hide the filename prefix in output |
| `-E` | Treat pattern as an extended regular expression (ERE) |
| `-f file` | Read patterns from a file, one per line |

**Examples:**
```bash
# Basic search
./grep Hello test.txt

# Case-insensitive + line numbers
./grep -i -n hello test.txt

# Extended regex — alternation
./grep -E "(Hello|helo)" test.txt

# Wildcard expansion
./grep Hello "test*.txt"

# Pattern file
./grep -f patterns.txt test.txt
```

**How It Works:**

The `Grep` class owns the filename, pattern, and all option flags. On each run it:
1. Expands the filename argument against the filesystem if it contains a wildcard
2. Compiles the pattern into a `std::regex` — literal in basic mode (metacharacters escaped), as-is in `-E` mode
3. Reads each file line by line, tests each line against the compiled regex, and prints or counts matches
4. Wraps matched text in ANSI escape codes for terminal highlighting
