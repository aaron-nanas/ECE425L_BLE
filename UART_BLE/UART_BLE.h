/**
 * @file UART_BLE.h
 *
 * @brief Header file for the UART_BLE driver.
 *
 * This file contains the function definitions for the UART_BLE driver.
 *
 * @note For more information regarding the UART module, refer to the
 * Universal Asynchronous Receivers / Transmitters (UARTs) section
 * of the TM4C123GH6PM Microcontroller Datasheet.
 * Link: https://www.ti.com/lit/gpn/TM4C123GH6PM
 *
 * @note Assumes that the system clock (50 MHz) is used.
 *
 * The Adafruit BLE UART module uses the following pinout:
 *  - BLE UART MOD (Pin 1)  <-->  Tiva LaunchPad Pin PB7
 *  - BLE UART CTS (Pin 2)	<-->  Tiva LaunchPad Pin GND
 *  - BLE UART TXO (Pin 3) 	<-->  Tiva LaunchPad Pin PB0 (U1RX)
 *  - BLE UART RXI (Pin 4)  <-->  Tiva LaunchPad Pin PB1 (U1TX)
 *  - BLE UART VIN (Pin 5)  <-->  Tiva LaunchPad VCC (3.3V)
 *  - BLE UART RTS (Pin 6)  <-->  Not Connected
 *  - BLE UART GND (Pin 7)  <-->  Tiva LaunchPad GND
 *  - BLE UART DFU (Pin 8)  <-->  Not Connected
 *
 * @note For more information regarding the Adafruit BLE UART module, refer
 * to the following link.
 * - Link: https://www.adafruit.com/product/2479
 *
 * @author Aaron Nanas
 */

#include "TM4C123GH6PM.h"
#include "SysTick_Delay.h"
#include "string.h"

#define UART1_RECEIVE_FIFO_EMPTY_BIT_MASK 0x10
#define UART1_TRANSMIT_FIFO_FULL_BIT_MASK 0x20

/**
 * @brief Carriage return character
 */
#define UART1_CR   0x0D
/**
 * @brief Line feed character
 */
#define UART1_LF   0x0A
/**
 * @brief Back space character
 */
#define UART1_BS   0x08
/**
 * @brief escape character
 */
#define UART1_ESC  0x1B
/**
 * @brief space character
 */
#define UART1_SP   0x20
/**
 * @brief delete character
 */
#define UART1_DEL  0x7F

/**
 * @brief The UART_BLE_Init function initializes the UART1 module.
 *
 * This function configures the UART1 module with the following configuration:
 *
 * - Parity: Disabled
 * - Bit Order: Least Significant Bit (LSB) first
 * - Character Length: 8 data bits
 * - Stop Bits: 1
 * - UART Clock Source: System Clock (50 MHz)
 * - Baud Rate: 9600
 *
 * @note The PB1 (TX) and PB0 (RX) pins are used for UART communication via USB.
 *
 * @return None
 */
void UART_BLE_Init(void);

/**
 * @brief The UART_BLE_Input_Character function reads a character from the UART data register.
 *
 * This function waits until a character is available in the UART receive buffer
 * from the serial terminal input and returns the received character as a char type.
 *
 * @param None
 *
 * @return The received character from the serial terminal as a char type.
 */
char UART_BLE_Input_Character(void);

/**
 * @brief The UART_BLE_Output_Character function transmits a character via UART to the serial terminal.
 *
 * This function waits until the UART transmit buffer is ready to accept
 * a new character and then writes the specified character in the transmit buffer to the serial terminal.
 *
 * @param data The character to be transmitted to the serial terminal.
 *
 * @return None
 */
void UART_BLE_Output_Character(char data);

/**
 * @brief The UART_BLE_Input_String function reads a string from the UART receive buffer.
 *
 * This function reads characters from the UART receive buffer in the serial terminal
 * until a line feed (LF) character is encountered.
 * The characters are stored in the provided buffer (buffer_pointer) up to the specified maximum length (buffer_size).
 * The function supports backspace (BS) character for deleting characters from the buffer.
 *
 * @param buffer_pointer Pointer to the buffer where the received string will be stored.
 * @param buffer_size Maximum length of the buffer.
 *
 * @return Returns the total number of characters received from the input string.
 */
int UART_BLE_Input_String(char *buffer_pointer, uint16_t buffer_size);

/**
 * @brief The UART_BLE_Output_String function transmits a null-terminated string via UART to the serial terminal.
 *
 * This function transmits characters from the provided string (pt) via UART to the serial terminal
 * until a null character is encountered.
 *
 * @param pt Pointer to the null-terminated string to be transmitted.
 *
 * @return None
 */
void UART_BLE_Output_String(char *pt);

/**
 * @brief The UART_BLE_Reset function issues a reset command by sending the ATZ string to the BLE module.
 *
 * @param None
 *
 * @return None
 */
void UART_BLE_Reset(void);

/**
 * @brief The Check_UART_BLE_Data function checks if the specified string (data_string) is 
 * in the given char array (UART_BLE_Data_Buffer).
 *
 * @param UART_BLE_Data_Buffer The char array that contains the command string received from the BLE module.
 * @param data_string Pointer to the string to be checked.
 *
 * @return Returns 1 if the specified string is in the buffer. Otherwise, returns 0.
 */
uint8_t Check_UART_BLE_Data(char UART_BLE_Data_Buffer[], char *data_string);
