CC ?= gcc
CFLAGS ?= -O2 -g -Wall
LDLIBS ?= -pthread -lm

.PHONY: all clean
all: shellac

shellac: shellac_main.c shellac_control.c shellac_job.c shellac_util.c
	$(CC) $(CFLAGS) -o $@ shellac_main.c shellac_control.c shellac_job.c shellac_util.c $(LDLIBS)

clean:
	rm -f shellac *.o
