# Battery Display Controller (C)

A low-level battery meter simulator that turns raw voltage and mode-register values into a readable battery percentage or voltage display. The controller represents each digit as a seven-segment bitmask and packs the full display into an integer.

## What the code does

- Reads simulated hardware ports and validates voltage and display mode.
- Converts the sensor reading into a `batt_t` state containing millivolts, percentage, and mode.
- Encodes numerical digits and status indicators using bit shifts, masks, and integer arithmetic.
- Updates the simulated display register and renders its result in the terminal.

**Core concepts:** bit manipulation, C structs, fixed-width values, input validation, and hardware-style register interfaces.

## Key files

- [`batt_update.c`](batt_update.c) — controller logic: `set_batt_from_ports`, `set_display_from_batt`, and `batt_update`.
- [`batt.h`](batt.h) — controller data structures and interfaces.
- [`batt_main.c`](batt_main.c) and [`batt_sim.c`](batt_sim.c) — supplied command-line simulator and display support.

## Build and run

Requires Linux, a C compiler, and `make`. From this project directory:

```bash
make
./batt_main 7000 V
./batt_main 7000 P
make clean
```

The arguments are **raw sensor units** (0.0005 volts per unit in this C simulator) and display mode: `V` for voltage or `P` for percent.

**Also implemented in assembly:** [x86-64 battery controller](https://github.com/aditya-agrawal3006/x86-64-assembly-coursework/tree/main/projects/battery-display-controller).

> University of Maryland CMSC216 coursework. The hardware simulator, interfaces, and some supporting files originated from the course.
