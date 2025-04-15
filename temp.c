#include "app/shared.h"
#include "msp430fr2355.h"
#include "shared.h"

while(1){
    UCB1CTLW0 |= UCTR; // put into TX mode
    UCB1CTLW0 |= UCTXSTT; // generate start condition

    while ((UCB1IFG & UCSTPIFG) == 0){} // wait for stop
            UCB1IFG &= ~UCSTPIFG; // clear stop flag

    UCB1TBCNT = 0x02; // send 2 bytes of data

    // recieve data from RX
    UCB1CTLW0 &= ~UCTR; // put into RX mode
    UCB1CTLW0 |= UCTXSTT; // generate start condtion

    while ((UCB1IFG & UCSTPIFG) == 0){} // wait for stop
        UCB1IFG &= ~UCSTPIFG; // clear stop flag
}

void get_pelt_temp(int window) { 

    data_cnt = 0;

    /* voltage = (ADC_Value*3.3)/(4095); // gets voltage value from equation
    double in = 2196200 + ((1.8639-voltage)/.00000388); // from equation
    double root = sqrt(in); */ //from equation
    // temp_B = -1481.96 + root; // combine equation for temp

   // temp b is what the peltier gives

    if(ave_cnt != 0){ // populates array when its empty
        ave_cnt--;
        average[ave_cnt] = temp_B;
    }

    else{ // shifts bits up to make room for new data (temps)
        for(i=8; i>0; i--){
            average[i] = average[i-1]; // populate the array if empty
        }
        average[0] = temp_B;

        for(i=0; i<window; i++){ // window size n for average temp of n
            total_1 = total_1 + average[i];
        }
        pelt_temp = (total_1/window); // convert to celcius average
        total_1 = 0;
    }

    return;
}

// 4351