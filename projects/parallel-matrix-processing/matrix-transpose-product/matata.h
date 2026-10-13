#pragma once 

#include <string.h>             // for memset()
#include <stdlib.h>
#include <stdio.h>
#include <error.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>
#include <unistd.h>
#include <assert.h>
#include <pthread.h>

#define DIFFTOL 1e-04           // tolerated difference between expect/actual answers

typedef struct {                // data type for matrices
  long rows;                    // number of rows
  long cols;                    // number of columns
  float *data;                  // actual data for matrix
} matrix_t;

typedef struct {                // data type for vectors, not used
  long len;
  float *data;
} vector_t;


#define MGET(mat,i,j) ((mat).data[((i)*((mat).cols)) + (j)])
#define VGET(vec,i)   ((vec).data[(i)])

#define MSET(mat,i,j,x) ((mat).data[((i)*((mat).cols)) + (j)] = (x))
#define VSET(vec,i,x)   ((vec).data[(i)] = (x))


// matata_util.c
int vector_init(vector_t *vec, long len);
int matrix_init(matrix_t *mat, long rows, long cols);
int matrix_copy(matrix_t *src, matrix_t *des);
void vector_free_data(vector_t *vec);
void matrix_free_data(matrix_t *mat);
int vector_read_from_file(char *fname, vector_t *vec_ref);
int matrix_read_from_file(char *fname, matrix_t *mat_ref);
void vector_write(FILE *file, vector_t vec);
void matrix_write(FILE *file, matrix_t mat, char *fmt);
void vector_fill_sequential(vector_t vec);
void matrix_fill_sequential(matrix_t mat);
void pb_srand(unsigned long seed);
void vector_fill_random(vector_t vec, int lo, int hi);
void matrix_fill_random(matrix_t mat, int lo, int hi);

int matata_BASE(matrix_t mat, matrix_t ans);
int matata_OPTM(matrix_t mat, matrix_t ans, int thread_count);
