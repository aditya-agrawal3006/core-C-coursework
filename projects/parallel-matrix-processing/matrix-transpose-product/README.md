# Parallel Matrix Transpose Product (`AᵀA`)

Calculates the matrix product of a matrix transpose and the original matrix. A baseline implementation provides a correctness reference; the optimized version uses POSIX threads to parallelize matrix work.

## Implementation

- [`matata_base.c`](matata_base.c) — baseline `AᵀA` calculation.
- [`matata_optm.c`](matata_optm.c) — parallelized implementation and worker coordination.
- [`matata_util.c`](matata_util.c) and [`matata.h`](matata.h) — matrix storage and shared interfaces.
- [`matata_print.c`](matata_print.c) — prints baseline/optimized matrices and flags differing entries.
- [`matata_benchmark.c`](matata_benchmark.c) — compares performance at multiple matrix sizes.

## Build and run

```bash
make
./matata_print 3 3 2
./matata_benchmark
make clean
```

`matata_print <rows> <cols> <thread_count>` is a practical small-case check before running the larger benchmark. Floating-point results are compared with a tolerance defined in the shared header.

**Concepts:** matrix multiplication, row/column indexing, thread partitioning, cache-aware computation, and numerical verification.

> CMSC216 coursework; supporting matrix utilities and benchmark scaffolding may be course-provided.
