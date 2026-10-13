# Parallel Matrix Processing

Two C projects exploring how multithreading and memory-access patterns affect matrix computations. Both retain a straightforward baseline implementation and a separate optimized version so the results can be compared.

| Project | Computation | Where to start |
| --- | --- | --- |
| [Diagonal Sum](diagonal-sum/) | Accumulates values along matrix diagonals | [README](diagonal-sum/README.md) |
| [Transpose Product](matrix-transpose-product/) | Computes `Aᵀ × A` | [README](matrix-transpose-product/README.md) |

## Engineering focus

- Divide computational work across POSIX worker threads.
- Compare optimized and reference outputs for correctness.
- Measure wall-clock runtime and investigate scaling across thread counts and matrix sizes.
- Work with contiguous matrix storage, thread arguments, and synchronization.

## Running

Build each subproject individually on Linux:

```bash
cd diagonal-sum
make
./sumdiag_print 4 2
```

See the subproject READMEs for benchmark commands and their input arguments. Performance varies by hardware and workload; no particular speedup is guaranteed.

> University of Maryland CMSC216 coursework. Some benchmark harnesses, headers, and matrix utilities were course-provided.
