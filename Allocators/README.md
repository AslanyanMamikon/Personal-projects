# Allocators

A collection of custom C++ memory allocator implementations.

```
Allocators/
├── Custom Allocator/
│   ├── Allocator.h
│   └── Allocator.cpp
└── Pool Allocator/
    ├── Memory_pool.h
    ├── Pool_allocator.h
    └── main.cpp
```

---

## Custom Allocator

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

---

## Pool Allocator

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

---

## Design Notes

- `MemoryPool` stores the free list inline within unused blocks — zero metadata overhead per block.
- Block size is `max(sizeof(T), sizeof(Block*))` to ensure a free-list pointer always fits in a slot.
- `PoolAllocator` only supports single-element allocation (`n == 1`); requesting more throws `std::bad_alloc`.
- The pool does **not** grow — exhaustion prints a warning and returns `nullptr`. Size your pool accordingly.

---

## Building

```bash
# Pool Allocator demo
g++ -std=c++17 -o pool_demo "Pool Allocator/main.cpp" && ./pool_demo

# Custom Allocator demo
g++ -std=c++17 -o alloc_demo "Custom Allocator/Allocator.cpp" && ./alloc_demo
```
