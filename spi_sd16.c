/*! \file spi_sd16.c
    \brief Functions to setup, read from, and write to an SD card the SPI1 module
    
    Uses the 16 bit functionality of the SPI module 
    
*/


#include <p33FJ256GP510A.h>
#include "defs.h"
#include "globals.h"
#include "spi_sd.h"
#include "spi_sd16.h"
#include <libpic30.h> //for delays


/*! \brief changes the SPI1 module between 8bit and 16bit modes 
*	1 = 16 bit
*	0 = 8 bit*/

void SPI1_16bit(unsigned char mode16_bitval) {
	unsigned char cs1_pin_state; 
	cs1_pin_state = CS1_PIN; 
	CS1_PIN = 1; 
	SPI1STATbits.SPIEN = 0; 
	SPI1CON1bits.MODE16 = mode16_bitval;
	SPI1STATbits.SPIEN = 1; 
	SPI1STATbits.SPIROV = 0; // clear collision flag 
	CS1_PIN = cs1_pin_state;

}	
		


/*! \brief Reads single int (16 bit) from SPI1
 *         
 * Low-level function that reads a single byte from the SSP1 buffer (clocks out 0xFF).
 * NOTE: The spi module be be setup for 16 bit operation prior to using this function. 
 */

unsigned int SPI1Read16(void) {
	unsigned int data;

	if(SPI1STATbits.SPIRBF) { // already have some data to return, don't initiate a read
		data = SPI1BUF;
		SPI1STATbits.SPIROV = 0;
		return data;
	}

	// We don't have any data to read yet, so initiate a read
	SPI1BUF = 0xFFFF;  // write dummy data to initiate an SPI read
	while(!SPI1STATbits.SPIRBF); // wait until the data is finished reading
	data = SPI1BUF;

	SPI1STATbits.SPIROV = 0;
	return data;
}


/*! \brief writes integer to SPI1 
 *         
 * Low level function that writes a 16bit value to SPI1. This is a useful way to increase the sd card write speeds. 
 * NOTE: The spi module be be setup for 16 bit operation prior to using this function. 
 *  
 */

void SPI1Write16(unsigned int data) {
	unsigned int temp; 
	SPI1STATbits.SPIROV = 0;        // clear out any rx overflow 
  	temp = SPI1BUF;                 // dummy read to clear flag 
  	while (SPI1STATbits.SPITBF);    // wait for TX buffer to dump 
  	SPI1BUF = data; 
  	while(!SPI1STATbits.SPIRBF); 
}

/* SPI Clock fix (UGLY BAND AID!) */

	

/*! \brief Writes a block in a multi-block write using 16bit SPI
 *
 *	Writes a single block. The write must have been initialized using SD_WriteMultiBlock(). See section 7.2.4 of the SD Card Physical Layer Simplified Specification Version 3.01
 *  Enables MODE16 in SPI1 to increase speeds and then disables it afterward for support of other SPI1 functions. 
 *
 * \sa SD_WriteMultiBlockInit(), SD_WriteMultiBlockEnd(), SDI1Write16()
 *
 */

unsigned char SD_WriteMultiBlock16(unsigned char *data) {
    unsigned int inc = 0;
    unsigned int status = 0;
    unsigned int temp = 0; //used to store the combination of two bytes into one int
    
    
    CS1_PIN = 0; //enabled SD card 
   	
   	 
   	temp = 0; 
   	temp |= 0xFC00; 
   	temp |= (*data); 
   	data++;
   	
   	SPI1Write16(temp); //send data token
  	
  	Nop(); 
  	
	//temp = 0xAAAA;
	for(inc = 0; inc < 255; inc++) {
		temp = 0; 
		temp |= (*data) << 8;
		data++; 
		temp |= *data; 
		data++;
		SPI1Write16(temp);
	}
	    
	temp = 0; 
	temp |= (*data) << 8;
	temp |= 0x00FF; 
	    
    SPI1Write16(temp); //send CRC
    SPI1Write16(0xFFFF); 
  	
     /*After receiving the data the card will return a data token.
     * The data token format is:
         0bXXX00101 = successful write */

    inc = 0;
    do {
		if(inc++ > MULTI_TOKEN_TIMEOUT) {
            Nop(); Nop(); 
            return 2; /*! \return 2 = Error: no data token response*/
  			
  		}      
        status = SPI1Read16();
    } while(status != 0xFFFF);//   (status & 0x1F) != 0x05); //wait for data token

	CS1_PIN = 1; 
	

    return 0;
     
}

