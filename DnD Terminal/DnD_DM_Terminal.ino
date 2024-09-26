/*================================================================*/
/* Libraries */
/*================================================================*/

#include <U8g2lib.h>
#include <Wire.h>
#include <GEM_u8g2.h>
// #include <SD.h> // Maybe some SD incorporation?
#include "dnd_bitmaps.h"

U8G2_SH1106_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

GEM_u8g2 menu(u8g2, GEM_POINTER_ROW, GEM_ITEMS_COUNT_AUTO);

/*================================================================*/
/* Pin Definitions */
/*================================================================*/

#define sensorPin A0
const uint8_t select_button = 4;
const uint8_t home_button = 11;
const uint8_t next_button = 7;
const uint8_t prev_button = 8;
const uint8_t up_button = 12;
const uint8_t down_button = 13;


// Default Pin Definitions
/*  
 *  Default SDA = A4;
 *  Default SLA = A5;
*/

/*================================================================*/
/* Forward Function Declarations */
/*================================================================*/

void setupMenu();
void DrawRoll(uint8_t x, uint8_t v);
void DiceRollEnter();
void DiceRollExit();
void DiceRollContext();
void DiceRoll();

/*================================================================*/
/* Dice Definitions */
/*================================================================*/

// Dice roll animation toggle
byte DiceFrames = 7;
byte currentDiceFrame = DiceFrames;

// Dice values for rolls
const uint8_t diceValues[7] = {2, 4, 6, 8, 10, 12, 20};
int rollCount = 8;

const Splash dicePatterns[] = 
{
  d2, 
  d4, 
  d6, 
  d8, 
  d10, 
  d12, 
  d20, 
  d2r, 
  d4r, 
  d6r, 
  d8r, 
  d10r, 
  d12r, 
  d20r
};


/*
// Bitmap pointer array for easy display toggling
const uint8_t* const dicePatterns[] PROGMEM = 
{
  d2_bm,
  d4_bm,
  d6_bm,
  d8_bm,
  d10_bm,
  d12_bm,
  d20_bm,
  d2r_bm,
  d4r_bm,
  d6r_bm,
  d8r_bm,
  d10r_bm,
  d12r_bm,
  d20r_bm
};
*/

#define SUCCEED 0
#define FAIL 1
#define PERFECT 2

// Create the Dice Roller Page
GEMPage menuDicePage("DnD DM Terminal");

// Establish a selection array of dice options
SelectOptionByte selectDiceOptions[] = 
{
  {"D2", 0}, 
  {"D4", 1}, 
  {"D6", 2}, 
  {"D8", 3}, 
  {"D10", 4}, 
  {"D12", 5},
  {"D20", 6}
};

// Establish the dice selection index
byte dice_selection = 0;

// Create a menu select that can toggle between dice options
GEMSelect selectDice(sizeof(selectDiceOptions)/sizeof(SelectOptionByte), selectDiceOptions);

// Create menu item for dice select
GEMItem Dice_MenuItem("Dice:", dice_selection, selectDice);

// Create menu button that will trigger DiceRoll() function. It will run animation sequence.
GEMItem Roll_MenuItem("Roll Dice!", DiceRoll);

/*================================================================*/
/* Menu Definitions */
/*================================================================*/

// Animation variables
const uint8_t interval = 400;
unsigned long previousMillis = 0;

// Create menu page object of class GEMPage. Menu page holds menu items (GEMItem) and represents menu level.
// Menu can have multiple menu pages (linked to each other) with multiple menu items each
GEMPage menuMainPage("DnD DM Terminal");

// Create menu item for dice select
GEMItem DicePage_MenuItem("Dice Roller", menuDicePage);

/*================================================================*/
/* Menu Functions */
/*================================================================*/

void SetupMenu() 
{
  // Add menu items to dice page
  menuDicePage.addMenuItem(Dice_MenuItem);
  menuDicePage.addMenuItem(Roll_MenuItem);

  // Add menu items to main page
  menuMainPage.addMenuItem(DicePage_MenuItem);

  // Specify parent menu page for the Settings menu page
  menuDicePage.setParentMenuPage(menuMainPage);

  // Add menu page to menu and set it as current
  menu.setMenuPageCurrent(menuMainPage);
}

/*================================================================*/
/* Dice Functions */
/*================================================================*/

void DiceRoll()
{
  menu.context.loop = DiceRollContext;
  menu.context.enter = DiceRollEnter;
  menu.context.exit = DiceRollExit;
  menu.context.allowExit = false; // Setting to false will require manual exit from the loop
  menu.context.enter();
}

// Invoked once to begin dice roll process
void DiceRollEnter() 
{
  // Clear sreen
  u8g2.clear();
}

// Invoked once when the GEM_KEY_CANCEL key is pressed
void DiceRollExit() 
{
  // Reset variables
  rollCount = 8;
  previousMillis = 0;
  currentDiceFrame = DiceFrames;

  // Draw menu back on screen and clear context
  menu.reInit();
  menu.drawMenu();
  menu.clearContext();
}

// Invoked every loop iteration
void DiceRollContext() 
{
  // Create a variable to store rolled value
  uint8_t roll;

  // Detect key press manually using U8g2 library
  byte key = u8g2.getMenuEvent();

  if (key == GEM_KEY_CANCEL) 
  {
    // Exit animation routine if GEM_KEY_CANCEL key was pressed
    menu.context.exit();
  } 
  else
  {
    // Repeat animation loop several times
    while (rollCount > 0)
    {
      unsigned long currentMillis = millis();

      if (currentMillis - previousMillis >= interval) 
      {
        // Reset timer
        previousMillis = currentMillis;
        
        // Display a random dice throw
        roll = random(1, diceValues[dice_selection] + 1);
        DrawRoll(SUCCEED, roll);

        rollCount--;

        /*
        Serial.print("roll count: ");
        Serial.print(rollCount);
        Serial.print(", roll valued: "); 
        Serial.print(roll);
        Serial.print(", dice select: "); 
        Serial.print(dice_selection + currentDiceFrame);
        Serial.println();
        */
      }
    }

    // Leave final roll as stationary image
    while (rollCount == 0)
    {
      unsigned long currentMillis = millis();

      if (currentMillis - previousMillis >= interval) 
      {

        // Display a random dice throw once
        roll = random(1, diceValues[dice_selection] + 1);

        if (roll == 1 && dice_selection == 6) 
        {
          // Check for nat 1
          DrawRoll(FAIL, roll);
        }
        else if (roll == 20 && dice_selection == 6) 
        {
          // Check for nat 20
          DrawRoll(PERFECT, roll);
        }
          else
        {
          // Draw any other roll
          DrawRoll(SUCCEED, roll);
        }
        rollCount--;
      }
    }
  }
}

void DrawRoll(uint8_t X /* check for special roll */,
              uint8_t v /*rolled value */) 
{
  // Change arary index for animation
  currentDiceFrame = (currentDiceFrame == DiceFrames ? 0 : DiceFrames);

  u8g2.firstPage();
  do
  {
    // Display dice graphics
    switch (X) 
    {
      case SUCCEED:
        u8g2.drawXBMP(14 /* x position */, 
                      13 /* y position */, 
                      dicePatterns[dice_selection].width /* horiz bytes */, 
                      dicePatterns[dice_selection].height /* height */, 
                      dicePatterns[dice_selection + currentDiceFrame].image);
        break;
      case FAIL:
        u8g2.drawXBMP(10 /* x position */, 
                      12 /* y position */, 
                      skull.width /* horiz bytes */, 
                      skull.height /* height */, 
                      skull.image);
        break;
      case PERFECT:
        u8g2.drawXBMP(10 /* x position */, 
                      12 /* y position */, 
                      star.width /* horiz bytes */, 
                      star.height /* height */, 
                      star.image);
        break;
    }

    u8g2.setFont(u8g2_font_timB24_tr);

    // Create a buffer string to display the rolled value
    char buf[4];
    snprintf(buf, sizeof(buf), "%d", v);
    
    // Adjust position based on number of digits
    uint8_t x_pos = (v < 10) ? 87 : 80;
    
    // Draw rolled value
    u8g2.drawStr(x_pos, 21, buf);

    /*
    Serial.print("buffer: ");
    Serial.print(buf);
    Serial.print(", roll valued: "); 
    Serial.print(v);
    Serial.println();
    */
  }
  while (u8g2.nextPage());
}

/*================================================================*/
/* Arduino Initialization */
/*================================================================*/

void setup() 
{
  // Serial monitor for debugging
  // Serial.begin(9600);
  
  // Initialize display
  u8g2.begin(
    /* menu_select_pin= */ select_button, 
    /* menu_next_pin= */ next_button, 
    /* menu_prev_pin= */ prev_button, 
    /* menu_up_pin= */ up_button, 
    /* menu_down_pin= */ down_button, 
    /* menu_home_pin= */ home_button);

  // Ensure Randomness
  randomSeed(analogRead(sensorPin));
  
  // Initalize menu, setup and draw
  menu.init();
  SetupMenu();
  menu.drawMenu();
}

/*================================================================*/
/* Loop Code */
/*================================================================*/

void loop() 
{
  // If menu is ready to accept button press...
  if (menu.readyForKey()) 
  {
    // detect key press using U8g2 library and pass pressed button to menu
    menu.registerKeyPress(u8g2.getMenuEvent());
  }
}