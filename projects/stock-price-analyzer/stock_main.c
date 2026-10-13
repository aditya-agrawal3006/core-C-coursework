#include "stock.h"

int main(int argc, char *argv[]){
    char *fname = argv[1];
    int max_height = atoi(argv[2]);

    // defaults for range
    int start = 0;
    if (argc >= 4) {
        start = atoi(argv[3]);
    }

    stock_t *st = stock_new();
    if (stock_load(st, fname) != 0) {
        printf("Failed to load stock file, exiting\n");
        stock_free(st);
        return 1;
    }

    // compute hi/lo and best buy/sell. if no good trade, then print that
    stock_set_hilo(st);
    if (stock_set_best(st) < 0) {
        printf("No viable buy/sell point\n");
    }

    // show the fields
    stock_print(st);

    // figure out stop
    int stop = st->count;
    if (argc == 5) {
        stop = atoi(argv[4]);
    }

    if (start < 0){
        start = 0;
    }
    if (start > st->count) {
        start = st->count;
    }
    if (stop < 0) {
        stop = 0;
    }
    if (stop > st->count) {
        stop = st->count;
    }
    if (stop < start) {
        stop = start;
    }

    // draw the plot
    stock_plot(st, max_height, start, stop);

    // free heap
    stock_free(st);
    return 0;
}