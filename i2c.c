
#include <p33FJ256GP510A.h>
#include "defs.h"
#include "globals.h"
#include "i2c.h"
#include <i2c.h>
#include <delay.h>
#include <libpic30.h> //for delays


unsigned char i2c_init() {
    /*Set I2C Baud Rate */
    /*Note: from dsPIC33F Family Reference Manual: I2C, section 19.4.3
    Baud rate is set according to the following equation:
    I2CBRG = (FCY/FSCL - FCY/10000000)-1   */

    I2C2BRG = 395; //(100KHz @ 40Mhz FCY)
    //I2C2BRG = 95; //(400Khz @ 50MHz FCY)
        //(slightly intermittent results at 400K, need stronger pullups)

    I2C2_SDA_PIN = 0;
    I2C2_SCL_PIN = 0;
    I2C2_SDA_PIN_DIR = 0;
    I2C2_SCL_PIN_DIR = 0;

    /*setup I2C Config */
    I2C2CONbits.I2CSIDL = 0; //continue device operation in idle mode
    I2C2CONbits.SCLREL = 0; //Release SCLx clock in slave mode
    I2C2CONbits.IPMIEN = 0; //IPMI enable
    I2C2CONbits.A10M = 0; //7 bit slave address
    I2C2CONbits.DISSLW = 1; //slew rate control disabled
    I2C2CONbits.SMEN = 0; //SMBus Input Levels  (???)
    I2C2CONbits.GCEN = 0; //general call interrupt in slave mode
    I2C2CONbits.STREN = 0; //disable clock stretching
    I2C2CONbits.ACKDT = 0; //send ACK during acknowledge  (1 == ACK, 0 = NACK)
    I2C2CONbits.ACKEN = 0; //used to initiate ACK sequence
    I2C2CONbits.RCEN = 0; //receive enable bit
    I2C2CONbits.PEN = 0; //initiate hardware stop condition
    I2C2CONbits.RSEN = 0; //repeated start condition enable bit
    I2C2CONbits.SEN = 0; //Start condition enable bit
    I2C2CONbits.I2CEN = 1; //enable I2C

    return 0;
}

unsigned char i2c_read_byte(unsigned char slave_address, unsigned char slave_register) {
    unsigned char i2c_val;
 
    /*begin start condition */
    IdleI2C2(); //wait for bus to be idle
    IFS3bits.MI2C2IF = 0; //clear interrupt flag
    I2C2CONbits.SEN = 1; //set start event
    while(I2C2CONbits.SEN); //wait for the startup sequence to complete

    /*send device address (r/w cleared) */
    I2C2TRN = slave_address & 0xFD;
    IFS3bits.MI2C2IF = 0;
    while(I2C2STATbits.TBF); //wait for data to clock out
    while(I2C2STATbits.ACKSTAT); //wait for device to acknowledge
    IdleI2C2();

    /*send slave register address*/
    I2C2TRN = slave_register; //status register of RTC
    IFS3bits.MI2C2IF = 0;
    while(I2C2STATbits.TBF); //wait for data to clock out
    while(I2C2STATbits.ACKSTAT); //wait for device to acknowledge
    IdleI2C2();

    /*initiate a repeated start*/
    //__delay_us(100);
    while(I2C2CON & 0x001F); //wait for the module to be ready (see 19.5.6 of dsPIC33 reference manual 12C)
    I2C2CONbits.RSEN = 1;
    while(I2C2CONbits.RSEN); //wait for slave to respond
    //__delay_us(100);
   
  


    /*send device address (read/write set) */
    I2C2TRN = slave_address | 0x1;
    IFS3bits.MI2C2IF = 0;
    while(I2C2STATbits.TBF); //wait for data to clock out
    while(I2C2STATbits.ACKSTAT); //wait for device to acknowledge
    IdleI2C2();

    /*receive data from device*/
    I2C2CONbits.RCEN = 1; //receive enable (start clocking for slave transfer)
    while(I2C2CONbits.RCEN); //wait for data  (NOTE: can also consider polling RBF bit here)
    i2c_val = I2C2RCV;
    IdleI2C2();
    
    NotAckI2C2();
    while(I2C2CONbits.ACKEN);


    /*generate stop bus event*/
    IFS3bits.MI2C2IF = 0;
    while(I2C2CON & 0x001F); //wait for the module to be ready (see 19.5.5 of dsPIC33 reference manual 12C)
    I2C2CONbits.PEN = 1;
    while(IFS3bits.MI2C2IF == 0);

    /*done!*/
    return i2c_val;
}


unsigned char i2c_write_byte (unsigned char slave_address, unsigned char slave_register, unsigned char data_out) {
    

    /*begin start condition */
    //while(I2C2STATbits.P == 1); //make sure the bus is idle
    IFS3bits.MI2C2IF = 0; //clear interrupt flag
    I2C2CONbits.SEN = 1; //set start event
    while(IFS3bits.MI2C2IF == 0); //wait for interrupt flag to signify end of start condition

    /*send device address (r/w cleared) */
    I2C2TRN = slave_address & 0xFD;
    IFS3bits.MI2C2IF = 0;
    while(I2C2STATbits.TBF); //wait for data to clock out
    while(I2C2STATbits.ACKSTAT); //wait for device to acknowledge
    IdleI2C2();

    /*send slave register address*/
    I2C2TRN = slave_register; //status register of RTC
    IFS3bits.MI2C2IF = 0;
    while(I2C2STATbits.TBF); //wait for data to clock out
    while(I2C2STATbits.ACKSTAT); //wait for device to acknowledge
    IdleI2C2();

    /*send data */
    I2C2TRN = data_out; //status register of RTC
    IFS3bits.MI2C2IF = 0;
    while(I2C2STATbits.TBF); //wait for data to clock out
    while(I2C2STATbits.ACKSTAT); //wait for device to acknowledge
    IdleI2C2();

    /*generate stop bus event*/
    IFS3bits.MI2C2IF = 0;
    while(I2C2CON & 0x001F); //wait for the module to be ready (see 19.5.5 of dsPIC33 reference manual 12C)
    I2C2CONbits.PEN = 1;
    while(IFS3bits.MI2C2IF == 0);
    
    /*done!*/

    return 0;

}


