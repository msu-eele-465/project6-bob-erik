

#include <msp430.h>
#include "lcd_shared.h"
#include "lcd_commands.h"
#include <stdint.h>
#include <string.h>
#include <stdint.h>

volatile uint8_t CursorState = 0; // tracks cursor current state
volatile uint8_t SendNextChar = 0;
volatile unsigned char next_char = 0x00 ;
volatile unsigned char next_location = 0x00;

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;               // Stop watchdog timer
    P2OUT &= ~0b11000000;                   // 2.7, 2.6, 1.1, 1.0 will be MSB-LSB of 4 write bits
    P2DIR |= 0b11000000;
    P1OUT &= ~0b00110011;                    // P1.5 is Enable Pin
    P1DIR |= 0b00110011;                    // P1.4 is RS pin

    TB0CCTL0 |= CCIE;                            //CCIE enables Timer B0 interrupt
    TB0CCR0 = 4000;                            //sets Timer B0 to 1 second (32.768 kHz)
    TB0CTL |= TBSSEL_1 | ID_0 | MC__UP | TBCLR;    //ACLK, No divider, Up mode, Clear timer

    __enable_interrupt(); 

    // Disable low-power mode / GPIO high-impedance
    PM5CTL0 &= ~LOCKLPM5;


    initLCD();
    clearLCD();

    while(1)
    {   
        if (SendNextChar == 1) {
            SendNextChar = 0;
            goToDDRLCD(next_location);
            writeChar(next_char);
        }
    }
}

#pragma vector = TIMER0_B0_VECTOR               //time B0 ISR
__interrupt void TIMERB0_ISR(void) {
    SendNextChar = 1;
    if (next_char == 0xFF) {
        next_char = 0x00;
    }
    else {
        next_char = next_char + 0x01;
    }
    if ((next_location & ~0xF0) == 0x00) {
        next_location ^= 0x40;
        next_location |= 0x0F;
    }
    else {
        next_location = next_location - 0x01;
    }
    TB0CCTL0 &= ~CCIFG; 
} 
