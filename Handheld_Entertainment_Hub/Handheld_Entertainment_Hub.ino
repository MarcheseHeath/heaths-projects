/*================================================================*/
/* Libraries and Objects */
/*================================================================*/

#include <U8g2lib.h>
#include <Wire.h>
#include <TaskScheduler.h>
#include "dnd_bitmaps.h"
#include <EEPROM.h>

U8G2_SH1106_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

unsigned long startTime;
volatile unsigned long timeStamp;
char buf[20];

/*================================================================*/
/* EEPROM Stored Data */
/*================================================================*/

#define EEPROM_ADDR_DINO_SCORE      0
#define EEPROM_ADDR_REACTION_TIME   4
#define EEPROM_ADDR_PET_HAPPINESS   8
#define EEPROM_ADDR_BUTTON_COUNT    20
#define EEPROM_ADDR_ROLL_COUNT      24

unsigned long totalButtonCount = 0;
unsigned long totalRollCount = 0;
unsigned long bestDinoScore = 0;
unsigned long bestReactionTime = 999999; // initialize as "very slow"

/*================================================================*/
/* Pin Definitions */
/*================================================================*/

// Take floating voltage for random seed
#define sensorPin A0
#define battery A1

// Buttons from left to right
#define button1 2
#define button2 3
#define button3 4
#define button4 5

// LEDs from left to write
#define green 6
#define red 7
#define yellow 8
#define blue 9

// PWM for piezo
#define piezo1 10   // Positive
#define piezo2 11   // Negative

// Default I2C Definitions
/*  
 *  Default SDA = A4;
 *  Default SLA = A5;
*/

/*================================================================*/
/* Piezo Frequencies */
/*================================================================*/

#define C4 262
#define D4 294
#define E4 330
#define F4 349
#define G4 392
#define A4 440
#define C5 523

// Beegees loop for fun
int melody[] = 
{
  G4, A4, 0, C5, 0, 0, G4, 0, 0, 0,
  E4, 0, D4, E4, G4, 0,
  D4, E4, 0, G4, 0, 0,
  D4, 0, E4, 0, G4, 0, A4, 0, C5, 0
};

const int konami[] = {button1, button1, button3, button3, button2, button3, button2, button4};
int konamiIndex = 0;

const int musicNotes[] = {C4, D4, E4, F4}; // C, D, E, F

int noteDuration = 115; // This essentially sets the tempo, 115 is just about right for a disco groove :)
int LEDnumber = 0; // Keeps track of which LED we are on during the beegees loop

/*================================================================*/
/* Display Definitions */
/*================================================================*/

unsigned long lastActivity = 0;
bool screensaverActive = false;
int logoX = 30, logoY = 30, logoDX = 1, logoDY = 1;

/*================================================================*/
/* Dice Roll Definitions */
/*================================================================*/

// Dice values for rolls
const uint8_t diceValues[7] = {2, 4, 6, 8, 10, 12, 20};
const char *diceNames[7] = {"D2","D4","D6","D8","D10","D12","D20"};

// Establish the dice selection index
volatile byte diceSelection = 0;
volatile bool rollTriggered = false;
volatile int lastRoll;

/*  Bitmap array for easy display toggling defined in bitmap header
const Splash dicePatterns[] = 
{
  d2, d4, d6, d8, d10, d12, d20,
  d2r, d4r, d6r, d8r, d10r, d12r, d20r
};
*/

#define SUCCEED 0
#define FAIL 1
#define PERFECT 2

/*================================================================*/
/* General Game Definitions */
/*================================================================*/

// Definitions and variables
#define OFF             0 //Used to control LEDs
#define NONE            0 //Used to check buttons

// Bitmask assignments for button choices
#define CHOICE_GREEN    (1 << 0)
#define CHOICE_RED      (1 << 1)
#define CHOICE_BLUE     (1 << 2)
#define CHOICE_YELLOW   (1 << 3)

/*================================================================*/
/* Repeat After Me Definitions */
/*================================================================*/

byte memoryBoard[32];               //Contains the combination of buttons as we advance

byte memoryRound = 0;               //Counts the number of succesful rounds the player has made it through
#define memoryRounds      12        // Number of rounds to succesfully win

#define memoryTimeLimit   3000      // Amount of time to press a button before game times out

volatile bool memoryActive = false; 
volatile int memoryWin = 0;        // 0 -default, 1 - lose, 2 - win

/*================================================================*/
/* Reaction Time Definitions */
/*================================================================*/

// Definitions and variables
bool reactionRunning = false;
unsigned long reactionStart = 0;
unsigned long reactionWait = 0;
unsigned long reactionPress = 0;
bool reactionTooEarly = false;
bool timedOut = false;

/*================================================================*/
/* Slot Machine Definitions */
/*================================================================*/

bool slotRunning = false;
int slot1 = 0, slot2 = 0, slot3 = 0;
unsigned long slotStart = 0;
bool slotSpinning = false;

const char *slotIcons[6] = {"@", "#", "$", "*", "&", "7"};

/*================================================================*/
/* Dino Game Definitions */
/*================================================================*/

bool dinoRunning = false;
bool dinoJumping = false;
bool dinoGameOver = false;
int dinoY = 40;
int dinoVelocity = 0;
const int dinoGravity = 2;
const int dinoGroundY = 40;
int cactusX1 = 128;
int cactusX2 = 180;
int dinoSpeed = 3;
unsigned long dinoLastSpeedInc = 0;
unsigned long dinoLastFrame = 0;
unsigned long dinoScore = 0;

/*================================================================*/
/* Magic 8 Ball Definitions */
/*================================================================*/

const char* ballResponses[] = 
{
  "Yes", "No", "Maybe", "Definitely", "Ask again", "Unclear", "For sure!", "Doubt it"
};
volatile bool ballActive = false;
volatile int ballIndex = -1;

/*================================================================*/
/* Digital Pet Definitions */
/*================================================================*/

int petHappiness = 50;
unsigned long lastPetUpdate = 0;

/*================================================================*/
/* Scheduler Setup */
/*================================================================*/

Scheduler runner;

void displayTask();
void buttonTask();

Task taskDisplay(100, TASK_FOREVER, &displayTask, &runner, true);
Task taskButtons(50, TASK_FOREVER, &buttonTask, &runner, true);

/*================================================================*/
/* Arduino Initialization */
/*================================================================*/

void setup() 
{
  // Serial monitor for debugging
  Serial.begin(9600);
  while(!Serial);
  Serial.println("Serial initialized");

  // Initialize display
  u8g2.begin();
  delay(1000);
  bootAnimation();

  pinMode(button1, INPUT_PULLUP);
  pinMode(button2, INPUT_PULLUP);
  pinMode(button3, INPUT_PULLUP);
  pinMode(button4, INPUT_PULLUP);
  
  pinMode(green, OUTPUT);
  pinMode(red, OUTPUT);
  pinMode(blue, OUTPUT);
  pinMode(yellow, OUTPUT);

  pinMode(piezo1, OUTPUT);
  pinMode(piezo2, OUTPUT);

  // Ensure Randomness
  randomSeed(analogRead(sensorPin));

  startTime = millis();
  lastActivity = millis();
  
  // Starting scheduler
  Serial.println("Starting scheduler...");

  // Load saved high data
  EEPROM.get(EEPROM_ADDR_DINO_SCORE, bestDinoScore);
  EEPROM.get(EEPROM_ADDR_REACTION_TIME, bestReactionTime);
  EEPROM.get(EEPROM_ADDR_PET_HAPPINESS, petHappiness);
  EEPROM.get(EEPROM_ADDR_BUTTON_COUNT, totalButtonCount);
  EEPROM.get(EEPROM_ADDR_ROLL_COUNT, totalRollCount);

  Serial.println(bestDinoScore);
  Serial.println(bestReactionTime);
  Serial.println(petHappiness);
  Serial.println(totalButtonCount);
  Serial.println(totalRollCount);
}

void loop() 
{
  runner.execute();

  // updateScreensaver();
  
  /*
  // Check for inactivity
  if (millis() - lastActivity > 120000) 
  {
    u8g2.setPowerSave(1); // Sleep display
  } 
  else 
  {
    u8g2.setPowerSave(0);
  }
  */
}

/*================================================================*/
/* Display Handler */
/*================================================================*/

enum ScreenState 
{
  MAIN_MENU,
  DICE_MENU,
  GAMES_MENU,
  FUN_MENU
};

ScreenState currentScreen = MAIN_MENU;

enum GameState 
{
  GAME_NONE,
  GAME_REPEAT,
  GAME_REACTION,
  GAME_SLOT,
  GAME_DINO
};

GameState currentGame = GAME_NONE;

enum FunState 
{
  FUN_NONE,
  FUN_BALL,
  FUN_PET,
  FUN_MUSIC
};

FunState currentFun = FUN_NONE;

int selection = 0; 
// MAIN MENU -- 0 = Dice Roller, 1 = Games
// GAMES MENU -- 0 = Repeat After Me, 1 = Reaction Time, 2 = Slot Machine, 3 = Dino Game
// FUN MENU -- 0 = Magic 8 Ball, 1 = Digital Pet, 2 = Music Player

void displayTask() 
{
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_6x10_tr);
    drawBattery(readBattery());

    // MENU SCREEN
    if (currentScreen == MAIN_MENU) 
    {
      // Title
      Serial.println("Main");
      u8g2.drawStr(10, 10, "Main Menu");
      u8g2.drawHLine(0, 12, 128);

      // Menu options
      switch (selection) 
      {
        case 0:
          u8g2.drawStr(15, 26, "  Fun");
          u8g2.drawStr(5, 38, "> Dice Roller");
          u8g2.drawStr(15, 50, "  Games");
          break;
        case 1:
          u8g2.drawStr(15, 26, "  Dice Roller");
          u8g2.drawStr(5, 38, "> Games");
          u8g2.drawStr(15, 50, "  Fun");
          break;
        case 2:
          u8g2.drawStr(15, 26, "  Games");
          u8g2.drawStr(5, 38, "> Fun");
          u8g2.drawStr(15, 50, "  Dice Roller");
          break;
      }
      
      u8g2.setFont(u8g2_font_4x6_tf);
      u8g2.drawStr(5, 63, "[2] Sel [1/3] Up/Dwn");
    }

    // DICE MENU SCREEN
    else if (currentScreen == DICE_MENU) 
    {
      // Title
      Serial.println("Dice");
      u8g2.drawStr(10, 10, "Dice Roller");
      u8g2.drawHLine(0, 12, 128);

      // Dice options
      u8g2.drawStr(10, 30, "Selected:");
      u8g2.drawStr(70, 30, diceNames[diceSelection]);

      // Display roll
      if (lastRoll > 0) 
      {
        snprintf(buf, sizeof(buf), "Last Roll: %d", lastRoll);
        u8g2.drawStr(10, 50, buf);
      }

      u8g2.setFont(u8g2_font_4x6_tf);
      u8g2.drawStr(5, 63, "[2] Roll [1/3] Dice [4] Bck");

      while (rollTriggered) displayDiceRoll();
    } 

    // GAMES MENU SCREEN
    else if (currentScreen == GAMES_MENU) 
    {
      if (currentGame == GAME_NONE)
      {
        // Title
        Serial.println("Games");
        u8g2.drawStr(10, 10, "Games Menu");
        u8g2.drawHLine(0, 12, 128);
        setLEDs(OFF);

        switch (selection)
        {
          case 0:
            u8g2.drawStr(15, 26, "  Dino Game");
            u8g2.drawStr(5, 38, "> Repeat After Me");
            u8g2.drawStr(15, 50, "  Reaction Timer");
            break;
          case 1:
            u8g2.drawStr(15, 26, "  Repeat After Me");
            u8g2.drawStr(5, 38, "> Reaction Timer");
            u8g2.drawStr(15, 50, "  Slot Machine");
            break;
          case 2:
            u8g2.drawStr(15, 26, "  Reaction Timer");
            u8g2.drawStr(5, 38, "> Slot Machine");
            u8g2.drawStr(15, 50, "  Dino Game");
            break;
          case 3:
            u8g2.drawStr(15, 26, "  Slot Machine");
            u8g2.drawStr(5, 38, "> Dino Game");
            u8g2.drawStr(15, 50, "  Repeat After Me");
            break;
        }

        u8g2.setFont(u8g2_font_4x6_tf);
        u8g2.drawStr(5, 63, "[2] Sel [1/3] Up/Dwn [4] Bck");
      }
      else if (currentGame == GAME_REPEAT)
      {
        displayMemoryGame();
      }
      else if (currentGame == GAME_REACTION)
      {
        displayReactionGame();
        updateReactionGame();
      }
      else if (currentGame == GAME_SLOT)
      {
        displaySlotMachine();
        updateSlotMachine();
      }
      else if (currentGame == GAME_DINO)
      {
        displayDinoGame();
        updateDinoGame();
      }
    }

    // FUN MENU SCREEN
    if (currentScreen == FUN_MENU) 
    {
      if (currentFun == FUN_NONE)
      {
        // Title
        Serial.println("Fun");
        u8g2.drawStr(10, 10, "Fun Menu");
        u8g2.drawHLine(0, 12, 128);

        // Menu options
        switch (selection) 
        {
          case 0:
            u8g2.drawStr(15, 26, "  Music Player");
            u8g2.drawStr(5, 38, "> Magic 8 Ball");
            u8g2.drawStr(15, 50, "  Digital Pet");
            break;
          case 1:
            u8g2.drawStr(15, 26, "  Magic 8 Ball");
            u8g2.drawStr(5, 38, "> Digital Pet");
            u8g2.drawStr(15, 50, "  Music Player");
            break;
          case 2:
            u8g2.drawStr(15, 26, "  Digital Pet");
            u8g2.drawStr(5, 38, "> Music Player");
            u8g2.drawStr(15, 50, "  Magic 8 Ball");
            break;
        }
        
        u8g2.setFont(u8g2_font_4x6_tf);
        u8g2.drawStr(5, 63, "[2] Sel [1/3] Up/Dwn [4] Bck");
      }
      else if (currentFun == FUN_BALL)
      {
        displayMagicBall();
        updateMagicBall();
      }
      else if (currentFun == FUN_PET)
      {
        displayDigitalPet();
        updateDigitalPet();
      }
      else if (currentFun == FUN_MUSIC)
      {
        displayMusicPlayer();
        updateMusicPlayer();
      }
    }
  } 
  while (u8g2.nextPage());
}

// CURRENTLY NOT BEING USED
void displayStats() 
{
  u8g2.drawStr(10, 10, "Device Stats");
  u8g2.drawHLine(0, 12, 128);
  drawBattery(readBattery());

  unsigned long uptime = (millis() - startTime) / 1000;
  snprintf(buf, sizeof(buf), "Uptime: %lus", uptime);
  u8g2.drawStr(10, 30, buf);
  snprintf(buf, sizeof(buf), "Buttons: %lu", totalButtonCount);
  u8g2.drawStr(10, 45, buf);
  snprintf(buf, sizeof(buf), "Rolls: %lu", totalRollCount);
  u8g2.drawStr(10, 60, buf);
}

void updateScreensaver() 
{
  if (millis() - lastActivity > 60000 && !screensaverActive) screensaverActive = true;
  if (screensaverActive) {
    logoX += logoDX; logoY += logoDY;
    if (logoX <= 0 || logoX >= 100) logoDX = -logoDX;
    if (logoY <= 10 || logoY >= 50) logoDY = -logoDY;
    u8g2.firstPage(); 
    do 
    {
      u8g2.drawStr(logoX, logoY, "Sleeping...");
    } 
    while (u8g2.nextPage());
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

/*================================================================*/
/* Button Handler */
/*================================================================*/

void buttonTask() 
{
  uint8_t buttonCount = 0;
  uint8_t rollCount = 0;
  unsigned long now = millis();
  static unsigned long lastPress = 0;

  if (now - lastPress < 200) return;
  
  if (currentScreen == MAIN_MENU)
  {
    if (digitalRead(button1) == LOW) 
    {
      selection = (selection == 0) ? 2 : selection - 1;

      lastPress = now;
      buttonCount++;
    }

    else if (digitalRead(button3) == LOW) 
    {
      selection = (selection + 1) % 3;

      lastPress = now;
      buttonCount++;
    }

    else if (digitalRead(button2) == LOW) 
    {
      switch (selection)
      {
        case 0:
          selection = 0;
          currentScreen = DICE_MENU;
          break;
        case 1:
          selection = 0;
          currentScreen = GAMES_MENU;
          break;
        case 2:
          selection = 0;
          currentScreen = FUN_MENU;
          break;
      }

      lastPress = now;
      buttonCount++;
    }
  }

  else if (currentScreen == DICE_MENU)
  {
    if (!rollTriggered)
    {
      if (digitalRead(button1) == LOW) 
      {
        diceSelection = (diceSelection == 0) ? 6 : diceSelection - 1;

        lastPress = now;
        buttonCount++;
      }

      else if (digitalRead(button3) == LOW) 
      {
        diceSelection = (diceSelection + 1) % 7;

        lastPress = now;
        buttonCount++;
      }

      else if (digitalRead(button2) == LOW) 
      {
        rollTriggered = true;

        lastPress = now;
        rollCount++;
        buttonCount++;
      }

      else if (digitalRead(button4) == LOW) 
      {
        selection = 0; // Dice roller index
        currentScreen = MAIN_MENU;

        lastPress = now;
        buttonCount++;
      }
    }
  }

  else if (currentScreen == GAMES_MENU)
  {
    // Navigation inside games menu
    if (digitalRead(button1) == LOW && currentGame == GAME_NONE)
    {
      selection = (selection == 0) ? 3 : selection - 1;

      lastPress = now;
      buttonCount++;
    }

    else if (digitalRead(button3) == LOW && currentGame == GAME_NONE)
    {
      selection = (selection + 1) % 4;

      lastPress = now;
      buttonCount++;
    }

    // Select
    else if (digitalRead(button2) == LOW && currentGame == GAME_NONE)
    {
      switch (selection)
      {
        case 0: 
          currentGame = GAME_REPEAT;
          playMemoryGame(); 
          break;
        case 1: 
          currentGame = GAME_REACTION; 
          playReactionGame(); 
          break;
        case 2: 
          currentGame = GAME_SLOT; 
          startSlotMachine(); 
          break;
        case 3: 
          currentGame = GAME_DINO; 
          playDinoGame();
          break;
      }

      lastPress = now;
      buttonCount++;
    }

    // Back
    else if (digitalRead(button4) == LOW)
    {
      switch (currentGame)
      {
        case GAME_NONE: 
          selection = 1;  // Games index
          currentScreen = MAIN_MENU; 
          break;
        case GAME_REPEAT: 
          if (!memoryActive)
          {
            selection = 0;  // Repeat index
            currentGame = GAME_NONE;
          }
          break;
        case GAME_REACTION: 
          if (!reactionRunning)
          {
            selection = 1;  // Reaction index
            currentGame = GAME_NONE;
          }
          break;
        case GAME_SLOT: 
          if (!slotSpinning)
          {
            selection = 2;  // Slots index
            currentGame = GAME_NONE;
          }
          break;
        case GAME_DINO: 
          if (!dinoRunning)
          {
            selection = 3;  // Slots index
            currentGame = GAME_NONE;
          }
          break;
      }

      lastPress = now;
      buttonCount++;
    }
  }

  else if (currentScreen == FUN_MENU)
  {
    // Navigation inside games menu
    if (digitalRead(button1) == LOW && currentFun == FUN_NONE)
    {
      selection = (selection == 0) ? 2 : selection - 1;

      lastPress = now;
      buttonCount++;
    }

    else if (digitalRead(button3) == LOW && currentFun == FUN_NONE)
    {
      selection = (selection + 1) % 3;

      lastPress = now;
      buttonCount++;
    }

    // Select
    else if (digitalRead(button2) == LOW && currentFun == FUN_NONE)
    {
      switch (selection)
      {
        case 0: 
          currentFun = FUN_BALL;
          break;
        case 1: 
          currentFun = FUN_PET; 
          break;
        case 2: 
          currentFun = FUN_MUSIC;
          break;
      }

      lastPress = now;
      buttonCount++;
    }

    // Back
    else if (digitalRead(button4) == LOW)
    {
      switch (currentFun)
      {
        case FUN_NONE: 
          selection = 2;  // Fun index
          currentScreen = MAIN_MENU; 
          break;
        case FUN_BALL: 
          ballActive = false;
          selection = 0;  // 8 Ball index
          currentFun = FUN_NONE;
          break;
        case FUN_PET:
          selection = 1;  // Pet index
          currentFun = FUN_NONE;
          break;
        case FUN_MUSIC: 
          if (digitalRead(button1) == LOW)
          {
            selection = 2;  // Music index
            currentFun = FUN_NONE;
          }
          break;
      }

      lastPress = now;
      buttonCount++;
    }
  }

  if (buttonCount > 0)  // Update EEPROM for button push
  {
    Serial.println("Button Triggered");
    lastActivity = millis();
    totalButtonCount++;
    EEPROM.put(EEPROM_ADDR_BUTTON_COUNT, totalButtonCount);
  }
  if (rollCount > 0)  // Update EEPROM for dice rolls
  {
    totalRollCount++;
    EEPROM.put(EEPROM_ADDR_ROLL_COUNT, totalRollCount);
  }
}

/*================================================================*/
/* Battery Voltage Functions */
/*================================================================*/

float readBattery() 
{
  int raw = analogRead(battery);
  // 5.0V reference * divider ratio
  // For 100k/100k divider, multiply by 2
  float voltage = (raw / 1023.0) * 5.0 * 2.0;
  return voltage;
}

void drawBattery(float voltage) 
{
  // Map voltage to bars (0–3)
  int bars = 0;
  if (voltage > 4.0) bars = 3;
  else if (voltage > 3.8) bars = 2;
  else if (voltage > 3.5) bars = 1;
  else bars = 0;

  // Draw battery outline in top-right corner
  int x = 108, y = 2, w = 18, h = 8;
  u8g2.drawFrame(x, y, w, h);        // main body
  u8g2.drawBox(x + w, y + 2, 2, 4);  // small terminal

  // Draw filled bars
  int barWidth = 4;
  for (int i = 0; i < bars; i++) 
  {
    int bx = x + 2 + i * (barWidth + 1);
    u8g2.drawBox(bx, y + 2, barWidth, h - 4);
  }
}

/*================================================================*/
/* Dice Roll Functions */
/*================================================================*/

void displayDiceRoll() 
{
  // Animate roll
  for (int i = 0; i < 8; i++) 
  {
    lastRoll = random(1, diceValues[diceSelection] + 1);
    DrawRoll(SUCCEED, lastRoll, i);
    delay(200);
  }

  // Critical roll display
  if (lastRoll == 1 && diceSelection == 6)
    DrawRoll(FAIL, lastRoll, 1); // Nat 1
  else if (lastRoll == 20 && diceSelection == 6)
    DrawRoll(PERFECT, lastRoll, 1); // Nat 20
  else
    DrawRoll(SUCCEED, lastRoll, 1);
  
  delay(2000);
  rollTriggered = false;
}

void DrawRoll(uint8_t type, uint8_t value, int i)
{
  // Ensure bitmaps are changed between regular and spinning states
  byte tempSelection;
  if (i % 2 == 0) tempSelection = diceSelection + 7;
  else tempSelection = diceSelection;

  switch (type) 
  {
    case SUCCEED: // Normal
      u8g2.drawXBMP(14, 13,
                    dicePatterns[tempSelection].width,
                    dicePatterns[tempSelection].height,
                    dicePatterns[tempSelection].image);
      break;
    case FAIL: // Nat 1 - fail
      u8g2.drawXBMP(10, 12, skull.width, skull.height, skull.image);
      break;
    case PERFECT: // Nat 20 - perfect
      u8g2.drawXBMP(10, 12, star.width, star.height, star.image);
      break;
  }

  u8g2.setFont(u8g2_font_timB24_tr);
  snprintf(buf, sizeof(buf), "%d", value);
  uint8_t x_pos = (value < 10) ? 87 : 80;
  u8g2.drawStr(x_pos, 21, buf);
}

/*================================================================*/
/* General Use Functions */
/*================================================================*/

// Toggle buzzer every buzz_delay_us, for a duration of buzz_length_ms.
void buzz(int length, int delay)
{
  // Convert total play time from milliseconds to microseconds
  unsigned long length_us = length * (long)1000;

  // Loop until the remaining play time is less than a single buzz_delay_us
  while (length_us > (delay * 2))
  {
    length_us -= delay * 2; //Decrease the remaining play time

    // Toggle the buzzer at various speeds
    digitalWrite(piezo1, LOW);
    digitalWrite(piezo2, HIGH);
    delayMicroseconds(delay);

    digitalWrite(piezo1, HIGH);
    digitalWrite(piezo2, LOW);
    delayMicroseconds(delay);
  }
}

// Play the winner sound and lights
void playWinner()
{
  setLEDs(CHOICE_GREEN | CHOICE_BLUE);
  winnerSound();
  setLEDs(CHOICE_RED | CHOICE_YELLOW);
  winnerSound();
  setLEDs(CHOICE_GREEN | CHOICE_BLUE);
  winnerSound();
  setLEDs(CHOICE_RED | CHOICE_YELLOW);
  winnerSound();
}

// Play the winner sound
// This is just a unique (annoying) sound we came up with, there is no magic to it
void winnerSound()
{
  // Toggle the buzzer at various speeds
  for (byte x = 250 ; x > 70 ; x--)
  {
    for (byte y = 0 ; y < 3 ; y++)
    {
      digitalWrite(piezo2, HIGH);
      digitalWrite(piezo1, LOW);
      delayMicroseconds(x);

      digitalWrite(piezo2, LOW);
      digitalWrite(piezo1, HIGH);
      delayMicroseconds(x);
    }
  }
}

// Play the loser sound/lights
void playLoser(void)
{
  setLEDs(CHOICE_RED | CHOICE_GREEN);
  buzz(C4, 1500);

  setLEDs(CHOICE_BLUE | CHOICE_YELLOW);
  buzz(C4, 1500);

  setLEDs(CHOICE_RED | CHOICE_GREEN);
  buzz(C4, 1500);

  setLEDs(CHOICE_BLUE | CHOICE_YELLOW);
  buzz(C4, 1500);
}

// Toggle LEDs according to associated bits
void setLEDs(byte leds)
{
  if ((leds & CHOICE_GREEN) != 0)
    digitalWrite(green, HIGH);
  else
    digitalWrite(green, LOW);
  if ((leds & CHOICE_RED) != 0)
    digitalWrite(red, HIGH);
  else
    digitalWrite(red, LOW);

  if ((leds & CHOICE_BLUE) != 0)
    digitalWrite(blue, HIGH);
  else
    digitalWrite(blue, LOW);

  if ((leds & CHOICE_YELLOW) != 0)
    digitalWrite(yellow, HIGH);
  else
    digitalWrite(yellow, LOW);
}

// Randomly toggle LEDs 
void discoMode() 
{
  for (int i=0; i<30; i++) 
  {
    setLEDs(random(1,15)); // Randomly assign LED combinations
    buzz(50, random(500,1500));
    delay(100);
  }
  setLEDs(OFF);
}

/*================================================================*/
/* Repeat After Me (memory) Functions */
/*================================================================*/

void displayMemoryGame()
{
  drawBattery(readBattery());

  switch (memoryWin)
    {
    case 0:
      u8g2.drawStr(10, 30, "Repeat After Me");
      u8g2.drawHLine(0, 12, 128);

      snprintf(buf, sizeof(buf), "Round: %d", memoryRound);
      u8g2.drawStr(10, 50, buf);
      break;
    case 1:
      u8g2.drawStr(10, 30, "YOU LOSE!");
      u8g2.drawStr(10, 45, "[2] Restart [4] Back");
      break;
    case 2:
      u8g2.drawStr(10, 30, "YOU WIN!");
      u8g2.drawStr(10, 45, "[2] Restart [4] Back");
      break;
    }
}

// Check restart conditions
void updateMemoryGame()
{
  // Restart game
  if (!memoryActive && digitalRead(button2) == LOW) playMemoryGame();
}

void playMemoryGame()
{
  memoryWin = 0;
  memoryRound = 0; // Reset the game to the beginning
  memoryActive = true;
  
  delay(3000); // Wait a few seconds

  while (memoryRound < memoryRounds) 
  {
    addMoves(); // Add a button to the current moves, then play them back

    playMoves(); // Play back the current game board

    // Then require the player to repeat the sequence.
    for (byte currentMove = 0 ; currentMove < memoryRound ; currentMove++)
    {
      byte choice = checkButton(); // See what button the user presses

      if (choice == 0) 
      {
        memoryWin = 1;
        memoryActive = false;
        playLoser();
        return; // If wait timed out, player loses
      }

      if (choice != memoryBoard[currentMove]) 
      {
        memoryWin = 1;
        memoryActive = false;
        playLoser();
        return; // If the choice is incorect, player loses
      }
    }

    delay(1000); // Player was correct, delay before next round
  }
  
  memoryWin = 2;
  memoryActive = false;
  playWinner();
}

// Wait for a button to be pressed. 
// Returns one of LED colors (LED_RED, etc.) if successful, 0 if timed out
byte checkButton()
{
  timeStamp = millis(); // Remember the time we started the this loop

  while ( (millis() - timeStamp) < memoryTimeLimit) // Loop until too much time has passed
  {
    byte button = memoryButton();

    if (button != NONE)
    { 
      toner(button, 150); // Play the button the user just pressed

      while(memoryButton() != NONE) ;  // Now let's wait for user to release button

      delay(10); // This helps with debouncing and accidental double taps

      return button;
    }
  }

  return NONE; // If we get here, we've timed out!
}

// Returns a '1' bit in the position corresponding to CHOICE_RED, CHOICE_GREEN, etc.
byte memoryButton()
{
  if (digitalRead(button1) == 0) return(CHOICE_GREEN); 
  else if (digitalRead(button2) == 0) return(CHOICE_RED); 
  else if (digitalRead(button3) == 0) return(CHOICE_BLUE); 
  else if (digitalRead(button4) == 0) return(CHOICE_YELLOW);

  return(NONE); // If no button is pressed, return none
}

// Adds a new random button to the game sequence, by sampling the timer
void addMoves()
{
  byte newButton = random(0, 4); //min (included), max (exluded)

  // We have to convert this number, 0 to 3, to CHOICEs
  if(newButton == 0) newButton = CHOICE_GREEN;
  else if(newButton == 1) newButton = CHOICE_RED;
  else if(newButton == 2) newButton = CHOICE_BLUE;
  else if(newButton == 3) newButton = CHOICE_YELLOW;

  memoryBoard[memoryRound++] = newButton; // Add this new button to the game array
}

// Plays the current contents of the game moves
void playMoves()
{
  for (byte currentMove = 0 ; currentMove < memoryRound ; currentMove++) 
  {
    toner(memoryBoard[currentMove], 150);

    // Wait some amount of time between button playback
    // Shorten this to make game harder
    delay(100); // 150 works well. 75 gets fast.
  }
}

// Play associated noise for each button
void toner(byte which, int length)
{
  setLEDs(which); //Turn on a given LED

  //Play the sound associated with the given LED
  switch(which) 
  {
  case CHOICE_GREEN:
    buzz(length, 1136); 
    break;
  case CHOICE_RED:
    buzz(length, 568); 
    break;
  case CHOICE_BLUE:
    buzz(length, 851); 
    break;
  case CHOICE_YELLOW:
    buzz(length, 638); 
    break;
  }

  setLEDs(OFF); // Turn off all LEDs
}

/*================================================================*/
/* Reaction Time Functions */
/*================================================================*/

// Initialize reaction game settings
void playReactionGame()
{
  reactionRunning = true;
  reactionTooEarly = false;
  reactionPress = 0;
  reactionStart = millis();
  reactionWait = random(2000, 10000); // random delay before signal
  setLEDs(OFF);
}

// Display reaction game on OLED
void displayReactionGame()
{
  u8g2.setFont(u8g2_font_6x10_tr);
  u8g2.drawStr(10, 10, "Reaction Timer");
  u8g2.drawHLine(0, 12, 128);
  drawBattery(readBattery());

  // Check that game is still active
  if (!reactionRunning)
  {
    u8g2.drawStr(10, 35, "[2] Restart [4] Back");
    return;
  }

  if (!reactionTooEarly && millis() - reactionStart < reactionWait)
  {
    u8g2.drawStr(10, 40, "Wait for the light...");
  }
  else if (!reactionTooEarly && reactionPress == 0)
  {
    // Signal light ON
    setLEDs(CHOICE_GREEN | CHOICE_RED | CHOICE_BLUE | CHOICE_YELLOW);
    u8g2.drawStr(10, 40, "GO!");
  }
  else if (reactionPress > 0)
  {
    snprintf(buf, sizeof(buf), "Time: %lu ms", reactionPress - (reactionStart + reactionWait));
    u8g2.drawStr(10, 40, buf);
    snprintf(buf, sizeof(buf), "Best: %lu ms", bestReactionTime);
    u8g2.drawStr(10, 55, buf);
  }
  else if (reactionPress < 0)
  {
    u8g2.drawStr(10, 40, "Timed out!");
  }
  else if (reactionTooEarly)
  {
    u8g2.drawStr(10, 40, "Too early!");
  }
}

// Update LEDs and check for button press
void updateReactionGame()
{
  // Press detection
  if (reactionRunning && digitalRead(button2) == LOW)
  {
    if (millis() - reactionStart < reactionWait)
    {
      reactionTooEarly = true;
      reactionRunning = false;
      setLEDs(CHOICE_RED);
      buzz(200, 1500);
    }
    else
    {
      reactionPress = millis();
      reactionRunning = false;
      setLEDs(CHOICE_GREEN);
      buzz(100, 700);

      // --- SAVE NEW BEST TIME ---
      unsigned long reactionTime = reactionPress - (reactionStart + reactionWait);
      if (reactionTime < bestReactionTime)
      {
        bestReactionTime = reactionTime;
        EEPROM.put(EEPROM_ADDR_REACTION_TIME, bestReactionTime);
        Serial.print("New fastest reaction time: ");
        Serial.println(bestReactionTime);
      }
    }
  }

  // Timed out
  else if (reactionRunning && (millis() - (reactionStart + reactionWait)) > 10000) 
  {
    reactionPress = -1; // Negative indicates a time out
    setLEDs(OFF);
    reactionRunning = false;
  }

  // Restart game
  else if (!reactionRunning && digitalRead(button2) == LOW && !reactionTooEarly) playReactionGame();
}

/*================================================================*/
/* Slot Machine Functions */
/*================================================================*/

// Initialize slot machine settings
void startSlotMachine()
{
  slotRunning = true;
  slotSpinning = false;
  slot1 = slot2 = slot3 = 0;
}

// Display slot machine on OLED
void displaySlotMachine()
{
  u8g2.setFont(u8g2_font_6x10_tr);
  u8g2.drawStr(10, 10, "Slot Machine");
  u8g2.drawHLine(0, 12, 128);
  drawBattery(readBattery());

  char buf[20];
  snprintf(buf, sizeof(buf), "%s %s %s", slotIcons[slot1], slotIcons[slot2], slotIcons[slot3]);
  u8g2.setFont(u8g2_font_logisoso22_tr);
  u8g2.drawStr(20, 50, buf);

  u8g2.setFont(u8g2_font_4x6_tf);
  if (!slotSpinning)
    u8g2.drawStr(10, 63, "[2] Spin  [4] Bck");
  else
    u8g2.drawStr(10, 63, "Spinning...");
}

void updateSlotMachine()
{
  if (!slotSpinning && digitalRead(button2) == LOW)
  {
    slotSpinning = true;
    slotStart = millis();
    buzz(100, 800);
  }

  if (slotSpinning)
  {
    // Animate spin
    slot1 = random(0, 6);
    slot2 = random(0, 6);
    slot3 = random(0, 6);

    if (millis() - slotStart > 1500)
    {
      slotSpinning = false;
      timeStamp = millis();
      
      // Check for win
      if (slot1 == slot2 && slot2 == slot3)
      {
        setLEDs(CHOICE_GREEN);
        playWinner();
      }
      else
      {
        setLEDs(CHOICE_RED);
        playLoser();
      }
      setLEDs(OFF);
    }
    
    if (!slotSpinning && millis() - timeStamp > 1500) setLEDs(OFF); 
  }
}

/*================================================================*/
/* Dino Game Functions */
/*================================================================*/

// Initialize dino game
void playDinoGame()
{
  dinoRunning = true;
  dinoJumping = false;
  dinoGameOver = false;
  dinoY = dinoGroundY;
  dinoVelocity = 0;
  cactusX1 = 128;
  cactusX2 = 180;
  dinoSpeed = 3;
  dinoScore = 0;
  dinoLastSpeedInc = millis();
  dinoLastFrame = millis();
  setLEDs(OFF);
}

// Display the dino game on OLED
void displayDinoGame()
{
  u8g2.setFont(u8g2_font_6x10_tr);
  u8g2.drawStr(10, 10, "Dino Game");
  u8g2.drawHLine(0, 12, 128);
  drawBattery(readBattery());

  if (dinoGameOver)
  {
    u8g2.setFont(u8g2_font_6x10_tr);
    u8g2.drawStr(30, 30, "GAME OVER!");
    u8g2.drawStr(10, 45, "[2] Restart [4] Back");
    return;
  }

  // Draw dino
  u8g2.drawBox(10, dinoY, 10, 10);
  // Draw cacti
  u8g2.drawBox(cactusX1, dinoGroundY, 8, 15);
  u8g2.drawBox(cactusX2, dinoGroundY, 8, 15);
  // Draw ground
  u8g2.drawHLine(0, 58, 128);
  // Draw score
  snprintf(buf, sizeof(buf), "Score: %lu", dinoScore / 10);
  u8g2.drawStr(10, 63, buf);
}

// Update dino game state
void updateDinoGame()
{
  unsigned long now = millis();
  if (now - dinoLastFrame < 50) return;
  dinoLastFrame = now;

  // --- RESTART HANDLING ---
  if (dinoGameOver)
  {
    if (digitalRead(button2) == LOW)
    {
      playDinoGame();
    }
    return;
  }

  // --- JUMP HANDLING ---
  if (digitalRead(button2) == LOW && !dinoJumping && dinoY == dinoGroundY)
  {
    dinoJumping = true;
    dinoVelocity = -13;
    buzz(50, 1000);
  }

  // --- PHYSICS ---
  if (dinoJumping)
  {
    dinoY += dinoVelocity;
    dinoVelocity += dinoGravity;
    if (dinoY >= dinoGroundY)
    {
      dinoY = dinoGroundY;
      dinoJumping = false;
    }
  }

  // --- MOVE CACTI ---
  cactusX1 -= dinoSpeed;
  cactusX2 -= dinoSpeed;
  if (cactusX1 < -10) cactusX1 = 128 + random(0, 30);
  if (cactusX2 < -10) cactusX2 = 128 + random(30, 60);

  // --- SPEED UP ---
  if (now - dinoLastSpeedInc > 3000)
  {
    dinoSpeed++;
    dinoLastSpeedInc = now;
  }

  // --- SCORE ---
  dinoScore++;

  // --- COLLISION DETECTION ---
  bool hitCactus1 = (cactusX1 < 20 && cactusX1 > 5 && dinoY == dinoGroundY);
  bool hitCactus2 = (cactusX2 < 20 && cactusX2 > 5 && dinoY == dinoGroundY);

  if (hitCactus1 || hitCactus2)
  {
    dinoGameOver = true;
    dinoRunning = false;
    setLEDs(CHOICE_RED);
    buzz(D4, 800);

    // --- SAVE NEW HIGH SCORE ---
    if (dinoScore > bestDinoScore)
    {
      bestDinoScore = dinoScore;
      EEPROM.put(EEPROM_ADDR_DINO_SCORE, bestDinoScore);
      Serial.println("New Dino high score saved!");
    }
  }
}

/*================================================================*/
/* Magic 8 Ball Functions */
/*================================================================*/

void displayMagicBall() 
{
  u8g2.setFont(u8g2_font_6x10_tr);
  u8g2.drawStr(20, 10, "Magic 8-Ball");
  u8g2.drawHLine(0, 12, 128);
  drawBattery(readBattery());

  if (!ballActive) 
  {
    u8g2.setFont(u8g2_font_4x6_tf);
    u8g2.drawStr(10, 63, "[2] Ask the Oracle [4] Bck");
  } 
  else 
  {
    u8g2.setFont(u8g2_font_6x12_tr);
    u8g2.drawCircle(30, 40, 20);
    u8g2.drawStr(30, 40, ballResponses[ballIndex]);

    u8g2.setFont(u8g2_font_4x6_tf);
    u8g2.drawStr(10, 63, "[2] Ask again [4] Bck");
  }
  
}

void updateMagicBall() 
{
  if (digitalRead(button2) == LOW) 
  {
    ballActive = true;
    ballIndex = random(0, 8);
    buzz(100, 800);
  } 
}

/*================================================================*/
/* Magic 8 Ball Functions */
/*================================================================*/

// VERY BIG WORK IN PROGESS NOT COMPLETE

void displayDigitalPet() 
{
  u8g2.setFont(u8g2_font_6x10_tr);
  u8g2.drawStr(20, 10, "Digital Pet");
  u8g2.drawHLine(0, 12, 128);
  drawBattery(readBattery());

  snprintf(buf, sizeof(buf), "Happiness: %d", petHappiness);
  u8g2.drawStr(10, 35, buf);
  u8g2.drawStr(10, 50, "[1] Feed [3] Play [4] Back");
}

void updateDigitalPet() 
{
  if (millis() - lastPetUpdate > 5000) 
  {
    if (petHappiness > 0) petHappiness--;
    lastPetUpdate = millis();
  }

  if (digitalRead(button1) == LOW) 
  { 
    petHappiness += 5; 
    buzz(50, 1000); 
  }
  if (digitalRead(button3) == LOW) 
  { 
    petHappiness += 3; 
    buzz(50, 700); 
  }
  if (petHappiness > 100) petHappiness = 100;

  EEPROM.put(EEPROM_ADDR_PET_HAPPINESS, petHappiness);
}

/*================================================================*/
/* Music Player Functions */
/*================================================================*/

void displayMusicPlayer() 
{
  u8g2.drawStr(20, 10, "Music Player");
  u8g2.drawHLine(0, 12, 128);
  drawBattery(readBattery());

  u8g2.drawStr(10, 40, "[1–4] Notes  [1+4] Back");
}

void updateMusicPlayer() 
{
  if (digitalRead(button1) == LOW) buzz(150, 1908); // C
  if (digitalRead(button2) == LOW) buzz(150, 1700); // D
  if (digitalRead(button3) == LOW) buzz(150, 1515); // E
  if (digitalRead(button4) == LOW) buzz(150, 1353); // F
}

/*================================================================*/
/* Easter Egg Function */
/*================================================================*/

// DOESNT DO ANYTHING CURRENTLY
void checkKonami() 
{
  for (int i = 0; i < 8; i++) 
  {
    if (digitalRead(konami[i]) == LOW) 
    {
      konamiIndex++;
      delay(150);
    } 
    else if (konamiIndex == 8) 
    {
      discoMode();
      konamiIndex = 0;
    }
  }
}