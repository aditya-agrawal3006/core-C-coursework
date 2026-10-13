CC ?= gcc
CFLAGS ?= -O2 -g -Wall
LDLIBS ?= -pthread -lm

.PHONY: all clean
all: stock_main stock_multimain

stock_main: stock_main.c stock_funcs.c
	$(CC) $(CFLAGS) -o $@ stock_main.c stock_funcs.c $(LDLIBS)

stock_multimain: stock_multimain.c stock_funcs.c
	$(CC) $(CFLAGS) -o $@ stock_multimain.c stock_funcs.c $(LDLIBS)

clean:
	rm -f stock_main stock_multimain *.o
