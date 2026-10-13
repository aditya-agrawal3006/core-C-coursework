// hashmap.h: Hash Map header file
// more information


#ifndef HASHMAP_H
#define HASHMAP_H 1
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Type for linked list nodes in hash map
typedef struct hashnode {
  char key[128];                // string key for items in the map
  char val[128];                // string value for items in the map
  struct hashnode *next;        // pointer to next node, NULL if last node
} hashnode_t;

// Type of hash table
typedef struct {
  int item_count;               // how many key/val pairs in the table
  int table_size;               // how big is the table array
  hashnode_t **table;           // array of pointers to nodes which contain key/val pairs
} hashmap_t;

#define HASHMAP_DEFAULT_TABLE_SIZE 5 // default size of table for main application

// functions defined in hash_funcs.c
long  hashcode(char key[]);
int   next_prime(int num);

void  hashmap_init(hashmap_t *hm, int table_size);
int   hashmap_put(hashmap_t *hm, char key[], char value[]);
void  hashmap_expand(hashmap_t *hm);
char *hashmap_get(hashmap_t *hm, char key[]);
void  hashmap_free_table(hashmap_t *hm);

void  hashmap_write_items(hashmap_t *hm, FILE *out);
void  hashmap_show_structure(hashmap_t *hm);
void  hashmap_save(hashmap_t *hm, char *filename);
int   hashmap_load(hashmap_t *hm, char *filename);

#endif
