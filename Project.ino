
//GAME OVERVIEW
//-----------------
  /*
  This is a rhythm*-based game where notes "fall" down three columns of LEDs (red, green, and blue).
  Players must press corresponding buttons when the notes reach the bottom row.
  The game plays the Super Mario Bros theme while notes fall in sync with the music.
  Players are scored based on accuracy and receive different endings based on performance.

  Code that is not mine is sourced with links in-line with the code.


  *: Not really as the notes aren't tied to the music technically

 */


//IMPORTS
//-----------------

#include "pitches.h" /* This is used for defining the frequencies of musical notes.
                      https://github.com/hibit-dev/buzzer/tree/master/lib */     

//PIN CONFIGURATION
//-----------------

//Red LEDs
const int redLed1 = 11; //Column 1. It flashes if you make enough errors in your game. It will end the song pre-emptively.
const int redLed2 = 10;
const int redLed3 = 9;

//Green LEDs
const int greenLed1 = 8; //Column 2. It flashes if you make one or more mistakes but not enough mistakes where the game ends pre-maturely (you still win).
const int greenLed2 = 7;
const int greenLed3 = 6;

//Blue LEDs
const int blueLed1 = 5; //Column 3. It flashes when you complete the melody loop perfectly.
const int blueLed2 = 4;
const int blueLed3 = 3;

//Button pins
const int redButton = 2;
const int greenButton = 1;
const int blueButton = 0;
const int resetButton = 13; //Resets game. Yes I ran out of ports. I would've increased functionality but I don't want to daisychain anything together.

//Audio output
const int speakerPin = 12;


// GAME CONFIGURATION
// -----------------


//Game configuration settings
const int delayTime = 500;      //Change this for increased or decreased difficulty.
const int buttonDebounce = 50;  //Debounce time for buttons
const int maxNotesAtOnce = 3;   //Maximum number of notes that can be falling at once. The reason for 3 is that for a 9x9 grid, there is no possible way to fit 4 active lights out of 9 without having 2 or more lights on the same row.

//The reason for not wanting 2 lights on the same row is that you cannot press both buttons at the same time (within that margin of error granted by the code to register as a successful hit). There's probably a way around it but this code is already more complicated than I want it to be already.




//SOUND EFFECTS
//------------


//Mario theme melody (partial)

int melody[] = { //Refactored but this is not my code: https://github.com/robsoncouto/arduino-songs/blob/master/supermariobros/supermariobros.ino

  //Comments from github repo

  // Super Mario Bros theme
  // Score available at https://musescore.com/user/2123/scores/2145
  // Theme by Koji Kondo


  //End of that comment

  NOTE_E5, NOTE_E5, REST, NOTE_E5, REST, NOTE_C5, NOTE_E5, //1
  NOTE_G5, REST, NOTE_G4, REST, 
  NOTE_C5, NOTE_G4, REST, NOTE_E4, //3
  NOTE_A4, NOTE_B4, NOTE_AS4, NOTE_A4, 
  NOTE_G4, NOTE_E5, NOTE_G5, NOTE_A5, NOTE_F5, NOTE_G5, 
  REST, NOTE_E5, NOTE_C5, NOTE_D5, NOTE_B4, 
  NOTE_C5, NOTE_G4, REST, NOTE_E4, //Repeats from 3
  NOTE_A4, NOTE_B4, NOTE_AS4, NOTE_A4, 
  NOTE_G4, NOTE_E5, NOTE_G5, NOTE_A5, NOTE_F5, NOTE_G5, 
  REST, NOTE_E5, NOTE_C5, NOTE_D5, NOTE_B4, 
  
  REST, NOTE_G5, NOTE_FS5, NOTE_F5, NOTE_DS5, NOTE_E5, //7
  REST, NOTE_GS4, NOTE_A4, NOTE_C4, REST, NOTE_A4, NOTE_C5, NOTE_D5, 
  REST, NOTE_DS5, REST, NOTE_D5, 
  NOTE_C5, REST, 
  
  REST, NOTE_G5, NOTE_FS5, NOTE_F5, NOTE_DS5, NOTE_E5, //Repeats from 7
  REST, NOTE_GS4, NOTE_A4, NOTE_C4, REST, NOTE_A4, NOTE_C5, NOTE_D5, 
  REST, NOTE_DS5, REST, NOTE_D5, 
  NOTE_C5, REST, 
  
  NOTE_C5, NOTE_C5, NOTE_C5, REST, NOTE_C5, NOTE_D5, //11
  NOTE_E5, NOTE_C5, NOTE_A4, NOTE_G4,
  
  
  NOTE_C5, NOTE_C5, NOTE_C5, REST, NOTE_C5, NOTE_D5, NOTE_E5, //13
  REST,
  NOTE_C5, NOTE_C5, NOTE_C5, REST, NOTE_C5, NOTE_D5,
  NOTE_E5, NOTE_C5, NOTE_A4, NOTE_G4,
  NOTE_E5, NOTE_E5, REST, NOTE_E5, REST, NOTE_C5, NOTE_E5,
  NOTE_G5, REST, NOTE_G4, REST,
  NOTE_C5, NOTE_G4, REST, NOTE_E4, //19
  
  NOTE_A4, NOTE_B4, NOTE_AS4, NOTE_A4,
  NOTE_G4, NOTE_E5, NOTE_G5, NOTE_A5, NOTE_F5, NOTE_G5,
  REST, NOTE_E5, NOTE_C5, NOTE_D5, NOTE_B4,
  NOTE_C5, NOTE_G4, REST, NOTE_E4, //Repeats from 19
  
  NOTE_A4, NOTE_B4, NOTE_AS4, NOTE_A4,
  NOTE_G4, NOTE_E5, NOTE_G5, NOTE_A5, NOTE_F5, NOTE_G5,
  REST, NOTE_E5, NOTE_C5, NOTE_D5, NOTE_B4,
  
  NOTE_E5, NOTE_C5, NOTE_G4, REST, NOTE_GS4, //23
  NOTE_A4, NOTE_F5, NOTE_F5, NOTE_A4,
  NOTE_D5, NOTE_A5, NOTE_A5, NOTE_A5, NOTE_G5, NOTE_F5,
  
  NOTE_E5, NOTE_C5, NOTE_A4, NOTE_G4, //25
  NOTE_E5, NOTE_C5, NOTE_G4, REST, NOTE_GS4,
  NOTE_A4, NOTE_F5, NOTE_F5, NOTE_A4,
  NOTE_B4, NOTE_F5, NOTE_F5, NOTE_F5, NOTE_E5, NOTE_D5,
  
  NOTE_C5, NOTE_E4, NOTE_E4, NOTE_C4, //27
  NOTE_E5, NOTE_E5, REST, NOTE_E5, REST, NOTE_C5, NOTE_E5,
  NOTE_G5, REST, NOTE_G4, REST,
  NOTE_E5, NOTE_C5, NOTE_G4, NOTE_A4,
  
  
};


//Durations refactored: 2 is dotted quarter note instead of -4. Avoiding negatives as that causes... Issues.
//Here are the comments from the original code:

// notes of the moledy followed by the duration.
// a 4 means a quarter note, 8 an eighteenth , 16 sixteenth, so on
// !!negative numbers are used to represent dotted notes,
// so -4 means a dotted quarter note, that is, a quarter plus an eighteenth!!

//End of comment from that repo

int durations[] = { //Same thing here. Refactored but not my code. https://github.com/robsoncouto/arduino-songs/blob/master/supermariobros/supermariobros.ino
  8, 8, 8, 8, 8, 8, 8, //1
  4, 4, 8, 4, 
  4, 8, 4, 4, //3
  4, 4, 8, 4, 
  8, 8, 8, 4, 8, 8, 
  8, 4, 8, 8, 4, 
  4, 8, 4, 4, //Repeat from 3
  4, 4, 8, 4, 
  8, 8, 8, 4, 8, 8, 
  8, 4, 8, 8, 4, 
  
  4, 8, 8, 8, 4, 8, //7
  8, 8, 8, 8, 8, 8, 8, 8, 
  4, 4, 8, 4, 
  2, 2, 
  
  4, 8, 8, 8, 4, 8, //Repeat from 7
  8, 8, 8, 8, 8, 8, 8, 8, 
  4, 4, 8, 4, 
  2, 2, 
  
  8, 4, 8, 8, 8, 4, //11
  8, 4, 8, 2,
  
  
  8, 4, 8, 8, 8, 8, 8, //13
  1,
  8, 4, 8, 8, 8, 4,
  8, 4, 8, 2,
  8, 8, 8, 8, 8, 8, 8,
  4, 4, 4, 4,
  4, 8, 4, 4, //19
  
  4, 4, 8, 4,
  8, 8, 8, 4, 8, 8,
  8, 4, 8, 8, 4,
  4, 8, 4, 4, //Repeat from 19
  
  4, 4, 8, 4,
  8, 8, 8, 4, 8, 8,
  8, 4, 8, 8, 4,
  
  8, 4, 8, 4, 4, //23
  8, 4, 8, 2,
  8, 8, 8, 8, 8, 8,
  
  8, 4, 8, 2, //25
  8, 4, 8, 4, 4,
  8, 4, 8, 2,
  8, 8, 8, 8, 8, 8,
  
  8, 4, 8, 2, //27
  8, 8, 8, 8, 8, 8, 8,
  4, 4, 4, 4,
  8, 8, 8, 4,
  

  

};
const int melodyLength = sizeof(durations) / sizeof(int);

//Game over sound separated from main melody. Plays for each "ending". Feels weird if you succeed but when playing around with this, I actually hit the Arduino's file size limit so I want to streamline things.
int gameOverMelody[] = {
  NOTE_C5, NOTE_G4, NOTE_E4,
  NOTE_A4, NOTE_B4, NOTE_A4, NOTE_GS4, NOTE_AS4, NOTE_GS4,
  NOTE_G4, NOTE_D4, NOTE_E4
};

int gameOverDurations[] = {
  4, 4, 4,
  8, 8, 8, 8, 8, 8,
  8, 8, 2
};

const int gameOverMelodyLength = sizeof(gameOverDurations) / sizeof(int);

//Sound effects. Both are overlaid while the sond is playing
const int errorNote = NOTE_B3;
const int successNote = NOTE_B5;


// GAME STATE TRACKING
// ------------------

//Structure to track multiple falling notes.
struct FallingNote {
  int column;     //0 = red, 1 = green, 2 = blue
  int step;       //0 = top LED, 1 = middle, 2 = bottom
  bool active;    //Whether this note is currently active
  bool hit;       //Whether this note has been hit by player
};

//Game state variables
int missCount = 0;      //Count of misses for game over condition
int maxMisses = 10;     //Maximum misses allowed before game over. set to 10 because why not?
unsigned long lastNoteTime = 0;
unsigned long lastMelodyTime = 0;
int melodyIndex = 0;    //Current position in melody


//Game ending tracking
int gameOverReason = -1; 
//-1 = still playing
//0 = too many misses
//1 = completed with misses
//2 = perfect game

//Track multiple falling notes
FallingNote notes[maxNotesAtOnce]; //Array to track all active falling notes. Uses the defined (by yours truly) FallingNote struct 

//Game state
bool inGame = true;     //If false, play full Mario theme
bool gameOver = false;  //Game over state


//CORE GAME FUNCTIONS
//-----


//Check if we've reached the end of the melody
//If true then it's complete and game over funciton will play.
bool isMainLoopComplete() {
  return melodyIndex >= melodyLength;
}

//Plays the tone. This function plays all tones.
void playTone(int frequency, int duration) {
  tone(speakerPin, frequency, duration);
}

void stopTone() { //Default function for cutting the sound of the tone.
  noTone(speakerPin);
}

//STEP 1

//Set up the game and initialize components on the hardware level. Yes we already did variable configs but this is where the rubber meets the road
void setup() {
  //Initialize LED pins
  pinMode(redLed1, OUTPUT);
  pinMode(redLed2, OUTPUT);
  pinMode(redLed3, OUTPUT);
  pinMode(greenLed1, OUTPUT);
  pinMode(greenLed2, OUTPUT);
  pinMode(greenLed3, OUTPUT);
  pinMode(blueLed1, OUTPUT);
  pinMode(blueLed2, OUTPUT);
  pinMode(blueLed3, OUTPUT);
  
  //Initialize buttons with pull-up resistors
  pinMode(redButton, INPUT_PULLUP);
  pinMode(greenButton, INPUT_PULLUP);
  pinMode(blueButton, INPUT_PULLUP);
  pinMode(resetButton, INPUT_PULLUP);
  
  pinMode(speakerPin, OUTPUT);
  
  //Turn all LEDs off (HIGH = off)
  turnOffAllLEDs();
  
  //Initialize all notes as inactive
  for (int i = 0; i < maxNotesAtOnce; i++) {
    notes[i].active = false;
    notes[i].hit = false;
  }
  
  randomSeed(analogRead(A0)); //You brought this up in class. I found it interesting that this creates a seed that the then "random" function pulls from from the voltage of the A0 port.
  
  //Create initial notes to make sure game starts with notes in at least 2 columns. 
  //Technically there will only be one to start out with but after that split second, there will always be two lights on display at all times. I just don't want the lights to be on the same row.
  ensureMinimumColumnsActive();
}

void turnOffAllLEDs() { //Turns off all leds
  digitalWrite(redLed1, HIGH);
  digitalWrite(redLed2, HIGH);
  digitalWrite(redLed3, HIGH);
  digitalWrite(greenLed1, HIGH);
  digitalWrite(greenLed2, HIGH);
  digitalWrite(greenLed3, HIGH);
  digitalWrite(blueLed1, HIGH);
  digitalWrite(blueLed2, HIGH);
  digitalWrite(blueLed3, HIGH);
}

//Play game over animation with specific column flashing based on game result
void playEndAnimation() {
  //Clear all LEDs
  turnOffAllLEDs();
  
  //Determine which column to flash based on game over reason
  int flashColumn;
  switch(gameOverReason) {
    case 0: //Too many misses - flash red column
      flashColumn = 0;
      break;
    case 1: //Completed with some misses - flash green column
      flashColumn = 1;
      break;
    case 2: //Perfect game (no misses) - flash blue column
      flashColumn = 2;
      break;
    default:
      flashColumn = 0; //Default to red if something went wrong
  }
  
  //Flash the appropriate column three times
  for (int i = 0; i < 3; i++) {
    //Turn on LEDs for the selected column
    switch(flashColumn) {
      case 0: //Red column
        digitalWrite(redLed1, LOW);
        digitalWrite(redLed2, LOW);
        digitalWrite(redLed3, LOW);
        break;
      case 1: //Green column
        digitalWrite(greenLed1, LOW);
        digitalWrite(greenLed2, LOW);
        digitalWrite(greenLed3, LOW);
        break;
      case 2: //Blue column
        digitalWrite(blueLed1, LOW);
        digitalWrite(blueLed2, LOW);
        digitalWrite(blueLed3, LOW);
        break;
    }
    
    delay(200);
    
    turnOffAllLEDs();
    delay(200);
  }
}

//Play game over melody (now separate from main melody)
void playGameOverSound() {
  for (int i = 0; i < gameOverMelodyLength; i++) {
    int duration = 1000 / gameOverDurations[i];
    playTone(gameOverMelody[i], duration);
    
    //Add pause between notes
    delay(duration * 1.30);
    stopTone();
  }
}

//Check buttons and handle hits - only valid for bottom row
void checkButtons() {
  static bool redLastState = HIGH;
  static bool greenLastState = HIGH;
  static bool blueLastState = HIGH;
  
  //Read current button states
  bool redCurrent = digitalRead(redButton);
  bool greenCurrent = digitalRead(greenButton);
  bool blueCurrent = digitalRead(blueButton);
  
  //Detect button presses (only on transition from HIGH to LOW)
  bool redPressed = (redCurrent == LOW && redLastState == HIGH); //Bottom must be lit and the last state that it was in must not have been lit.
  bool greenPressed = (greenCurrent == LOW && greenLastState == HIGH); //Again there is a max of 3 active lights going on at the same time.
  bool bluePressed = (blueCurrent == LOW && blueLastState == HIGH);
  
  //Store current states for next time
  redLastState = redCurrent;
  greenLastState = greenCurrent;
  blueLastState = blueCurrent;
  
  //Check each column only if its button is pressed
  if (redPressed || greenPressed || bluePressed) { //If either of the three bottom buttons are pressed.
    //Track if any valid hit was registered
    bool validHitRegistered = false; //Flag variable
    
    //Only allow hits for notes at the bottom row
    for (int i = 0; i < maxNotesAtOnce; i++) {
      //Only check active notes that haven't been hit yet and are at bottom row
      if (notes[i].active && !notes[i].hit && notes[i].step == 2) {
        bool correctButtonPressed = false;
        
        //Check if correct button was pressed for this note
        switch(notes[i].column) {
          case 0: correctButtonPressed = redPressed; break;
          case 1: correctButtonPressed = greenPressed; break;
          case 2: correctButtonPressed = bluePressed; break;
        }
        
        if (correctButtonPressed) {
          //Perfect hit
          playTone(successNote, 100);
          notes[i].hit = true;
          validHitRegistered = true;
          break; //Only hit one note per button press
        }
      }
    }
    
    //If no valid hit found but a button was pressed, it's a complete miss
    if (!validHitRegistered) {
      //Miss penalty
      playTone(errorNote, 100);
      missCount++;
      
      //Check if max misses reached
      if (missCount >= maxMisses) {
        gameOver = true;
        gameOverReason = 0; //0 = too many misses. works as a flag. as stated up top: 0 is fail, 1 is good game with some misses and 2 is perfect.
      }
    }
  }
}

//Update all LED states based on current notes
//Ensures only one light per row is on
void updateLEDs() {
  //First turn all LEDs off
  turnOffAllLEDs();
  
  //Arrays to track which LEDs in each row are going to be lit
  bool topRowLit[3] = {false, false, false};
  bool midRowLit[3] = {false, false, false};
  bool botRowLit[3] = {false, false, false};
  
  //First pass: mark which LEDs should be on. goes through each colour and each row. works as there can only be one active note on one row.
  for (int i = 0; i < maxNotesAtOnce; i++) {
    if (notes[i].active && !notes[i].hit) {
      switch(notes[i].step) {
        case 0: topRowLit[notes[i].column] = true; break; 
        case 1: midRowLit[notes[i].column] = true; break;
        case 2: botRowLit[notes[i].column] = true; break;
      }
    }
  }
  
  //Second pass: actually light the LEDs
  //Top row (only turn on one LED per row)
  if (topRowLit[0]) {
    digitalWrite(redLed1, LOW);
  } else if (topRowLit[1]) {
    digitalWrite(greenLed1, LOW);
  } else if (topRowLit[2]) {
    digitalWrite(blueLed1, LOW);
  }
  
  //Middle row (only turn on one LED per row)
  if (midRowLit[0]) {
    digitalWrite(redLed2, LOW);
  } else if (midRowLit[1]) {
    digitalWrite(greenLed2, LOW);
  } else if (midRowLit[2]) {
    digitalWrite(blueLed2, LOW);
  }
  
  //Bottom row (only turn on one LED per row)
  if (botRowLit[0]) {
    digitalWrite(redLed3, LOW);
  } else if (botRowLit[1]) {
    digitalWrite(greenLed3, LOW);
  } else if (botRowLit[2]) {
    digitalWrite(blueLed3, LOW);
  }
}

//Check if row is occupied in any column
bool isRowOccupied(int step) {
  for (int i = 0; i < maxNotesAtOnce; i++) {
    if (notes[i].active && !notes[i].hit && notes[i].step == step) {
      return true;
    }
  }
  return false;
}

//Count active notes in each column
//Monitors how many active notes are in each column

//STEP 2.2

void countActiveNotesPerColumn(int columnCounts[3]) {
  //Initialize counts to zero
  columnCounts[0] = columnCounts[1] = columnCounts[2] = 0;
  
  //Count active notes in each column (lit up)
  for (int i = 0; i < maxNotesAtOnce; i++) {
    if (notes[i].active && !notes[i].hit) { //If light is on and not hit, increment the columnCounts variable for the column.
      columnCounts[notes[i].column]++; //This counts every single note on the light board (all 9) starts red then green then blue.
    }
  }
}

//STEP 2.1

//Count how many columns have active notes
int countActiveColumns() {
  int columnCounts[3] = {0, 0, 0};
  countActiveNotesPerColumn(columnCounts);
  
  int activeColumns = 0;
  for (int i = 0; i < 3; i++) {
    if (columnCounts[i] > 0) {
      activeColumns++;
    }
  }
  
  return activeColumns;
}

//STEP 2.3

//Create a new note in a specific column
bool createNoteInColumn(int column) {
  //Find an inactive slot for a new note
  for (int i = 0; i < maxNotesAtOnce; i++) {
    if (!notes[i].active) {
    //A bit counterinitive. It's going through the space delegated to see if we can have anohter note thrown into the game.

      //Check if top row is already occupied
      if (isRowOccupied(0)) {
        return false; //Cannot create note if row is already occupied
      }
      //Else start at top of the colmumn and create note.
      
      notes[i].column = column;
      notes[i].step = 0;        //Start at top
      notes[i].active = true;   //Activate note
      notes[i].hit = false;     //Reset hit status
      
      
      return true;
      //This then goes to step 3
    }
  }
  return false; //No free slots
}

//Update all falling notes
void updateNotes() {
  for (int i = 0; i < maxNotesAtOnce; i++) {
    if (notes[i].active) {
      if (notes[i].step >= 2) {
        //Note has reached bottom
        if (!notes[i].hit) {
          //Missed note penalty
          playTone(errorNote, 100);
          missCount++;
          
          //Check for game over due to too many misses
          if (missCount >= maxMisses) {
            gameOver = true;
            gameOverReason = 0; //0 = too many misses
          }
        }
        //Deactivate note regardless if hit or missed
        notes[i].active = false;
      } else {
        //Check if next row is already occupied by another column
        bool nextRowOccupied = false;
        int nextStep = notes[i].step + 1;
        
        for (int j = 0; j < maxNotesAtOnce; j++) { //IMPORTANT! This checks to see that there will not be a collusion. Remember that there cannot be two or more notes in the same row.
          if (j != i &&  //Not the same note
          notes[j].active && //Other note is active
          !notes[j].hit && //Other note hasn't been hit
          notes[j].step == nextStep //Other note is in next row
          && notes[j].column != notes[i].column) { //Different columns
            nextRowOccupied = true;
            break;
          }
        }
        
        //Move the note down if next row is not occupied by another column
        if (!nextRowOccupied) {
          notes[i].step++;
        }
      }
    }
  }
}

//STEP 2

//Ensure at least 2 columns have active notes (after start)
void ensureMinimumColumnsActive() {
  int activeColumns = countActiveColumns();
  
  //If we have fewer than 2 active columns and game is in progress. THIS SHOULD TRIGGER WHEN THE GAME FIRST STARTS (don't want 2 active notes on the same row) (we can have 2 or more on the same column though). Also everything is blank to begin with.
  if (activeColumns < 2) {
    //Find columns without notes
    int inactiveColumns[3] = {-1, -1, -1}; //Default values
    int inactiveCount = 0;
    int columnCounts[3] = {0, 0, 0};
    countActiveNotesPerColumn(columnCounts);
    
    for (int i = 0; i < 3; i++) { //Goes through and sees which columns are inactive
      if (columnCounts[i] == 0) { //If 0 or if inactive.
        inactiveColumns[inactiveCount++] = i; //If all inactive: inactiveColumns = [0, 1, 2]. if blue was only inactive then = [2, -1, -1]. -1 just from allocating array space and giving a default value. they are not used.
      } //Keeping the same example: if all were not active then inactivecount would be = 3. Just blue would be = 1
    }
    
    //Try to create notes in inactive columns if we find any.
    if (inactiveCount > 0) { 
      //Randomize which inactive columns to use
      int columnsNeeded = 2 - activeColumns; //Remember that this doesn't go negative as it's in a if statement that only occurs if there is less than 2 activeColumns
      
      //Inplementation of the "fisher yates shuffle" algorithm to randomize columns. Sounds fancy but it's not that bad:
      //When all columns inactive: inactiveColumns = [0, 1, 2], inactiveCount = 3. three cards in the deck to shuffle.
      //When two columns inactive: inactiveColumns = [0, 1, -1], inactiveCount = 2. two cards in the deck to shuffle
      //When one column inactive: inactiveColumns = [2, -1, -1], inactiveCount = 1. Doesn't active algo as you have only one column to shuffle.

      //Loop starts from last index (inactiveCount - 1) and goes down to 1
      for (int i = inactiveCount - 1; i > 0; i--) {
        int j = random(i + 1); //Generates random index between 0 and i inclusive. For i = 2: random(3) gives 0, 1, or 2
        int temp = inactiveColumns[i]; //Temp variable for current element. keeping to the same example i = 2 so temp = 2
        inactiveColumns[i] = inactiveColumns[j]; //Replaces current element with element at random index j. For i = 2 and j = 1: [0, 1, 2] becomes [0, 1, 1]
        inactiveColumns[j] = temp; //Places the saved element from position i into position j, finishing the swap. If i=2 and j=1: [0, 1, 2] becomes [0, 2, 1]
        //Keeping to the same example: this would play out one more time with i = 1 so that every element in the array gets touched.
      }

      //For more information: https://en.wikipedia.org/wiki/Fisher%E2%80%93Yates_shuffle
      
      //Try to create notes in the needed columns
      for (int i = 0; i < columnsNeeded && i < inactiveCount; i++) {
        createNoteInColumn(inactiveColumns[i]); //Create note in randomly chosen column.
      }
    }
  }
}

//Reset the game to initial state
void resetGame() {
  //Play a short reset animation
  turnOffAllLEDs();
  
  //Flash all LEDs once quickly
  digitalWrite(redLed1, LOW);
  digitalWrite(redLed2, LOW);
  digitalWrite(redLed3, LOW);
  digitalWrite(greenLed1, LOW);
  digitalWrite(greenLed2, LOW);
  digitalWrite(greenLed3, LOW);
  digitalWrite(blueLed1, LOW);
  digitalWrite(blueLed2, LOW);
  digitalWrite(blueLed3, LOW);
  delay(100);
  turnOffAllLEDs();
  
  //Reset game variables
  missCount = 0;
  gameOver = false;
  inGame = true;
  melodyIndex = 0;
  gameOverReason = -1;
  
  //Reset all notes
  for (int i = 0; i < maxNotesAtOnce; i++) {
    notes[i].active = false;
    notes[i].hit = false;
  }
  
  //Play a success tone
  playTone(NOTE_C5, 100);
  delay(100);
  playTone(NOTE_E5, 100);
  delay(100);
  playTone(NOTE_G5, 200);
  
  //Small delay before starting the game again
  delay(500);
  
  //Create initial notes to ensure game starts with lights in at least 2 columns
  ensureMinimumColumnsActive();
}

//STEP 3

void loop() {
  //Track previous state of reset button for edge detection
  static bool resetLastState = HIGH;
  bool resetCurrent = digitalRead(resetButton);
  
  //Check if reset button is pressed (only on transition from HIGH to LOW)
  if (resetCurrent == LOW && resetLastState == HIGH) {
    resetGame();
    delay(300);
  }
  
  resetLastState = resetCurrent; //Then the delay allows the board to read HIGH again. infinite resets.
  
  //Check if game should end
  if (gameOver) {
    //Play game over animation and sound
    turnOffAllLEDs();
    playEndAnimation();
    
    //Play game over melody
    playGameOverSound();
    
    //Reset game
    resetGame();
    return;
  }
  
  if (inGame) {
    unsigned long currentTime = millis(); //This is needed as a reference point for how long we are into the melody.

  if (currentTime - lastMelodyTime >= (1000 / durations[melodyIndex])) { //1000 is one second. then the 8s are eighth notes, 4s are quarters etc.
  lastMelodyTime = currentTime;
  stopTone();  //Stop previous tone
  
  //For example, it loads a eighth note so 1000 / 8 = 125
  //10750 - 10500 >=  125
  //250 >= 125 yes play the note


  //Play the current note in the melody
  int duration = 1000 / durations[melodyIndex];
  playTone(melody[melodyIndex], duration); //Same logic
  
  //Move to next note in melody.
  melodyIndex = melodyIndex + 1;
  
  //Check if we've completed the main loop
  if (melodyIndex >= melodyLength) {
    gameOver = true;
    
    //Set game over reason based on miss count
    if (missCount == 0) {
      gameOverReason = 2; //Perfect game - no misses
    } else {
      gameOverReason = 1; //Completed with some misses
    }
  }
    
    }
    
    //Update notes position and create new notes every delayTime milliseconds
    if (currentTime - lastNoteTime >= delayTime) {
      lastNoteTime = currentTime; //Similar logic to the changing of the notes.
      
      //First update existing notes
      updateNotes();
      
      //Check if we need to ensure minimum columns with lights
      ensureMinimumColumnsActive();
      
      //Only create a new note if top row is free
      if (!isRowOccupied(0)) {
        //Find columns with no active notes
        int columnCounts[3] = {0, 0, 0};
        countActiveNotesPerColumn(columnCounts);
        
        int emptyColumns[3] = {-1, -1, -1};
        int emptyCount = 0;
        
        for (int c = 0; c < 3; c++) { //Look for and add to counter variable however many empty columns. Similar function to countActiveNotesPerColumn()
          if (columnCounts[c] == 0) { //Minor redundancy but for clarity sake.
            emptyColumns[emptyCount++] = c;
          }
        }
        
        //Prioritize empty columns for new notes
        if (emptyCount > 0) {
          int randomEmptyColumn = emptyColumns[random(emptyCount)];
          createNoteInColumn(randomEmptyColumn);
        } else if (random(10) < 3) { //30% chance to create another note in a random column
          int randomColumn = random(3);
          createNoteInColumn(randomColumn);
        }
      }
      
      //Update the LEDs to show current state
      updateLEDs();
    }
    
    //Check for button presses
    checkButtons();
    
    //Add small delay to prevent button bounce
    delay(buttonDebounce);
  }
}