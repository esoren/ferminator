/*! \file init.c
    \brief Contains init function which setups ports and peripherals

    Put things here you want to run only once on device reset. 

*/


#include <p33FJ256GP510A.h>
#include "defs.h"
#include "globals.h"

/*! \brief Initializes ports and peripherals
 *
 *	Initializes all IO ports on the device (default to inputs) as well as peripherals including SPI, PPS, Interrupts, and Timers.
 *
 */

void init(void) {

    //The following two instructions set the device clock (Fosc) to 80Mhz when the external clock is 16Mhz.
    CLKDIV = 0x0000; //Doze clock 1:1, Doze disabled, PLL N2 = 2, PLL N1 = 2
    PLLFBD = 0x0012; //M = 20

	__builtin_write_OSCCONH(OSCCON & 0b11111011); 
	__builtin_write_OSCCONH(OSCCON | 0b00000011); 
	__builtin_write_OSCCONL(OSCCON | 0b01); 
		
    //RESET all ports as inputs
    TRISA = 0xFFFF; //setup porta as inputs
    LATA  = 0x0000;
    TRISB = 0xFFFF; //setup portb as inputs
    LATB  = 0x0000;
    TRISC = 0xFFFF; //setup portc as inputs
    LATC  = 0x0000;
    TRISD = 0xFFFF; //setup portd as inputs 
    LATD  = 0x0000;
    TRISE = 0xFFFF; //setup porte as inputs
    LATE  = 0x0000; 
    TRISF = 0xFFFF; //setup portf as inputs
    LATF  = 0x0000; 
 	TRISG = 0xFFFF; //setup portg as inputs
 	LATG  = 0x0000; 
 	
    //SETUP INDIVIDUAL IO PINS
    LED3_DIR = 0; //set debug leds as output
    LED4_DIR = 0;
 
    //SD pins
    SDO1_PIN_DIR = 0; //SDO1 as output
    SDI1_PIN_DIR = 1; //SDI1 as input
    CS1_PIN_DIR = 0; //CS1 as output
    SCK1_PIN_DIR = 0; //SCK1 as output
    SCK1_PIN = 1; //default on idle state
    
    //LCD PINS
    //turn off all characters/segments:
    K1 = 1; //K pins are active low
    K2 = 1;
    K3 = 1;
    K4 = 1;
    K5 = 1;
    AA1 = 0; //A pins are active high
    AB2 = 0;
    AC3 = 0;
    AD = 0;
    AE = 0;
    AF = 0;
    AG = 0;
    ADP = 0;
    //setup lcd pins as outputs
    K1_DIR = 0;
    K2_DIR = 0;
    K3_DIR = 0;
    K4_DIR = 0;
    K5_DIR = 0;
    AA1_DIR = 0;
    AB2_DIR = 0;
    AC3_DIR = 0;
    AD_DIR = 0;
    AE_DIR = 0;
    AF_DIR = 0;
    AG_DIR = 0;
    ADP_DIR = 0;


    //setup pins AN0-AN11 as digital IO
    AD1PCFGL = 0xFFFF;
	AD1PCFGH = 0xFFFF; 
	
    //TIMER SETUP
    //timer2 is used to create a 50Khz sampling frequency for the adc.
    T2CONbits.T32 = 0; //16 bit timer
    T2CONbits.TCKPS = 0b00; //1:1 prescaler
    T2CONbits.TCS = 0; //use external clock (FOSC/2)
    T2CONbits.TGATE = 0; //gated time accumulation disabled
    PR2 = TIMER2_PERIOD; //800 instructions => 50KHz sample rate 
    
  
    

}

