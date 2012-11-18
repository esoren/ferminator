/*! \file sram.c
    \brief Contains inline functions to read and write from external memory
    
    
*/


#include <p33FJ256GP510A.h>
#include "defs.h"
#include "globals.h"
#include "sram.h"
#include <libpic30.h> //for delays

/*! \brief Writes a single byte to SRAM 
 *	
 *  This inline function loads the SRAM data port with sram_data and writes the data to the SRAM chip, assuming that A0-A18 are setup to point to the correct memory address.
 */
 
 inline void memory_write(unsigned char sram_data) {
    DATA_PORT_DIR &= 0xFF00;
    DATA_PORT_OUT &= 0xFF00;
    DATA_PORT_OUT |= sram_data;
    WE = 0;
    CE = 0;
    Nop();
    Nop();
    CE = 1;
    WE = 1;
    return;
} 

/*! \brief Reads a single byte from SRAM
  *	
 *  This inline function loads returns a byte pointed to by A0-A18 on the SRAM chip
 */
 
 inline unsigned char memory_read() {
    unsigned char sram_data;
    DATA_PORT_DIR |= 0x00FF;
    Nop();
    CE = 0;
    Nop();
    Nop();
    Nop();
    Nop();
    sram_data = DATA_PORT_IN & 0xFF;
    CE = 1; 
    return sram_data;
} 

/*! \brief Writes the value in buff_pointer to the SRAM address pins
*/   
inline void set_memory_address(unsigned long buff_pointer) {
	ADDR_L = buff_pointer & 0xFFFF; //assign bits 0-15
	A16 = (buff_pointer >> 16) & 0x01; 
	A17 = (buff_pointer >> 17) & 0x01; 
	A18 = (buff_pointer >> 18) & 0x01; 
	return; 
}

/*! \brief Returns the value of the A0-A18 pins
*/
inline unsigned long get_memory_address() {
	unsigned long buff_pointer = 0;
	buff_pointer |= ADDR_L;
	buff_pointer |= (unsigned long)A18 << 18;
	buff_pointer |= (unsigned long)A17 << 17;
	buff_pointer |= (unsigned long)A16 << 16;
	return buff_pointer;
}
