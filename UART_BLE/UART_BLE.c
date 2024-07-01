/**
 * @file UART_BLE.c
 *
 * @brief Source code for the UART_BLE driver.
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

#include "UART_BLE.h"

void UART_BLE_Init(void)
{
	// Enable the clock to UART1 by setting the 
	// R1 bit (Bit 1) in the RCGCUART register
	SYSCTL->RCGCUART |= 0x02;
	
	// Enable the clock to Port B by setting the
	// R10 bit (Bit 1) in the RCGCGPIO register
	SYSCTL->RCGCGPIO |= 0x02;
	
	// Disable the UART1 module before configuration by clearing
	// the UARTEN bit (Bit 0) in the CTL register
	UART1->CTL &= ~0x01;
	
	// Set the baud rate by writing to the DIVINT field (Bits 15 to 0)
	// and the DIVFRAC field (Bits 15 to 0) in the IBRD and FBRD registers, respectively.
	// The integer part of the calculated constant will be written to the IBRD register,
	// while the fractional part will be written to the FBRD register.
	// N = (System Clock Frequency) / (16 * Baud Rate)
	// N = (50,000,000) / (16 * 9600) = 325.5208333 (N = 325)
	// F = ((0.5208333 * 64) + 0.5) = 33.8333312 (F = 33)
	UART1->IBRD = 325;
	UART1->FBRD = 33;
	
	// Configure the data word length of the UART packet to be 8 bits by 
	// writing a value of 0x3 to the WLEN field (Bits 6 to 5) in the LCRH register
	UART1->LCRH |= 0x60;
	
	// Enable the transmit and receive FIFOs by setting the FEN bit (Bit 4) in the LCRH register
	UART1->LCRH |= 0x10;
	
	// Select one stop bit to be transmitted at the end of a UART frame by
	// clearing the STP2 bit (Bit 3) in the LCRH register
	UART1->LCRH &= ~0x08;
	
	// Disable the parity bit by clearing the PEN bit (Bit 1) in the LCRH register
	UART1->LCRH &= ~0x02;
	
	// Enable the UART1 module after configuration by setting
	// the UARTEN bit (Bit 0) in the CTL register
	UART1->CTL |= 0x01;
	
	// Configure the PB1 (U1TX) and PB0 (U1RX) pins to use the alternate function
	// by setting Bits 1 to 0 in the AFSEL register
	GPIOB->AFSEL |= 0x03;
	
	// Clear the PMC1 (Bits 7 to 4) and PMC0 (Bits 3 to 0) fields in the PCTL register before configuration
	GPIOB->PCTL &= ~0x000000FF;
	
	// Configure the PB1 pin to operate as a U1TX pin by writing 0x1 to the
	// PMC1 field (Bits 7 to 4) in the PCTL register
	// The 0x1 value is derived from Table 23-5 in the TM4C123G Microcontroller Datasheet
	GPIOB->PCTL |= 0x00000010;
	
	// Configure the PB0 pin to operate as a U1RX pin by writing 0x1 to the
	// PMC0 field (Bits 3 to 0) in the PCTL register
	// The 0x1 value is derived from Table 23-5 in the TM4C123G Microcontroller Datasheet
	GPIOB->PCTL |= 0x00000001;
	
	// Enable the digital functionality for the PB1 and PB0 pins
	// by setting Bits 1 to 0 in the DEN register
	GPIOB->DEN |= 0x03;
	
	// Set PB7 as an output GPIO pin
	GPIOB->DIR |= 0x80;
	
	// Configure PB7 to function as a GPIO pin
	GPIOB->AFSEL &= ~0x80;
	
	// Enable Digital Functionality for PB7
	GPIOB->DEN |= 0x80;
}

char UART_BLE_Input_Character(void)
{
	while((UART1->FR & UART1_RECEIVE_FIFO_EMPTY_BIT_MASK) != 0);
	
	return (char)(UART1->DR & 0xFF);
}

void UART_BLE_Output_Character(char data)
{
	while((UART1->FR & UART1_TRANSMIT_FIFO_FULL_BIT_MASK) != 0);
	UART1->DR = data;
}

int UART_BLE_Input_String(char *buffer_pointer, uint16_t buffer_size) 
{
	int length = 0;
	int string_size = 0;
	
	// Read the last received data from the UART Receive Buffer
	char character = UART_BLE_Input_Character();
	
	// Check if the received character is a line feed. Otherwise,
	// for each valid character, increment the string_size variable which will
	// indicate how many characters have been detected from the input string
	while(character != UART1_LF)
	{
		// Remove the character from the buffer is the received character is a backspace character
		if (character == UART1_BS)
		{
			if (length)
			{
				buffer_pointer--;
				length--;
				UART_BLE_Output_Character(UART1_BS);
			}
		}
		
		// Otherwise, if there are more characters to be read, store them in the buffer
		else if (length < buffer_size && character != UART1_CR)
		{
			*buffer_pointer = character;
			buffer_pointer++;
			length++;
			string_size++;
		}
		character = UART_BLE_Input_Character();
	}
	*buffer_pointer = 0;
	
	return string_size;
}

void UART_BLE_Output_String(char *pt)
{
	while(*pt)
	{
		UART_BLE_Output_Character(*pt);
		pt++;
	}
}

void UART_BLE_Reset(void)
{
	GPIOB->DATA |= 0x80;
	SysTick_Delay1ms(1000);
	
	UART_BLE_Output_String("ATZ\r\n");
	SysTick_Delay1ms(3000);
	
	GPIOB->DATA &= ~0x80;
}

uint8_t Check_UART_BLE_Data(char UART_BLE_Buffer[], char *data_string)
{
	if (strstr(UART_BLE_Buffer, data_string) != NULL)
	{
		return 0x01;
	}
	
	else
	{
		return 0x00;
	}
}