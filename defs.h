/*! \file defs.h
    \brief Contains all definitions
    
    
*/

#ifndef INC_DEFS_H
#define INC_DEFS_H


#define FCY 40000000UL ///40 MIPS operating speed (used for __delay_ms() and __delay_S())


/** @defgroup MODEM_PINS Modem Pins
 * @{ */
//parallel pins for modem nibble transmissions
#define MODEM0      LATDbits.LATD4
#define MODEM1      LATDbits.LATD5
#define MODEM2      LATDbits.LATD6
#define MODEM3      LATDbits.LATD7
#define MODEM0_DIR  TRISDbits.TRISD4
#define MODEM1_DIR  TRISDbits.TRISD5
#define MODEM2_DIR  TRISDbits.TRISD6
#define MODEM3_DIR  TRISDbits.TRISD7

#define MODEM_CTRL_0        PORTDbits.RD1		///The HEYRX pin is used by the MODEM to indicate to the CPU that a nibble transmission is ready on the modem pins
#define MODEM_CTRL_0_DIR    TRISDbits.TRISD1
#define MODEM_CTRL_1        LATDbits.LATD2		///The CLRTX pin is used by the CPU to tell the MODEM it has received a nibble and acted accordingly
#define MODEM_CTRL_1_DIR    TRISDbits.TRISD2
#define MODEM_CTRL_2        PORTDbits.RD3		///The SYNC flag is used by the modem to tell the CPU a sync pulse needs to be placed into the buffer
#define MODEM_CTRL_2_DIR    TRISDbits.TRISD3
/** @} */


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

/** @defgroup ADC_PINS ADC Pins
 * @{ */
#define SDI2_PULLUP_ENABLE  CNPU1bits.CN9PUE
#define SDI2_PIN            PORTGbits.RG7
#define SDI2_PIN_DIR        TRISGbits.TRISG7
#define SCK2_PIN            LATGbits.LATG6
#define SCK2_PIN_DIR        TRISGbits.TRISG6
#define CS2_PIN             LATGbits.LATG8
#define CS2_PIN_DIR         TRISGbits.TRISG8
/** @} */

/** @defgroup SRAM_PINS SRAM Pins
 * @{ */
#define ADDR_L			LATB				///A0-A15
#define ADDR_L_DIR		TRISB
#define A16				LATFbits.LATF3  	///A16
#define A16_DIR			TRISFbits.TRISF3  	///A16
#define A17				LATFbits.LATF2  	///A17
#define A17_DIR			TRISFbits.TRISF2  	///A17
#define A18				LATDbits.LATD14		///A18
#define A18_DIR			TRISDbits.TRISD14	///A18
#define DATA_PORT_IN	PORTA				///only the LSB 0-7 are used for D0-D7
#define DATA_PORT_OUT	LATA
#define DATA_PORT_DIR   TRISA
#define CE				LATAbits.LATA10
#define CE_DIR			TRISAbits.TRISA10
#define OE				LATFbits.LATF13
#define OE_DIR			TRISFbits.TRISF13
#define WE				LATFbits.LATF12
#define WE_DIR			TRISFbits.TRISF12
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

/** @defgroup MODEM_COMMANDS Modem Commands
 * @{ */
#define SYNC        0x80 /// insert sync flag into data
#define HEADER      0x81 /// start the header
#define CONFIG      0x82 /// not used
#define START       0x83 /// end the header, start recording
#define STOP        0x84 /// stop recording
#define LED1_ON     0b10101010 ///turn LED 1 on
#define LED1_OFF    0b01010101 /// turn LED 1 off
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
#endif
