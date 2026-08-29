#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED Display Configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Button Pins
#define BUTTON_RIGHT_PIN 14
#define BUTTON_UP_PIN    27

// Long press duration (2000 milliseconds = 2 seconds)
const unsigned long LONG_PRESS_TIME = 2000; 

// Grid dimensions
#define ROWS 4
#define COLS 8

// PAGE 0: Letters Layout
char pageLetters[ROWS][COLS] = {
  {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'},
  {'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P'},
  {'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X'},
  {'Y', 'Z', ' ', '<', '>', '_', '_', '_'} // ' ' is Space, '<' is Backspace, '>' is Next Page
};

// PAGE 1: Numbers & Special Characters Layout (With Colon ':')
char pageSymbols[ROWS][COLS] = {
  {'1', '2', '3', '4', '5', '6', '7', '8'},
  {'9', '0', '.', ',', '?', '!', ':', '@'}, // Added ':' right here
  {'#', '$', '%', '&', '*', '(', ')', '-'},
  {'+', '/', '=', ' ', '<', '>', '_', '_'}  // '>' switches back to Letters
};

// Navigation & Selection State
int currentPage = 0; // 0 = Letters, 1 = Symbols
int currentRow = 0;
int currentCol = 0;
String typedText = "";

// Button state variables
bool lastRightState = HIGH;
bool lastUpState = HIGH;
unsigned long rightPressedTime = 0;
unsigned long upPressedTime = 0;
bool isRightLongPressed = false;
bool isUpLongPressed = false;

void setup() {
  Serial.begin(115200);

  // Initialize Pins with Internal Pull-ups
  pinMode(BUTTON_RIGHT_PIN, INPUT_PULLUP);
  pinMode(BUTTON_UP_PIN, INPUT_PULLUP);

  // Initialize OLED Display
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  
  // Back to default unrotated view orientation
  display.setRotation(0); 
  
  display.clearDisplay();
  updateDisplay();
}

void loop() {
  checkButtons();
}

void checkButtons() {
  bool currentRightState = digitalRead(BUTTON_RIGHT_PIN);
  bool currentUpState = digitalRead(BUTTON_UP_PIN);
  unsigned long now = millis();

  // --- RIGHT BUTTON LOGIC ---
  if (lastRightState == HIGH && currentRightState == LOW) {
    rightPressedTime = now;
    isRightLongPressed = false;
  } 
  else if (lastRightState == LOW && currentRightState == LOW) {
    if (!isRightLongPressed && (now - rightPressedTime >= LONG_PRESS_TIME)) {
      isRightLongPressed = true;
      selectCharacter(); // Select character on 2-second hold
    }
  } 
  else if (lastRightState == LOW && currentRightState == HIGH) {
    if (!isRightLongPressed) { 
      // Short press -> Move Right & Wrap around current row
      currentCol = (currentCol + 1) % COLS;
      updateDisplay();
    }
  }
  lastRightState = currentRightState;

  // --- UP BUTTON LOGIC ---
  if (lastUpState == HIGH && currentUpState == LOW) {
    upPressedTime = now;
    isUpLongPressed = false;
  } 
  else if (lastUpState == LOW && currentUpState == LOW) {
    if (!isUpLongPressed && (now - upPressedTime >= LONG_PRESS_TIME)) {
      isUpLongPressed = true;
      selectCharacter(); // Select character on 2-second hold
    }
  } 
  else if (lastUpState == LOW && currentUpState == HIGH) {
    if (!isUpLongPressed) {
      // Short press -> Move Up & Wrap around columns
      currentRow = (currentRow - 1 + ROWS) % ROWS;
      updateDisplay();
    }
  }
  lastUpState = currentUpState;
}

void selectCharacter() {
  // Get active layout mapping
  char selected = (currentPage == 0) ? pageLetters[currentRow][currentCol] : pageSymbols[currentRow][currentCol];
  
  if (selected == '>') {
    // Jump between Letter and Symbol modes
    currentPage = (currentPage == 0) ? 1 : 0;
    // Keep cursor on home cell so it doesn't clip out of boundary layouts
    currentRow = 0;
    currentCol = 0;
  } 
  else if (selected == '<') { 
    // Backspace function
    if (typedText.length() > 0) {
      typedText.remove(typedText.length() - 1);
    }
  } 
  else if (selected == '_') {
    // Empty padding slots do nothing
  } 
  else {
    typedText += selected; // Append standard layout entry or Space
  }
  
  updateDisplay();
  delay(400); // Visual pause to prevent immediate double-typing
}

void updateDisplay() {
  display.clearDisplay();

  // 1. Live Text Output Frame (Top Half)
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  
  if (typedText == "") {
    display.print("[Hold 2s to Type]");
  } else {
    display.print(typedText);
  }
  
  // Clean horizontal divider line
  display.drawLine(0, 22, 128, 22, SSD1306_WHITE);

  // 2. Keyboard Grid Matrix Layout (Bottom Half)
  int startY = 26;
  int cellWidth = 15;
  int cellHeight = 9;

  for (int r = 0; r < ROWS; r++) {
    for (int c = 0; c < COLS; c++) {
      char characterToPrint = (currentPage == 0) ? pageLetters[r][c] : pageSymbols[r][c];
      
      int x = c * cellWidth + 4;
      int y = r * cellHeight + startY;

      // High contrast text cursor selection block highlighting
      if (r == currentRow && c == currentCol) {
        display.fillRect(x - 1, y - 1, cellWidth - 1, cellHeight, SSD1306_WHITE);
        display.setTextColor(SSD1306_BLACK); 
      } else {
        display.setTextColor(SSD1306_WHITE);
      }

      display.setCursor(x, y);
      display.print(characterToPrint);
    }
  }
  display.display();
}