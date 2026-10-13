# Parallel Matrix Diagonal Sum

Computes a sum for each diagonal of a matrix. The project contains a reference implementation and optimized variants that distribute work across threads.

## Implementation

- [`sumdiag_base.c`](sumdiag_base.c) — straightforward reference calculation.
- [`sumdiag_optm.c`](sumdiag_optm.c) — optimized versions and POSIX thread worker logic.
- [`sumdiag_util.c`](sumdiag_util.c) and [`sumdiag.h`](sumdiag.h) — matrix/vector utilities and interfaces.
- [`sumdiag_print.c`](sumdiag_print.c) — prints and compares reference/optimized results.
- [`sumdiag_benchmark.c`](sumdiag_benchmark.c) — runtime comparison across matrix sizes and thread counts.

## Build and run

```bash
make
./sumdiag_print 4 2
./sumdiag_benchmark
make clean
```

`sumdiag_print <size> <thread_count>` is useful for checking small cases. The benchmark is configured for larger matrices and can take longer to finish.

**Concepts:** pthreads, work partitioning, diagonal indexing, memory access, correctness comparisons, and performance measurement.

> CMSC216 coursework; benchmark and data-generation scaffolding may be course-provided.
