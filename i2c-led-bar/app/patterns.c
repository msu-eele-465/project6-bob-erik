#include <msp430.h>
#include "patterns.h"

#define LED_PORT_P1 P1OUT
#define LED_DIR_P1  P1DIR
#define LED_PORT_P2 P2OUT
#define LED_DIR_P2  P2DIR

static int phaseTime = 25000;
static unsigned char currentPattern = 0;

static const unsigned char pattern_toggle[2] = {
    0b10101010,  // Step 0
    0b01010101   // Step 1
};

static const unsigned char pattern_in_and_out[6] = {
    0b00011000,  // Step 0
    0b00100100,  // Step 1
    0b01000010,  // Step 2
    0b10000001,  // Step 3
    0b01000010,  // Step 4
    0b00100100   // Step 5
};

static const unsigned char pattern_static = 0b10101010;

static unsigned char toggle_index = 0;
static unsigned char inout_index = 0;
static unsigned char up_counter = 0;

void init_LED_Patterns(void) {
    LED_DIR_P1 |= BIT0 | BIT1 | BIT4 | BIT5 | BIT6 | BIT7;
    LED_DIR_P2 |= BIT6 | BIT7;
    LED_PORT_P1 &= ~(BIT0 | BIT1 | BIT4 | BIT5 | BIT6 | BIT7);
    LED_PORT_P2 &= ~(BIT6 | BIT7);
}

void set_LED_Pattern(int pattern) {
    currentPattern = pattern;
    toggle_index = 0;
    inout_index = 0;
    up_counter = 0;

    if (pattern == 0) {
        outputToLEDs(pattern_static);
    } else {
        outputToLEDs(0x00);
    }
}

void set_Phase_Time(int time) {
    phaseTime = time;
    TB0CCR0 = phaseTime;
}

unsigned char get_Current_Pattern(void) {
    return currentPattern;
}

void update_LED(void) {
    switch (currentPattern) {
        case 0:
            outputToLEDs(pattern_static);
            break;

        case 1:
            outputToLEDs(pattern_toggle[toggle_index]);
            toggle_index = (toggle_index + 1) % 2;
            break;

        case 2:
            outputToLEDs(up_counter);
            up_counter++;
            break;

        case 3:
            outputToLEDs(pattern_in_and_out[inout_index]);
            inout_index = (inout_index + 1) % 6;
            break;

        default:
            outputToLEDs(0x00);
            break;
    }
}

void outputToLEDs(unsigned char val) {
    // Map bits to P1 and P2:
    // P1.0 (bit 0), P1.1 (bit 1), P2.6 (bit 2), P2.7 (bit 3), 
    // P1.4 (bit 4), P1.5 (bit 5), P1.6 (bit 6), P1.7 (bit 7)
    LED_PORT_P1 = (LED_PORT_P1 & ~(BIT0 | BIT1 | BIT4 | BIT5 | BIT6 | BIT7)) |
                  ((val & BIT0) ? BIT0 : 0) |
                  ((val & BIT1) ? BIT1 : 0) |
                  ((val & BIT4) ? BIT4 : 0) |
                  ((val & BIT5) ? BIT5 : 0) |
                  ((val & BIT6) ? BIT6 : 0) |
                  ((val & BIT7) ? BIT7 : 0);

    LED_PORT_P2 = (LED_PORT_P2 & ~(BIT6 | BIT7)) |
                  ((val & BIT2) ? BIT6 : 0) |
                  ((val & BIT3) ? BIT7 : 0);
}