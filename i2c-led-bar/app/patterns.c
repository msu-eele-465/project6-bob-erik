#include <msp430.h>
#include <math.h>
#include "patterns.h"

#define LED_PORT_P1 P1OUT
#define LED_DIR_P1  P1DIR
#define LED_PORT_P2 P2OUT
#define LED_DIR_P2  P2DIR

static int phaseTime = 25000;
static unsigned char currentPattern = 0;
static const unsigned char pattern_static = 0b10101010;

void init_LED_Patterns(void) {
    LED_DIR_P1 |= BIT0 | BIT1 | BIT4 | BIT5 | BIT6 | BIT7;
    LED_DIR_P2 |= BIT6 | BIT7;
    LED_PORT_P1 &= ~(BIT0 | BIT1 | BIT4 | BIT5 | BIT6 | BIT7);
    LED_PORT_P2 &= ~(BIT6 | BIT7);
}

void set_LED_Pattern(int pattern) {
    currentPattern = pattern;
    cooling_index = 0;
    heating_index = 0;

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

        case 1: // heating
            heating_index = heating_index << 1;
            heating_index |= BIT0;
            outputToLEDs(heating_index); 
            heating_index &= ~((heating_index >> 7) * 0xFF);
            break;

        case 2: // cooling
            cooling_index = cooling_index >> 1;
            cooling_index |= BIT7;
            outputToLEDs(cooling_index); 
            cooling_index &= ~((cooling_index & ~(0b11111110)) * 0xFF);
            break;

        /*case 3:
            outputToLEDs(pattern_in_and_out[inout_index]);
            inout_index = (inout_index + 1) % 6;
            break;
        */
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