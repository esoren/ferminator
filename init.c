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
    
    //ADC pins
    SDI2_PIN_DIR = 1; //SDI2 as input
    CS2_PIN_DIR = 0; //CS2 as output
    SCK2_PIN_DIR = 0; //SCK2 as output

    //SD pins
    SDO1_PIN_DIR = 0; //SDO1 as output
    SDI1_PIN_DIR = 1; //SDI1 as input
    CS1_PIN_DIR = 0; //CS1 as output
    SCK1_PIN_DIR = 0; //SCK1 as output
    SCK1_PIN = 1; //default on idle state
    
    //SETUP MODEM PINS
    MODEM_CTRL_0_DIR = 1; //input
    MODEM_CTRL_1_DIR = 0; //output
    MODEM_CTRL_2_DIR = 1; //input
    
    
    //modem nibble as input:
    MODEM0_DIR = 1;
    MODEM1_DIR = 1;
    MODEM2_DIR = 1;
    MODEM3_DIR = 1;
    

    //setup pins AN0-AN11 as digital IO
    AD1PCFGL = 0xFFFF;
	AD1PCFGH = 0xFFFF; 
	
    //SETUP SPI2 for ADC
    //the SPI2 module is used for communicating with the ADC. The SPI2 clock (SCK2) should
    //be at 2.4MHz or less. See Fig. 1 in the ADS8325 datasheet for more info. 
    SPI2CON1 = 0x0836; //SMP=0, CKE=0, CKP=0, SSx as IO, SDO2 as IO, Master, fSCK = fTY/16
    SPI2CON1bits.CKP = 0; //clock polarity high 
    SPI2CON1bits.CKE = 0; //clock polarity high 
    SPI2CON2 = 0x0000; //framed sync pulse disabled
    SPI2STAT = 0x8000; //enable SPI2, SPI2 on during idle
    SDI2_PULLUP_ENABLE = 1;
    SPI2STATbits.SPIEN = 1; //turn on SPI2 

    //TIMER SETUP
    //timer2 is used to create a 50Khz sampling frequency for the adc.
    T2CONbits.T32 = 0; //16 bit timer
    T2CONbits.TCKPS = 0b00; //1:1 prescaler
    T2CONbits.TCS = 0; //use external clock (FOSC/2)
    T2CONbits.TGATE = 0; //gated time accumulation disabled
    PR2 = TIMER2_PERIOD; //800 instructions => 50KHz sample rate 
    
    //INTERUPTS
    //we are using 2 interrupts on the system:
    //1.) DMA0 transfer complete so that we can respond when a block of data has been written to the sd card
    //2.) Timer2 interrupt as our timebase for the ADC
    TMR2_FLAG = 0; //clear timer2 interrupt flag
    DMA0_FLAG = 0; //clear dma0 interrupt flag
    TMR2_IE = 1; //enable timer2 interrupt
    DMA0_IE = 1; //enable DMA0 interrupt
    IPC1bits.T2IP = 6; //timer 2 interrupt priority 6
    IPC1bits.DMA0IP = 5; //dma0 interrupt priority 5
    
    //DMA SETUP
    DMA0PAD = (volatile unsigned int) &SPI2BUF; //Select SPI2 as the DMA0 peripheral
    DMA0CNT = 2; //transfer 3 bytes 
    DMA0STA = __builtin_dmaoffset(&dma_adc_buf); //points towards the adc variable in DMA memory
    DMA0STB = __builtin_dmaoffset(&dma_adc_buf); //points towards the adc variable in DMA memory
    DMA0CON = 0xC801; //byte transfers, NULLW = 1, no ping-pong, continuous, read, channel enabled
    DMA0REQ = 0x0021;  //sets up the DMA transfer to be triggered by SPI2 transfer (needs to be kickstarted by CPU FORCE)
    
    /*DMA1PAD = (volatile unsigned int) &SPI2BUF; //Select SPI2 as the DMA0 peripheral
    DMA1CNT = 2; 
    DMA1STA = __builtin_dmaoffset(&dma_dummy);
    DMA1STB = __builtin_dmaoffset(&dma_dummy);
    DMA1CON = 0xE010; 
    DMA1REQbits.IRQSEL = 0b0100001;  //sets up the DMA transfer to be triggered by SPI2 transfer (needs to be kickstarted by CPU FORCE)*/
    
        
    
    //SRAM SETUP
    A16 = 0;
    A16_DIR = 0; 
    A17 = 0; 
    A17_DIR = 0; 
    A18 = 0;
    A18_DIR = 0; 
    ADDR_L = 0; 
    ADDR_L_DIR 	= 0;
    DATA_PORT_DIR 	|= 0x00FF; //set the data pins as input
    DATA_PORT_OUT 	&= 0xFF00; //clear the data pins
    CE = 1; 
    CE_DIR = 0; 
    WE = 0; 
    WE_DIR = 0;
    OE = 0;  
    OE_DIR = 0;  
    
    

}

