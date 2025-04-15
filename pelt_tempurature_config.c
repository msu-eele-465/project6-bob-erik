// have to use B1 since you are using B0 already
// On page 13 of the LM92 datasheet, there is a tempurature read figure. We will need D3-D14 if im understanding correctly.
// I am not sure how to grab the value that it will be sending via I2c but the framework should be there to make it work.
// Below is the config, I dont know if you want to make a separate file for it or just put it in your controller main.c
// -----------------------------------
    
UCB1CTLW0 |= UCSWRST; // UCSWRST=1 for eUSCI_B1 in SW reset
UCB1CTLW0 |= UCSSEL__SMCLK;
UCB1BRW = 10;

UCB1CTLW0 |= UCMODE_3; // put into i2c mode
UCB1CTLW0 |= UCMST; // put into master mode
UCB1I2CSA = dataRead[0]; // slave address here

UCB1TBCNT = 0x02; // send 2 bytes of data
UCB1CTLW1 |= UCASTP_2; // auto stop

// Configure Ports
P1SEL1 &= ~BIT3; // P1.3 = SCL 
P1SEL0 |= BIT3;

P1SEL1 &= ~BIT2; // P1.2 SDA 
P1SEL0 |= BIT2;

PM5CTL0 &= ~LOCKLPM5; // disable LPM 

// Take out of SW reset
UCB1CTLW0 &= ~UCSWRST; 

// Enable interrupts
UCB1IE |= UCTXIE0; // enable i2c Tx0 IRQ
UCB1IE |= UCRXIE0; // enable i2c Rx0 IRQ
__enable_interrupt(); 


// -------------------------------------

// here is the interrupt. You can change the structure or format as needed to integrate with your code.

#pragma vector=EUSCI_B1_VECTOR
__interrupt void EUSCI_B1_I2C_ISR(void){

    if(count = 0) then
    Data_In[0] = UCB1RXBUF;
    count = 1;
    else
    Data_In[1] = UCB1RXBUF;
    count = 0;
    end if;
    
    switch(UCB1IV)   
        case 0x16:
            Data_In = UCB1RXBUF; // retrieve data
            break;
        
        case 0x18:
            UCB1TXBUF = 0x03; // send reg address
            break;
        
        default:
            break;

  /*  if (Data_Cnt == (sizeof(Packet) - 1)) {
        UCB1TXBUF = Packet[Data_Cnt];
        Data_Cnt = 0;
    } else {
        UCB1TXBUF = Packet[Data_Cnt];
        Data_Cnt++;
    }*/
}

// As for the led patterns, i think we basically got those figured out yesterday.

#include "app/shared.h"
#include "msp430fr2355.h"
#include "shared.h"

while(1){
    UCB1CTLW0 |= UCTR; // put into TX mode
    UCB1CTLW0 |= UCTXSTT; // generate start condition

    while ((UCB1IFG & UCSTPIFG) == 0){} // wait for stop
            UCB1IFG &= ~UCSTPIFG; // clear stop flag

    UCB1TBCNT = 0x02; // send 2 bytes of data needed according to LM92 data sheet

    // recieve data from RX
    UCB1CTLW0 &= ~UCTR; // put into RX mode
    UCB1CTLW0 |= UCTXSTT; // generate start condtion

    while ((UCB1IFG & UCSTPIFG) == 0){} // wait for stop
        UCB1IFG &= ~UCSTPIFG; // clear stop flag
}

void get_pelt_temp(int window) { 

   // data_cnt = 0;

    /* voltage = (ADC_Value*3.3)/(4095); // gets voltage value from equation
    double in = 2196200 + ((1.8639-voltage)/.00000388); // from equation
    double root = sqrt(in); */ //from equation
    // temp_B = -1481.96 + root; // combine equation for temp

   // temp b is what the peltier gives i think

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
/* 
int heat_patt_step = 0;
int cool_patt_step = 0;

void heatpatt(){
    
    switch(heat_patt_step){
                case 0:
                    P1OUT = 1;
                    break;
                case 1:
                    P1OUT = 3;
                    break;
                case 2:
                    P1OUT = 7;
                    break;
                case 3:
                    P1OUT = 15;
                    break;
                case 4:
                    P1OUT = 31;
                    break;
                case 5:
                    P1OUT = 63;
                    break;
                case 6:
                    P1OUT = 127;
                    break;
                case 7:
                    P1OUT = 255;
                    break;
                default: 
                    P1OUT = 0xFF;
                    break;

    }
    _delay_cycles(525000);
    patt7step++;
    if (heat_patt_step > 7){
        heat_patt_step = 0;
        return; 
    }
}

void heatpatt(){
    
    switch(heat_patt_step){
                case 0:
                    P1OUT = 1;
                    break;
                case 1:
                    P1OUT = 3;
                    break;
                case 2:
                    P1OUT = 7;
                    break;
                case 3:
                    P1OUT = 15;
                    break;
                case 4:
                    P1OUT = 31;
                    break;
                case 5:
                    P1OUT = 63;
                    break;
                case 6:
                    P1OUT = 127;
                    break;
                case 7:
                    P1OUT = 255;
                    break;
                default: 
                    P1OUT = 0xFF;
                    break;

    }
    _delay_cycles(525000);
    patt7step++;
    if (cool_patt_step > 7){
        cool_patt_step = 0;
        return; 
    }
} */