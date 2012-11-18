#ifndef INC_SRAM_H
#define INC_SRAM_H

inline void memory_write(unsigned char sram_data); 
inline unsigned char memory_read();
inline void set_memory_address(unsigned long buff_pointer);
inline unsigned long get_memory_address();

#endif

