/*! \file adc.c
    \brief Contains functions pertaining to the communication between the cpu and the ADC chip. 
    
    
*/

#include <p33FJ256GP510A.h>
#include <libpic30.h> //for delays
#include "defs.h"
#include "globals.h"




/*! \brief Reads single byte from SPI2
 *         
 * Low-level function that reads a single byte from the SSP2 buffer (clocks out 0xFF).
 *  
 */


unsigned char SPI2Read(void) {
	unsigned char data;

	if(SPI2STATbits.SPIRBF) { // already have some data to return, don't initiate a read
		data = SPI2BUF;
		SPI2STATbits.SPIROV = 0;
		return data;
	}

	// We don't have any data to read yet, so initiate a read
	SPI2BUF = 0xFF;  // write dummy data to initiate an SPI read
	while(!SPI2STATbits.SPIRBF); // wait until the data is finished reading
	data = SPI2BUF;

	SPI2STATbits.SPIROV = 0;
	return data;
}

/*! \brief Writes single byte to SPI2
 *         
 * Low level function that writes a single byte to the SSP2 buffer.
 *  
 */
 
void SPI2Write(unsigned char data) {
	// DO NOT WAIT FOR SPITBF TO BE CLEAR HERE
	// (for some reason, it doesn't work on this side of the write data).

	// Write the data!
	SPI2BUF = data;

	// Wait until send buffer is ready for more data.
	while(SPI2STATbits.SPITBF);
}

/*! \brief Grabs a sample from the ADC
 * 
 * LEGACY: THIS FUNCTION IS NO LONGER USED! (It will currently return a dummy value of 0xFF00)
 *
 * Returns a single 16-bit sample from the ADC. 
 * \sa isr()
 */

unsigned int get_adc(void) {
    return 0xFF00;
}	
