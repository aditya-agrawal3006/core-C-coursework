#include <stdio.h>
#include <string.h>
#include "hashmap.h"

int main(int argc, char *argv[])
{
    int echo = 0; // controls echoing, 0: echo off, 1: echo on
    if (argc > 1 && strcmp("-echo", argv[1]) == 0)
    { // turn echoing on via -echo command line option
        echo = 1;
    }

    printf("Hashmap Main\n");
    printf("Commands:\n");
    printf("  hashcode <key>   : prints out the numeric hash code for the given key (does not change the hash map)\n");
    printf("  put <key> <val>  : inserts the given key/val into the hash map, overwrites existing values if present\n");
    printf("  get <key>        : prints the value associated with the given key or NOT FOUND\n");
    printf("  print            : shows contents of the hashmap ordered by how they appear in the table\n");
    printf("  structure        : prints detailed structure of the hash map\n");
    printf("  clear            : reinitializes hash map to be empty with default size\n");
    printf("  save <file>      : writes the contents of the hash map the given file\n");
    printf("  load <file>      : clears the current hash map and loads the one in the given file\n");
    printf("  next_prime <int> : if <int> is prime, prints it, otherwise finds the next prime and prints it\n");
    printf("  expand           : expands memory size of hashmap to reduce its load factor\n");
    printf("  quit             : exit the program\n");

    hashmap_t hm;
    hashmap_init(&hm, HASHMAP_DEFAULT_TABLE_SIZE);

    char cmd[128]; // character array (string), used for the command and any command args
    int success;   // capture results of running commands on the hash map

    while (1)
    {
        printf("HM> ");                     // print prompt
        success = fscanf(stdin, "%s", cmd); // read a command
        if (success == EOF)
        {                 // check for end of input
            printf("\n"); // found end of input
            break;        // break from loop
        }

        if (strcmp("quit", cmd) == 0)
        { // check for exit command
            if (echo)
            {
                printf("quit\n");
            }
            break; // break from loop
        }

        else if (strcmp("put", cmd) == 0)
        { // insertion
            char key[128];
            char val[128];
            fscanf(stdin, "%s %s", key, val); // read key and value
            if (echo)
            {
                printf("put %s %s\n", key, val);
            }

            success = hashmap_put(&hm, key, val); // call hash map function
            if (success == 0)
            {
                printf("Overwriting previous key/val\n");
            }
        }

        else if (strcmp("get", cmd) == 0)
        { // get command
            char key[128];
            fscanf(stdin, "%s", key); // read a string
            if (echo)
            {
                printf("get %s\n", key);
            }

            char *ith = hashmap_get(&hm, key); // call hash map function
            if (ith == NULL)
            { // check for success
                printf("NOT FOUND\n");
            }
            else
            {
                printf("FOUND: %s\n", ith);
            }
        }

        else if (strcmp("structure", cmd) == 0)
        { // structure command
            if (echo)
            {
                printf("structure\n");
            }

            hashmap_show_structure(&hm);
        }

        else if (strcmp("expand", cmd) == 0)
        { // expand command
            if (echo)
            {
                printf("expand\n");
            }

            hashmap_expand(&hm);
        }

        else if (strcmp("hashcode", cmd) == 0)
        { // hashcode command
            fscanf(stdin, "%s", cmd);
            if (echo)
            {
                printf("hashcode %s\n", cmd);
            }

            printf("%ld\n", hashcode(cmd));
        }

        else if (strcmp("clear", cmd) == 0)
        { // clear command
            if (echo)
            {
                printf("clear\n");
            }
            hashmap_free_table(&hm);
            hashmap_init(&hm, HASHMAP_DEFAULT_TABLE_SIZE);
        }

        else if (strcmp("print", cmd) == 0)
        { // print command
            if (echo)
            {
                printf("print\n");
            }
            hashmap_write_items(&hm, stdout);
        }

        else if (strcmp("save", cmd) == 0)
        { // save command
            char filename[128];
            fscanf(stdin, "%s", filename);
            if (echo)
            {
                printf("save %s\n", filename);
            }

            hashmap_save(&hm, filename);
        }

        else if (strcmp("load", cmd) == 0)
        { // load command
            char filename[128];
            fscanf(stdin, "%s", filename);
            if (echo)
            {
                printf("load %s\n", filename);
            }

            success = hashmap_load(&hm, filename);
            if (!success)
            {
                printf("load failed\n");
            }
        }

        else if (strcmp("next_prime", cmd) == 0)
        { // next_prime command
            int num;
            fscanf(stdin, "%d", &num);
            if (echo)
            {
                printf("next_prime %d\n", num);
            }

            printf("%d\n", next_prime(num));
        }

        else
        { // unknown command
            if (echo)
            {
                printf("%s\n", cmd);
            }
            printf("unknown command %s\n", cmd);
        }
    }
    // end main while loop

    hashmap_free_table(&hm);
    return 0;
}
