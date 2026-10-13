#include "stock.h"

int main(int argc, char *argv[]) {
  int max_height = atoi(argv[1]);
  int file_count = argc - 2;
  char **files = &argv[2];

  stock_t **arr = stock_load_all(files, file_count);
  if (arr == NULL) {
    printf("Problems loading all stock files, exiting\n");
    return 1;
  }

  // set hi/lo for each stock
  for (int i = 0; i < file_count; i++) {
    stock_set_hilo(arr[i]);
  }

  // plot
  stock_multiplot(arr, file_count, max_height);

  // free memoy
  for (int i = 0; i < file_count; i++) {
    stock_free(arr[i]);
  }
  
  free(arr);
  return 0;
}