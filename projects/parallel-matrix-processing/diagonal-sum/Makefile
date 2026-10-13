CC ?= gcc
CFLAGS ?= -O2 -g -Wall
LDLIBS ?= -pthread -lm

.PHONY: all clean
all: sumdiag_benchmark sumdiag_print

sumdiag_benchmark: sumdiag_benchmark.c sumdiag_util.c sumdiag_base.c sumdiag_optm.c
	$(CC) $(CFLAGS) -o $@ sumdiag_benchmark.c sumdiag_util.c sumdiag_base.c sumdiag_optm.c $(LDLIBS)

sumdiag_print: sumdiag_print.c sumdiag_util.c sumdiag_base.c sumdiag_optm.c
	$(CC) $(CFLAGS) -o $@ sumdiag_print.c sumdiag_util.c sumdiag_base.c sumdiag_optm.c $(LDLIBS)

clean:
	rm -f sumdiag_benchmark sumdiag_print *.o
