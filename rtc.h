#ifndef INC_RTC_H
#define INC_RTC_H





typedef struct timeData {
    unsigned char hours;
    unsigned char minutes;
    unsigned char seconds;
    unsigned char day;
    unsigned char month;
    unsigned char year;
    unsigned char timestring[13]; //enough room for "YYMMDD-HHMMSS"
}timeData;

unsigned char read_time(timeData *pTimeData);
unsigned char rtc_init();
char makedigit (unsigned char number, unsigned char base);
unsigned char load_reset_time(timeData *pTimeData);
unsigned char write_time(timeData *pTimeData);

#endif