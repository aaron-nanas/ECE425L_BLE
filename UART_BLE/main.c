/*
 * @file main.c
 *
 * @brief Main source code for the UART_BLE program.
 *
 * This file contains the main entry point and function definitions for the UART_BLE program.
 * 
 * The following modules are used:
 *  - UART0: Used to print the measured distance from the US-100 on the serial terminal
 *  - UART1: Used to establish communication between the Tiva TM4C123G LaunchPad
 *           and an Adafruit BLE UART module
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
 * @note For more information regarding the UART module, refer to the
 * Universal Asynchronous Receivers / Transmitters (UARTs) section
 * of the TM4C123GH6PM Microcontroller Datasheet.
 * - Link: https://www.ti.com/lit/gpn/TM4C123GH6PM
 *
 * @note For more information regarding the Adafruit BLE UART module, refer
 * to the following link.
 * - Link: https://www.adafruit.com/product/2479
 *
 * @author Aaron Nanas
 */
 
#include "TM4C123GH6PM.h"
#include "stdlib.h"
#include "SysTick_Delay.h"
#include "UART0.h"
#include "UART_BLE.h"
#include "GPIO.h"

#define BUFFER_SIZE   128

void Process_UART_BLE_Data(char UART_BLE_Buffer[]);

int main(void)
{		
	// Initialize the SysTick timer used to provide blocking delay functions
	SysTick_Delay_Init();
	
	// Initialize the RGB LED on the TM4C123G LaunchPad
	RGB_LED_Init();
	
	// Initialize an array to store the characters received from the Adafruit BLE UART module
	char UART_BLE_Buffer[BUFFER_SIZE];

	// Initialize the UART0 module which will be used to print characters on the serial terminal
	UART0_Init();
	
	// Initialize the UART1 module which will be used to communicate with the Adafruit BLE UART module
	UART_BLE_Init();
	
	// Provide a short delay after initialization and reset the Adafruit BLE UART module
	SysTick_Delay1ms(1000);
	UART_BLE_Reset();
	
	// Send a message to the Adafruit BLE UART module to check if the connection is stable
	UART_BLE_Output_String("UART BLE Active\n");
	SysTick_Delay1ms(1000);
	
	while(1)
	{
		int string_size = UART_BLE_Input_String(UART_BLE_Buffer, BUFFER_SIZE);
		
		UART0_Output_String("String Size: ");
		UART0_Output_Unsigned_Decimal(string_size);
		UART0_Output_Newline();
		
		UART0_Output_String("UART BLE Data: ");
		
		for (int i = 0; i < string_size; i++)
		{
			if (UART_BLE_Buffer[i] == 0)
			{
				UART0_Output_Character('A');
			}
			else
			{
				UART0_Output_Character(UART_BLE_Buffer[i]);
			}
		}
		
		UART0_Output_Newline();
		
		Process_UART_BLE_Data(UART_BLE_Buffer);
		
		UART0_Output_Newline();
	}
}

void Process_UART_BLE_Data(char UART_BLE_Buffer[])
{
	if (Check_UART_BLE_Data(UART_BLE_Buffer, "RGB LED GREEN"))
	{
		RGB_LED_Output(RGB_LED_GREEN);
	}
	
	else if (Check_UART_BLE_Data(UART_BLE_Buffer, "RGB LED BLUE"))
	{
		RGB_LED_Output(RGB_LED_BLUE);
	}
	
	else if (Check_UART_BLE_Data(UART_BLE_Buffer, "RGB LED RED"))
	{
		RGB_LED_Output(RGB_LED_RED);
	}
	
	else if (Check_UART_BLE_Data(UART_BLE_Buffer, "RGB LED OFF"))
	{
		RGB_LED_Output(RGB_LED_OFF);
	}
	
	else if (Check_UART_BLE_Data(UART_BLE_Buffer, "ATZ"))
	{
		UART0_Output_String("UART BLE Reset Command Issued");
		UART0_Output_Newline();
	}
	
	else if (Check_UART_BLE_Data(UART_BLE_Buffer, "OK"))
	{
		// Read the last received data from the UART Receive Buffer
		
		// Note: After a reset is issued and the module responds with "OK",
		// the module transmits a null character. This prevents the null character
		// from being added to the buffer when the user sends a command string for the first time
		char character = UART_BLE_Input_Character();
		
		UART0_Output_String("UART BLE Response Received");
		UART0_Output_Newline();
	}
	
	else
	{
		UART0_Output_String("UART BLE Command Not Found");
		UART0_Output_Newline();
	}
}
