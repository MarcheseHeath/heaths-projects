#include <stdio.h> 
#include <stdlib.h>
#include "msp.h"
#include "Project_Functions.c"

void LED_Blink();
void delay32(int t);
void Digital_Read();
float tempRead(void);
void ADCInit(void);
void UARTInit(void);
void TX(char text[]);
int RX(void);

int main (void) //Intialize the MCU, runs code once
{
	
	UARTInit();	//Start UART (Default pin is A0)
	ADCInit();	//Start ADC
	
	while (1) //Loop code infinitely
	{	
		TX("MSP432 MENU\n\n\r1) RGB Control\n\r2) Digital Input\n\r3) Temperature Reading\n\n\r");	//Display menu in console
		int in = RX();	//Recieves input values
		if(in == 1)	//RGB Control
		{
			LED_Blink();
		}
		else if(in == 2)	//Digital Input
		{
			Digital_Read();
		}
		else if(in == 3)	//Temperature Control
		{
			Temperature_Read();
		}
		else if(in << 1 || in >> 3)	//Invalid Input
		{
			TX("\nPlease enter a valid selection\n\n\r");
		}
	}
}