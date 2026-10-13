# Stock Price Analyzer

A command-line C program for loading historical stock prices, identifying minimum/maximum prices, finding candidate buy/sell intervals, and plotting the price series in the terminal. It also supports viewing multiple stocks together.

This project is about **array algorithms and data processing**, not live market data or an automated trading strategy.

## Features

- Parse price histories from local text files into dynamically allocated structures.
- Identify high and low prices and calculate a best historical buy/sell interval.
- Display an ASCII plot of a selected date/index range at a chosen height.
- Load multiple stock datasets and render a side-by-side comparison.
- Manage allocation and cleanup for price arrays and multi-stock collections.

**Core concepts:** parsing, arrays, dynamic memory, time-series inspection, and algorithmic price analysis.

## Key files

- [`stock_funcs.c`](stock_funcs.c) — loading, high/low and buy/sell calculations, and plotting.
- [`stock.h`](stock.h) — data structures and public functions.
- [`stock_main.c`](stock_main.c) — single-stock driver.
- [`stock_multimain.c`](stock_multimain.c) — multiple-stock driver.
- [`data/`](data/) — small sample datasets.

## Build and run

```bash
make
./stock_main data/stock-daily12-aapl_us_d.txt 15
./stock_main data/stock-daily12-aapl_us_d.txt 15 0 10
./stock_multimain 15 data/stock-ascending.txt data/stock-descending.txt
make clean
```

Single-stock arguments: `<price-file> <plot-height> [start-index] [stop-index]`. For multiple stocks: `<plot-height> <price-file>...`.

> University of Maryland CMSC216 coursework. Some drivers and supporting interfaces originated from the course.
