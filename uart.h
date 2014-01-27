
#ifndef INC_SERIAL_H
#define INC_SERIAL_H

unsigned char uart_init(); 
unsigned char uart_write_byte(unsigned char byte);
unsigned char uart_write_string(unsigned char *string, unsigned char length);


#endif