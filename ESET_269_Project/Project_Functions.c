void delay32(int t)   //This function creates the delay in between LED blinks with System32
{
	TIMER32_1 -> CONTROL |= 0x43;    //Configure one-shot mode for Timer32_1
	TIMER32_1 -> LOAD = (t * 3000000) - 1;   //Creates delay time of t seconds from clock speed (3MHz)
	TIMER32_1 -> CONTROL |= 0x80;   //Starts Timer32_1
	while ((TIMER32_1 -> RIS & 1) == 0) {}  //This loop is the "Delay"
	TIMER32_1 -> INTCLR = 0; //Reset Timer32_1
	return;
}

void delaySys()		//This function creates the delay in between Temperature reads with SystemTick
{
	SysTick -> LOAD = (3000000) - 1;	//Creates a delay time of 1 second from clock speed (3MHz)
	SysTick -> CTRL |= 0x4;	//
	SysTick -> CTRL |= 0x1;	//Starts SysTick delay
	while ((SysTick -> CTRL & 0x10000) == 0) {}	//While COUNTERFLAG is not set
	SysTick -> CTRL &= ~0x1;	//Stops SysTick delay
	return;
}

void TX(char text[])    //This function transmits data to the microcontroller
{
    int i =0;
    while(text[i] != '\0')
    {
        EUSCI_A0 ->TXBUF = text[i];
        while((EUSCI_A0 ->IFG & 0x02) == 0)
        {
            //wait until character sent
        }
        i++;
    }
    return;
}

int RX(void)    //This function recieves data from the microcontroller
{
    int i = 0;
    char command[2];
    char x;
    while(1)
    {
        if((EUSCI_A0 ->IFG & 0x01) !=0) //data in RX buffer
        {
            command[i] = EUSCI_A0 ->RXBUF;
            EUSCI_A0 ->TXBUF = command[i]; //echo
            while((EUSCI_A0 ->IFG & 0x02)==0); //wait
            if(command[i] == '\r')
            {
                command[i] = '\0';
                break;
            }
            else
            {
                i++;
            }
        }
    }
    x = atoi(command);
    TX("\n\r");
    return x;
}

void LED_Blink()    //This function will perform the RGB blinking when called on
{	
	int bias, toggle, blink;    //Establish variables
    TX("\nEnter Combination of RGB (1 - 7): ");	//How many LEDS turn on?
	bias = RX();
	if (bias > 7 || bias < 1)	//Defaults a value to 7 if it is out of range
	{
		bias = 7;
	}
	TX("Enter Toggle Time (in seconds): ");	//Toggle time determines how long leds are illuminated
	toggle = RX();
	TX("Enter Number of Blinks: ");	//Number of times the LEDs blink
	blink = RX();
	TX("Now blinking...\n\n\r");
	
	P2 -> SEL0 &= ~7; //Setting P2.2-P2.0 to I/O
	P2 -> SEL1 &= ~7;
	P2 -> DIR |= 7;
	P2 -> OUT &= ~7; //Reset LEDs
	
    for (int i = 0; i < blink; i++)
    {
        if (i == blink - 1)
        {
            P2 -> OUT |= bias;  //Turn on LEDs
            delay32(toggle);	//Time that LEDs are turned ON

            P2 -> OUT &= ~bias;	//Turn off LEDs
            continue; //No Reason to delay the last cycle
        }
				P2 -> OUT |= bias;  //Turn on LEDs
        delay32(toggle);	//Time that LEDs are turned ON

        P2 -> OUT &= ~bias;	//Turn off LEDs
        delay32(toggle);	//Time that LEDs are turned OFF
    }
    TX("Blinking Complete!\n\n\r");
	return;
}

void Digital_Read()    //This function will perform the Digital I/O Reading when called on
{
    P1 -> SEL0 &= ~0x12;
	P1 -> SEL1 &= ~0x12; //Setting P1 to GIO, Input
	P1 -> DIR &= ~0x12;
	P1 -> REN |= 0x12; //Setting Pull up Resistors, Output
	P1 -> OUT |= 0x12;
	
	int b1, b2;
	b1 = P1 -> IN & 0x10;
	b2 = P1 -> IN & 0x02; //Logic for Checking Buttons
	if (b1 >= 1 && b2 >= 1) //Checks for no buttons
    {
        TX("\nNo Buttons Pressed\n\n\r");
    }
	else if (b1 >= 1 && b2 == 0)  //Checks for button 1
    {
        TX("\nButton 1 Pressed\n\n\r");
    }
	else if (b1 == 0 && b2 >= 1)  //Checks for button 2
    {
        TX("\nButton 2 Pressed\n\n\r");
    }
	else if (b1 == 0 && b2 == 0)  //Checks for both buttons
    {
        TX("\nBoth Buttons Pressed\n\n\r");
    }
    return;
}

float tempRead(void)    //This function will read the temperature of the microcontroller once using the ADC
{
    float temp; //temperature variable
    uint32_t cal30 = TLV -> ADC14_REF2P5V_TS30C; //calibration constant
    uint32_t cal85 = TLV -> ADC14_REF2P5V_TS85C; //calibration constant
    float calDiff = cal85 - cal30; //calibration difference
    ADC14 -> CTL0 |= 0x01; //start conversion
    while((ADC14 -> IFGR0) ==0)
    {
        //wait for conversion
    }
    temp = ADC14 -> MEM[0]; //assign ADC value
    temp = (temp - cal30) * 55; //math for temperature per manual
    temp = (temp / calDiff) + 30; //math for temperature per manual
    return temp; //return temperature in degrees C
}

void Temperature_Read() //This function will read the temperature of the microcontroller a set amount of times when called on
{
    int count;
    char out[50];
	TX("\nEnter Number of Temperature Readings (1-5): ");
	count = RX();
	if (count >= 5 || count <= 0)
    {
        count = 5;
    }
	TX("\n");
	for (int i = 0; i < count; i++)
    {
        float C = tempRead();
        float F = (C * 1.8) + 32;
        sprintf(out, "Reading %d: %d.%02d C & %d.%02d F\n\r",i + 1,(int)C,(int)((C - (int)C) * 100),(int)F,(int)((F - (int)F) * 100));
        TX(out);
        delaySys();
	}
		TX("\n");
    return;
}

void ADCInit(void)  //This function initializes the Analog to Digital unit for the temperature scan
{
    //Ref_A settings
    REF_A ->CTL0 &= ~0x8; //enable temp sensor
    REF_A ->CTL0 |= 0x30; //set ref voltage
    REF_A ->CTL0 &= ~0x01; //enable ref voltage
    //do ADC stuff
    ADC14 ->CTL0 |= 0x10; //turn on the ADC
    ADC14 ->CTL0 &= ~0x02; //disable ADC
    ADC14 ->CTL0 |=0x4180700; //no prescale, mclk, 192 SHT
    ADC14 ->CTL1 &= ~0x1F0000; //configure memory register 0
    ADC14 ->CTL1 |= 0x800000; //route temp sense
    ADC14 ->MCTL[0] |= 0x100; //vref pos int buffer
    ADC14 ->MCTL[0] |= 22; //channel 22
    ADC14 ->CTL0 |=0x02; //enable adc
    return;
}

void UARTInit(void) //This function initializes UART for the microcontroller so that user communication is established
{
    EUSCI_A0 ->CTLW0 |= 1;
    EUSCI_A0 ->MCTLW = 0;
    EUSCI_A0 ->CTLW0 |= 0x80;
    EUSCI_A0 ->BRW = 0x34;
    EUSCI_A0 ->CTLW0 &= ~0x01;
    P1->SEL0 |= 0x0C;
    P1->SEL1 &= ~0x0C;
    return;
}