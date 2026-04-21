// Libraries
#include <L298N.h>
#include <Wire.h>
#include <U8g2lib.h>
#include "drill_bitmaps.h"

// Arduino UNO Pin Definitions
#define IN1 10
#define IN2 11
#define EN 6  // PWM Capable

#define FAN_SENSE 8
#define FAN_CTL 9 // PWM Capable

#define POT A0
#define THERMISTOR A1

// Default SDA A4
// Default SCL A5

// Data Variables
volatile unsigned int motorRead = 0; // Motor speed adjustment value from POT
volatile unsigned short motorSpeed = 0;  // Calculated motor speed
volatile unsigned int tempReading = 0; // Temp reading
volatile float tempValue = 0.0; // Calculated temp value
volatile const unsigned int criticalTemp = 400; // NEED TO ADJUST FOR SYSTEM EFFICIENCY
volatile unsigned int fanRead = 0; // Fan interrupt counter for speed sensing
volatile unsigned int fanSpeed = 0;  // Measured speed in RPM


// Display Pages
enum DisplayPage 
{
   PAGE_HOME, 
   PAGE_SPINNING  
};

DisplayPage currentPage = PAGE_HOME;

// Timing variables
unsigned long startMillis;
unsigned long lastMotorMillis = 0;
unsigned long lastTempMillis = 0;
unsigned long lastFanMillis = 0;
unsigned long lastDisplayMillis = 0;

// Update intervals (ms)
const unsigned long MOTOR_INTERVAL = 100; // Update motor speed every 100 ms
const unsigned long TEMP_INTERVAL  = 1000;  // Check temp every second
const unsigned long DISPLAY_INTERVAL = 100; // Update screen every 100 ms
const unsigned long FAN_INTERVAL = 60000; // Update rpm every minute

// Animate spin
const char *spinFrames[8] = 
{
  drill_frame_0_bits, 
  drill_frame_1_bits, 
  drill_frame_2_bits, 
  drill_frame_3_bits,
  drill_frame_4_bits,
  drill_frame_5_bits,
  drill_frame_6_bits,
  drill_frame_7_bits
};
int spinState = 0;

// Create U8G2 display object (SH1106 128x64, I2C)
U8G2_SH1106_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// Create motor object
L298N motor(EN, IN1, IN2);

void setup() 
{
  Serial.begin(115200);
  while(!Serial);
  
  // Initialize motor
  motor.setSpeed(0);

  // Initalize Display
  u8g2.begin();
  u8g2.setPowerSave(0);

  // Initialize fan
  pinMode(FAN_CTL, OUTPUT);
  analogWrite(FAN_CTL, 0);
  attachInterrupt(digitalPinToInterrupt(FAN_SENSE), fanRead, RISING);
}

void loop() 
{
  unsigned long now = millis();

  // ----- Update Motor -----
  if (now - lastMotorMillis >= MOTOR_INTERVAL) 
  {
    lastMotorMillis = now;

    motorRead = analogRead(POT);

    motorSpeed = map(motorRead, 0, 1023, 0, 255);

    if (motorSpeed > 15) 
    {
      currentPage = PAGE_SPINNING;
      motor.setSpeed(motorSpeed);
      motor.forward();
      Serial.println(motorSpeed);

      // Update animation
      spinState = (spinState + 1) % 8;
    } 
    else 
    {
      currentPage = PAGE_HOME;
      motor.stop();
      spinState = 0;
    }

    // Change this eventually to use rotary encoder!
    motorSpeed = map(motorRead, 0, 1023, 0, 7500);
  }

  // ----- Update Temperature -----
  if (now - lastTempMillis >= TEMP_INTERVAL) 
  {
    lastTempMillis = now;
    tempReading = analogRead(THERMISTOR);

    // ADD WAY TO CALCULATE AMBIENT TEMP
  }

  // ----- Update Fan -----
  if (tempReading - criticalTemp > 0) 
  {
    analogWrite(FAN_CTL, 204);

    if (now - lastFanMillis >= FAN_INTERVAL)
    {
      fanSpeed = fanRead * 30;
      fanRead = 0;

      // Fan speed not being used for anything
      //Serial.println("Fan Speed %d rpm", fanSpeed);
    }
  }
  else
  {
    analogWrite(FAN_CTL, 0);
  }

  // ----- Update Display -----
  if (now - lastDisplayMillis >= DISPLAY_INTERVAL) 
  {
    lastDisplayMillis = now;
    updateDisplay();
  }
}

void updateDisplay() 
{
  char buf[16];
  u8g2.firstPage();
  do 
  {
    if (currentPage == PAGE_HOME) // HOME PAGE
    {
      u8g2.setFont(u8g2_font_6x10_tf);
      u8g2.drawStr(0, 10, "Mini Drill Press");
      u8g2.drawHLine(0, 12, 128);

      uint8_t boxX = 10, boxY = 20, boxW = 108, boxH = 40;
      u8g2.drawFrame(boxX, boxY, boxW, boxH);

      u8g2.setFont(u8g2_font_4x6_tf);
      u8g2.setCursor(boxX + 5, boxY + 8);
      u8g2.print("Not Drilling");

      // Smooth run-time counter
      unsigned long elapsed = (millis() - startMillis) / 60000;
      u8g2.setCursor(boxX + 5, boxY + 18);
      u8g2.print("Run Time: ");
      sprintf(buf, "%lu min", elapsed);
      u8g2.print(buf);

      // Temperature
      u8g2.setCursor(boxX + 5, boxY + 28);
      u8g2.print("Temperature: ");
      sprintf(buf, "%d F", tempReading);
      u8g2.print(buf);

      // Placeholder
      u8g2.setCursor(boxX + 5, boxY + 38);
      u8g2.print("Heath Marchese :)");

    } 
    else // SPINNING PAGE
    {
      //u8g2.setFont(u8g2_font_helvB24_tf);
      u8g2.setFont(u8g2_font_ncenB08_tr);
      u8g2.drawStr(0, 10, "Device Drilling!");
      u8g2.drawHLine(0, 12, 128);

      u8g2.setFont(u8g2_font_6x10_tf);
      u8g2.drawStr(10, 24, "Current");
      u8g2.drawStr(12, 36, "Speed:");
      sprintf(buf, "%hd rpm", motorSpeed);
      u8g2.drawStr(8, 48, buf);

      // Drill spinning animation
      u8g2.drawXBMP(80, 16, 32, 32, spinFrames[spinState]);

      u8g2.setFont(u8g2_font_4x6_tf);
      u8g2.setCursor(24, 63);
      u8g2.print("Temperature: ");
      sprintf(buf, "%d F", tempReading);
      u8g2.print(buf);
    }
  } while (u8g2.nextPage());
}