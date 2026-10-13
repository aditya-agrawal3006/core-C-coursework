CC ?= gcc
CFLAGS ?= -O2 -g -Wall
LDLIBS ?= -pthread -lm

.PHONY: all clean
all: bake

bake: bake_main.c bake_funcs.c bake_util.c
	$(CC) $(CFLAGS) -o $@ bake_main.c bake_funcs.c bake_util.c $(LDLIBS)

clean:
	rm -f bake *.o
