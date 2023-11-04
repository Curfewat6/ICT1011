#include <Wire.h>
#include <RTCZero.h>
#include <TinyScreen.h>
#include <GraphicsBuffer.h>
#include <TimeLib.h>
#include <SPI.h>
#include "BMA250.h"
#include "bar.h"

BMA250 accel_sensor;
int x, y, z;
int lastX, lastY, lastZ;
int initialX, initialY, initialZ;
const int SAFE_ZONE_RANGE = 200; // Define the range for the safe zone
unsigned long lastMovementTime = 0;
const int NO_MOVEMENT_THRESHOLD = 10;
const unsigned long NO_MOVEMENT_DURATION = 3000; // 15 seconds
bool initialRead = true; // Flag to check if initial reading is set
double temp;
float hp = 80.0;
char timeString[9];
char dateString[11];
GraphicsBuffer displayBuffer = GraphicsBuffer(96, 64, colorDepth16BPP);

TinyScreen display = TinyScreen(TinyScreenDefault);


RTCZero rtc;

uint8_t menuTextY[8] = {1 * 8 - 1, 2 * 8 - 1, 3 * 8 - 1, 4 * 8 - 1, 5 * 8 - 1, 6 * 8 - 1, 7 * 8 - 3, 8 * 8 - 3};
unsigned long previousMillis = 0;  

// constants won't change :
const long interval = 10000;           // interval at which to blink (milliseconds)
// You can change buttons here, but you will need to change the arrow graphics elsewhere in the program to
// match what you change here 
const uint8_t upButton     = TSButtonUpperRight;
const uint8_t downButton   = TSButtonLowerRight;
const uint8_t viewButton   = TSButtonLowerRight;
const uint8_t clearButton  = TSButtonLowerRight;
const uint8_t backButton   = TSButtonUpperLeft;
const uint8_t selectButton = TSButtonLowerLeft;
const uint8_t menuButton   = TSButtonLowerLeft;

unsigned long sleepTimer = 0;
int sleepTimeout = 0;

volatile uint32_t counter = 0;

uint32_t lastPrint = 0;

uint8_t displayOn = 0;
uint8_t buttonReleased = 1;
uint8_t rewriteMenu = false;
unsigned long mainDisplayUpdateInterval = 20;
unsigned long lastMainDisplayUpdate = 0;

int brightness = 8;
uint8_t lastSetBrightness = 7;

const uint8_t displayStateHome = 0x01;
const uint8_t displayStateMenu = 0x02;
const uint8_t displayStateEditor = 0x03;
const uint8_t displayStateCalibration = 0x04;
const uint8_t displayTimer = 0x05;
const uint8_t displayPong = 0x06;

/////////////////////////////////////////////////HP BAR///////////////////////////////////////////
PercentBarHorizontal terriblebar(display, 2, 28, 94, 14, TS_8b_Gray, TS_8b_DarkRed, TS_8b_White, 0, false);
PercentBarHorizontal poorbar(display, 2, 28, 94, 14, TS_8b_Gray, TS_8b_Red, TS_8b_White, 0, false);
PercentBarHorizontal averagebar(display, 2, 28, 94, 14, TS_8b_Gray, TS_8b_Yellow, TS_8b_White, 0, false);
PercentBarHorizontal goodbar(display, 2, 28, 94, 14, TS_8b_Gray, TS_8b_Green, TS_8b_White, 0, false);
PercentBarHorizontal excellentbar(display, 2, 28, 94, 14, TS_8b_Gray, TS_8b_DarkGreen, TS_8b_White, 0, false);
///////////////////////////////////////////////////////////////////////////////////////////////
uint8_t currentDisplayState = displayStateHome;
/////////////////////////////////////////////////PONG WIP/////////////////////////////////////////
#define	BLACK           0x00
#define	BLUE            0xE0
#define	RED             0x03
#define	GREEN           0x1C
#define WHITE           0xFF
#define	GREY            0x6D

// Constant variables for the paddles
//
int kPaddleStep = 1;
int kLoopDelay = 0;
int kPaddleWidth = 2;
int kPaddleHeight = 15;
int kStickSensitivity = 100;

int kScreenTop = 0;
int kScreenLeft = 0;
int kScreenWidth = 96;
int kScreenHeight = 64;
int kScreenRight = kScreenWidth-1;
int kScreenBottom = kScreenHeight-1;

//Paddle Location
//
int kLPaddleX = 10;
int kRPaddleX = kScreenRight - kLPaddleX;

//Button Variables (Top 4 are x and y axis of joysticks)
int RX=0;
int RY=0;
int LX=0;
int LY=0;
byte leftButton=0;
byte rightButton=0;

//Paddle Variable Values
//
int paddleX = 10;
int lPaddleY = 10;
int rPaddleY = 10;


// Constant variables for the ball.
//
int kBallWidth = 4;
int kBallHeight = 4;
int ballX = 48;
int ballY = 32;
int bMovementX = 1;
int bMovementY = 1;
int kRight = 1;
int kLeft = -1;

//Score Values
int leftPlayerScore  = 0;
int rightPlayerScore = 0;

//Goal Start and end positions for the 3 modes of pong
int kTennisGoalUpper   = 0;
int kTennisGoalLower   = 64;

int kHandballGoalUpper = 0;
int kHandballGoalLower = 64;

int kHockeyGoalUpper   = 22;
int kHockeyGoalLower   = 42;


//Three modes of pong
enum { kTennisMode = 0, kHandballMode, kHockeyMode , kMaxModes };
int mode = kHockeyMode;

//Initial value of button before being pressed
int preLeftButton  = 0;
int preRightButton = 0;

//Pausing booleans
 bool prevPaused = false;
 bool paused     = false;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Make Serial Monitor compatible for all TinyCircuits processors
#if defined(ARDUINO_ARCH_AVR)
  #define SerialMonitorInterface Serial
#elif defined(ARDUINO_ARCH_SAMD)
  #define SerialMonitorInterface SerialUSB
#endif

void setup() {
  Wire.begin();
  SerialMonitorInterface.begin(200000);
  delay(100);
  rtc.begin();
  // ///////////////////////////////////////////////////////////////////////////////////////////////////////////// UPDATE THIS ON THE DAY OF DEMO AND VIDEO
  rtc.setTime(10, 38, 0); // Set the time: 12:00:00 (hh:mm:ss)
  rtc.setDate(4, 11, 2023); // Set the date: 1 January 2023
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////// DONT FORGET////////////////////////////////
  display.begin();
  display.setFlip(true);
  display.setBitDepth(1);
  display.initDMA();  
  accel_sensor.begin(BMA250_range_2g, BMA250_update_time_64ms); //added shit
  if (displayBuffer.begin()) {
    //memory allocation error- buffer too big
    while (1) {
      SerialMonitorInterface.println("Display buffer memory allocation error!");
      delay(1000);
    }
  }
  displayBuffer.setFont(thinPixel7_10ptFontInfo);
  displayBuffer.clear();
  initHomeScreen();
  requestScreenOn();
  //terriblebar.updateBarEndInfo();
}

void loop() {
  displayOn = 1;
  accel_sensor.read(); //read accel
  x= accel_sensor.X;
  y = accel_sensor.Y;
  z = accel_sensor.Z;
  temp = ((accel_sensor.rawTemp * 0.5) + 24.0);

  byte hours = rtc.getHours();
  byte minutes = rtc.getMinutes();

  byte day = rtc.getDay();
  byte month = rtc.getMonth();
  byte year = rtc.getYear();

  sprintf(timeString, "%02d:%02d", hours, minutes);
  sprintf(dateString, "%02d/%02d/%02d", day, month, year-16);
  if(currentDisplayState == displayPong){
    if ( display.getButtons(TSButtonUpperLeft)){
      newMenu(3);
    }
    startPong();  
  }
  if (displayOn && (millisOffset() > mainDisplayUpdateInterval + lastMainDisplayUpdate)) {
    lastMainDisplayUpdate = millis();
    display.endTransfer();
    display.goTo(0, 0);
    display.startData();
    display.writeBufferDMA(displayBuffer.getBuffer(), displayBuffer.getBufferSize());
  }

  if (currentDisplayState == displayStateHome) {
    if ( display.getButtons(TSButtonLowerRight) || display.getButtons(TSButtonUpperRight) || display.getButtons(TSButtonLowerLeft) || display.getButtons(TSButtonUpperLeft)){
      displayBuffer.clear();  // if a button is pressed, clear the screen
    }

    updateMainDisplay(); 
    liveDisplay(y); // This is the main home screen display
    display.writeBufferDMA(displayBuffer.getBuffer(), displayBuffer.getBufferSize());
    delay (20); // Making this smaller will make the screen more scattered, making it longer will mean you need to hold in buttons longer
  } else {
    drawMenu();
  }
  if (currentDisplayState == displayTimer){
    if ( display.getButtons(TSButtonUpperLeft)){
      newMenu(3);
    }
    testing();
  }
  checkButtons();
  updateMainDisplay();
  if (initialRead) {
    initialX = x;
    initialY = y;
    initialZ = z;
    initialRead = false; // Ensure this block doesn't run again
    lastMovementTime = millis(); // Reset the last movement time
  }
  // Check for no movement
  if(abs(x - lastX) < NO_MOVEMENT_THRESHOLD && abs(y - lastY) < NO_MOVEMENT_THRESHOLD && abs(z - lastZ) < NO_MOVEMENT_THRESHOLD) {
    if(millis() - lastMovementTime > NO_MOVEMENT_DURATION) {
      SerialMonitorInterface.println("No movement detected for 15 seconds.");
      hp -= 10.0;
      //updateDisplay("why r u not moving");
      lastMovementTime = millis();
    }
  } else {
    lastMovementTime = millis();
  }

  // Update last known position
  lastX = x;
  lastY = y;
  lastZ = z;

  if (x < initialX - SAFE_ZONE_RANGE || x > initialX + SAFE_ZONE_RANGE ||
      y < initialY - SAFE_ZONE_RANGE || y > initialY + SAFE_ZONE_RANGE ||
      z < initialZ - SAFE_ZONE_RANGE || z > initialZ + SAFE_ZONE_RANGE) {
    SerialMonitorInterface.println("The device has exited the safe zone.");
    //updateDisplay("get back in wtf");
  }
}

uint32_t millisOffset() {
  return millis();
}

uint32_t getSecondsCounter() {
  return millis()/1000;
}

int requestScreenOn() {
  sleepTimer = millisOffset();
  if (!displayOn) {
    setTime(counter);
    displayOn = 1;
    updateMainDisplay();
    return 1;
  }
  return 0;
}

void checkButtons() {
  byte buttons = display.getButtons();
  if (buttonReleased && buttons) {
    if (displayOn) {
      buttonPress(buttons);
    }
    requestScreenOn();
    buttonReleased = 0;
  }
  if (!buttonReleased && !(buttons & 0x1F)) {
    buttonReleased = 1;
  }
}

void initHomeScreen() {
  displayBuffer.clearWindow(0, 1, 96, 5);
  updateMainDisplay();
}

void updateMainDisplay() {
  if (lastSetBrightness != brightness) {
    display.setBrightness(brightness);
    lastSetBrightness = brightness;
  }
  updateDateTimeDisplay();
  if (currentDisplayState == displayStateHome) {
    displayBuffer.setCursor(9, menuTextY[6]);
    displayBuffer.print("Menu");
    displayBuffer.setCursor(60, menuTextY[6]);
    displayBuffer.print("Study");
    
    leftArrow(0, 57);
    rightArrow(90, 57);
    rewriteMenu = false;
  }
}


void updateDateTimeDisplay() {
  displayBuffer.clearWindow(0, 0, 96, 8);

  displayBuffer.setCursor(0, -1);
  displayBuffer.print(dateString);

  displayBuffer.setCursor(70, -1);
  displayBuffer.print(timeString);
}

void liveDisplay(int movement) {
  unsigned long currentMillis = millis();
  pBar();
  //displayBuffer.setCursor(0, menuTextY[1]);
  //displayBuffer.print("Mango Watch");
  if (movement > 100){
    //displayBuffer.setCursor(0, menuTextY[4]);
    //displayBuffer.print("Why r u running?");
   // displayBuffer.print("");
  }
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    displayBuffer.clearWindow(0, 39,96,10);   
  }
}
void pBar(){
  displayBuffer.clearWindow(0, 25, 96, 14);
  displayBuffer.setCursor(2,16);
  displayBuffer.print("Pet Hp");
  hp /= 100;
  if(hp*100 < 20) // terrible
  {
     terriblebar.tick(hp);
  }
  else if(hp*100 < 40 && hp*100 >=20) // poor
  {
     poorbar.tick(hp);
  }
  else if(hp*100 < 60 && hp*100 >=40) // average
  {
     averagebar.tick(hp);
  }
 else if(hp*100 < 80 && hp*100 >=60) // good
  {
     goodbar.tick(hp);
  }
 else //(hp*100 >=80) excellent
  {
     excellentbar.tick(hp);
  }
  hp *= 100;
  delay(20); // How often values are updated and printed  
}


  
