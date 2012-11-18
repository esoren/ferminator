
#ifndef INC_SPI_SD16_H
#define INC_SPI_SD16_H

void SPI1Write16(unsigned int data);
unsigned int SPI1Read16(void);
unsigned char SD_WriteMultiBlock16(unsigned char *data);
void SPI1_16bit(unsigned char mode16_bitval); 


#endif

