#include <p33FJ256GP510A.h>
#include "defs.h"
#include "globals.h"
#include "rtc.h"
#include "i2c.h"


unsigned char rtc_init() {
    unsigned char i2c_buf;
    i2c_buf = i2c_read_byte(RTC_ADDRESS, RTC_HOURS);
    i2c_buf &= 0x40; //clear bit 7 to make it 24-hour time
    i2c_write_byte(RTC_ADDRESS, RTC_HOURS, i2c_buf);
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

    


    return 0;

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