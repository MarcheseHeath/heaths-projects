#include <EEPROM.h> // Allows arduino to access and utilize ROM
#include <LiquidCrystal_I2C.h> // Allows arduino to inilitialize Liquid Crystal Display
#include <SPI.h> // Auxillary SD Library that improves functionality
#include "SD.h" // Allows arduino to access SD Card Reader module
#include "TMRpcm.h" // Allows arduino to interpret .wav files
#include "Rotary.h" // Allows arduino to initialize Rotary Encoder

// Pin Definitions
#define ROTARY_A 3
#define ROTARY_B 2
#define SWITCH 4
const int buzzerPin = 8;
const int cs = 10;

// Default Pin Definitions
/* 
 *  Default SDA = A4;
 *  Default SLA = A5;
 *  Default MOSI = D11;
 *  Default MISO = D12;
 *  Default SCLK = D13;
*/

File root; // Empty file type to verify SD card functionality in Serial Monitor

// Modules
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Initialise LCD (16x2 Capacity)
Rotary encoder(ROTARY_A, ROTARY_B);	// Initialise the encoder on pins 2 and 3 (interrupt pins)
TMRpcm playback; // Initalize the functions that can play the sound from .wav files in SD card


// Determines when the display should update
volatile bool updateDisplay = false;

// Display variables
int index = 0;
const String menuText[] = {"Jack Singing", "Nanga", "Ginny", "Jamie", "2020 ", "2024", "Dino Game"};

// EEPROM handling
// Uncomment next line to clear out EEPROM and reset variables
//#define RESET_EEPROM
#define EEPROM_ADDRESS 0
#define EEPROM_MAGIC 0x0BAD0DAD
typedef struct 
{
  uint32_t magic;
  long dinoHighScore;
}

EEPROM_DATA;

EEPROM_DATA EepromData; // Current EEPROM settings

// Game Setup
int score = 0; // Temporary score for game
boolean playState = false; // Game starts only when this becomes true
boolean gameOver = false;  // Determines if the display menu should return
boolean gameButton = false; // Ensures button works when dino game active

// Dino Game Setup
boolean dinoOnGround = true;  // Check if the dino is in the ground
// Distance values to ensure obstacles have proper spacing
int dist = 0;   
int distTwo = 0;

//bitmap array for the dino character
  byte dino [8]        
  { B00000,
    B00111,
    B00101,
    B10111,
    B11100,
    B11111,
    B01101,
    B01100,
  };
  
  //bitmap array for the tree character
  byte tree [8]      
  {
    B00011,
    B11011,
    B11011,
    B11011,
    B11011,
    B11111,
    B01110,
    B01110
  };

//-------------------------------------------------------------------
//        Arduino Initialisation
//-------------------------------------------------------------------

void setup() 
{
  // Set pins from encoder as interrupts when the pin changes
  attachInterrupt(digitalPinToInterrupt(ROTARY_A), encChange, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ROTARY_B), encChange, CHANGE);

  // Initialise pin as input with pull-up enabled and debounce variable for encoder button
  pinMode(SWITCH, INPUT_PULLUP);

  // Set output pin for buzzer noises
  pinMode(buzzerPin,OUTPUT);

  // Set output pin and volume for .wav file output signal
  playback.speakerPin = 9;
  playback.setVolume(5);
  
  // Initialise the LCD, start the backlight
  //Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  
  // Print a "bootup" message for five seconds
  lcd.setCursor(0,0);
  lcd.print("Rilee's  Box of");
  lcd.setCursor(0, 1);
  lcd.print("Trinkets");
  delay(5000);
  lcd.clear();
  
  // Check to see if SD card is present and functional
  if (!SD.begin(cs))
  {
    lcd.setCursor(0,0);
    lcd.print("ERROR WITH MICRO");
    lcd.setCursor(0,1);
    lcd.print("    SD CARD!    ");
    delay(5000);
    lcd.clear();
  }
  
  // Enter main menu
  displayMenu();
  
  // Check to see that files on SD card can be read successfully
  //root = SD.open("/");
  //printDirectory(root, 0);
  //Serial.println("done!");
}

//-------------------------------------------------------------------
//        Arduino Loop Code
//-------------------------------------------------------------------

void loop() 
{
  // Select the current menu selection with button press
  if (testButton(true) && !gameButton) 
  {
    // Clear screen for selection
    lcd.clear();

    switch (index)
    {
      case 0: Choice1(); break;
      case 1: Choice2(); break;
      case 2: Choice3(); break;
      case 3: Choice4(); break;
      case 4: Choice5(); break;
      case 5: Choice6(); break;
    }
  }
    
  // Update display if needed
  if (updateDisplay && !gameButton) 
  {
    displayMenu();
    updateDisplay = false;
  }
}

//-------------------------------------------------------------------
//        Custom Functions
//-------------------------------------------------------------------

// ----- List all files in the Micro SD Default Directory -----
/*
void printDirectory(File dir, int numTabs) 
{
 while (true) 
 {
  File entry =  dir.openNextFile();
    if (! entry) 
    {
      // no more files
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) 
    {
     Serial.print('\t');
    }
    Serial.print(entry.name());
    if (entry.isDirectory()) 
    {
      Serial.println("/");
      printDirectory(entry, numTabs + 1);
    } 
    else 
    {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
}
*/

// ----- Display the current menu -----
void displayMenu()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  if (index == 6)
  {
    lcd.print("Play Game: ");
  }
  else
  {
    lcd.print("Play Sound: ");
  }
  lcd.setCursor(0, 1);
  lcd.print(menuText[index]);
}

// ----- Screen displayed when a song is playing -----
void soundPlaying( )
{
  lcd.setCursor(0,0);
  lcd.print("Now Playing!");
}

// ----- Update the menu index -----
void updateMenu(char state)
{
  updateDisplay = true;
  if (state == DIR_CW)
  {
    if (index != 6)
    {
     index += 1;
    }
    else
    {
      index = 0;
    }
  }
  else
  {
    if (index != 0)
    {
     index -= 1;
    }
    else
    {
      index = 6;
    }
  }
}

// ----- Check when encoder button has been pressed and released -----
bool testButton(bool waitForRelease) 
{
  bool pressed = false;
  if (digitalRead(SWITCH) == LOW) 
  {
    pressed = true;
    while (waitForRelease && digitalRead(SWITCH) == LOW)
    {
      yield();
    }
  }
  return pressed;
}

// ----- Change the menu based on encoder rotation -----
void encChange() 
{
  unsigned char state = encoder.process();
  if (state != DIR_NONE) 
  {
    updateMenu(state);
  }
}

// ----- Write the EepromData Structure to EEPROM -----
void writeEepromData(void)
{
  //This function uses EEPROM.update() to perform the write, so does not rewrites the value if it didn't change.
  EEPROM.put(EEPROM_ADDRESS, EepromData);
}


// ----- Read EepromData structure from EEPROM, initialise if necessary -----
void readEepromData(void)
{
#ifndef RESET_EEPROM
  EEPROM.get(EEPROM_ADDRESS,EepromData);
  if (EepromData.magic != EEPROM_MAGIC)
  {
#endif  
    EepromData.magic = EEPROM_MAGIC;
    EepromData.dinoHighScore = 0;
    writeEepromData();
#ifndef RESET_EEPROM
  }
#endif  
}

// ----- Choice 1 -----
void Choice1()
{
  playback.play("JACK.WAV");
  soundPlaying();
  delay(10000);
  displayMenu();
}

// ----- Choice 2 -----
void Choice2()
{
  playback.play("NANGA.WAV");
  soundPlaying();
  delay(30000);
  displayMenu();
}

// ----- Choice 3 -----
void Choice3()
{
  playback.play("GINNY.WAV");
  soundPlaying();
  delay(20000);
  displayMenu();
}

// ----- Choice 4 -----
void Choice4()
{
  playback.play("JAMIE.WAV");
  soundPlaying();
  delay(19000);
  displayMenu();
}

// ----- Choice 5 -----
void Choice5()
{
  playback.play("2020.WAV");
  soundPlaying();
  delay(48000);
  displayMenu();
}

// ----- Choice 6 -----
void Choice6()
{
  playback.play("2024.WAV");
  soundPlaying();
  delay(48000);
  displayMenu();
}

// ----- Choice 8: Dino Game Initialization -----
void DinoGame()
{  
  gameButton = true; // Ensure the button only works for dino game
  gameOver = false; // Allows user to close and reopen game

  lcd.createChar(7, dino);  // Create dino sprite
  lcd.createChar(6,tree); // Create tree sprite
  lcd.setCursor(0,1);
  lcd.write(7); // Draw dino

  while (!gameOver) // Wait for game to start
  {
    if (!playState)
    {
      lcd.setCursor(0,0);
      lcd.print("Press to start");
    }
    if (playState)
    {
      startGame();
    }
    if (testButton(true))
    {
      playState = true;
    }
    delay(100);
  }

  gameButton = false;
  displayMenu(); //If gameOver becomes true then this will return user to menu
}

// ----- Dino Game Code -----
void startGame()
{ 
  lcd.clear();

  //Generate two random distances for the gap between the trees
  dist = random(4,9);  
  distTwo = random(4,9);

  // this for loop is to make the trees move
  for (int i = 16; i >= -(dist + distTwo); i--)
  { 
    // Print current score
    lcd.setCursor(13,0);
    lcd.print(score); 

    if(testButton(true)) // When the button is pushed print dino on top and set dinoOnGround to false
    {  
      lcd.setCursor(1,0);
      lcd.write(7);
      lcd.setCursor(1,1);
      lcd.print(" ");

      tone(buzzerPin,700,100);
      dinoOnGround = false;
    }
    else // When the button is not pushed print dino on bottom and set dinoOnGround to true
    {    
      lcd.setCursor(1,1); 
      lcd.write(7);
      lcd.setCursor(1,0);
      lcd.print(" ");

      dinoOnGround = true;
    }

    // This prints the trees so that it moves with the for loop
    lcd.setCursor(i, 1); 
    lcd.write(6);
    lcd.setCursor(i + 1, 1);
    lcd.print(" ");

    lcd.setCursor(i + dist, 1);
    lcd.write(6);
    lcd.setCursor(i + dist + 1, 1);
    lcd.print(" ");

    lcd.setCursor(i + dist + distTwo, 1);
    lcd.write(6);
    lcd.setCursor(i + dist + distTwo + 1, 1);
    lcd.print(" ");

    //When the trees reaches the starting point
    if ((i + dist + distTwo) == -1)
    {
      i = 12;                    
    } // they are taken to the end and set up randomly once more
    
    /*  
    Now check if the position of the trees 
    and that of the dino coincides, but we
    only need to do this if the dino is in the ground.
        
    Use the dinoOnGround variable and check if the
    condition is satisfied. If so the game is over
    */

    if(i == 1 && (dinoOnGround == true))
    {
      continueScreen(score);
      score = 0;
      break; 
    }
    else if(i + dist == 1 && (dinoOnGround == true))
    {
      continueScreen(score);
      score = 0;
      break;
    }
    else if(i+dist+distTwo == 1 && (dinoOnGround == true))
    {
      continueScreen(score);
      score = 0;
      break;
    }
    // Increase score for each loop successful iteration
    score++;  
    delay(250);
  }
}

void continueScreen(int score)
{
  // Display game over screen and high score
  lcd.clear();
  lcd.print("Game Over");
  
  if (score > EepromData.dinoHighScore)
  {
    EepromData.dinoHighScore = score;
    writeEepromData();
  }
  
  
  lcd.setCursor(0,1);
  lcd.print("HIGH: ");
  lcd.print(EepromData.dinoHighScore);
  delay(5000);
  lcd.clear();
  
  // Clear game conditions
  playState = false;
  gameOver = true;

  // Display continue option
  for (int x = 0; x < 9; x++)
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Try again? Press");
    lcd.setCursor(0,1);
    lcd.print("now! Menu in ");
    lcd.print(9 - x);
    if (testButton(true)) // Checks to see if button pressed for retry
    {
      lcd.clear();
      gameOver = false;
      break;
    }
    delay(1000);
  }
}