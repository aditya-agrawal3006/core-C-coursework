# Unix Job-Control Shell (Shellac)

An interactive shell written in C that launches programs as child processes and keeps track of their execution state. It demonstrates how a shell coordinates user input, processes, and background jobs using Unix system calls.

## Features

- Read and tokenize commands in a continuous prompt loop.
- Launch external programs using `fork` and `execvp`.
- Track jobs, update their execution status, and wait for individual jobs to finish.
- Support built-in commands: `help`, `jobs`, `wait`, `pause`, `tokens`, and `exit`.
- Manage job records and release resources on exit.

**Systems concepts:** processes, `waitpid`, file descriptors, command parsing, job-state management, and Unix I/O.

## Code layout

- [`shellac_main.c`](shellac_main.c) — shell loop, user commands, and input handling.
- [`shellac_job.c`](shellac_job.c) — process creation, execution, and per-job state.
- [`shellac_control.c`](shellac_control.c) — job table and wait/update operations.
- [`shellac_util.c`](shellac_util.c) — utility and parsing functions.
- [`shellac.h`](shellac.h) — shared definitions.

## Build and run

Requires Linux or a compatible Unix environment:

```bash
make
./shellac
```

Try entering `help`, `jobs`, `tokens one two three`, or a command such as `/bin/echo hello` at the `(shellac)` prompt. Type `exit` to close the shell. For input echoing, start it with `./shellac --echo`.

This is a learning-oriented shell, not a complete replacement for Bash.

> University of Maryland CMSC216 coursework; some shell interfaces and supporting code were provided with the assignment.
