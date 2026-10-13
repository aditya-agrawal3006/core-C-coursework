# Build Automation Tool — Bake

A small Make-inspired build utility written in C. It reads rules from a `Bakefile`, resolves dependencies, determines which targets need rebuilding, and launches the corresponding build commands.

## How it works

1. Parse rules, targets, prerequisites, and commands from the selected build file.
2. Add supported implicit rules and resolve dependency relationships.
3. Detect cycles in the requested dependency chain.
4. Check whether a target needs rebuilding based on its dependencies and file state.
5. Execute the necessary commands through child processes and report the number of updates.

**Systems concepts:** filesystem metadata, dependency graphs, recursion, `fork`, `execvp`, `waitpid`, and error handling.

## Key files

- [`bake_main.c`](bake_main.c) — command-line options and overall update workflow.
- [`bake_funcs.c`](bake_funcs.c) — dependency checks, cycle detection, update scheduling, and command execution.
- [`bake_util.c`](bake_util.c) — Bakefile parsing and supporting utilities.
- [`bake.h`](bake.h) — shared rule and command structures.

## Build and run

From this project directory on Linux:

```bash
make
./bake
./bake -f path/to/Bakefile
./bake -f path/to/Bakefile target_name
make clean
```

Provide a `Bakefile` in the working directory (or pass `-f` with a path). The default target is taken from its first rule if you do not specify one.

> University of Maryland CMSC216 coursework; parsing utilities, interfaces, and other scaffolding may include course-provided code.
