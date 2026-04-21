/*--------------------------------------------------------------------------------------*/
/*-------------------------- VARIABLES AND DEFINITIONS ---------------------------------*/
/*--------------------------------------------------------------------------------------*/

// Libraries
#include <Stdio.h>        // Standard I/O Lib
#include <Wire.h>         // IC2 Lib
#include <U8g2lib.h>      // Display Lib
#include <ESP32Servo.h>   // Servo Lib

// Create U8G2 display object (SH1106 128x64, I2C)
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ 22, /* data=*/ 21);

// Create servo objects 
Servo elbow1;
Servo elbow2;
Servo swivel;
Servo gripper;

// Establish PWM
ESP32PWM pwm;

// Published values for SG90 servos; adjust if needed
int minUs = 1000;
int maxUs = 2000;

// Raw pot values
uint16_t braw, e1raw, e2raw, sraw, graw;

// Joint Angles (in degrees)
int bpos, e1pos, e2pos, spos, gpos;

// Stepper variables
int currentBasePos = 0; // Actual base pos

int stepIndex = 0;
const int steps[4][4] = 
{
  {1,0,0,1},
  {1,0,1,0},
  {0,1,1,0},
  {0,1,0,1}
};


// Save pos state
bool saveState = false;

// Pin definitions
#define BASE_POT 32
#define ELBOW1_POT 33
#define ELBOW2_POT 25
#define SWIVEL_POT 26
#define GRIPPER_POT 27
#define SAVE 13
#define ELBOW1_PIN 19
#define ELBOW2_PIN 18
#define SWIVEL_PIN 17
#define GRIPPER_PIN 16
#define STEP1 4
#define STEP2 0
#define STEP3 2
#define STEP4 15

/*--------------------------------------------------------------------------------------*/
/*----------------------------- MAIN ARDUINO PROGRAM -----------------------------------*/
/*--------------------------------------------------------------------------------------*/

void setup() 
{
  // Init I2C pins and Serial
  Wire.begin(21,22);
  Serial.begin(115200);
  delay(1000);
  
  // Init display
  u8g2.begin();
  bootAnimation();
  delay(1000);

  // Init button
  pinMode(SAVE, INPUT_PULLUP);

  // Allow allocation of all timers
	ESP32PWM::allocateTimer(0);
	ESP32PWM::allocateTimer(1);
	ESP32PWM::allocateTimer(2);
	ESP32PWM::allocateTimer(3);

  // Attach servos
  elbow1.attach(ELBOW1_PIN, minUs, maxUs);
  elbow2.attach(ELBOW2_PIN, minUs, maxUs);
  swivel.attach(SWIVEL_PIN, minUs, maxUs);
  gripper.attach(GRIPPER_PIN, minUs, maxUs);

  // Stepper pins
  pinMode(STEP1, OUTPUT);
  pinMode(STEP2, OUTPUT);
  pinMode(STEP3, OUTPUT);
  pinMode(STEP4, OUTPUT);

  // Potentiometer Data Aquisiton Task
  xTaskCreate(
    Aquisition,          // Task function
    "AquisitionTask",    // Name
    2048,                // Stack size
    NULL,                // Parameters
    5,                   // Priority
    NULL);               // Task handle

  // Motor Driver Task
  xTaskCreate(
    Driver,              // Task function
    "DriverTask",        // Name
    2048,                // Stack size
    NULL,                // Parameters
    5,                   // Priority
    NULL);               // Task handle

  // OLED Display Task
  xTaskCreate(
    Display,             // Task function
    "DisplayTask",       // Name
    2048,                // Stack size
    NULL,                // Parameters
    2,                   // Priority
    NULL);               // Task handle  
}

void loop() 
{
  delay(10);
}

/*--------------------------------------------------------------------------------------*/
/*----------------------------- DATA AQUISITION TASK -----------------------------------*/
/*--------------------------------------------------------------------------------------*/

static void Aquisition(void *arg) 
{
  bool lastPress = HIGH;
  unsigned long lastDebounce = 0;

  for (;;)
  {
    unsigned long now = millis();
    
    // Read raw ADC (0–4095 on ESP32)
    braw = analogRead(BASE_POT);
    e1raw = analogRead(ELBOW1_POT);
    e2raw = analogRead(ELBOW2_POT);
    sraw = analogRead(SWIVEL_POT);
    graw = analogRead(GRIPPER_POT);

    // Map to angles (0–180)
    bpos  = map(braw,  0, 4095, 0, 180);
    e1pos = map(e1raw, 0, 4095, 0, 180);
    e2pos = map(e2raw, 0, 4095, 0, 180);
    spos  = map(sraw,  0, 4095, 0, 180);
    gpos  = map(graw,  0, 4095, 0, 180);

    // Save state button (hold for 3 seconds)
    bool press = digitalRead(SAVE);
    if (lastPress == HIGH && press == LOW && now - lastDebounce > 3000) 
    {
      saveState = !saveState; // Invert save state

      lastDebounce = now;
    }

    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

/*--------------------------------------------------------------------------------------*/
/*------------------------------- MOTOR DRIVER TASK ------------------------------------*/
/*--------------------------------------------------------------------------------------*/

static void Driver(void *arg) 
{
  for (;;)
  {
    elbow1.write(e1pos);
    elbow2.write(e2pos);
    swivel.write(spos);
    gripper.write(gpos);



    if (currentBasePos < bpos)
    {
      stepMotor(1);
      currentBasePos++;
    }
    else if (currentBasePos > bpos)
    {
      stepMotor(-1);
      currentBasePos--;
    }

    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

void stepMotor(int dir)
{
  stepIndex += dir;

  if (stepIndex > 3) stepIndex = 0;
  if (stepIndex < 0) stepIndex = 3;

  digitalWrite(STEP1, steps[stepIndex][0]);
  digitalWrite(STEP2, steps[stepIndex][1]);
  digitalWrite(STEP3, steps[stepIndex][2]);
  digitalWrite(STEP4, steps[stepIndex][3]);
}

/*--------------------------------------------------------------------------------------*/
/*--------------------------------- DISPLAY TASK ---------------------------------------*/
/*--------------------------------------------------------------------------------------*/

static void Display(void *arg) 
{
  for (;;)
  {
    u8g2.clearBuffer(); // Clear internal memory
    char outputString[10];

    u8g2.setFont(u8g2_font_6x10_tf);  // Set title font
    u8g2.drawStr(0, 10, "Heath - Robot Arm v1.00"); // Program title
    u8g2.drawHLine(0, 12, 128);  // Horizontal line under title

    // Centered content inside box
    u8g2.setFont(u8g2_font_4x6_tf); // Set section font

    u8g2.setCursor(10, 24);
    u8g2.print("Base: ");
    sprintf(outputString, "%d Deg", currentBasePos);
    u8g2.print(outputString);
    
    u8g2.setCursor(10, 34);
    u8g2.print("Elbow 1: ");
    sprintf(outputString, "%d Deg", e1pos);
    u8g2.print(outputString);
    
    u8g2.setCursor(10, 44);
    u8g2.print("Elbow 2: ");
    sprintf(outputString, "%d Deg", e2pos);
    u8g2.print(outputString);

    u8g2.setCursor(10, 54);
    u8g2.print("Swivel: ");
    sprintf(outputString, "%d Deg", bpos);
    u8g2.print(outputString);

    u8g2.setCursor(10, 64);
    u8g2.print("Gripper: ");
    sprintf(outputString, "%d Deg", gpos);
    u8g2.print(outputString);

    u8g2.sendBuffer();

    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

void bootAnimation() 
{
  for (int i=0; i < 100; i += 5) 
  {
    u8g2.firstPage(); 
    do 
    {
      u8g2.setFont(u8g2_font_timB24_tr);
      u8g2.drawStr(0, 30, "Booting...");
      
      int barWidth = map(i, 0, 100, 0, 120); // Map progress to bar width
      u8g2.drawFrame(0, 50, 128, 10); // Outer frame
      u8g2.drawBox(2, 50, barWidth, 6); // Filling bar
    } 
    while (u8g2.nextPage());
    delay(30);
  }
}