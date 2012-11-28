/*! \file main.c
    \brief Main file containing initializing routines, ISR, and modem communication
    
    
*/

#include <p33FJ256GP510A.h>
#include <stdio.h>
#include <stdlib.h>
#include "defs.h"
#include "spi_sd.h"
#include "globals.h"
#include "init.h"
#include <libpic30.h> //for delays

/* CONFIG SETTINGS  */
//see section 22.1 of the PIC33FJ256GP510A datasheet for config settings
//see p33FJ256GP510A.h for definitions
_FOSCSEL( FNOSC_FRC & IESO_OFF )  //consider enabling IESO since we are using a 16MHz oscillator
_FOSC( POSCMD_EC & OSCIOFNC_OFF & FCKSM_CSECMD) //EC oscillator, oscillator switching on, FSCM off
_FWDT( FWDTEN_OFF ) // WDT software enabled/disabled (off)
_FICD( ICS_PGD2 & JTAGEN_OFF ) //jtag disabled, communicate on PGED/C 1


/* FUNCTION PROTOTYPES*/
int main (void);
void error(unsigned int error_num); 
	
/*! \brief Halts operation and blinks LEDs
 *         
 *	A debugging function which halts the operation of the sensor (endless while-loop) and blinks DEBUG_LEDs 3 and 4. 
 *  You can tell the specific error number according to the ratio LED3:LED4 blinks. For instance, if LED3 blinks three 
 *  times every time LED4 blinks once then the error_num is 3 (BUFFER_OVERFLOW).
 */

void error(unsigned int error_num) {
    unsigned int x = 0;
    while(1==1) {
        for(x = 0; x < error_num; x++) {
            LED3 = 1;
            __delay_ms(200);
            LED3 = 0;
            __delay_ms(200);
        }
        LED4 = 1;
        __delay_ms(200);
        LED4 = 0;
        __delay_ms(200);
    }
}

/*! \brief DMA0 ISR - formats ADC sample and writes it to memory
 *
 *	_DMA0Interrupt() is the DMA channel 0 interrupt service routine (ISR). DMA is used to retrieve information from the ADC DMA buffer. Since the ADC requires an external clock (which it gets from the SPI2 clock) it would require too much of the cpu's time to wait for the SPI clock to complete the transfer. The TIMER2 interrupt starts the DMA transfer. This function takes the information in stored in the buffer, formats it, and stores it in the global variable 'sample' where it is processed by the timer2 ISR. This introduces a 1 sample "lag" in the record.
 *
 */

void __attribute__((__interrupt__)) _DMA0Interrupt(void);
void __attribute__((__interrupt__, auto_psv)) _DMA0Interrupt(void)
{

    DMA0_FLAG = 0;
    return;
    
}


/*! \brief DMA1 ISR - formats ADC sample and writes it to memory
 *
 *	_DMA0Interrupt() is the DMA channel 0 interrupt service routine (ISR). DMA is used to retrieve information from the ADC DMA buffer. Since the ADC requires an external clock (which it gets from the SPI2 clock) it would require too much of the cpu's time to wait for the SPI clock to complete the transfer. The TIMER2 interrupt starts the DMA transfer. This function takes the information in stored in the buffer, formats it, and stores it in the global variable 'sample' where it is processed by the timer2 ISR. This introduces a 1 sample "lag" in the record.
 *
 */

void __attribute__((__interrupt__)) _DMA1Interrupt(void);
void __attribute__((__interrupt__, auto_psv)) _DMA1Interrupt(void)
{
    unsigned int adc_accum = 0;
    LED3 = 1;
    LED4 = 1;
    adc_accum += *(&dma_adc_buf);
    adc_accum += *(&dma_adc_buf+1);
    adc_accum += *(&dma_adc_buf+2);
    adc_accum += *(&dma_adc_buf+3);
    adc_accum += *(&dma_adc_buf+4);
    adc_accum += *(&dma_adc_buf+5);
    adc_accum += *(&dma_adc_buf+6);
    adc_accum += *(&dma_adc_buf+7);
    adc_accum += *(&dma_adc_buf+8);
    adc_accum += *(&dma_adc_buf+9);
    adc_accum += *(&dma_adc_buf+10);
    adc_accum += *(&dma_adc_buf+11);
    adc_accum += *(&dma_adc_buf+12);
    adc_accum += *(&dma_adc_buf+13);
    adc_accum += *(&dma_adc_buf+14);
    adc_accum += *(&dma_adc_buf+15);
    
    LCD_value = adc_accum >> 4; //divide by 16 to get the average


    DMA1_FLAG = 0;
    return;

}


/*! \brief Interrupt Service Routine - updates LCD based on global variables
 *         
 *	_T2Interrupt() is the TIMER2 interrupt service routine (ISR). This interrupt occurs service routine is set to interrupt on the overflow of Timer2.  It updates the display based on the global variables "LCD_value", "LCD_dots", and "LCD_digit". It increments the digit variable.
 *  
 */
	
//
void __attribute__((__interrupt__)) _T2Interrupt(void);
void __attribute__((__interrupt__, auto_psv)) _T2Interrupt(void)
{
    LCD_digit = (LCD_digit + 1) % 5;
    lcd_display(LCD_digit, LCD_value, LCD_dots);
 
    
    TMR2_FLAG = 0;
    return;
}

#pragma code

//---------------MAIN LOOP---------- 

/*! \brief Main loop
 *         
 *	The main loop of the sensor is reponsible for handling communications with the MODEM IC and initializing a block write when the active SD Card buffer is full. 
 *  \sa SD_WriteMultiBlock()
 */

int main (void) {
    
    unsigned char command = 0;
    unsigned char status = 0;
    unsigned char sd_state = 0;
    unsigned char sd_attempt = 0;
    unsigned long mem_address = 0;
    unsigned char inc = 0;
    unsigned char digit_inc = 0;
    unsigned int count = 0;

    LCD_value = 1234;
    init();
    LED3 = 1; 
    T2CONbits.TON = 1;
    AD1CON1bits.SAMP = 1; //start converting
    *(&dma_adc_buf) = 5; 
    while(1==1){
        count++;
        if(count == 10000) {
            count = 0;
        }
        //LCD_value = count;
        LCD_dots = 0b00100000;
        __delay_ms(10);

    }





    sd_address = SD_START_ADDRESS; /* the writing starts a few kb into the sd card to leave room for housekeeping */
    LED3 = 1;
    __delay_ms(200);
    LED4 = 1;
    __delay_ms(4000); //4 second delay to allow analog stages to stabilize


    Nop();
    Nop(); 
    sd_attempt = 0;
    do {
        if(++sd_attempt > 3) {
            error(SD_CARD_NOT_INITIALIZED);  //error();  //critical error - halt the cpu and blink leds.
	}
 	__delay_ms(100); //wait 1 second for SD card to initialize
        sd_state = sd_init_status(); //returns 0 upon successful init, positive otherwise

    } while(sd_state != 0);
	
    SD_PreEraseBlocks(8); //set the number of blocks to pre-erase to 8. This should increase the multi-block write performance.
    
    //clear the buf_pos so that we are pointing to the start of the buffer
   head=tail=0;
	

    
   status = SD_ReadBlock(10000, &receive_buffer);
   	
    LED3 = 1; //inactive state
    LED4 = 0; 

    
    SD_WriteMultiBlockInit(SD_START_ADDRESS); 
    //SPI1_16bit(1); //turn on 16 bit transfers for increased speed
    TIMER2_ON = 1;
    
     
    while(1==1) {
		
		
	if((head < tail) || (head >= tail+BLOCK_SIZE)) {
			status = SD_WriteMultiBlock(tail); 
			tail += 512; 
			tail &= BUFFER_SIZE-1; 
			sd_address++; 
		}	
	}				
    
    
    //MODEM COMM WAIT LOOP----------
    while(1==1) {
     
       
    }
	
    return 0;
}

