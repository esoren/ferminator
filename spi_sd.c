/*! \file spi_sd.c
    \brief Functions to setup, read from, and write to an SD card the SPI1 module
    
    
*/


#include <p33FJ256GP510A.h>
#include "defs.h"
#include "globals.h"
#include "spi_sd.h"
#include <libpic30.h> //for delays

/*! \brief Hangs until SD card is ready
 *         
 *  This function writes dummy data (0xFF) to SPI1 until the response is 0xFF. This is useful for clocking the SD card between commands and during initialization.
 *  
 */	

unsigned char sd_card_ready(void) {
	unsigned char status = 0; 
	unsigned int i = 0;
	
	for(i = 0; i < 5000; i++) {
		status = SPI1Read();
		if(status == 0xFF) {
			return 1; /*! \return 1 = card is ready */
		}
	}
	return 0; /*! \return 1 = timeout */
}			

/*! \brief Reads single byte from SPI1
 *         
 * Low-level function that reads a single byte from the SSP1 buffer (clocks out 0xFF).
 *  
 */

unsigned char SPI1Read(void) {
	unsigned char data;

	if(SPI1STATbits.SPIRBF) { // already have some data to return, don't initiate a read
		data = SPI1BUF;
		SPI1STATbits.SPIROV = 0;
		return data;
	}

	// We don't have any data to read yet, so initiate a read
	SPI1BUF = 0xFF;  // write dummy data to initiate an SPI read
	while(!SPI1STATbits.SPIRBF); // wait until the data is finished reading
	data = SPI1BUF;

	SPI1STATbits.SPIROV = 0;
	return data;
}



/*! \brief Writes single byte to SPI1
 *         
 * Low level function that writes a single byte to the SSP1 buffer and waits until it is clocked out.
 *  
 */

void SPI1Write(unsigned char data) {
	unsigned char temp; 
	SPI1STATbits.SPIROV = 0;        // clear out any rx overflow 
  	temp = SPI1BUF;                 // dummy read to clear flag 
  	while (SPI1STATbits.SPITBF);    // wait for TX buffer to dump 
  	SPI1BUF = data; 
  	while(!SPI1STATbits.SPIRBF); 
}

/*! \brief Initializes an SDHC v2.0 card for writing
 *         
 * Initializes an SDHC 2.0 card. Does not support SDSC cards or MMC cards. See file "SDCardInit-Flowchart.pdf" in the Documentation directory. 
 *  
 */
 
unsigned char sd_init_status() {
    unsigned int i,n,u = 0;
    unsigned char status = 0;

   

    //The SPI1 module is initially configured with a clock speed of ~300KHz
    //(assuming Fty = 40MHz). This is according to the sd card specification
    //which calls for a clock <= 400KHz during initialization. Later the clock
    //will be increased to full speed.
    SPI1CON1 = 0x0061; //primary prescale=1:16, secondary prescale=1:8, 8bit operation, master mode
    SPI1CON1bits.CKP = 1; //clock idle state high level
    SPI1CON1bits.CKE = 0; //data collected on rising edge of clock
    SPI1CON1bits.SMP = 1;
    SPI1STATbits.SPIROV = 0; //clear the SPIROV bit 
    SPI1STATbits.SPIEN = 1; //turn on SPI 
	
	
	
	CS1_PIN = 1;  // Turn off SD Card
    for(i=0; i < 20; i++)
        SPI1Write(0xFF); // give SD Card about a hundred clock cycles to boot up
    CS1_PIN = 0;
	
	
		
    //SEND CMD0
    i = 0;
    do {
        if(i++ > CMD0_TIMEOUT)
            return 1; /*! \return 1 = CMD0 ERROR (timeout) */
        
		
		CS1_PIN = 1; 
		SPI1Write(0xFF);
		CS1_PIN = 0; 
		SPI1Write(0xFF);

        if(sd_card_ready() != 1)
        	return 2; /*! \return 2 = CMD0 ERROR -- sd card not ready */
		
		
		//cmd0 = 0x40 00 00 00 00 95
		SPI1Write(0x40);
		SPI1Write(0x00);
    	SPI1Write(0x00);
		SPI1Write(0x00);
		SPI1Write(0x00);
		SPI1Write(0x95);
		
		n = 0;
		do {
        	if(n++ > 100)
				break; //try sending the command again

        	status = SPI1Read();
		} while (status == 0xFF); //0xFF is the default response -- wait for something else
		if(status != 0xFF) {
			Nop(); Nop(); Nop(); Nop();
		}	
	//at this point we have either received a response or have broken out of the loop after 20 bytes of 0xFF
    } while(status != 0x01); //wait for R1 response
	
    SPI1Write(0xFF);  //dummy clocks
    SPI1Write(0xFF);
	
    //SEND CMD8
    i = 0;
    do {
        if(i++ > CMD8_TIMEOUT)
            return 3; /*! \return 3 = CMD8 ERROR (timeout) */

    CS1_PIN = 1;
	SPI1Write(0xFF);
	CS1_PIN = 0; 
	SPI1Write(0xFF);

	if(sd_card_ready() != 1)
            return 4; /*! \return 4 = CMD8 ERROR -- sd card not ready */
		
	SPI1Write(0xFF);
	SPI1Write(0x48); //cmd8 = 0x48 00 00 01 AA 87
	SPI1Write(0x00);
	SPI1Write(0x00);
	SPI1Write(0x01);
	SPI1Write(0xAA);
	SPI1Write(0x87);
		
	n = 0;
	do {
            if(n++ > 10)
		break; 

            status = SPI1Read();
            //the command response should be 0x01 00 00 01 AA
            //(AA is a mirrored copy of byte 1 in the command we sent)
            if(status == 0x01) {
                SPI1Read(); //0x00
		SPI1Read(); //0x00
		SPI1Read(); //0x01 
		status = SPI1Read(); //0xAA
		break; 		
            }
	} while (1==1);
        // if status != 0xAA here, then we never got the correct response
        // and we need to send the command again
    } while(status != 0xAA); //wait for the echo response at the end of R7

    SPI1Write(0xFF);
    SPI1Write(0xFF);

    //SEND CMD58

    i = 0;
    do {
		
        if(i++ > CMD58_TIMEOUT)
            return 5; /*! \return 5 = CMD58 ERROR (timeout) */

        CS1_PIN = 1;
	SPI1Write(0xFF);
	CS1_PIN = 0; 
	SPI1Write(0xFF);
	
        if(sd_card_ready() != 1)
            return 6;  /*! \return 6 = CMD58 ERROR -- sd card not ready */
		
		
	SPI1Write(0xFF);
	SPI1Write(0x7A); //cmd58 = 0x7A 00 00 00 00 FF
	SPI1Write(0x00);
	SPI1Write(0x00);
	SPI1Write(0x00);
	SPI1Write(0x00);
	SPI1Write(0xFF);
		
	n = 0;
	do {
            if(n++ > 10)
		break; 
			
            status = SPI1Read();
            //the bytes after 0x01 don't really matter, the SPI1Read's are just to flush them out
            if(status == 0x01) {
		SPI1Read(); //0x00
		SPI1Read(); //0xFF
		SPI1Read(); //0x80
		SPI1Read(); //0x00
		status = 0x01; 
		break; 		
            }
	} while (1==1); 
    } while(status != 0x01); //wait for the echo response at the end of R7

    SPI1Write(0xFF);
    SPI1Write(0xFF);



    //SEND CMD55/ACMD41


    //CMD55 tells the sd card that the next command is an application specific
    //command
    i = 0;
    do {
        if(i++ > ACMD41_CMD55_TIMEOUT) 
            return 7; /*! \return 7 = ACMD41/CMD55 ERROR (timeout) */
		
		
	//Send CMD55 until R1 response 
		u = 0; 
		do { 
            if(u++ > 20)
				break; //if we aren't getting the response we want from CMD55 break through and try ACMD41 anyway

            CS1_PIN = 1;
            SPI1Write(0xFF);
            CS1_PIN = 0;
            SPI1Write(0xFF);
			
            if(sd_card_ready() != 1)
                return 8; /*! \return 8 = ACMD41/CMD55 ERROR (SPI1Wait() timeout) */
			
            SPI1Write(0x77); //CMD55 = 0x77 00 00 00 00 FF
            SPI1Write(0x00);
            SPI1Write(0x00);
            SPI1Write(0x00);
            SPI1Write(0x00);
            SPI1Write(0x01);
					
            n = 0;
            do {
				if(n++ > 20)
                    break;
				
				status = SPI1Read();
		
            } while(status & 0x80);
			
		} while(status != 0x01); 
	
	
		CS1_PIN = 1; 
		SPI1Write(0xFF);
		SPI1Write(0xFF);
		CS1_PIN = 0; 
		SPI1Write(0xFF);
	
        if(sd_card_ready() != 1)
            return 9; /*! \return 9 = ACMD41/CMD55 ERROR (SPI1Wait() timeout 2) */
		
		//now that the card is accepting an application specific command, we can send
        //ACMD41

    	SPI1Write(0x69);
		SPI1Write(0x40);
		SPI1Write(0x00);
		SPI1Write(0x00);
		SPI1Write(0x00);
		SPI1Write(0x01);
		
		n = 0;
		do {
        	if(n++ > 20)
				break; 
			
       		status = SPI1Read();
			
		} while(status & 0x80); 
	
    } while(status != 0x00); //the card will return 0x01 until it has exited the idle state. This can take up to 1 second


    CS1_PIN = 1; //unselect SD card
    //set the combined prescalers to 2:1, so the SPI module can now run at 20Mhz (assuming Fty = 40Mhz)
    
    SPI1STATbits.SPIEN = 0; 
    SPI1CON1bits.SPRE = 0b110; //secondary prescaler = 2:1
    SPI1CON1bits.PPRE = 0b11;  //primary prescaler = 1:1
	SPI1STATbits.SPIEN = 1; 
    
    CS1_PIN = 0; //select the card
	
    i = 0;
    do {
	if(i++ > CMD58_TIMEOUT2) 
            return 10; /*! \return 10 = CMD58 ERROR (timeout) */

    CS1_PIN = 1;
	SPI1Write(0xFF);
	CS1_PIN = 0; 
	SPI1Write(0xFF);
	
        if(sd_card_ready() != 1)
            return 11; /*! \return 11 = CMD58 ERROR (SPI1Wait() timeout) */
		
		
		
    SPI1Write(0xFF);
	SPI1Write(0x7A); //cmd58 = 0x7A 00 00 00 00 FF
	SPI1Write(0x00);
	SPI1Write(0x00);
	SPI1Write(0x00);
	SPI1Write(0x00);
	SPI1Write(0xFF);
		
	n = 0;
	do {
            if(n++ > 20)
		break; 
			
            status = SPI1Read();
            if(status == 0x00) {
		SPI1Read(); //0x00
		SPI1Read(); //0xFF
		SPI1Read(); //0x80
		SPI1Read(); //0x00
		status = 0x00; 
		break; 		
            }
        } while (1==1);
    } while(status != 0x00); //wait for the echo response at the end of R7

    SPI1Write(0xFF);
    SPI1Write(0xFF);
	
    CS1_PIN = 1; //deselect SD card
	
    return 0; /*! \return 0 = Successfully initialized and ready to read/write */
}


/*! \brief Writes a command to the SD Card 
 *
 *	Writes a standard SD command to the card. Adds the necessary framing bits (start and stop). Returns the card's response to the command. See section 7.3.1.1 of the SD Card Physical Layer Simplified Specification Version 3.01 for information about the command format. 
 * 
 *	\return R1 Response Format (single byte)
 */
 
unsigned char SD_WriteCommand(unsigned char* cmd) {
    unsigned int i;
    unsigned char response;
    unsigned char saved_CS1_PIN = CS1_PIN;

    // SD Card Command Format
    // (from Section 5.2.1 of SanDisk SD Card Product Manual v1.9).
    // Frame 7 = 0
    // Frame 6 = 1
    // Command (6 bits)
    // Address (32 bits)
    // Frame 0 = 1

    // Set the framing bits correctly
    cmd[0] |= (1<<6);
    cmd[0] &= ~(1<<7);
    cmd[5] |= 1;
	
    // Send the 6 byte command
    CS1_PIN = 0;
    for(i = 0; i < 6; ++i) {
        SPI1Write(*cmd);
		cmd++;
    }
	
    // Wait for the response
    i = 0;
    do {
    	response = SPI1Read();
    	if(i > 100)
            break;
	i++;
    } while(response == 0xFF);
	
    CS1_PIN = 1;
    SPI1Write(0xFF); //Sandisk SD Card Product Manual v1.9 section 5.1.8
    CS1_PIN = saved_CS1_PIN;
	
    return(response);
}


/*! \brief Starts a multi-block write 
 *
 *	Starts a multi-block write on the SD card using CMD25. See section 7.2.4 of the SD Card Physical Layer Simplified Specification Version 3.01
 *
 * \sa SD_WriteMultiBlock(), SD_WriteMultiBlockEnd()
 *
 */

unsigned char SD_WriteMultiBlockInit (unsigned long addr) {
    unsigned char i = 0;
    unsigned char status = 0;
	
    CS1_PIN = 1;
    SPI1Write(0xFF);
    CS1_PIN = 0;
    SPI1Write(0xFF);
			
    if(sd_card_ready() != 1)
	return 1; /*! \return 1 = sd card is not ready */
		
	
    /* Write CMD25 */
    SPI1Write(0b01011001);
    SPI1Write((addr & 0xFF000000) >> 24);
    SPI1Write((addr & 0x00FF0000) >> 16);
    SPI1Write((addr & 0x0000FF00) >> 8);
    SPI1Write((addr & 0x000000FF));
    SPI1Write(0xFF);
	
    while(1==1) {
	status = SPI1Read();
	if(i++ > 20)
            return 2; /*! \return 2 = CMD25 response timeout */

    } while (status != 0x00) //wait for R1 response 
    //ready to initiate multi-block write
    return 0;
}

/*! \brief Writes a block in a multi-block write
 *
 *	Writes a single block. The write must have been initialized using SD_WriteMultiBlock(). See section 7.2.4 of the SD Card Physical Layer Simplified Specification Version 3.01
 *
 * \sa SD_WriteMultiBlockInit(), SD_WriteMultiBlockEnd()
 *
 */

unsigned char SD_WriteMultiBlock(unsigned long addr) {
    unsigned int inc = 0;
    unsigned char status = 0;

    
    CS1_PIN = 0; //enable SD card
    SPI1Write(0xFC); //send data token

    for(inc = 0; inc < BLOCK_SIZE; inc++) {
		//FIX THIS!!!!set_memory_address(addr++); // write the buffer pointer to memory address pins
		//FIX THIS!!!!data = memory_read();
		//FIX THIS!!!!SPI1Write(data); //Write the data to the sd card
    }

    SPI1Write(0xFF); //send CRC
    SPI1Write(0xFF);

    /*After receiving the data the card will return a data token.
     * The data token format is:
         0bXXX00101 = successful write */

    inc = 0;
    do {
	if(inc++ > MULTI_TOKEN_TIMEOUT)
            return 2; /*! \return 2 = Error: no data token response*/
        status = SPI1Read();
    } while((status & 0x1F) != 0x05); //wait for data token

    inc = 0;
    while(SPI1Read() == 0x00) { //the sd card will hold the data line low until the block is written
    	inc++;    /*I should have a timeout here, but I don't know how long
                    the card can take to write.. I need to look into this*/
    }

    return 0;
     
}	


/*! \brief Writes a block in a multi-block write using the DMA module
 *
 *	Writes a single block. The write must have been initialized using SD_WriteMultiBlock(). See section 7.2.4 of the SD Card Physical Layer Simplified Specification Version 3.01
 *
 * \sa SD_WriteMultiBlockInit(), SD_WriteMultiBlockEnd()
 *
 */

unsigned char SD_WriteMultiBlockDMA(unsigned char *send_ptr) {
	/*This section of code only works if a multi-block write is already 
      in progress. Start a multi-block write with SD_WriteMultiBlockInit()*/
    /*
    unsigned int inc = 0; 
	unsigned char status = 0;
	
	//setup DMA
	DMACON1 = 0b00100100;
	DMACON2 = 0b00000000; 
	DMABCH = ((char)(511 >> 8));
	DMABCL = 511 & 0xFF; 
	TXADDRH = ((char)((unsigned int)send_ptr >> 8)) ;
	TXADDRL = (unsigned int)send_ptr & 0xFF; 
	Nop();
	
	
	SPI1Write(0xFC); //send data token
	
	//use DMA to write data
	PIR3bits.SSP1IF = 0;
	DMACON1bits.DMAEN = 1; 
	Nop();
	Nop();
	while(PIR3bits.SSP1IF == 0); //TIMEOUT CHECK HERE
	
	SPI1Write(0xFF); //write CRC
	SPI1Write(0xFF);
	
	do {
		status = SPI1Read();
	} while(status & 0x1F != 0x05); //wait for data token
		
	inc = 0;
	while(SPI1Read() == 0x00) { //wait for busy signal
		inc++; //TIMEOUT CHECK HERE
	}
         */
	return 0; 
}	

/*! \brief Ends a multi-block write
 *
 *	Sends the end token (0xFD) to end a multi-block write session. See section 7.2.4 of the SD Card Physical Layer Simplified Specification Version 3.01
 *
 * \sa SD_WriteMultiBlockInit(), SD_WriteMultiBlock()
 *
 */


unsigned char SD_WriteMultiBlockEnd () {
    unsigned int inc = 0; 
    SPI1Write(0b11111101); //send end token
    SPI1Write(0xFF); //wait 2 bytes for busy signal 
    SPI1Write(0xFF); //wait 2 bytes for busy signal 
    inc = 0;
    while(SPI1Read() == 0x00) { //the sd card will hold the data line low while it is busy
    	inc++;    				/*I should have a timeout here, but I don't know how long
                    			the card can take to write.. I need to look into this*/
    }
    CS1_PIN = 1; //deselect sd card
    SPI1Write(0xFF); //clock
    return 0;
}		

/*! \brief Writes a single block without DMA
 *
 *  Writes a block at address (addr). THIS FUNCTION DOES NOT CURRENTLY WORK!
 *
 * \sa SD_WriteBlockDMA()
 *
 */

unsigned char SD_WriteBlock(unsigned long addr, unsigned char *data) {//, unsigned char *buf) {
    unsigned int inc;
    unsigned char status;
    unsigned char CMD24_WRITE_SINGLE_BLOCK[] = {24,0x00,0x00,0x00,0x00,0xFF};
    CMD24_WRITE_SINGLE_BLOCK[1] = ((addr & 0xFF000000) >> 24);
    CMD24_WRITE_SINGLE_BLOCK[2] = ((addr & 0x00FF0000) >> 16);
    CMD24_WRITE_SINGLE_BLOCK[3] = ((addr & 0x0000FF00) >> 8);
    CMD24_WRITE_SINGLE_BLOCK[4] = ((addr & 0x000000FF));
	
    CS1_PIN = 0; //enabled SD card
	
    // Send the write command
    status = SD_WriteCommand(CMD24_WRITE_SINGLE_BLOCK);

    if(status != 0x00)
        return 1; /*! \return 1 = Error: invalid response from CMD24 */
	
    SPI1Write(0xFE); //send data token
	
    for(inc = 0; inc < BLOCK_SIZE; inc++) {
		SPI1Write(*data);
        data++;
    }
    SPI1Write(0xFF); //write CRC
    SPI1Write(0xFF);
    
    /*After receiving the data the card will return a data token.
     * The data token format is: 
         0bXXX00101 = successful write
         0bXXX01011 = CRC error
         0xXXX01101 = write error*/

    inc = 0;
    do {
        if(inc++ > 100)
            return 2; /* \return 2 = Error: no data token after write*/

        status = SPI1Read();

        if((status & 0x1F) == 0x0D)
            return 3; /* \return 3 = Error: write error (0x0D data token response)*/
        
    } while((status & 0x1F) != 0x05); //wait for data token
		
    inc = 0;
    while(SPI1Read() == 0x00) { //the sd card will hold the data line low until the block is written
    	inc++;    /*I should have a timeout here, but I don't know how long
                    the card can take to write.. I need to look into this*/
    }
	
    CS1_PIN = 1;
    SPI1Write(0xFF);
    return 0;
}				
	
/*! \brief Writes a single block with DMA
 *
 * Writes a block of 512 bytes (starting at the location pointed to by send_ptr) to the SD Card. Uses DMA to increase the transfer speed. CMD24
 *
 * THIS FUNCTION IS NOT CURRENTLY IMPLEMENTED. IT WILL RETURN 0!
 *
 */


unsigned char SD_WriteBlockDMA(unsigned long addr, unsigned char *send_ptr) {//, unsigned char *buf) {
    
    /*
    unsigned int inc;
    unsigned char status;
    unsigned char CMD24_WRITE_SINGLE_BLOCK[] = {24,0x00,0x00,0x00,0x00,0xFF};
    CMD24_WRITE_SINGLE_BLOCK[1] = ((addr & 0xFF000000) >> 24);
    CMD24_WRITE_SINGLE_BLOCK[2] = ((addr & 0x00FF0000) >> 16);
    CMD24_WRITE_SINGLE_BLOCK[3] = ((addr & 0x0000FF00) >> 8);
    CMD24_WRITE_SINGLE_BLOCK[4] = ((addr & 0x000000FF));

    CS1_PIN = 0; //enabled SD card
	
	//setup DMA
    DMACON1 = 0b00100100;
    DMACON2 = 0b00000000;
    DMABCH = ((char)(511 >> 8));
    DMABCL = 511 & 0xFF;
    TXADDRH = ((char)((unsigned int)send_ptr >> 8)) ;
    TXADDRL = (unsigned int)send_ptr & 0xFF;
    Nop();
    // Send the write command
    status = SD_WriteCommand(CMD24_WRITE_SINGLE_BLOCK);
    if(status != 0) {
        return 1; //error
	}
	SPI1Write(0xFE); //send data token
	
	//use DMA to write data
	PIR3bits.SSP1IF = 0;
	DMACON1bits.DMAEN = 1; 
	Nop();
	Nop();
	while(PIR3bits.SSP1IF == 0); //TIMEOUT CHECK HERE
	
	SPI1Write(0xFF); //write CRC
	SPI1Write(0xFF);
	
	
	do {
		status = SPI1Read();
	} while(status & 0x1F != 0x05); //wait for data token
		
	inc = 0;
	while(SPI1Read() == 0x00) {
		inc++;
	}	
	
		
		
	CS1_PIN = 1; 
	SPI1Write(0xFF);
	
	*/
	return 0;
}				
	
/*! \brief Reads the status register of the SD Card
 *
 *  Reads the status register of the SD Card and loads it into a 512byte array.
 *  This function is most useful for SD card diagnostics.
 *
 * 
 *
 */

	
unsigned char SD_ReadStatus(unsigned char *buf) {
    unsigned int i;
    unsigned char status;
    

    CS1_PIN = 0; //enable SD card
    if(sd_card_ready() != 1)
	return 1; /* \return 1 = Error: sd card not ready*/
	

    SPI1Write(0x77); //CMD55 = 0x77 00 00 00 00 FF
    SPI1Write(0x00);
    SPI1Write(0x00);
    SPI1Write(0x00);
    SPI1Write(0x00);
    SPI1Write(0x01);
    SPI1Write(0xFF);
	
    do {
        status = SPI1Read();
    } while (status == 0xFF);
	
    SPI1Read();
    SPI1Read();
	
    // Send the read command
    SPI1Write(0b01001101); //CMD13
    SPI1Write(0x00);
    SPI1Write(0x00);
    SPI1Write(0x00);
    SPI1Write(0x00);
    SPI1Write(0xFF);

    SPI1Write(0xFF);
    do {
	status = SPI1Read();
    } while (status == 0xFF);
	

    // Now wait for the "Start Block" token	(0xFE)
    // (see SanDisk SD Card Product Manual v1.9 section 5.2.4. Data Tokens)
    i = 0;
    SPI1Read();
	
    do {
        status = SPI1Read();
    } while (status != 0xFE);

    //load the result of the status register into the receive buffer
    for(i = 0; i < BLOCK_SIZE; ++i) {
        status = SPI1Read();
	*buf = status;
	buf++;
    }
	
    // Read CRC bytes
    status = SPI1Read();
    status = SPI1Read();

    CS1_PIN = 1; //unselect SD Card

    // Following a read transaction, the SD Card needs 8 clocks after the end
    // bit of the last data block to finish up its work.
    // (from SanDisk SD Card Product Manual v1.9 section 5.1.8)
    SPI1Write(0xFF);

    return 0; /* \return 0 = successfully read status register*/
}

/*! \brief Reads a block of the SD card
 *
 *  Reads a 512Byte block of data from the SD Card at the specified address. Data is stored in a 512Byte array pointed to by *buf
 *
 *
 */

unsigned char SD_ReadBlock(unsigned long addr, unsigned char *buf) {
    unsigned int i;
    unsigned char status;
    unsigned char CMD17_READ_SINGLE_BLOCK[] = {17,0x00,0x00,0x00,0x00,0xFF};


    CMD17_READ_SINGLE_BLOCK[1] = ((addr & 0xFF000000) >> 24);
    CMD17_READ_SINGLE_BLOCK[2] = ((addr & 0x00FF0000) >> 16);
    CMD17_READ_SINGLE_BLOCK[3] = ((addr & 0x0000FF00) >> 8);
    CMD17_READ_SINGLE_BLOCK[4] = ((addr & 0x000000FF));

    CS1_PIN = 0; //enable SD card

    // Send the read command
    status = SD_WriteCommand(CMD17_READ_SINGLE_BLOCK);
    if(status != 0)
        return 1;  /* \return 1 = Error: invalid response from CMD17*/
	
	
    // Now wait for the "Start Block" token	(0xFE)
    // (see SanDisk SD Card Product Manual v1.9 section 5.2.4. Data Tokens)
    i = 0;
    do {
        if(i++ > START_BLOCK_TIMEOUT)
            return 2; /* \return 2 = Error: no start block response
                       */
		status = SPI1Read();
    } while(status != 0xFE);
	
    // Read off all the bytes in the block
    for(i = 0; i < BLOCK_SIZE; ++i) {
	status = SPI1Read();
	*buf = status;
	buf++;
    }
	
    // Read CRC bytes
    status = SPI1Read();
    status = SPI1Read();

    CS1_PIN = 1; //unselect SD Card

    // Following a read transaction, the SD Card needs 8 clocks after the end
    // bit of the last data block to finish up its work.
    // (from SanDisk SD Card Product Manual v1.9 section 5.1.8)
    SPI1Write(0xFF);

    return 0; /* \return 0 = Successfully read block from sd card. Data is stored in the receive buffer*/
}

/*! \brief Sets the number of blocks to be pre-erased on the SD Card
 *
 * The number of blocks an sd card pre-erasing can increase the speed of writes when using multi-block write.   
 *
 *
 */

unsigned char SD_PreEraseBlocks(unsigned int blocks) {
	unsigned int i = 0;
        unsigned int u = 0;
        unsigned int n = 0;
	unsigned char status = 0; 

    do {
        if(i++ > PRE_ERASE_TIMEOUT) 
            return 2; /*! \return 2 = ACMD41/CMD55 ERROR (timeout) */
		
		
	//Send CMD55 until R1 response 
		u = 0; 
		do { 
            if(u++ > 20)
				break; //if we aren't getting the response we want from CMD55 break through and try ACMD23 anyway

            CS1_PIN = 1;
            SPI1Write(0xFF);
            CS1_PIN = 0;
            SPI1Write(0xFF);
			
            if(sd_card_ready() != 1)
                return 3; /*! \return 3 = CMD55 Wait timeout  */
			
            SPI1Write(0x77); //CMD55 = 0x77 00 00 00 00 FF
            SPI1Write(0x00);
            SPI1Write(0x00);
            SPI1Write(0x00);
            SPI1Write(0x00);
            SPI1Write(0x01);
					
            n = 0;
            do {
				if(n++ > 20)
                    break;
				
				status = SPI1Read();
		
            } while(status & 0x80);
			
		} while(status != 0x01); 
	
	
		CS1_PIN = 1; 
		SPI1Write(0xFF);
		SPI1Write(0xFF);
		CS1_PIN = 0; 
		SPI1Write(0xFF);
	
        if(sd_card_ready() != 1)
            return 4; /*! \return 4 = sd_card_ready() timeout */
		
		//now that the card is accepting an application specific command, we can send
        //ACMD23

    	SPI1Write(0x57); //ACMD23 = 0x57 00 00 XX XX FF (where XXXX = # of pre-erase blocks)
		SPI1Write(0x00);
		SPI1Write(0x00);
		SPI1Write(0x00);
		SPI1Write(blocks);
		SPI1Write(0xFF);
		
		n = 0;
		do {
        	if(n++ > 20)
				break; 
			
       		status = SPI1Read();
			
		} while(status != 0x00); 
	
    } while(status != 0x00); //the card will return 0x01 until it has exited the idle state. This can take up to 1 second

	

	return 0; /*! \return 0 = successfully changed pre-erase blocks on sd card  */
}


