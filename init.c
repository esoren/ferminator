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
    LED3 = 0;
    LED4 = 0;
    LED6 = 0;
    T0_LED = 0;
    T1_LED = 0;
    T2_LED = 0;
    LED3_DIR = 0; //set debug leds as output
    LED4_DIR = 0;
    LED6_DIR = 0;
    T0_LED_DIR = 0;
    T1_LED_DIR = 0;
    T2_LED_DIR = 0;



 
    //SD pins
    SDO1_PIN_DIR = 0; //SDO1 as output
    SDI1_PIN_DIR = 1; //SDI1 as input
    CS1_PIN_DIR = 0; //CS1 as output
    SCK1_PIN_DIR = 0; //SCK1 as output
    SCK1_PIN = 1; //default on idle state
    
    //LCD PINS
    //turn off all characters/segments:
    K1 = 0; //K pins are active high
    K2 = 0;
    K3 = 0;
    K4 = 0;
    K5 = 0;
    AA1 = 1; //A pins are active low
    AB2 = 1;
    AC3 = 1;
    AD = 1;
    AE = 1;
    AF = 1;
    AG = 1;
    ADP = 1;
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

   
    //INPUT PINS
    SW_SEL_DIR      = 1;
    SW_SET_DIR      = 1;
    SW_ENC_A_DIR    = 1;
    SW_ENC_B_DIR    = 1;
    SW_ENC_PUSH_DIR = 1;
    

    //HEATER SETUP
    HEATER1 = 0;
    HEATER2 = 0;
    HEATER1_DIR = 0; //output
    HEATER2_DIR = 0; //output


    //setup pins AN0-AN11 as digital IO
    AD1PCFGL = 0xFFFF;
    AD1PCFGH = 0xFFFF;
    AD1PCFGLbits.PCFG3 = 0; //setup T0, T2, and T2 as analog inputs
    AD1PCFGLbits.PCFG4 = 0;
    AD1PCFGLbits.PCFG5 = 0;
    T0_DIR = 1;
    T1_DIR = 1;
    T2_DIR = 1;
	
    //TIMER SETUP
    //timer2 is to update the LCD on a 300 hz basis (60hz refresh per char)
    T2CONbits.T32 = 0; //16 bit timer
    T2CONbits.TCKPS = 0b11; //1:256 prescaler
    T2CONbits.TCS = 0; //use external clock (FOSC/2)
    T2CONbits.TGATE = 0; //gated time accumulation disabled
    PR2 = TIMER2_PERIOD; //800 instructions => 195Hz sample rate
    
     //INTERUPTS
    //we are using 3 interrupts on the system:
    //1.) DMA0 transfer complete so that we can respond when a block of data has been written to the sd card
    //2.) Timer2 interrupt as our timebase for the LCD
    //3.) DMA1 interrupt from the ADC module

    TMR2_FLAG = 0; //clear timer2 interrupt flag
    DMA1_FLAG = 0;
    //DMA0_FLAG = 0; //clear dma0 interrupt flag
    TMR2_IE = 1; //enable timer2 interrupt
    //DMA0_IE = 1; //enable DMA0 interrupt
    DMA1_IE = 1; //enable ADC DMA interrupt
    IPC1bits.T2IP = 4; //timer 2 interrupt priority 4
    IPC3bits.DMA1IP = 5; //adc interrupt priority 5
    //IPC1bits.DMA0IP = 5; //dma0 interrupt priority 5

    //ADC SETUP
    AD1CSSL = 0b0000000000111000;
    AD1CON1bits.AD12B = 1; //12 bit operation
    AD1CON1bits.SSRC = 0b111; //auto-sample period
    AD1CON2bits.VCFG = 0b001; //Vref+, VSS
    AD1CON3bits.ADRC = 0; //derive ADC clock from internal clock
    AD1CON1bits.FORM = 00; //store results as unsigned integer
    AD1CON2bits.CHPS = 00; //only sample channel 0;
    AD1CON3bits.ADCS = 0x3F; //T_AD = TCY * 64   (CHECK THIS LATER, T_AD should be > 117.6ns)
    AD1CON3bits.SAMC = 14; //14 TAD in a conversion
    AD1CON4bits.DMABL = 0b100; //16 words of buffer for each input
    AD1CON2bits.ALTS = 0; //always sample on MUXA
    AD1CON1bits.ASAM = 1; //autosample
    AD1CON1bits.ADDMABM = 1; //write buffer sequentially
    AD1CON2bits.SMPI = 0xF; //interrupt every sixteen conversions
    AD1CON2bits.BUFM = 0; //always start at beginning of buffer
    ADC_SOURCE = T2_AN; //select T2


    //DMA Setup

    //ADC DMA
    DMA1CONbits.SIZE = 0; //word
    DMA1CONbits.DIR = 0; //ADC -> RAM
    DMA1CONbits.HALF = 0; //initiate interrupt when buffer is full
    DMA1CONbits.NULLW = 0; //dont send null transfer back to peripheral
    DMA1CONbits.MODE = 0; //continuoius, ping pong disabled
    DMA1CONbits.AMODE = 0; //check this
    DMA1STA = __builtin_dmaoffset(&dma_adc_buf); //point to the buffer
    DMA1REQbits.IRQSEL = 13; //IRQ of ADC1
    DMA1PAD = (int)&ADC1BUF0;
    DMA1CNT = 31;
    


    AD1CON1bits.ADON = 1; //turn on the ADC module
    DMA1CONbits.CHEN = 1;



    

}

