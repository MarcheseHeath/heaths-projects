#include <LiquidCrystal_I2C.h>
#include <L298N.h>
#include <floatToString.h>
#include "Rotary.h"

//Pins
#define ROTARY_A 3
#define ROTARY_B 2
const unsigned int IN1 = 8;
const unsigned int IN2 = 7;
const unsigned int EN = 9;
const int buttonPin = A3;

//Modules
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Initialise Liquid Crystal Display (16x2 Capacity)
Rotary encoder(ROTARY_A, ROTARY_B);	// Initialise the encoder on pins 2 and 3 (interrupt pins)
L298N motor(EN, IN1, IN2); // Create one motor instance

// Position variables for joints
float Pos = 0, Pos2 = 0;
char Parr[15], P2arr[15];

// State of push button
int buttonState = 0;

// Determines when the display should update
volatile bool updateDisplay = false;

// Time that has passed
long int time = 0;

//-------------------------------------------------------------------
//        Arduino Initialisation
//-------------------------------------------------------------------

void setup() 
{
  // Initialise the LCD, start the backlight and print a "bootup" message for two seconds
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.print("Double Pendulum");
  lcd.setCursor(0, 1);
  lcd.print("MXET 375 Project");
  delay(2000);

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Press the button");
  lcd.setCursor(0, 1);
  lcd.print("to turn on motor");

  // Set pins from encoder as interrupts when the pin changes
  attachInterrupt(digitalPinToInterrupt(ROTARY_A), encChange, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ROTARY_B), encChange, CHANGE);

  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);  
}

//-------------------------------------------------------------------
//        Arduino Loop Code
//-------------------------------------------------------------------

void loop() 
{
  for( ; ; )
  {
    buttonState = digitalRead(buttonPin); // read the state of the pushbutton value:
    if (buttonState == HIGH) // Button is pressed
    {       
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Intializing...");
      delay(2000);
      break;
    } 
    else 
    {
      continue;
    }
  }

  // Change display to show Angular Position
  lcd.clear();
  displayAngles();
  lcd.setCursor(0, 1);
  delay(500);
  
  motor.setSpeed(255);

  for (int x = 0; x < 9; x++)
  {
    motor.forwardFor(500);

    Pos += 1.693/9;
    updateDisplay = true;
    displayAngles();
    updateDisplay = false;
  }

  motor.stop();
  motor.reset();

  for (int x = 0; x < 3; x++)
  {
    Pos -= 0.698/3;
    updateDisplay = true;
    displayAngles();
    updateDisplay = false;
  }

  for (int x = 0; x < 10; x++)
  {
    motor.backwardFor(100);

    Pos -= 1.972/10;
    updateDisplay = true;
    displayAngles();
    updateDisplay = false;
  }

  motor.stop();

  for(int x = 0; x < 12; x++)
  {
    Pos += 1.833/12;
    updateDisplay = true;
    displayAngles();
    updateDisplay = false;
  }

  for(int x = 0; x < 10; x++)
  {
    Pos -= 1.309/10;
    updateDisplay = true;
    displayAngles();
    updateDisplay = false;
  }

  for(int x = 0; x < 10; x++)
  {
    Pos += 0.785/10;
    updateDisplay = true;
    displayAngles();
    updateDisplay = false;
  }

  for(int x = 0; x < 11; x++)
  {
    Pos -= 0.521/11;
    updateDisplay = true;
    displayAngles();
    updateDisplay = false;
  }

  for(int x = 0; x < 7; x++)
  {
    Pos += 0.301/7;
    updateDisplay = true;
    displayAngles();
    updateDisplay = false;
  }
  
  for(int x = 0; x < 8; x++)
  {
    Pos -= 0.112/8;
    updateDisplay = true;
    displayAngles();
    updateDisplay = false;
  }
  
  for( ; ; )
  {
    if (updateDisplay) 
    {
      displayAngles();
      updateDisplay = false;
    }
  }
}

//-------------------------------------------------------------------
//        Custom Functions
//-------------------------------------------------------------------

// ----- Determine the direction of encoder rotation -----
void encChange() 
{
  unsigned char state = encoder.process();
  if (state != DIR_NONE) 
  {
    updateAngle(state);
  }
}

// ----- Display current angular position -----
void displayAngles() 
{
  lcd.setCursor(0, 0);
  lcd.print(((char)242) + formatNumber(Pos,"1: "," rads",16));
  lcd.setCursor(0, 1);
  lcd.print(((char)242) + formatNumber(Pos2,"2: "," rads",16));
  Serial.println(formatSerial(Pos,Pos2));
}

// ----- Change the current angle based on stepValue -----
//  state - Either DIR_CW or DIR_CCW
int updateAngle(unsigned char state)
{
  if (state == DIR_CW)
  {
    Pos2 += 0.3142;
  }
  else
  {
    Pos2 -= 0.3142;
  }
  updateDisplay = true;
}

// ----- Pad string with spaces -----
// s - string to pad
// prefix - String to prefix number with
// returns padded String
String padString(String s, int pad)
{
  String sOut = String(s);
  int len = pad - s.length();
  for (uint8_t i = 0; i < len; i++)
  {
    sOut += ' ';
  }
  return sOut;
}

// ----- Display a number with comma seperators -----
// number - number to format
// prefix - String to prefix number with
// postfix - String to append to number
// pad - Add spaces to the right pad string
// returns String with formatted number
String formatNumber(float number, String prefix, String postfix, int pad)
{
  String s = "";
  if (number < 0 && number > -0.3)
  {
    number = 0;
  }
  s = prefix + number + postfix;
  return padString(s, pad);
}

// ----- Format serial output to be collected -----
// Pos - First angle to be extracted
// Pos2 - Second angle to be extracted
String formatSerial(float P, float P2)
{
  time = millis();
  String s = ", ", neg = "-", p1 = "", p2 = "";
  floatToString(P,Parr,sizeof(Parr),3);
  floatToString(P2,P2arr,sizeof(P2arr),3);
  p1 = Parr;
  p2 = P2arr;
  if (P < 0 && P > -1)
  {
    p1 = neg + Parr;
  }
  if (P2 < 0 && P2 > -1)
  {
    p2 = neg + P2arr;
  }
  s = time + s + p1 + s + p2;
  return s;
}