/*! \file globals.c
    \brief Contains all global variables
    
    
*/
#include "defs.h"

/*SETUP GLOBAL VARIABLES*/
unsigned char LCD_digit = 0;
unsigned char LCD_dots = 0;
unsigned int LCD_value = 0;
unsigned long sd_address = 0; ///current active sd card address
unsigned char receive_buffer[512];
unsigned char *receive_ptr = &receive_buffer[0];
unsigned volatile int sample = 0; /// holds the most recently aquired sample
unsigned volatile char sync_flag = 0; ///a flag which indicates the ISR should write a sync flag instead of the sample data for a a couple of samples. A nonzero value indicates that is sync event is occuring.
unsigned volatile long head = 0; ///write position of the buffer (where the next adc sample is stored in the circular buffer)
unsigned volatile long tail = 0; ///read position of the buffer (where the samples are read out of the buffer and sent to the sd card)
unsigned int dma_adc_buf[16] __attribute__((space(dma))); //space in DMA memory to hold the ADC results
//unsigned char dma_dummy  __attribute__((space(dma))) = 0xFF; //dummy variable for clocking the DMA module
unsigned int T0_temp = 0;
unsigned int T1_temp = 0;
unsigned int T2_temp = 0;
