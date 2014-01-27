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
#include "temp_lookup.h"
#include "display.h"
#include <i2c.h>
#include "i2c.h"
#include <libpic30.h> //for delays
#include "uart.h"
#include "rtc.h"

/* CONFIG SETTINGS  */
//see section 22.1 of the PIC33FJ256GP510A datasheet for config settings
//see p33FJ256GP510A.h for definitions
_FOSCSEL( FNOSC_FRC & IESO_OFF )  //consider enabling IESO since we are using a 16MHz oscillator
_FOSC( POSCMD_EC & OSCIOFNC_OFF & FCKSM_CSECMD) //EC oscillator, oscillator switching on, FSCM off
_FWDT( FWDTEN_OFF ) // WDT software enabled/disabled (off)
_FICD( ICS_PGD2 & JTAGEN_OFF ) //jtag disabled, communicate on PGED/C 1

/* STATIC VARIABLES */
static unsigned char LCD_dots = 0;
static unsigned int LCD_value = 0;
static unsigned int T0_temp = 0;
static unsigned int T1_temp = 0;
static unsigned int T2_temp = 0;


/* FUNCTION PROTOTYPES*/
int main (void);
void error(unsigned int error_num);
void __attribute__((__interrupt__)) _DMA0Interrupt(void);
void __attribute__((__interrupt__)) _DMA1Interrupt(void);
	
/*! \brief Halts operation and blinks LEDs
 *         
 *	A debugging function which halts the operation of the sensor (endless while-loop) and blinks DEBUG_LEDs 3 and 4. 
 *  You can tell the specific error number according to the ratio LED3:LED4 blinks. For instance, if LED3 blinks three 
 *  times every time LED4 blinks once then the error_num is 3.
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
 *	_DMA0Interrupt() is the DMA channel 0 interrupt service routine (ISR).
 *      DMA is used to retrieve information from the ADC DMA buffer. Since the
 *      ADC requires an external clock (which it gets from the SPI2 clock) it
 *      would require too much of the cpu's time to wait for the SPI clock to
 *      complete the transfer. The TIMER2 interrupt starts the DMA transfer.
 *      This function takes the information in stored in the buffer, formats it,
 *      and stores it in the global variable 'sample' where it is processed by
 *      the timer2 ISR. This introduces a 1 sample "lag" in the record.
 *
 */


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


void __attribute__((__interrupt__, auto_psv)) _DMA1Interrupt(void)
{
    unsigned int adc_accum = 0;
    unsigned int lookup_count = 0;
    unsigned int lcd_value_rounded = 0;
    unsigned int adc_high = 0;
    unsigned int adc_low = 0;
    unsigned int range = 0;
    unsigned int fraction = 0;

    LED3 = 1;
    LED4 = 1;
    adc_accum += (int)*(&dma_adc_buf);
    adc_accum += (int)*(&dma_adc_buf+1);
    adc_accum += (int)*(&dma_adc_buf+2);
    adc_accum += (int)*(&dma_adc_buf+3);
    adc_accum += (int)*(&dma_adc_buf+4);
    adc_accum += (int)*(&dma_adc_buf+5);
    adc_accum += (int)*(&dma_adc_buf+6);
    adc_accum += (int)*(&dma_adc_buf+7);
    adc_accum += (int)*(&dma_adc_buf+8);
    adc_accum += (int)*(&dma_adc_buf+9);
    adc_accum += (int)*(&dma_adc_buf+10);
    adc_accum += (int)*(&dma_adc_buf+11);
    adc_accum += (int)*(&dma_adc_buf+12);
    adc_accum += (int)*(&dma_adc_buf+13);
    adc_accum += (int)*(&dma_adc_buf+14);
    adc_accum += (int)*(&dma_adc_buf+15);

    if(ADC_SOURCE == T1_AN || ADC_SOURCE == T2_AN) {
        /*T1_AN and T2_AN are thermistor inputs (100k)*/

        /* Note: the thermistors are currently implemented using a lookup table
         * with linear interpolation between the steps of the table.
         *
         * In the future we can (and should) leverage the power of the dsPic to
         * calculate the temperature directly.  */
       
        /*Navigate the lookup table until we've gone over the ADC reading*/
        while(adc_accum < *(&lookup_adc_0+lookup_count) && lookup_count < LOOKUP_0_LENGTH) {
            lookup_count++;
        }
        lcd_value_rounded = (int)(*(&lookup_temp_0+lookup_count)*10); //lower temperature bound

        adc_high = (int)(*(&lookup_adc_0+lookup_count-1));
        adc_low = (int)(*(&lookup_adc_0+lookup_count));
        range = adc_high - adc_low;  //total range used for linterp

        fraction = 10*(adc_accum - adc_low);
        fraction = fraction / range;
        fraction = 9 - fraction; // difference to add for linterp

        lcd_value_rounded += fraction; //final interpolated data

    } else {
        /*T0_AN is the thermocouple input*/

        /* According to the AD8495 Datasheet Rev. 3 (pg 14)
         Tmj    = (Vout - Vref)/(5mV/C)     (Vref = GND = 0V)
         *      = Vout*200
         *    Vout = (adc_accum / 16) / 2^16 * 2.5
         Tmj    = adc_accum / 16 * 2.5 / 2^16 * 200 (* 10 for lcd_val)
         Tmj    = adc_accum * 312.5 / 2^16
         *
         */



        //fraction =  (20000*(long)adc_accum)/((long)65535);
        lcd_value_rounded = (3125*(long)adc_accum)>>16;
        lcd_value_rounded = lcd_value_rounded / 10;
        

        Nop();
        Nop();


    }



    switch(ADC_SOURCE) {
        case T0_AN:
            T0_temp = lcd_value_rounded;
            ADC_SOURCE = T1_AN;
            break;
        case T1_AN:
            T1_temp = lcd_value_rounded;
            ADC_SOURCE = T2_AN;
            break;
        case T2_AN:
            T2_temp = lcd_value_rounded;
            ADC_SOURCE = T0_AN;
            break;
    }
        
    





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
    static unsigned char LCD_digit = 0; //assuming this is done only once with a static variable
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
    unsigned char status = 0;
    unsigned char sd_state = 0;
    unsigned char sd_attempt = 0;
    unsigned char inc = 0;
    unsigned char digit_inc = 0;
    unsigned int count = 0;
    unsigned char input_sensor = 2; 
    unsigned long sd_address = 0; ///current active sd card address
    unsigned char receive_buffer[512];
    unsigned char transmit_buffer[2048];
    unsigned char *transmit_ptr = &transmit_buffer[0];
    unsigned char *receive_ptr = &receive_buffer[0];
    unsigned volatile long head = 0; ///write position of the buffer (where the next adc sample is stored in the circular buffer)
    unsigned volatile long tail = 0; ///read position of the buffer (where the samples are read out of the buffer and sent to the sd card)
    timeData getTime;



    LCD_value = 0000;
    init();
    lcd_init();
    timer2_init();
    adc_init();
    i2c_init();
    uart_init();
    rtc_init();




    LED3 = 1; 
    TIMER2_ON = 1;
    ADC1_SAMP = 1; //start converting
    LCD_dots = 0b00000011;
    T2_LED = 1;

    //HEATER1 = 1; //turn on heater 1
    AD1CHS0bits.CH0SA = 3;


    sd_address = SD_START_ADDRESS; /* the writing starts a few kb into the sd card to leave room for housekeeping */
    //__delay_ms(1000); //1 second delay to allow analog stages to stabilize

    //sd_state = sd_init(); //returns 0 upon successful init, positive otherwise
    
    //clear the buf_pos so that we are pointing to the start of the buffer
    head=tail=0;

    LED3 = 1; //inactive state
    LED4 = 0;


    
    
    TIMER2_ON = 1;
    sd_address = SD_START_ADDRESS;

   

   IFS0bits.U1TXIF=0;
   while(1==1){
       read_time(&getTime);
       uart_write_string(&getTime.timestring[0], 13);

       __delay_ms(1000);
   }
//        if(SW_SEL == 0) {
//            input_sensor = (input_sensor + 4) % 3;
//            T0_LED = 0;
//            T1_LED = 0;
//            T2_LED = 0;
//            switch(input_sensor){
//                case 0:
//                    T0_LED = 1;
//                    break;
//                case 1:
//                    T1_LED = 1;
//                    break;
//                case 2:
//                    T2_LED = 1;
//                    break;
//            }
//            __delay_ms(250);
//        }
//
//
//
//        switch(input_sensor){
//            case 0:
//                T0_LED = 1;
//                LCD_value = T0_temp;
//                break;
//            case 1:
//                T1_LED = 1;
//                LCD_value = T1_temp;
//                break;
//            case 2:
//                T2_LED = 1;
//                LCD_value = T2_temp;
//                break;
//        }
//
//        __delay_ms(100);
//    }

//
    
    

	
    return 0;
}

