
#ifndef INC_SPI_SD_H
#define INC_SPI_SD_H

unsigned char SPI1Wait(void);
void SPI1Write(unsigned char data);
unsigned char SPI1Read(void);
unsigned char sd_init(void);
unsigned char SPI_RW(unsigned char data);
unsigned char SD_WriteCommand(unsigned char* cmd);
unsigned char SD_ReadBlock(unsigned long addr, unsigned char *buf);
unsigned char SD_WriteBlock(unsigned long addr, unsigned char *data);
unsigned char SD_WriteMultiBlock(unsigned long addr);
unsigned char SD_WriteMultiBlockInit(unsigned long addr); 
unsigned char SD_WriteMultiBlockEnd(); 
unsigned char SD_WriteBlockDMA(unsigned long addr, unsigned char *send_ptr);
unsigned char SD_ReadStatus(unsigned char *buf);
unsigned char SD_PreEraseBlocks(unsigned int blocks);
unsigned char InitSD();
void wipe_sd(void); 
void pps_unlock(void); 
void pps_lock(void); 

#endif

