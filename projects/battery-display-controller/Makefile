CC ?= gcc
CFLAGS ?= -O2 -g -Wall
LDLIBS ?= -pthread -lm

.PHONY: all clean
all: batt_main

batt_main: batt_main.c batt_sim.c batt_update.c
	$(CC) $(CFLAGS) -o $@ batt_main.c batt_sim.c batt_update.c $(LDLIBS)

clean:
	rm -f batt_main *.o
