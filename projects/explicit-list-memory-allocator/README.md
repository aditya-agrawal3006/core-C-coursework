# Explicit Free-List Memory Allocator

A custom heap allocator that manages blocks using explicit **available** and **used** lists. It demonstrates how memory allocation works beneath `malloc`/`free` by tracking block metadata, finding reusable space, and growing the heap when needed.

## Key implementation details

- Initializes a heap-backed memory region using `mmap`.
- Maintains block headers/footers and doubly linked lists for tracking allocated and free memory.
- Searches for suitable free blocks, splits larger blocks, and coalesces adjacent free blocks.
- Implements custom `el_malloc` and `el_free` operations.
- Supports appending additional mapped pages and printing heap diagnostics.

**Core concepts:** pointer arithmetic, heap layout, linked lists, block coalescing, memory mapping, and resource cleanup.

## Key files

- [`el_malloc.c`](el_malloc.c) — allocator implementation, block-list management, and heap growth.
- [`el_malloc.h`](el_malloc.h) — allocator structs, constants, and function declarations.
- [`el_demo.c`](el_demo.c) — example allocation, deallocation, reuse, and heap-extension sequence.

## Build and run

Run on Linux (the implementation uses POSIX memory mapping):

```bash
make
./el_demo
make clean
```

The demo prints block/list statistics as it allocates, frees, and reuses memory. This is an educational allocator, **not a drop-in replacement for the system allocator**.

> University of Maryland CMSC216 coursework. Some interfaces and demonstration code were supplied as part of the assignment.
