// matata_print.c: shows results of computing A^T*A for the BASE
// and OPTM versions; useful for debugging when used with small sizes.
//
// usage: ./matata_print <SIZE>
//   <SIZE> : integer size for matrix like '3' or '7'

#include "matata.h"

int main(int argc, char *argv[]){
  if(argc < 4){
    printf("usage: %s <rows> <cols> <nthreads>\n",argv[0]);
    exit(1);
  }

  printf("==== Matrix A^T*A Print ====\n");
  long rows = atoi(argv[1]);
  long cols = atoi(argv[2]);
  long nthreads = atoi(argv[3]);
  matrix_t mat;
  matrix_t base_ans, optm_ans;

  matrix_init(&mat,rows,cols);
  matrix_fill_sequential(mat);

  matrix_init(&base_ans,cols,cols);
  matrix_init(&optm_ans,cols,cols);

  matata_BASE(mat,base_ans);
  matata_OPTM(mat,optm_ans,nthreads);

  printf("Original Matrix:\n");
  matrix_write(stdout, mat, "%6.2f ");
  printf("\n");

  printf("BASE Matrix A^T*A :\n");
  matrix_write(stdout, base_ans, "%.6f ");
  printf("\n");

  printf("OPTM Matrix A^T*A :\n");
  matrix_write(stdout, optm_ans, "%.6f ");
  printf("\n");

  printf("BASE/OPTM Element Comparison:\n");

  printf("[%3s][%3s]: %12s %12s\n","i","j","BASE","OPTM");
  for(int i=0; i<cols; i++){
    for(int j=0; j<cols; j++){
      float base_ij = MGET(base_ans,i,j);
      float optm_ij = MGET(optm_ans,i,j);
      float diff = fabsf(base_ij - optm_ij);
      char *diffstr = (diff > DIFFTOL) ? "***" : "";
      printf("[%3d][%3d]: %12.6f %12.6f %s\n",
             i,j,base_ij,optm_ij,diffstr);
    }
  }
    
  matrix_free_data(&mat);       // clean up data
  matrix_free_data(&base_ans);
  matrix_free_data(&optm_ans);
  return 0;
}
