// stock_funcs.c: support functions for the stock_main program.

#include "stock.h"

stock_t *stock_new() {
    stock_t *s = malloc(sizeof(stock_t));
    s->data_file = NULL;
    s->count = -1;
    s->prices = NULL;
    s->lo_index = -1;
    s->hi_index = -1;
    s->best_buy = -1;
    s->best_sell = -1;
    return s;
}
// PROBLEM 1: Allocate a new stock struct and initialize its fields.
// Integer fields like 'count' and 'lo_index' should be initialied to
// -1.  Pointer fields like 'prices' should be initialized to
// NULL. The stock should be heap-allocated using malloc() and
// returned.  Since this is an allocation function, no use of 'free()'
// should appear.

void stock_free(stock_t *stock){
    // Edge case
    if (stock == NULL) {
        return;
    }

    // Free fields if present
    if (stock->data_file != NULL) {
        free(stock->data_file);
    }
    if (stock->prices != NULL) {
        free(stock->prices);
    }
    // Free "stock" itself
    free(stock);
}
// PROBLEM 1: Free a stock. Check the 'data_file' and 'prices' fields:
// if they are non-NULL, then free them. Then free the pointer to
// 'stock' itself.

void stock_print(stock_t *stock){
    printf("==STOCK DATA==\n");

    if (stock->data_file==NULL) {
        printf("data_file: NULL\n");
    } else {
        printf("data_file: %s\n",stock->data_file);
    }

    printf("count: %d\n", stock->count);

    // if prices poiter is not set, then print special
    if (stock->prices == NULL) {
        printf("prices: NULL\n");
    } else {
        int n = stock->count;

        if (n <= 0) {
            printf("prices: []\n");
        } else if (n == 1) {
            printf("prices: [%.2f]\n", stock->prices[0]);
        } else if (n == 2) {
            printf("prices: [%.2f, %.2f]\n", stock->prices[0], stock->prices[1]);
        } else if (n == 3) {
            printf("prices: [%.2f, %.2f, %.2f]\n", stock->prices[0], stock->prices[1], 
                stock->prices[2]);
        } else {
            // when there are more than 3 values, show them then elipses
            printf("prices: [%.2f, %.2f, %.2f, ...]\n", stock->prices[0], 
                stock->prices[1], stock->prices[2]);
        }
    }

    printf("lo_index:  %d\n", stock->lo_index);
    printf("hi_index:  %d\n", stock->hi_index);
    printf("best_buy:  %d\n", stock->best_buy);
    printf("best_sell: %d\n", stock->best_sell);

    // show profit if best indices are valid and in-bounds
    // otherwise show 0.0
    double profit = 0.0;
    if (stock->prices != NULL && stock->best_buy  >= 0 && stock->best_sell >= 0 &&
        stock->count >= 0 && stock->best_buy < stock->count && stock->best_sell < stock->count) {
        profit = stock->prices[stock->best_sell] - stock->prices[stock->best_buy];
    }

    printf("profit:    %.2f\n", profit);
}
// PROBLEM 1: Prints data about a stock that is passed in via a
// pointer. Uses the syntax ptr->field to access fields of the struct
// pointed by 'stock'.  Output follows the general convention:
//
// ==STOCK DATA==
// data_file: data/stock-jagged.txt                      
// count: 15
// prices: [103.00, 250.00, 133.00, ...]
// lo_index:  8
// hi_index:  11
// best_buy:  8
// best_sell: 11
// profit:    232.00
//
// Each line prints a field of the stock_t struct. In all cases,
// floating point numbers are printed with 2 decimal digits of
// accuracy.
//
// NULLS FOR FIELDS
// The fields 'data_file' and 'prices' may be NULL in which case they
// will be printed specially as in
// 
// data_file: NULL
// prices: NULL
//
// This requires a manual if/else check for NULL values for these
// pointers.
// 
// PRICES FIELD
// When printing the 'prices' field, if the 'count' field is 0 to 3,
// print the entire array as in
//
// prices: []                        # count == 0
// prices: [70.00]                   # count == 1
// prices: [50.00, 90.00]            # count == 2
// prices: [59.00, 45.00, 103.00]    # count == 3
//
// Otherwise, print the first 3 elements with a ... at the end as in
//
// prices: [10.00, 20.00, 30.00, ...] # count > 3
//
// PROFIT
// There is no 'profit' field in the struct. Instead, calculate the
// profit as the difference between the price at the 'best_sell' index
// and 'best_buy' index.  If these indices are -1 indicating the best
// buy/sell time is not known or not viable, print a proit of 0.0

void stock_set_hilo(stock_t *stock){
    if (stock->count <= 0) {
        return; // if count is 0, do nothing
    }

    int lo = 0;
    int hi = 0;

    // iterate to find highs and lows
    for (int i = 1; i < stock->count; i++) {
        if (stock->prices[i] < stock->prices[lo]) {
            lo = i;
        }
        if (stock->prices[i] > stock->prices[hi]) {
            hi = i;
        }
    }

    stock->lo_index = lo;
    stock->hi_index = hi;
}
// PROBLEM 2: Sets the index of 'lo_index' and 'hi_index' fields of
// the stock to be the positions in 'prices' of the lowest and highest
// values present in it. Uses a simple loop over the array 'prices'
// which is 'count' elements long to examine each for high/low. If
// 'count' is zero, makes no changes to 'lo_index' and 'hi_index'.

int stock_set_best(stock_t *stock){
    // We need at least 2 prices/count
    if (stock->prices == NULL || stock->count <= 1) {
        stock->best_buy = -1;
        stock->best_sell = -1;
        return -1;
    }

    double best_profit = 0.0;
    int best_b = -1;
    int best_s = -1;

    // brute force method O(N^2): try every buy, every later sell
    for (int i = 0; i < stock->count - 1; i++) {
        for (int j = i + 1; j < stock->count; j++) {
            double p = stock->prices[j] - stock->prices[i];

            if (p > best_profit) {
                best_profit = p;
                best_b = i;
                best_s = j;
            } else if (p == best_profit && p > 0) {
                // tie breaker
                if (best_b == -1 || i < best_b || (i == best_b && j < best_s)) {
                    best_b = i;
                    best_s = j;
                }
            }
        }
    }

    if (best_profit > 0.0) {
        stock->best_buy = best_b;
        stock->best_sell = best_s;
        return 0;
    } else {
        stock->best_buy = -1;
        stock->best_sell = -1;
        return -1;
    }
}
// PROBLEM 2: Sets the 'best_buy' and 'best_sell' fields of 'stock'.
// This corresponds to the pair which produces the best profit. On
// determining the best buy/sell indices which produce a positive
// profit, sets these fields in 'stock' and returns 0. If there is no
// buy/sell point which would result in a positive profit, sets the
// 'best_buy' and 'best_sell' indices to -1 and returns -1. Always
// calculates the earliest buy/sell pair of indices that would get the
// best profit: if 5,8 and 5,9 and 7,10 all give the same, maximal
// profit, the best buy/sell indices are set to 5,7.
// 
// ALGORITHM NOTES
// One intuitive algorithm to compute this is to try every possible
// buy index (outer loop) and every possible sell index after it
// (inner loop) looking for the maximum profit produced in it. This is
// a O(N^2) algorithm with N=count.  Using this algorithm is a good
// start.
//
// Some MAKEUP CREDIT will be awarded for implementing a more
// efficient, O(N) algorithm but THIS FUNCTION MUST IMPLEMENT THE
// O(N^2) ALGORITHM to earn credit. Implement the O(N) version
// elsewhere for Makeup Credit.

int count_lines(char *filename){
    // opens file if possible
    FILE *fin = fopen(filename, "r");
    if (fin == NULL) {
        printf("Could not open file '%s', cannot count lines\n", filename);
        return -1;
    }

    // counts lines in file
    int c;
    int lines = 0;
    while ((c = fgetc(fin)) != EOF) {
        if (c == '\n') {
            lines++;
        }
    }

    fclose(fin);
    return lines;
}
// PROBLEM 2: Opens file named 'filename' and counts how many times
// the '\n' newline character appears in it which corresponds to how
// many lines of text are in it.  Makes use of either fscanf() with
// the %c format to read single characters or alternative I/O
// functions like fgetc(). Closes the file before returning a count of
// how many lines are it it.  If for any reason the file cannot be
// opened, prints a message like
//
//   Could not open file '<FILENAME>', cannot count lines
//
// with <FILENAME> substituted for the name of the file. Returns -1 to
// indicate failure in this case.

int stock_load(stock_t *stock, char *filename){
    // trys to count lines, if not returns error message
    int n = count_lines(filename);
    if (n < 0) {
        printf("Unable to open stock file '%s', cannot load stock data\n", filename);
        return -1;
    }

    // open for reading and displays error message
    FILE *fin = fopen(filename, "r");
    if (fin == NULL) {
        printf("Unable to open stock file '%s', cannot load stock data\n", filename);
        return -1;
    }

    // allocate memory and read them into array
    // skip the time label, then read the price
    double *arr = (double *)malloc(sizeof(double) * n);
    for (int i = 0; i < n; i++) {
        // %*s means read a string but discard it - google
        if (fscanf(fin, "%*s %lf", &arr[i]) != 1) {
            arr[i] = 0.0;
        }
    }
    fclose(fin);

    // set "stock" feilds
    if (stock->data_file != NULL) {
        free(stock->data_file);
    }
    stock->data_file = strdup(filename);
    stock->prices = arr;
    stock->count = n;

    return 0;
}
// PROBLEM 2: Loads a stock from file 'filename' into 'stock' filling
// its 'prices' and 'count' fields in. Makes use of the count_lines()
// function to determine how many lines are in the file which will
// dictate 'count' and the length of 'prices'. Allocates space in the
// heap for the stock's 'prices' array, opens the 'filename' and
// iterates through reading prices into the array. The data format for
// prices files is
//
// time_03 133.00
// time_04 143.00
// time_05 168.00
// time_06 91.00
// 
// where each line has a time as as single string and a price which is
// floating point number. The times can be ignored which can be
// accomplished with a fscanf() call with format "%*s" to read a
// string but ignore/discard it.
// 
// Assigns the 'datafile' field to be a duplicated string of
// 'filename' for which 'strdup()' is extremely useful. This string
// must be free()'d later likely in 'stock_free()'
// 
// On successfully loading the stock, returns 0.
//
// If 'filename' cannot be opened, prints the message 
// 
//   Unable to open stock file '<FILENAME>', cannot load stock data
//
// with '<FILENAME>' substituted in for the name of the stock and
// returns -1.

void stock_plot(stock_t *stock, int max_height, int start, int stop){
    // edge case    
    if (stock->prices == NULL || stock->count <= 0) {
        return;
    }

    // set start/stop value
    if (start < 0){
        start = 0;
    }
    if (stop < 0 || stop > stock->count){
        stop = stock->count;
    }
    if (start > stop) {
        start = stop;
    }

    double min_price = 0.0;
    double max_price = 0.0;

    if (stock->lo_index >= 0 && stock->lo_index < stock->count) {
        min_price = stock->prices[stock->lo_index];
    }

    if (stock->hi_index >= 0 && stock->hi_index < stock->count) {
        max_price = stock->prices[stock->hi_index];
    }

    double range = max_price - min_price;
    double step = (max_height > 0) ? (range / max_height) : 0.0;

    // header block
    printf("==PLOT DATA==\n");
    printf("start/stop:  %d %d\n", start, stop);
    printf("max_height:  %d\n", max_height);
    printf("price range: %.2f\n", range);
    printf("plot step:   %.2f\n", step);

    // top axis
    printf("%10s +", "");
    for (int i = start; i < stop; i++) {
        char ch = '-';
        if (stock->best_buy >= 0 && stock->best_sell >= 0) {
            if (i == stock->best_buy) ch = 'B';
            else if (i == stock->best_sell) ch = 'S';
            else if (i > stock->best_buy && i < stock->best_sell) ch = '=';
        }
        putchar(ch);
    }

    printf("+\n");

    // main body
    for (int h = max_height - 1; h >= 0; h--) {
        double thresh = min_price + (double)h * step;

        // left label and bar
        printf("%10.2f |", thresh);

        for (int i = start; i < stop; i++) {
            char c = ' ';
            if (stock->prices[i] >= thresh) {
                // hi/lo get letters, others are *
                if (i == stock->hi_index){
                    c = 'H';
                }
                else if (i == stock->lo_index){
                    c = 'L';    
                } 
                else{
                    c = '*';
                } 
            }
            putchar(c);
        }
        printf("|\n");
    }

    // bottom axis
    printf("%10s +", "");
    for (int i = start; i < stop; i++) {
        putchar((i % 5 == 0) ? '^' : '-');
    }
    printf("+\n");

    // numeric labels aligned to those carets
    printf("%11s", "");
    int lab = start;
    while (lab < stop && (lab % 5) != 0) {
        putchar(' ');
        lab++;
    }

    for (; lab < stop; lab += 5) {
        printf("%-5d", lab);
    }

    printf("\n");
}
// PROBLEM 2: Plots a graphical representation of stock
// information. First calculates and prints plot which is in the
// following format:
//
// ==PLOT DATA==
// start/stop:  0 15
// max_height:  14
// price range: 309.00
// plot step:   22.07
//            +--B=S----------+
//     300.93 |    H   *      |
//     278.86 | *  H   *      |
//     256.79 | *  H   *      |
//     234.71 | *  H   *      |
//     212.64 |**  H   *      |
//     190.57 |**  H * *      |
//     168.50 |**  H** *  *  *|
//     146.43 |**  H** *  ****|
//     124.36 |**  H****  ****|
//     102.29 |**  H****  ****|
//      80.21 |** *H***** ****|
//      58.14 |** *H***** ****|
//      36.07 |** *H***** ****|
//      14.00 |****H*****L****|
//            +^----^----^----+
//             0    5    10   
// 
// Here brief notes on the format with more detail in the project
// specification.
// - Parameters start, stop, and max_height are printed first along with
//   calculated information like the price_range (uses hi_index and
//   lo_index)
// - The main body of the plot is exactly max_height characters high. The
//   lowest line is the price at lo_index; the highest is hi_index minus
//   plot_step
// - The vertical axis prices can be printed with the format specifier
//   %10.2f to give the correct leading whitespace with 2 digits of
//   accuracy
// - If the hi and lo prices appear in the range, their bar is printed
//   with an H or an L while all other stocks are printed with a *
// - The top axis may include a B and an S at the positions of the
//   best_buy and best_sell index with a = between these to show the
//   period of ownership.
// - The bottom axis shows tic marks as ^ and below them index labels at
//   values divisible by 5. For the numeric index labels, it is easiest
//   to print spaces to a value divisible by 5 then use the format
//   specifier %-5d to print integers: this aligns left and prints
//   whitespace padding on the right to width 5. In a loop, one can
//   advance by +5 each time a label is printed.

// !!!!!!!!!!!int main(int argc, char *argv[]);
// PROBLEM 3: main() in stock_main.c
//
// The program must support three command line forms.
// 1. ./stock_main <stockfile> <max_height>
// 2. ./stock_main <stockfile> <max_height> <start> 
// 3. ./stock_main <stockfile> <max_height> <start> <stop>
//
// All forms have a the stock file as the first command line argument
// and the maximum plotting height as the 2nd argument. Forms 2 and 3
// optionally specify a starting time index (inclusive) and stopping
// time index (exclusive) to plot a time range for the stock prices. 
//
// NOTES
// - It is a good idea to check that the number of command line
//   arguments is either 3 (Form 1) or 4 (Form 2) or 5 (form 3) if
//   not, bail out from the program. There may or may not be an
//   automated test that checks that no action is taken when an
//   incorrect number of command line arguments appear but there will
//   definitely be tests for each of the forms indicated so a logical
//   and clean structure that attends to them is a good idea.
// - All command line arguments come into C programs as strings
//   (char*).  That means the numbers on the command line like
//   `max_height` will also be a string of characters and need to be
//   converted to an int to be used in the program.  The atoi()
//   function is useful for this: search for documentation on it and
//   use it for the conversion.
// - Make sure to check that loading stock data from a file
//   succeeds. If not, print the following error message:
//
//     Failed to load stock file, exiting
//
//   This message is in addition to the error message that is printed
//   by the stock_load() function. Return 1 from main() to indicate
//   that the program was not successful.
// - If stock data is successfully loaded, perform the requested
//   plotting using appropriate functions. Don't forget to free
//   heap-allocated memory before ending the program.
//
// NOTE: a second main() function is required for Problem 4 in a
// different file.

stock_t **stock_load_all(char *filenames[], int stock_count){
    // make an array of pointers to stocks
    stock_t **arr = malloc(sizeof(stock_t*) * stock_count);

    for (int i = 0; i < stock_count; i++) {
        arr[i] = stock_new();
        if (stock_load(arr[i], filenames[i]) != 0) {
            printf("Failed to load stock file %d\n", i);

            for (int j = 0; j <= i; j++) {
                if (arr[j] != NULL) {
                stock_free(arr[j]);
                }
            }

            free(arr);
            return NULL;
        }
    }

    return arr;
}
// PROBLEM 4: Loads all files named in `filenames[]` as stocks and
// returns an array of pointers to those stocks. The length of the
// `filenames[]` array is given in `stock_count`. Uses `stock_new() /
// stock_load()` to create and load individual stocks. If any stock
// fails to load, prints an error message
//
// Failed to load stock file <I>
//
// with <I> substituted for the index of the problematic file, then
// de-allocates all memory that has so far been allocated and returns
// NULL to indicate failures.

// max stocks supported for mult-stock printing in Problem 4
#define STOCKS_MAX_COUNT 26 

// characters to use for plotting stocks
char stock_chars[STOCKS_MAX_COUNT+1] ="abcdefghijklmnopqrstuvwxyz";

void stock_multiplot(stock_t **stocks, int stocks_count, int max_height){
    // figure stop_count (widest time) and min/max price across all
    int stop_count = 0;
    int found_any = 0;
    double min_price = 0.0;
    double max_price = 0.0;

    for (int i = 0; i < stocks_count; i++) {
        if (stocks[i]->count > stop_count) {
            stop_count = stocks[i]->count;
        }

        for (int t = 0; t < stocks[i]->count; t++) {
            double p = stocks[i]->prices[t];

            if (!found_any) {
                min_price = max_price = p;
                found_any = 1;
            } else {
                if (p < min_price) min_price = p;
                if (p > max_price) max_price = p;
            }
        }
    }

    double range = max_price - min_price;
    double step = (max_height > 0) ? (range / (double)max_height) : 0.0;

    if (step == 0.0) {
        step = 1.0;
    }

    // header block
    printf("==MULTIPLOT DATA==\n");
    printf("max_height:  %d\n", max_height);
    printf("stop_count:  %d\n", stop_count);
    printf("min price:   %.2f\n", min_price);
    printf("max price:   %.2f\n", max_price);
    printf("price range: %.2f\n", range);
    printf("plot step:   %.2f\n", step);
    printf("==LEGEND==\n");

    for (int i = 0; i < stocks_count; i++) {
        char c = stock_chars[i];
        printf("%c : %s\n", c, stocks[i]->data_file != NULL ? stocks[i]->data_file : "(null)");
    }
    printf("\n");

    // top axis
    printf("%10s +", "");
    for (int k = 0; k < stop_count * stocks_count; k++) {
        printf("-");
    }
    printf("+\n");

    // body
    for (int h = max_height - 1; h >= 0; h--) {
        double thresh = min_price + (double)h * step;
        printf("%10.2f |", thresh);

        for (int t = 0; t < stop_count; t++) {
            for (int s = 0; s < stocks_count; s++) {
                stock_t *st = stocks[s];
                if (t < st->count) {
                    double p = st->prices[t];
                    if (p >= thresh) {
                        char ch = stock_chars[s];
                        if (t == st->lo_index || t == st->hi_index) {
                            ch = (char)toupper((unsigned char)ch);
                        }
                        printf("%c", ch);
                    } else {
                        printf(" ");
                    }
                } else {
                    // this stock ended earlier, so empty col
                    printf(" ");
                }
            }
        }

        printf("|\n");
    }

    // bottom ticks: ^ every 5 time-indices (expanded by stocks_count cols)
    printf("%10s +", "");
    for (int t = 0; t < stop_count; t++) {
        if (t % 5 == 0) {
            printf("^");
            for (int k = 1; k < stocks_count; k++){
                printf("-");
            }
        } else {
            for (int k = 0; k < stocks_count; k++){
                printf("-");
            }
        }
    }
    printf("+\n");

    // numeric labels
    printf("%12s", "");
    int first = 0;

    // print labels at multiples of 5 until stop_count
    for (int t = first; t < stop_count; t += 5) {
        printf("%-5d", t);
        int pad = stocks_count * 5 - 5;
        
        for (int k = 0; k < pad; k++){
            printf(" ");
        }
    }
    printf("\n");
}
//
//
// PROBLEM 4: Plot all of the stocks in the `stocks` array which is
// `stocks_count` elements long. 
//
// EXAMPLE PLOT:
//
// ==MULTIPLOT DATA==
// max_height:  10
// stop_count:  15
// min price:   38.00
// max price:   270.00
// price range: 232.00
// plot step:   23.20
// ==LEGEND==
// a : data/stock-valley.txt
// b : data/stock-jagged.txt
//
//            +------------------------------+
//     246.80 |   b                 b B      |
//     223.60 |   b         b       b B      |
//     200.40 |   b         b       b B      |
//     177.20 |   b         b       b B      |
//     154.00 |   b     b   b       b B      |
//     130.80 |   b b b b   b       b B      |
//     107.60 |   b b b b   b       b B      |
//      84.40 |abab b b b b b    a abAB     b|
//      61.20 |abababab b b ba a a abAB   b b|
//      38.00 |abababababAbababaBababAB b b b|
//            +^---------^---------^---------+
//             0         5         10        
//
// DETAILS
//
// Plots start with the ==MULTIPLOT DATA== shown above which shows
// basic plotting parameters. The stop index, minimum price, and
// maximum price are set by finding the maximum count and min/max
// price among all stocks in the array.
//
// The ==LEGEND== shows the character which will be printed associated
// with each stock file. These are drawn from the provided
// `stock_chars[]` array with the 0th stock printed using the 0th
// character, 1th stock the 1th character, and so on. For each stock,
// the columns of the low and high prices for the stock are printed as
// upper case characters using the library `toupper()` conversion
// function to make it easier to identify the extrema for stocks.
//
// When plotting, each stock is printed in its own column. If there
// are 2 stocks, then each "time" index will have two columns as shown
// above. If there are 8 stocks, then each time index will have 8
// columns. The plotting area will be stocks_count*stop_count
// characters wide. If a stock has a lower count (shorter prices
// array) than stop_count, then blanks are printed in the columns
// beyond its count (true for the `a` stock above in the last few
// columns).
//
// If `stocks_count` is larger than `STOCKS_MAX_COUNT`, prints message
//   ERROR: stocks array length <COUNT> exceeds max <MAX>\n
// with <COUNT> and <MAX> substituted and immediately returns.
//
// NOTES
// - The `stocks` variable is an array of pointers; that means the
//   syntax `stocks[i]->field` accesses some field for the ith
//   stock. Adapt this syntax to the various cases of accessing data
//   for an individual stock.
// - Unlike the single plotting, plot parameters like the stop_count and
//   min/max price must be determined by analyzing all stocks. Use a
//   loop to iterate through to find the extreme values among stocks.
// - The plotting procedure itself will be similar to `stock_plot()`
//   except for an additional layer of looping: for time index <i>,
//   loop through all stocks and print their characters. Copying in
//   code from `stock_plot()` is a good idea and then making
//   adjustments is likely to yield good results.
// - Multiplots always start at time index 0 and go to the time index
//   of the stock with the largest prices[] count.
// - When plotting a stock, check that it has a count that is above
//   the current time index so that it will have data in its prices[]
//   array. Not all stocks in a multiplot will have the same count of
//   prices so it is possible to go out of bounds in arrays if one is
//   not careful.
// - The axis are wider: stop_count*stock_count characters wide. Use
//   this in some loops to print boundaries.
// - The axis label ticks are tricky. TRY USING PRINTF() by printing
//   the numeric label in a field of 5 characters wide, then print
//   additional 5-wide characters based stock_count to line up the
//   next location that needs a numeric label.
// - The function assumes that the high and low price indices have
//   been set for all stocks prior to them being passed in. A main()
//   is a good spot to do this. Since the best buy/sell points do not
//   show in this function, these fields can remain uninitialized not
//   affect the function's behavior.

// !!!!!!!!!!!!!!!!int main(int argc, char *argv[]);
// PROBLEM 4: main() function in stock_multimain.c: Load multiple
// stock files and plot them together.
//
// The program must support an arbitrary number of command line
// arguments according to the pattern:
// 1. ./stock_multimain <max_height> <stockfile1>
// 2. ./stock_multimain <max_height> <stockfile1> <stockfile2>
// 3. ./stock_multimain <max_height> <stockfile1> <stockfile2> <stockfile3>
// 4. etc.
// The first parameter is the maximum height to plot which remaining
// arguments as filenames containing the stock data to use in the
// multiplot.
//
// `stock_load_all()` is used to load the stock files. If something goes
// wrong, prints the error message:
//   Problems loading all stock files, exiting
// and exit with return code 1.
//
// The high/low prices are set for each stock using an appropriate
// service function and the stocks are plotted using
// `stock_multiplot()`
//
// SAMPLE OUTPUT OF STOCK_MULTIMAIN
// >> ./stock_multimain 10 data/s01.txt data/s02.txt data/s03.txt
// ==MULTIPLOT DATA==
// max_height:  10
// stop_count:  10
// min price:   1.00
// max price:   99.00
// price range: 98.00
// plot step:   9.80
// ==LEGEND==
// a : data/s01.txt
// b : data/s02.txt
// c : data/s03.txt
//
//            +------------------------------+
//      89.20 |    B                         |
//      79.40 |    B  bC               Abc  c|
//      69.60 |    B  bC               Abc  c|
//      59.80 |  c B  bC   a c         Abc bc|
//      50.00 |  c B  bC   a c       b Abc bc|
//      40.20 |a c B  bCa ca c       b Abc bc|
//      30.40 |a caB abCabca c       b Abc bc|
//      20.60 |a caB abCabcabca   b  b Abc bc|
//      10.80 |abcaB abCabcabcaBc b ab Abc bc|
//       1.00 |abcaBcabCabcabcaBcAbCabcAbcabc|
//            +^--------------^--------------+
//             0              5              
//
// NOTES
// - If fewer than 1 stock file is provided, print a usage
//   message. This case won't be tested but is instructive.
// - It is handy to use pointer arithmetic to locate where in argv[]
//   the names of stock files begin so that this portion of the array
//   can be passed directly to `stock_load_all()` and spare one the
//   need of copying strings.
// - Typically the function will load all stock files required, then
//   loop through and initialize their high/low price indices with an
//   appropriate function and then call the multi-plot function to
//   show them.  There is no need to set the best buy/sell indices in
//   this application.
// - The normal `stock_set_best()` function MUST be present with
//   Quadratic complexity (or worse)
// - The linear time version must also be present 
// - The linear time version MUST be used in `main()` and pass test cases
//   associated with running the whole program.
int stock_set_best_linear(stock_t *stock){
    return 0;
}
//
// OPTIONAL MAKEUP CREDIT: Identical to stock_set_best() except uses a
// linear time algorithm to determine the best buy/sell times