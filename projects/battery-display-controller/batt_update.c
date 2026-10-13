#include "batt.h"
#include <stdio.h>


/* Create an array of bit masks for each of the digits 0-9. 
The 0th element of the array contains a bit mask like 0b0111111 
which represents the bits that should be set for a 0 digit, 
the 1th element of this array has a mask like 0b0000110 which are the bits to be set for a 1. 
There should be ten entries in this array in indices 0-9. */
static int DIGITS[10] = {
    0b0111111, // 0
    0b0000110, // 1
    0b1011011, // 2
    0b1001111, // 3
    0b1100110, // 4
    0b1101101, // 5
    0b1111101, // 6
    0b0000111, // 7
    0b1111111, // 8
    0b1101111  // 9
};




int set_batt_from_ports(batt_t *batt){
    short port = BATT_VOLTAGE_PORT;
    // edge case
    if(port < 0){
        return 1;
    } 

    batt_t tmp; // temp variable
    tmp.mlvolts = (short)(port >> 1);
    int mv = tmp.mlvolts;
    int pct = 0;

    // Calculates the percent of battery
    if(mv > 3000){
        pct = (mv - 3000) >> 3;
        if(pct > 100){
            pct = 100;
        }
    }
    tmp.percent = (char)pct;

    // set mode to volts or percent
    if ((BATT_STATUS_PORT >> 4) & 1) {
        tmp.mode = 1;
    } else {
        tmp.mode = 2;
    }

    // set batt struct to tmp
    *batt = tmp;
    return 0;
}
// Uses the two global variables (ports) BATT_VOLTAGE_PORT and
// BATT_STATUS_PORT to set the fields of the parameter 'batt'.  If
// BATT_VOLTAGE_PORT is negative, then battery has been wired wrong;
// no fields of 'batt' are changed and 1 is returned to indicate an
// error.  Otherwise, sets fields of batt based on reading the voltage
// value and converting to precent using the provided formula. Returns
// 0 on a successful execution with no errors. This function DOES NOT
// modify any global variables but may access global variables.
//
// CONSTRAINT: Avoids the use of the division operation as much as
// possible. Makes use of shift operations in place of division where
// possible.
//
// CONSTRAINT: Uses only integer operations. No floating point
// operations are used as the target machine does not have a FPU.
// 
// CONSTRAINT: Limit the complexity of code as much as possible. Do
// not use deeply nested conditional structures. Seek to make the code
// as short, and simple as possible. Code longer than 40 lines may be
// penalized for complexity.





int set_display_from_batt(batt_t batt, int *display){
    int disp = 0;
    int p = batt.percent;
    int bars = 0;

    // checks how many bars to disply
    if (p >= 5) {
        bars |= 1;
    }
    if (p >= 30) {
        bars |= 2;
    }
    if (p >= 50) {
        bars |= 4;
    }
    if (p >= 70) {
        bars |= 8;
    }
    if (p >= 90) {
        bars |= 16;
    }
    // shift all 5 bits left into display bits
    disp |= (bars << 24);

    if(batt.mode == 1){
        // Percent mode
        disp |= 1;
        int hundreds = p / 100;
        int tens = (p / 10) % 10;
        int ones = p % 10;

        // left bits 17..23, middle 10..16, right 3..9
        if(hundreds){
            disp |= (DIGITS[hundreds] << 17);
        } 
        if(hundreds || tens) {
            disp |= (DIGITS[tens] << 10);
        }
        disp |= (DIGITS[ones] << 3);
    } else {
        // Volts mode: set 'V' and decimal point
        disp = disp | (1 << 1);
        disp = disp | (1 << 2);

        // round mV to nearest 10 mV
        int r10 = (batt.mlvolts + 5) / 10;
        int left  = r10 / 100; // interger
        int mid   = (r10 / 10) % 10; // tenths
        int right = r10 % 10; // hundredths
        disp |= (DIGITS[left]  << 17);
        disp |= (DIGITS[mid]   << 10);
        disp |= (DIGITS[right] << 3);
    }

    *display = disp;
    return 0;
}
// Alters the bits of integer pointed to by 'display' to reflect the
// data in struct param 'batt'.  Does not assume any specific bit
// pattern stored at 'display' and completely resets all bits in it on
// successfully completing.  Selects either to show Percent (mode=1) or
// Volts (mode=2). If Volts are displayed, only displays 3 digits
// rounding the lowest digit up or down appropriate to the last digit.
// Calculates each digit to display changes bits at 'display' to show
// the volts/percent according to the pattern for each digit. Modifies
// additional bits to show a decimal place for volts and a 'V' or '%'
// indicator appropriate to the mode. In both modes, places bars in
// the level display as indicated by percentage cutoffs in provided
// diagrams. This function DOES NOT modify any global variables but
// may access global variables. Always returns 0.
// 
// CONSTRAINT: Limit the complexity of code as much as possible. Do
// not use deeply nested conditional structures. Seek to make the code
// as short, and simple as possible. Code longer than 65 lines may be
// penalized for complexity.


int batt_update(){
    batt_t b;
    int disp;

    // Read from ports and fill the struct
    if (set_batt_from_ports(&b)) {
        return 1;
    }

    set_display_from_batt(b, &disp);
    BATT_DISPLAY_PORT = disp;

    return 0;
}
// Called to update the battery meter display.  Makes use of
// set_batt_from_ports() and set_display_from_batt() to access battery
// voltage sensor then set the display. Checks these functions and if
// they indicate an error, does NOT change the display and returns 1.
// If functions succeed, modifies BATT_DISPLAY_PORT to show current
// battery level and returns 0.
// 
// CONSTRAINT: Does not allocate any heap memory as malloc() is NOT
// available on the target microcontroller.  Uses stack and global
// memory only.