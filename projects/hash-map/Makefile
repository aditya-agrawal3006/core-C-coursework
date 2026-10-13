CC ?= gcc
CFLAGS ?= -O2 -g -Wall
LDLIBS ?= -pthread -lm

.PHONY: all clean
all: hashmap_main

hashmap_main: hashmap_main.c hashmap_funcs.c
	$(CC) $(CFLAGS) -o $@ hashmap_main.c hashmap_funcs.c $(LDLIBS)

clean:
	rm -f hashmap_main *.o
