/*! \file display.c
    \brief Functions to display on the LTC-4727JR 4-digit 7-segment display
*/

#include <p33FJ256GP510A.h>
#include "defs.h"
#include "globals.h"
#include <libpic30.h> //for delays



/*! \brief Displays value on the LCD screen
 *
 *  This function dislays a value on the LTC-4727JR Display.
 *  A value of more than 4 digits (ie: 12345) will be concatenated
 *  to the four least-significant digits (2345).
 *
 *  digit is a char between 0 and 4 -- it specifies which digit to be display
 *  (the 4 digits and then dots are multiplexed. 
 *  0 = (K1) (most significant digit)
 *  1 = (K2)
 *  2 = (K4)
 *  3 = (K5) (least significant digit)
 *  4 = dots (K3)
 *
 *  dots specifies which dots to light up:
 *  dots{6-0}
 *  bit6 of dots = DP1 (least significant)
 *  bit5 of dots = DP2
 *  bit4 of dots = DP3
 *  bit3 of dots = DP4  (most significant)
 *  bit2 of dots = L1
 *  bit1 of dots = L2
 *  bit0 of dots = L3
 *  (bit 7 is unused and should remain 0)
 *
 *  EXAMPLE:
 *  To display 100.3    ->
 *      value = 1003
 *      dots = 0b00010000  (16)
 *
 *  To display 11:33    ->
 *      value = 1133
 *      dots = 0b00000110  (6)
 * 
 */

unsigned char lcd_display(char digit, int value, char dots) {
    unsigned char segment_val = 0;


    //turn off all characters
    K1 = 0;
    K2 = 0;
    K3 = 0;
    K4 = 0;
    K5 = 0;


    //extract the character info for to assign the segments

    if(digit == 0) {   //K1, most significant digit
        segment_val = (value/1000)%10;
    } else if(digit == 1) { //K2
        segment_val = (value/100)%10;
    } else if(digit == 2) { //K4
        segment_val = (value/10)%10;
    } else if(digit == 3) { //K5
        segment_val = (value%10);
    } else { //dots
        segment_val = 10;
    }

    //turn off all segments
    AA1 = 1;
    AB2 = 1;
    AC3 = 1;
    AD = 1;
    AE = 1;
    AF = 1;
    AG = 1;
    ADP = 1;


    switch(segment_val) {
        case(0):
            AA1 = 0;
            AB2 = 0;
            AC3 = 0;
            AD = 0;
            AE = 0;
            AF = 0;
            break;
        case(1):
            AB2 = 0;
            AC3 = 0;
            break;
        case(2):
            AA1 = 0;
            AB2 = 0;
            AG = 0;
            AE = 0;
            AD = 0;
            break;
        case(3):
            AA1 = 0;
            AB2 = 0;
            AG = 0;
            AC3 = 0;
            AD = 0;
            break;
        case(4):
            AF = 0;
            AG = 0;
            AB2 = 0;
            AC3 = 0;
            break;
        case(5):
            AA1 = 0;
            AF = 0;
            AG = 0;
            AC3 = 0;
            AD = 0;
            break;
        case(6):
            AA1 = 0;
            AF = 0;
            AG = 0;
            AC3 = 0;
            AD = 0;
            AE = 0;
            break;
        case(7):
            AA1 = 0;
            AB2 = 0;
            AC3 = 0;
            break;
        case(8):
            AA1 = 0;
            AB2 = 0;
            AC3 = 0;
            AD = 0;
            AE = 0;
            AF = 0;
            AG = 0;
            break;
        case(9):
            AA1 = 0;
            AB2 = 0;
            AC3 = 0;
            AD = 0;
            AF = 0;
            AG = 0;
            break;
        case(10):
            if( (dots&0b01) == 1) {
                AA1 = 0;
            }
            if( ((dots>>1) & 0b01) == 1) {
                AB2 = 0;
            }
            if( ((dots>>2) & 0b01) == 1) {
                AC3 = 0;
            }
            break;
    }

    

    //add the dp's, then turn on the individual display

    if(digit == 0) {   //K1, most significant digit
        if( ((dots>>3) & 0b01) == 1) {
            ADP = 0;
        }
        K1 = 1;
    } else if(digit == 1) { //K2
        if( ((dots>>4) & 0b01) == 1) {
            ADP = 0;
        }
        K2 = 1;
    } else if(digit == 2) { //K4
        if( ((dots>>5) & 0b01) == 1) {
            ADP = 0;
        }
        K3 = 1;
    } else if(digit == 3) { //K5
        if( ((dots>>6) & 0b01) == 1) {
            ADP = 0;
        }
        K4 = 1;
    } else { //dots (K3)
        K5 = 1;
    }


    
    Nop();
    Nop();



    return 0; /*! \return 0 = success */
}