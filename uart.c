#include <p33FJ256GP510A.h>
#include "defs.h"
#include "globals.h"
#include "uart.h"



unsigned char uart_init() {
// UART SETUP
    TRISFbits.TRISF2 = 1;
    TRISFbits.TRISF3 = 0;
    
    U1MODE = 0x0000;
    /* See Equation 17.1 in the DSPIC UART Family Reference Guide (s17)*/
    /* (40000000/(115200*16)-1   (BRGH = 0)*/
    /* (40000000/(115200*4)-1   (BRGH = 1)*/    
    U1MODEbits.BRGH = 1; //Baud Rate: High speed
    U1BRG = 86; //115200 (0.2% error)

    U1MODEbits.USIDL = 1; //Discontinue operation when the device enters Idle mode
    U1MODEbits.IREN = 0; //IrDA encoder and decoder are disabled
    U1MODEbits.RTSMD = 1; //UxRTS is in Simplex mode
    U1MODEbits.UEN = 0b00; //UxTX and UxRX pins are enabled and used; UxCTS, UxRTS and BCLKx pins are controlled byport latches
    U1MODEbits.WAKE = 0; //Wake-up is disabled
    U1MODEbits.LPBACK = 0; //Loopback mode is disabled
    /* NOTE: Do we want to enable auto-baud-gen?*/
    U1MODEbits.ABAUD = 0; //Baud rate measurement disabled or completed
    U1MODEbits.URXINV = 0; //UxRX Idle state is ?0?
    U1MODEbits.PDSEL = 0b00; //8-bit data, no parity
    U1MODEbits.STSEL = 0; //1 Stop bit


    U1STA = 0x0000;
    U1STAbits.UTXISEL1 = 0;
    U1STAbits.UTXISEL0 = 0; //Interrupt generated when any character is transferred to the Transmit Shift Register (which implies at least one location is empty in the transmit buffer)
    U1STAbits.UTXINV = 0; //UxTX Idle state is ?0?
    U1STAbits.UTXBRK = 0; //Sync Break transmission is disabled or completed
    U1STAbits.URXISEL = 0b00; //Interrupt flag bit is set when a character is received
    U1STAbits.ADDEN = 0; //Address Detect mode disabled



    

    U1MODEbits.UARTEN = 1;
    U1STAbits.UTXEN = 1; //UARTx transmitter enabled; UxTX pin is controlled by UARTx (if UARTEN = 1)

    return 0;

}

unsigned char uart_write_byte(unsigned char byte) {
    U1TXREG=byte;
    while(U1STAbits.TRMT); //wait for the device to be done transmitting
    return 0; 
}