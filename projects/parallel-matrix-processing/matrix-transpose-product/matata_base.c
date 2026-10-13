#include "matata.h"

// exploits symmetry, uses a transpose matrix, does not have a good
// memory access pattern and is "serial" - single threaded
int matata_BASE(matrix_t mat, matrix_t ans) {
  matrix_t tra;                                 // allocate space for transpose matrix 
  matrix_init(&tra, mat.cols, mat.rows);

  for(int i=0; i<mat.rows; i++){                // copy elements into transpose matrix
    for(int j=0; j<mat.cols; j++){
      float mij = MGET(mat, i, j);
      MSET(tra, j, i, mij);
    }
  }

  for(int i=0; i<mat.cols; i++){                // compute ans = A^T * A
    for(int j=i; j<mat.cols; j++){              // compute upper triangle elements
      float sum = 0;
      for(int k=0; k<mat.rows; k++){            // sum over dot(row, col)
        sum += MGET(tra, i, k)*MGET(mat, k, j);
      }
      MSET(ans, i, j, sum);                     // exploit symmetry: assign upper
      MSET(ans, j, i, sum);                     // trianble element to lower triangle
    }
  }

  float max = -INFINITY;                        // calculate absolute max in ans
  for(int i=0; i<mat.cols; i++){
    for(int j=0; j<mat.cols; j++){
      float mij = fabsf(MGET(ans, i, j));
      if(mij > max){
        max = mij;
      }
    }
  }
  for(int i=0; i<mat.cols; i++){                // normalize matrix by dividing
    for(int j=0; j<mat.cols; j++){              // by absolute maximum value
      float mij = MGET(ans, i, j);
      MSET(ans,i,j, mij / max);
    }
  }

  matrix_free_data(&tra);                       // de-allocate transpose mat

  return 0;                                     // return success
}
