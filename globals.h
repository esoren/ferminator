#ifndef INC_GLOBALS_H
#define INC_GLOBALS_H
 
/*SETUP GLOBAL VARIABLES*/
extern unsigned char LCD_digit;
extern unsigned char LCD_dots;
extern unsigned int LCD_value;
extern unsigned long sd_address; 
extern unsigned char receive_buffer;
extern unsigned char *receive_ptr;
extern unsigned volatile char sync_flag;
extern volatile unsigned long head;
extern volatile unsigned long tail;
//extern int num_of_params;
extern unsigned int dma_adc_buf __attribute__((space(dma)));
//extern unsigned char dma_dummy __attribute__((space(dma)));
#endif

