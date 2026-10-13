CC ?= gcc
CFLAGS ?= -O2 -g -Wall
LDLIBS ?= -pthread -lm

.PHONY: all clean
all: el_demo

el_demo: el_demo.c el_malloc.c
	$(CC) $(CFLAGS) -o $@ el_demo.c el_malloc.c $(LDLIBS)

clean:
	rm -f el_demo *.o
