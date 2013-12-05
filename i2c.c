
#include <p33FJ256GP510A.h>
#include "defs.h"
#include "globals.h"
#include "i2c.h"
#include <i2c.h>





unsigned char i2c_read_byte(unsigned char slave_address, unsigned char slave_register) {
    unsigned char i2c_val;

    /*begin start condition */
    while(I2C2STATbits.P == 1); //make sure the bus is idle
    IFS3bits.MI2C2IF = 0; //clear interrupt flag
    I2C2CONbits.SEN = 1; //set start event
    while(IFS3bits.MI2C2IF == 0); //wait for interrupt flag to signify end of start condition

    /*send device address (r/w cleared) */
    I2C2TRN = slave_address & 0xFD;
    IFS3bits.MI2C2IF = 0;
    while(I2C2STATbits.TBF); //wait for data to clock out
    while(I2C2STATbits.ACKSTAT); //wait for device to acknowledge

    /*send slave register address*/
    I2C2TRN = slave_register; //status register of RTC
    IFS3bits.MI2C2IF = 0;
    while(I2C2STATbits.TBF); //wait for data to clock out
    while(I2C2STATbits.ACKSTAT); //wait for device to acknowledge

    /*initiate a repeated start*/
    while(I2C2CON & 0x001F); //wait for the module to be ready (see 19.5.6 of dsPIC33 reference manual 12C)
    I2C2CONbits.RSEN = 1;
    while(I2C2CONbits.RSEN); //wait for slave to response

    /*send device address (read/write set) */
    I2C2TRN = slave_address | 0x1;
    IFS3bits.MI2C2IF = 0;
    while(I2C2STATbits.TBF); //wait for data to clock out
    while(I2C2STATbits.ACKSTAT); //wait for device to acknowledge

    /*receive data from device*/
    I2C2CONbits.RCEN = 1; //receive enable (start clocking for slave transfer)
    while(I2C2CONbits.RCEN); //wait for data  (NOTE: can also consider polling RBF bit here)
    i2c_val = I2C2RCV;

    /*generate NACK*/
    IFS3bits.MI2C2IF = 0;
    while(I2C2CON & 0x001F); //wait for the module to be ready (see 19.5.4 of dsPIC33 reference manual 12C)
    I2C2CONbits.ACKDT = 1; //set acknowledge for NACK
    I2C2CONbits.ACKEN = 1;
    while(I2C2CONbits.ACKEN);
    I2C2CONbits.ACKDT = 0; //set acknowledge for ACK

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
    while(I2C2STATbits.P == 1); //make sure the bus is idle
    IFS3bits.MI2C2IF = 0; //clear interrupt flag
    I2C2CONbits.SEN = 1; //set start event
    while(IFS3bits.MI2C2IF == 0); //wait for interrupt flag to signify end of start condition

    /*send device address (r/w cleared) */
    I2C2TRN = slave_address & 0xFD;
    IFS3bits.MI2C2IF = 0;
    while(I2C2STATbits.TBF); //wait for data to clock out
    while(I2C2STATbits.ACKSTAT); //wait for device to acknowledge

    /*send slave register address*/
    I2C2TRN = slave_register; //status register of RTC
    IFS3bits.MI2C2IF = 0;
    while(I2C2STATbits.TBF); //wait for data to clock out
    while(I2C2STATbits.ACKSTAT); //wait for device to acknowledge

    /*send data */
    I2C2TRN = data_out; //status register of RTC
    IFS3bits.MI2C2IF = 0;
    while(I2C2STATbits.TBF); //wait for data to clock out
    while(I2C2STATbits.ACKSTAT); //wait for device to acknowledge

    /*generate stop bus event*/
    IFS3bits.MI2C2IF = 0;
    while(I2C2CON & 0x001F); //wait for the module to be ready (see 19.5.5 of dsPIC33 reference manual 12C)
    I2C2CONbits.PEN = 1;
    while(IFS3bits.MI2C2IF == 0);

    /*done!*/

    return 0;

}

//unsigned char i2c_read_byte(unsigned char slave_address, unsigned char slave_register) {
//    unsigned char i2c_val;
//
//    StartI2C2();
//    /* Wait till Start sequence is completed */
//    while(I2CCONbits.SEN );
//
//    /* Write Slave address and set master for transmission */
//    MasterWriteI2C2(slave_address & 0xFD);
//    /* Wait till address is transmitted */
//    while(I2CSTATbits.TBF);
//    while(I2CSTATbits.ACKSTAT);
//
//    /* Transmit slave register */
//    MasterputsI2C2(slave_register);
//    while(I2CSTATbits.TBF);
//    while(I2CSTATbits.ACKSTAT);
//
//    /*initiate a repeated start*/
//    RestartI2C2();
//    //while(I2C2CONbits.RSEN); //wait for slave to response
//
//    /*send device address (read/write set) */
//     MasterWriteI2C2(slave_address | 0x1);
//    /* Wait till address is transmitted */
//    while(I2CSTATbits.TBF);
//    while(I2CSTATbits.ACKSTAT);
//
//    /*receive data from device*/
//    i2c_val = MasterReadI2C2();
//
//    /*generate NACK*/
//    NotAckI2C2();
//
//    StopI2C2();
//    /* Wait till stop sequence is completed */
//    while(I2CCONbits.PEN);
//    CloseI2C2();
//
//    /*done!*/
//    return i2c_val;
//}
//
//
//unsigned char i2c_write_byte (unsigned char slave_address, unsigned char slave_register, unsigned char data_out) {
//    unsigned int config2, config1;
//
//
//
//
//    StartI2C2();
//    /* Wait till Start sequence is completed */
//    while(I2CCONbits.SEN );
//
//    /* Write Slave address and set master for transmission */
//    MasterWriteI2C2(slave_address & 0xFD);
//    /* Wait till address is transmitted */
//    while(I2CSTATbits.TBF);
//    while(I2CSTATbits.ACKSTAT);
//
//    /* Transmit slave register */
//    MasterputsI2C2(slave_register);
//    while(I2CSTATbits.TBF);
//    while(I2CSTATbits.ACKSTAT);
//
//    /* Transmit data */
//    MasterputsI2C2(data_out);
//    while(I2CSTATbits.TBF);
//    while(I2CSTATbits.ACKSTAT);
//
//    StopI2C2();
//    /* Wait till stop sequence is completed */
//    while(I2CCONbits.PEN);
//    CloseI2C2();
//
//
//}


//    while(1==1) {
//
//        StartI2C2();                // begin I2C communications
//        IdleI2C2();
//        MasterWriteI2C2( 0xD0 );          // addresses the chip
//        IdleI2C2();
//        MasterWriteI2C2( 0x01 );          // access register address for minutes
//        IdleI2C2();
//        MasterWriteI2C2( 0b00010010 );    // write value into minutes register
//        IdleI2C2();
//        StopI2C2();                 // stop condition I2C on bus
//
//        StartI2C2();                // begin I2C communications
//        IdleI2C2();
//        MasterWriteI2C2( 0xD0 );          // addresses the chip
//        IdleI2C2();
//        MasterWriteI2C2( 0x03 );          // access register address for day of the week
//        IdleI2C2();
//        MasterWriteI2C2( 0x04 );          // write value into day register
//        IdleI2C2();
//        StopI2C2();                 // stop condition I2C on bus
//
//        StartI2C2();                  // Start condition I2C on bus
//        IdleI2C2();
//        MasterWriteI2C2( 0xD0 );            // addresses the chip
//        IdleI2C2();
//        MasterWriteI2C2( 0x03 );            // write register address
//        IdleI2C2();
//        StopI2C2();                   // Stop condition I2C on bus
//
//        StartI2C2();                  // Start condition I2C on bus
//        IdleI2C2();
//        MasterWriteI2C2( 0xD1 );            // addresses the chip with a read bit
//        IdleI2C2();
//        i2c_val_main = MasterReadI2C2();          // read the value from the RTC and store in result
//        IdleI2C2();
//        NotAckI2C2();                 // Not Acknowledge condition.
////        IdleI2C2();
//        StopI2C2();                   // Stop condition I2C on bus
//        Nop();
//        Nop();
//        Nop();
//
//    }