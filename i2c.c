
#include <p33FJ256GP510A.h>
#include "defs.h"
#include "globals.h"
#include "i2c.h"
#include <i2c.h>
#include <delay.h>
#include <libpic30.h> //for delays




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
