CC ?= gcc
CFLAGS ?= -O2 -g -Wall
LDLIBS ?= -pthread -lm

.PHONY: all clean
all: matata_benchmark matata_print

matata_benchmark: matata_benchmark.c matata_util.c matata_base.c matata_optm.c
	$(CC) $(CFLAGS) -o $@ matata_benchmark.c matata_util.c matata_base.c matata_optm.c $(LDLIBS)

matata_print: matata_print.c matata_util.c matata_base.c matata_optm.c
	$(CC) $(CFLAGS) -o $@ matata_print.c matata_util.c matata_base.c matata_optm.c $(LDLIBS)

clean:
	rm -f matata_benchmark matata_print *.o
