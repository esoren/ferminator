#include <p33FJ256GP510A.h>
#include "defs.h"
#include "globals.h"
#include "rtc.h"
#include "i2c.h"




unsigned char rtc_init() {
//    timeData setTime;   // only uncomment this if you want to set the RTC time
//    load_reset_time(&setTime);
//    write_time(&setTime);

    unsigned char i2c_buf;
    i2c_buf = i2c_read_byte(RTC_ADDRESS, RTC_HOURS);
    i2c_buf &= 0b10111111; //clear bit 7 to make it 24-hour time
    i2c_write_byte(RTC_ADDRESS, RTC_HOURS, i2c_buf);
    return 0; 


}


unsigned char load_reset_time(timeData *pTimeData) {
    pTimeData->year = 14;
    pTimeData->month = 1;
    pTimeData->day = 26;
    pTimeData->hours = 21;
    pTimeData->minutes = 11;
    pTimeData->seconds = 10;
    return 0;

}



unsigned char read_time(timeData *pTimeData) {
    unsigned char i2c_buf = 0, temp = 0;

    i2c_buf = i2c_read_byte(RTC_ADDRESS, RTC_YEAR);
    temp = (i2c_buf >>4); //tens digit
    temp *= 10;
    i2c_buf = i2c_buf & 0x0F;
    temp += i2c_buf;
    pTimeData->year = temp;

    i2c_buf = i2c_read_byte(RTC_ADDRESS, RTC_MONTH);
    temp = (i2c_buf >>4); //tens digit
    temp = temp & 0x7;
    temp *= 10;
    i2c_buf = i2c_buf & 0x0F;
    temp += i2c_buf;
    pTimeData->month = temp;

    i2c_buf = i2c_read_byte(RTC_ADDRESS, RTC_DATE);
    temp = (i2c_buf >>4); //tens digit
    temp *= 10;
    i2c_buf = i2c_buf & 0x0F;
    temp += i2c_buf;
    pTimeData->day = temp;


    i2c_buf = i2c_read_byte(RTC_ADDRESS, RTC_HOURS);
    temp = (i2c_buf >>4); //tens digit
    temp = temp & 0x3;
    temp *= 10;
    i2c_buf = i2c_buf & 0x0F;
    temp += i2c_buf;
    pTimeData->hours = temp;

    i2c_buf = i2c_read_byte(RTC_ADDRESS, RTC_MINUTES);
    temp = (i2c_buf >>4); //tens digit
    temp *= 10;
    i2c_buf = i2c_buf & 0x0F;
    temp += i2c_buf;
    pTimeData->minutes = temp;

    i2c_buf = i2c_read_byte(RTC_ADDRESS, RTC_SECONDS);
    temp = (i2c_buf >>4); //tens digit
    temp *= 10;
    i2c_buf = i2c_buf & 0x0F;
    temp += i2c_buf;
    pTimeData->seconds = temp;


    /* Write the data to a string for serial and SD output */
    temp = pTimeData->year % 10;
    pTimeData->timestring[0] = (pTimeData->year-temp)/10 + 48;
    pTimeData->timestring[1] = temp + 48;
    
    temp = pTimeData->month % 10; 
    pTimeData->timestring[2] = (pTimeData->month-temp)/10 + 48;
    pTimeData->timestring[3] = temp + 48;
    
    temp = pTimeData->day % 10; 
    pTimeData->timestring[4] = (pTimeData->day-temp)/10 + 48;
    pTimeData->timestring[5] = temp + 48;

    pTimeData->timestring[6] = '-';
    
    temp = pTimeData->hours % 10;
    pTimeData->timestring[7] = (pTimeData->hours-temp)/10 + 48;
    pTimeData->timestring[8] = temp + 48;
    
    temp = pTimeData->minutes % 10; 
    pTimeData->timestring[9] = (pTimeData->minutes-temp)/10 + 48;
    pTimeData->timestring[10] = temp + 48;
    
    temp = pTimeData->seconds % 10; 
    pTimeData->timestring[11] = (pTimeData->seconds-temp)/10 + 48;
    pTimeData->timestring[12] = temp + 48;


    return 0;

}


unsigned char write_time(timeData *pTimeData) {
    unsigned char i2c_buf = 0, temp = 0, tens = 0, ones = 0;

    temp = pTimeData->year;
    ones = temp % 10;
    tens = temp / 10;
    i2c_buf = (tens << 4) | ones;
    i2c_write_byte(RTC_ADDRESS, RTC_YEAR, i2c_buf);

    temp = pTimeData->month;
    ones = temp % 10;
    tens = temp / 10;
    i2c_buf = (tens << 4) | ones;
    i2c_buf |= 0x80;
    i2c_write_byte(RTC_ADDRESS, RTC_MONTH, i2c_buf);

    temp = pTimeData->day;
    ones = temp % 10;
    tens = temp / 10;
    i2c_buf = (tens << 4) | ones;
    i2c_write_byte(RTC_ADDRESS, RTC_DATE, i2c_buf);
    
    temp = pTimeData->hours;
    ones = temp % 10;
    tens = temp / 10;
    i2c_buf = (tens << 4) | ones;
    //i2c_buf &= 0x3f;
    i2c_write_byte(RTC_ADDRESS, RTC_HOURS, i2c_buf);
   
    temp = pTimeData->minutes;
    ones = temp % 10;
    tens = temp / 10;
    i2c_buf = (tens << 4) | ones;
    i2c_write_byte(RTC_ADDRESS, RTC_MINUTES, i2c_buf);

    temp = pTimeData->seconds;
    ones = temp % 10;
    tens = temp / 10;
    i2c_buf = (tens << 4) | ones;
    i2c_write_byte(RTC_ADDRESS, RTC_SECONDS, i2c_buf);

    return 0;

}



/* Found this at: http://stackoverflow.com/questions/3694100/converting-to-ascii-in-c .
 Its an efficient way to implement itoa() on an embedded system.

 Example:  ascii_num = makedigit(&number, 10) would return the 10's digit in ascii
 */
char makedigit (unsigned char number, unsigned char base)
{
  static char map[] = "0123456789";
  int ix;

  for (ix=0; number >= base; ix++) { number -= base; }

  return map[ix];
}

//Test write the time (add functionality for this over serial later..)
    //i2c_write_byte(RTC_ADDRESS, RTC_HOUR, 13);
    //i2c_write_byte(RTC_ADDRESS, RTC_MINUTES, 0b01010111);
    //i2c_write_byte(RTC_ADDRESS, RTC_SECONDS, 7);


//   while(1==1) {
//       i2c_buf = i2c_read_byte(RTC_ADDRESS, RTC_MINUTES);
//       minutes = (i2c_buf >>4); //tens digit
//       minutes *= 10;
//       temp = i2c_buf & 0x0F;
//       minutes += temp; //ones digit
//
//       i2c_buf = i2c_read_byte(RTC_ADDRESS, RTC_SECONDS);
//       seconds = (i2c_buf >> 4); //tens digit
//       seconds *= 10;
//       temp = i2c_buf & 0x0F;
//       seconds += temp; //ones digit
//
//       LCD_value = 100*minutes + seconds;
//
//   }

    