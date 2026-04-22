/*--------------------------------------------------------------------------------------*/
/*-------------------------- VARIABLES AND DEFINITIONS ---------------------------------*/
/*--------------------------------------------------------------------------------------*/

// Libraries
#include <math.h>         // Math Functions
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

// Joint Angles (in degrees)
typedef struct {
  int bpos;
  int e1pos;
  int e2pos;
  int spos;
  int gpos;
} JointData;

JointData current = {90,90,90,90,45};

int stepIndex = 0;
const int steps[4][4] = 
{
  {1,0,0,1},
  {1,0,1,0},
  {0,1,1,0},
  {0,1,0,1}
};

// Inverse Kinematics Variables and Parameters
const float L1 = 45.0; // mm
const float L2 = 50.0;

float targetX = 75;
float targetY = 50;

// Playback Variables and Parameters
#define MAX_RECORD 100

JointData recordBuffer[MAX_RECORD];
int recordIndex = 0;
bool isRecording = false;
bool isPlaying = false;
int playIndex = 0;

// Queue Architecture for Multi-Tasking
QueueHandle_t driverQueue;
QueueHandle_t displayQueue;

// Machine Modes
typedef enum {
  MODE_POTS,
  MODE_IK,
  MODE_DANCE,
  MODE_PLAYBACK
} ControlMode;

volatile ControlMode mode = MODE_POTS;

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

  delay(100);
  
  // Init display
  u8g2.begin();
  bootAnimation();
  delay(100);

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

  driverQueue  = xQueueCreate(1, sizeof(JointData));
  displayQueue = xQueueCreate(1, sizeof(JointData));

  // Data Acquisiton Task
  xTaskCreate(
    Acquisition,          // Task function
    "AcquisitionTask",    // Name
    2048,                // Stack size
    NULL,                // Parameters
    5,                   // Priority
    NULL);               // Task handle

  // Command Task
  xTaskCreate(
    Commands,            // Task function
    "CommandTask",       // Name
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
/*----------------------------- DATA ACQUISITION TASK ----------------------------------*/
/*--------------------------------------------------------------------------------------*/

static void Acquisition(void *arg) 
{
  JointData data;

  for (;;)
  {    
    // POTS Mode
    if (mode == MODE_POTS)
    {
      // Map to angles (0–180)
      data.bpos  = map(analogRead(BASE_POT),   0, 4095, 0, 180);
      data.e1pos = map(analogRead(ELBOW1_POT), 0, 4095, 0, 180);
      data.e2pos = map(analogRead(ELBOW2_POT), 0, 4095, 0, 180);
      data.spos  = map(analogRead(SWIVEL_POT), 0, 4095, 0, 180);
      data.gpos  = map(analogRead(GRIPPER_POT),0, 4095, 0, 180);
    }

    // IK Mode
    else if (mode == MODE_IK)
    {
      inverseKinematics(&data);
    }

    // DANCE Mode
    else if (mode == MODE_DANCE)
    {
      robotDance(&data);
    }

    // PLAYBACK Mode
    else if (mode == MODE_PLAYBACK && isPlaying && recordIndex > 0)
    {
      data = recordBuffer[playIndex++];
      if (playIndex >= recordIndex) playIndex = 0;
    }

    // PLAYBACK Recording
    if (mode == MODE_PLAYBACK && isRecording && digitalRead(SAVE) == LOW)
    {
      if (recordIndex < MAX_RECORD)
      {
        recordBuffer[recordIndex++] = data;
        Serial.println("Saved position");
        vTaskDelay(pdMS_TO_TICKS(300));
      }
    }

    // Send latest data (overwrite if full)
    xQueueOverwrite(driverQueue, &data);
    xQueueOverwrite(displayQueue, &data);

    vTaskDelay(pdMS_TO_TICKS(20));
  }
}

void inverseKinematics(JointData *data)
{
  float x = targetX;
  float y = targetY;

  float d = sqrt(x*x + y*y);  // total distance from base joint

  d = constrain(d, fabs(L1 - L2), L1 + L2); // Verify robot can achieve distance

  float cos_theta2 = (x*x + y*y - L1*L1 - L2*L2) / (2 * L1 * L2);
  cos_theta2 = constrain(cos_theta2, -1, 1);

  float theta2 = acos(cos_theta2);
  float theta1 = atan2(y, x) - atan2(L2 * sin(theta2), L1 + L2 * cos(theta2));

  data->e1pos = degrees(theta1) + 90;
  data->e2pos = degrees(theta2) + 90;

  data->bpos = 90;
  data->spos = 120;
  data->gpos = 120;
}

/*--------------------------------------------------------------------------------------*/
/*--------------------------------- COMMAND TASK ---------------------------------------*/
/*--------------------------------------------------------------------------------------*/

static void Commands(void *arg)
{
  for (;;)
  {
    if (Serial.available())
    {
      String input = Serial.readStringUntil('\n');
      input.trim();

      if (input == "run_ik") mode = MODE_IK;
      else if (input == "run_pots") mode = MODE_POTS;
      else if (input == "run_dance") mode = MODE_DANCE;
      
      else if (input.startsWith("ik "))
      {
        sscanf(input.c_str(), "ik %f %f", &targetX, &targetY);
        mode = MODE_IK;
        Serial.printf("Target: %.1f %.1f\n", targetX, targetY);
      }

      else if (input == "playback_record")
      {
        recordIndex = 0;
        isRecording = true;
        Serial.println("Recording...");
      }
      else if (input == "playback_stop")
      {
        isRecording = false;
        isPlaying = false;
        Serial.println("Stopped");
      }
      else if (input == "playback")
      {
        mode = MODE_PLAYBACK;
        playIndex = 0;
        isPlaying = true;
        Serial.println("Playback...");
      }

      else
      {
        Serial.println("Invalid command");
      }
    }

    vTaskDelay(pdMS_TO_TICKS(50));
  }
}

/*--------------------------------------------------------------------------------------*/
/*------------------------------- MOTOR DRIVER TASK ------------------------------------*/
/*--------------------------------------------------------------------------------------*/

static void Driver(void *arg) 
{
  JointData data;

  for (;;)
  {
    if (xQueueReceive(driverQueue, &data, portMAX_DELAY) == pdTRUE)
    {
      current.e1pos = smoothStep(current.e1pos, data.e1pos, 2);
      current.e2pos = smoothStep(current.e2pos, data.e2pos, 2);
      current.spos  = smoothStep(current.spos,  data.spos,  2);
      current.gpos  = smoothStep(current.gpos,  data.gpos,  2);

      elbow1.write(current.e1pos);
      elbow2.write(current.e2pos);
      swivel.write(current.spos);
      gripper.write(current.gpos);

      if (current.bpos < data.bpos)
      {
        stepMotor(1);
        current.bpos++;
      }
      else if (current.bpos > data.bpos)
      {
        stepMotor(-1);
        current.bpos--;
      }
    }

    vTaskDelay(pdMS_TO_TICKS(5));
  }
}

int smoothStep(int current, int target, int step)
{
  if (abs(target - current) < step) return target;
  return current + (target > current ? step : -step);
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

  delayMicroseconds(1500);
}

void robotDance(JointData *data)
{
  static int t = 0;
  t++;

  data->bpos  = (t % 180);
  data->e1pos = 90 + 30 * sin(t * 0.1);
  data->e2pos = 90 + 30 * cos(t * 0.1);
  data->spos  = 90;
  data->gpos  = (t % 60);
}

/*--------------------------------------------------------------------------------------*/
/*--------------------------------- DISPLAY TASK ---------------------------------------*/
/*--------------------------------------------------------------------------------------*/

static void Display(void *arg) 
{
  JointData data;
  
  for (;;)
  {
    if (xQueueReceive(displayQueue, &data, portMAX_DELAY) == pdTRUE)
    {
      u8g2.clearBuffer(); // Clear internal memory

      u8g2.setFont(u8g2_font_6x10_tf);  // Set title font
      u8g2.drawStr(0, 10, "Robot Arm v2.00"); // Program title
      u8g2.drawHLine(0, 12, 128);  // Horizontal line under title

      // Centered content inside box
      u8g2.setFont(u8g2_font_4x6_tf); // Set section font

      const char *labels[] = {
        "Base: ",
        "Elbow 1: ",
        "Elbow 2: ",
        "Swivel: ",
        "Gripper: "
      };

      const int values[] = {
        data.bpos,
        data.e1pos,
        data.e2pos,
        data.spos,
        data.gpos
      };

      for (int i = 0; i < 5; i++)
      {
        u8g2.setCursor(10, 24 + (i * 10));
        u8g2.print(labels[i]);
        u8g2.print(values[i]);
        u8g2.print(" Deg");
      }
    }

    u8g2.sendBuffer();

    vTaskDelay(pdMS_TO_TICKS(10));
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