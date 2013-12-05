#ifndef INC_I2C_H
#define INC_I2C_H

unsigned char i2c_read_byte(unsigned char slave_address, unsigned char slave_register);
unsigned char i2c_write_byte(unsigned char slave_address, unsigned char slave_register, unsigned char data_out);



#endif