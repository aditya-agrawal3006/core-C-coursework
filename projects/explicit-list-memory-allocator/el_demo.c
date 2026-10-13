// el_demo.c: Shows use cases for el_malloc() and el_free(). This file
// can be used for testing but is not itself a test.

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "el_malloc.h"

void print_ptr(char *str, void *ptr){
  if(ptr == NULL){
    printf("%s: (nil)\n", str);
  }
  else{
    printf("%s: %p\n", str, ptr);
  }
}

int main(){
  printf("EL_BLOCK_OVERHEAD: %lu\n",EL_BLOCK_OVERHEAD);
  el_init(EL_HEAP_DEFAULT_SIZE);

  printf("INITIAL\n"); el_print_stats(); printf("\n");

  void *p1 = el_malloc(128);
  void *p2 = el_malloc(48);
  void *p3 = el_malloc(156);
  printf("MALLOC 3\n"); el_print_stats(); printf("\n");

  printf("POINTERS\n");
  print_ptr("p3",p3);
  print_ptr("p2",p2);
  print_ptr("p1",p1);
  printf("\n");

  void *p4 = el_malloc(22);
  void *p5 = el_malloc(64);
  printf("MALLOC 5\n"); el_print_stats(); printf("\n");

  printf("POINTERS\n");
  print_ptr("p5",p5);
  print_ptr("p4",p4);
  print_ptr("p3",p3);
  print_ptr("p2",p2);
  print_ptr("p1",p1);
  printf("\n");

  el_free(p1);
  printf("FREE 1\n"); el_print_stats(); printf("\n");

  el_free(p3);
  printf("FREE 3\n"); el_print_stats(); printf("\n");

  p3 = el_malloc(32);
  p1 = el_malloc(200);
  
  printf("ALLOC 3,1 AGAIN\n"); el_print_stats(); printf("\n");

  printf("POINTERS\n");
  print_ptr("p1",p1);
  print_ptr("p3",p3);
  print_ptr("p5",p5);
  print_ptr("p4",p4);
  print_ptr("p2",p2);
  printf("\n");

  el_free(p1);

  printf("FREE'D 1\n"); el_print_stats(); printf("\n");

  el_free(p2);

  printf("FREE'D 2\n"); el_print_stats(); printf("\n");

  p1 = el_malloc(3438);
  p2 = el_malloc(1024);
  printf("P2 FAILS\n");
  printf("POINTERS\n");
  print_ptr("p1",p1);
  print_ptr("p3",p3);
  print_ptr("p5",p5);
  print_ptr("p4",p4);
  print_ptr("p2",p2);
  el_print_stats(); printf("\n");

  el_append_pages_to_heap(3);
  printf("APPENDED PAGES\n"); el_print_stats(); printf("\n");

  p2 = el_malloc(1024);
  printf("P2 SUCCEEDS\n");
  printf("POINTERS\n");
  print_ptr("p1",p1);
  print_ptr("p3",p3);
  print_ptr("p5",p5);
  print_ptr("p4",p4);
  print_ptr("p2",p2);
  el_print_stats(); printf("\n");  

  el_free(p1);
  el_free(p2);
  el_free(p3);
  el_free(p4);
  el_free(p5);

  printf("FREE'D 1-5\n"); el_print_stats(); printf("\n");

  el_cleanup();
  return 0;
}
  
