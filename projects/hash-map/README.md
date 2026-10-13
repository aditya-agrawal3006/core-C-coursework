# Hash Map in C

A string-keyed hash map with an interactive command-line interface. The project implements hash-table operations while managing memory and table capacity directly in C.

## Features

- Insert or overwrite string key/value pairs and retrieve values by key.
- Calculate hash codes and inspect the table's contents and bucket structure.
- Expand the table to lower the load factor.
- Save entries to a file and load them back into the hash map.
- Use a prime-number helper for table sizing.

**Core concepts:** hashing, collision handling, resizing, dynamic memory, pointer manipulation, and file I/O.

## Key files

- [`hashmap_funcs.c`](hashmap_funcs.c) — hash map operations and storage utilities.
- [`hashmap.h`](hashmap.h) — table structure and API.
- [`hashmap_main.c`](hashmap_main.c) — interactive console with commands for reading and updating the map.

## Build and run

```bash
make
./hashmap_main
```

Example interactive commands:

```text
put apple 5
put banana 7
get apple
print
structure
expand
quit
```

The console also supports `save <file>`, `load <file>`, `clear`, `hashcode <key>`, and `next_prime <int>`. Run `make clean` to remove compiled output.

> University of Maryland CMSC216 coursework. Some public interfaces and CLI scaffolding were course-provided.
