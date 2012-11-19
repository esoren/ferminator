/*! \file defs.h
    \brief Contains all definitions
    
    
*/

#ifndef INC_DEFS_H
#define INC_DEFS_H


#define FCY 40000000UL ///40 MIPS operating speed (used for __delay_ms() and __delay_S())

/** @defgroup MISC_PINS Misc Pins
 * @{ */
#define LED3        LATDbits.LATD12
#define LED3_DIR    TRISDbits.TRISD12
#define LED4        LATDbits.LATD13
#define LED4_DIR    TRISDbits.TRISD13
/** @} */

/** @defgroup SD_PINS SD Pins
 * @{ */
#define SD_PWR 		LATAbits.LATA15
#define SD_PWR_DIR	TRISAbits.TRISA15		 
#define SDO1_PIN	LATFbits.LATF8
#define SDO1_PIN_DIR	TRISFbits.TRISF8
#define SDI1_PIN	PORTFbits.RF7
#define SDI1_PIN_DIR	TRISFbits.TRISF7
#define SCK1_PIN	LATFbits.LATF6
#define SCK1_PIN_DIR	TRISFbits.TRISF6
#define CS1_PIN		LATGbits.LATG3
#define CS1_PIN_DIR	TRISGbits.TRISG3
/** @} */


/** @defgroup ERROR_DEFS Error Definitions
 * @{ */
#define     GENERIC_ERROR			1 ///generic error flag
#define     SD_CARD_NOT_INITIALIZED		2 ///error initializing sd card
#define     BUFFER_OVERFLOW			3 ///buffer overflow
#define     BUFFER_POINTER			4 ///buffer pointer out of acceptable range
/** @} */

/** @defgroup TIMER_DEFS Timer Definitions
 * @{ */
#define TIMER2_ON		T2CONbits.TON
#define TIMER2_PERIOD 	800   //3200 = 12.5KHz 1600 = 25KHz, 800 = 50KHz, 500 = 80KHz, 640 = 62.5Khz
/** @} */

// R1 Response Codes (from SD Card Product Manual v1.9 section 5.2.3.1)
/** @defgroup SD_DEFS SD Card Definitions
 * @{ */
#define R1_IN_IDLE_STATE        1		/// The card is in idle state and running initializing process.
#define R1_ERASE_RESET          (1<<1)  /// An erase sequence was cleared before executing because of an out of erase sequence command was received.
#define R1_ILLEGAL_COMMAND      (1<<2)  /// An illegal command code was detected
#define R1_COM_CRC_ERROR        (1<<3)  /// The CRC check of the last command failed.
#define R1_ERASE_SEQ_ERROR      (1<<4)  /// An error in the sequence of erase commands occured.
#define R1_ADDRESS_ERROR        (1<<5)  /// A misaligned address, which did not match the block length was used in the command.
#define R1_PARAMETER            (1<<6)  /// The command's argument (e.g. address, block length) was out of the allowed range for this card.
#define BLOCK_SIZE              512     ///sd card block size
#define LOW                     0x0000  ///0x0000
#define HIGH                    0xFFFF  ///0xFFFF
#define BUFFER_SIZE             524288  /// size of the circular buffer (= size of SRAM)
#define SD_START_ADDRESS        0      	///leave 10 KB at beginning of card for misc. information
#define CMD0_TIMEOUT            500    	///CMD0 timeout
#define CMD8_TIMEOUT            500     ///CMD8 timeout
#define CMD58_TIMEOUT           200     ///CMD58 timeout
#define ACMD41_CMD55_TIMEOUT    1000    ///ACMD41/CMD55 timeout (should take ~1 second)
#define CMD58_TIMEOUT2          1000    ///CMD58 timeout after initialization
#define START_BLOCK_TIMEOUT     10000   ///CMD17 start block timeout
#define MULTI_TOKEN_TIMEOUT     1000    ///CMD24 receive token timeout
#define PRE_ERASE_TIMEOUT       1000	///CMD55 pre-erase timeout
/** @} */


/** @defgroup INTERRUPT_DEFS Interrupt Definitions
 * @{ */
#define DMA0_FLAG   IFS0bits.DMA0IF
#define DMA1_FLAG   IFS0bits.DMA1IF
#define DMA2_FLAG   IFS1bits.DMA2IF
#define TMR1_FLAG   IFS0bits.T1IF
#define TMR2_FLAG   IFS0bits.T2IF
#define TMR3_FLAG   IFS0bits.T3IF
#define DMA0_IE     IEC0bits.DMA0IE
#define DMA1_IE     IEC0bits.DMA1IE
#define TMR1_IE     IEC0bits.T1IE
#define TMR2_IE     IEC0bits.T2IE
#define TMR3_IE     IEC0bits.T3IE
/** @} */


/** @defgroup LCD_DISPLAY_DEFS LCD Display Definitions
 * @{ */
#define K1          LATBbits.LATB0
#define K1_DIR      TRISBbits.TRISB0
#define K2          LATBbits.LATB1
#define K2_DIR      TRISBbits.TRISB1
#define K3          LATBbits.LATB2
#define K3_DIR      TRISBbits.TRISB2
#define K4          LATEbits.LATE5
#define K4_DIR      TRISEbits.TRISE5
#define K5          LATEbits.LATE6
#define K5_DIR      TRISEbits.TRISE6

#define AA1         LATDbits.LATD7
#define AA1_DIR     TRISDbits.TRISD7
#define AB2         LATDbits.LATD6
#define AB2_DIR     TRISDbits.TRISD6
#define AC3         LATDbits.LATD5
#define AC3_DIR     TRISDbits.TRISD5
#define AD          LATDbits.LATD4
#define AD_DIR      TRISDbits.TRISD4
#define AE          LATDbits.LATD3
#define AE_DIR      TRISDbits.TRISD3
#define AF          LATDbits.LATD2
#define AF_DIR      TRISDbits.TRISD2
#define AG          LATDbits.LATD1
#define AG_DIR      TRISDbits.TRISD1
#define ADP         LATDbits.LATD0
#define ADP_DIR     TRISDbits.TRISD0

/** @} */


#endif