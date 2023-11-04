#define menu_debug_print true // Debug messages will print to the Serial Monitor when this is 'true'

uint16_t defaultFontColor = TS_16b_White;
uint16_t defaultFontBG = TS_16b_Black;
uint16_t inactiveFontColor = TS_16b_Gray;
uint16_t inactiveFontBG = TS_16b_Black;

uint8_t menuHistory[5];
uint8_t menuHistoryIndex = 0;
uint8_t menuSelectionLineHistory[5];
int currentMenu = 0;
int currentMenuLine = 0;
int lastMenuLine = -1;
int currentSelectionLine = 0;
int lastSelectionLine = -1;

void (*menuHandler)(uint8_t) = NULL;
uint8_t (*buttonHandler)(uint8_t) = NULL;
uint8_t (*editorHandler)(uint8_t, int*, char*, void (*)()) = NULL;

// Use this struct outline to create menus
typedef struct
{
  const uint8_t amtLines;
  const char* const * strings;
  void (*selectionHandler)(uint8_t);
} menu_info;

// First example menu variables for 5 options
static const char PROGMEM exampleMenuStrings0[] = "NOT USED";
static const char PROGMEM exampleMenuStrings1[] = "Brightness";
static const char PROGMEM exampleMenuStrings2[] = "NOT USED";
static const char PROGMEM exampleMenuStrings3[] = "NOT USED";
static const char PROGMEM exampleMenuStrings4[] = "NOT USED";

static const char* const PROGMEM exampleMenuStrings[] =
{
  exampleMenuStrings0,
  exampleMenuStrings1,
  exampleMenuStrings2,
  exampleMenuStrings3,
  exampleMenuStrings4,
};

const menu_info exampleMenuInfo =
{
  5,
  exampleMenuStrings, 
  exampleMenu, 
};

static const char PROGMEM studyMenuStrings0[] = "Timer";
static const char PROGMEM studyMenuStrings1[] = "Pet";

static const char* const PROGMEM studyMenuStrings[] =
{
  studyMenuStrings0,
  studyMenuStrings1,
};

const menu_info studyMenuInfo =
{
  2,
  studyMenuStrings,
  studyMenu,
};

const menu_info menuList[] = {exampleMenuInfo, studyMenuInfo};
#define exampleMenuIndex 0
#define studyMenuIndex 1


bool needMenuDraw = true;

void buttonPress(uint8_t buttons) {
  if (currentDisplayState == displayStateHome) {
    if (buttons == viewButton) {
      menuHandler = viewMenu;
      newMenu(studyMenuIndex);    
      menuHandler(3);
      
    } else if (buttons == menuButton) {
      menuHandler = viewMenu;
      newMenu(exampleMenuIndex);
      menuHandler(0);
    }
  } else if (currentDisplayState == displayStateMenu || currentDisplayState == displayStateCalibration) {
    if (menuHandler) {
      menuHandler(buttons);
    }
  } else if (currentDisplayState == displayStateEditor) {
    if (editorHandler) {
      editorHandler(buttons, 0, 0, NULL);
    }
  } 

}

void newMenu(int8_t newIndex) {
  currentMenuLine = 0;
  lastMenuLine = -1;
  currentSelectionLine = 0;
  lastSelectionLine = -1;
  if (newIndex >= 0) {
    menuHistory[menuHistoryIndex++] = currentMenu;
    currentMenu = newIndex;
  } else {
    if (currentDisplayState == displayStateMenu) {
      menuHistoryIndex--;
      currentMenu = menuHistory[menuHistoryIndex];
    }
  }
  if (menuHistoryIndex) {
    currentDisplayState = displayStateMenu;
    currentSelectionLine = menuSelectionLineHistory[menuHistoryIndex];
  } else {
    if (menu_debug_print)SerialMonitorInterface.print("New menu index ");
    if (menu_debug_print)SerialMonitorInterface.println("home");
    menuSelectionLineHistory[menuHistoryIndex + 1] = 0;
    currentDisplayState = displayStateHome;
    initHomeScreen();
  }
}

int currentVal = 0;
int digits[4];
int currentDigit = 0;
int maxDigit = 4;
int *originalVal;
void (*editIntCallBack)() = NULL;

uint8_t buttonStuff(uint8_t button){
  buttonHandler = buttonStuff;
  if (button == backButton){
    viewMenu(backButton);
  }
}
uint8_t editInt(uint8_t button, int *inVal, char *intName, void (*cb)()) {
  //if (menu_debug_print)SerialMonitorInterface.println("editInt");
  if (!button) {
    //if (menu_debug_print)SerialMonitorInterface.println("editIntInit");
    editIntCallBack = cb;
    currentDisplayState = displayStateEditor;
    editorHandler = editInt;
    currentDigit = 0;
    originalVal = inVal;
    currentVal = *originalVal;
    digits[3] = currentVal % 10; currentVal /= 10;
    digits[2] = currentVal % 10; currentVal /= 10;
    digits[1] = currentVal % 10; currentVal /= 10;
    digits[0] = currentVal % 10;
    currentVal = *originalVal;

    displayBuffer.clearWindow(0, 8, 96, 64);
    writeArrows();

    displayBuffer.fontColor(defaultFontColor, defaultFontBG);
    int width = displayBuffer.getPrintWidth(intName);
    displayBuffer.setCursor(96 / 2 - width / 2, menuTextY[2]);
    displayBuffer.print(intName);

    displayBuffer.setCursor(8, 15 - 6);
    displayBuffer.print("Back");
    displayBuffer.setCursor(8, 45 + 6);
    displayBuffer.print("Next/Save");

    displayBuffer.drawLine(1, 14,    1, 12, 0xFFFF);
    displayBuffer.drawLine(1, 12,    6, 12, 0xFFFF);

    displayBuffer.drawLine(1, 54,    1, 56, 0xFFFF);
    displayBuffer.drawLine(1, 56,    6, 56, 0xFFFF);

  } else if (button == upButton) {
    if (digits[currentDigit] < 9)
      digits[currentDigit]++;
  } else if (button == downButton) {
    if (digits[currentDigit] > 0)
      digits[currentDigit]--;
  } else if (button == selectButton) {
    if (currentDigit < maxDigit - 1) {
      currentDigit++;
    } else {
      //save
      int newValue = (digits[3]) + (digits[2] * 10) + (digits[1] * 100) + (digits[0] * 1000);
      *originalVal = newValue;
      viewMenu(backButton);
      if (editIntCallBack) {
        editIntCallBack();
        editIntCallBack = NULL;
      }
      return 1;
    }
  } else if (button == backButton) {
    if (currentDigit > 0) {
      currentDigit--;
    } else {
      if (menu_debug_print)SerialMonitorInterface.println("back");
      viewMenu(backButton);
      return 0;
    }
  }

  displayBuffer.setCursor(96 / 2 - 16, menuTextY[3] + 3);
  for (uint8_t i = 0; i < 4; i++) {
    if (i != currentDigit)displayBuffer.fontColor(inactiveFontColor, defaultFontBG);
    displayBuffer.print(digits[i]);
    if (i != currentDigit)displayBuffer.fontColor(defaultFontColor, defaultFontBG);
  }
  displayBuffer.print(F("   "));
  displayBuffer.fontColor(0xFFFF, ALPHA_COLOR);
  return 0;
}


void printCenteredAt(int y, char * text) {
  int width = displayBuffer.getPrintWidth(text);
  //displayBuffer.clearWindow(96 / 2 - width / 2 - 1, y, width + 2, 8);
  displayBuffer.clearWindow(10, y, 96 - 20, 10);
  displayBuffer.setCursor(96 / 2 - width / 2, y);
  displayBuffer.print(text);
}

int tempOffset = 0;

void saveTempCalibration() {
  tempOffset = constrain(tempOffset,0,20);
//  writeSettings();
}

void exampleMenu(uint8_t selection) {
  if (selection == 0) {
    //newMenu(dateTimeMenuIndex);
  }
  if (selection == 1) {
    char buffer[20];
    strcpy_P(buffer, (PGM_P)pgm_read_word(&(menuList[exampleMenuIndex].strings[selection])));
    editInt(0, &brightness, buffer, setBrightnessCB);
  }
  if (selection == 2) {
    // other stuff
    clearScreen();
    startPong();
  }
  if (selection == 3) {
  }
  if (selection == 4) {
      // do something
  }
}
void setBrightnessCB(){
  brightness = constrain(brightness, 0, 15);
}

// PET SUBMENU MIGHT DELETE BUT KEEPING IT FOR NOW INCASE WE NEED IT FOR 
void petMenu(uint8_t selection) {
  SerialMonitorInterface.print("\nalways in\n\n");
  SerialMonitorInterface.print(selection);
  if (menu_debug_print)SerialMonitorInterface.println("petHandler");
  if (selection == 0) {
    // This option is eat under pet submenu
  }
  if (selection == 1) {
    // This option is Play under pet submenu
    
  }
  if (selection == 2) {
    // This option is shit under pet submenu
  }
}
void studyMenu(uint8_t selection) {
  //if (menu_debug_print)SerialMonitorInterface.println("studyMenuHandler");
  if (selection == 0) {
    nameTag(0);
  }
  if (selection == 1) {
    display.clearWindow(0,0,96,64);
    drawBitmap();
    display.setBitDepth(1);
    display.setFlip(true);
  }

}
int changeDir;
int changeEnd;
int changeStart;
int yChange;

void drawMenu() {
  //for (int yChange = changeStart; yChange != changeEnd; yChange += changeDir) {
  if (needMenuDraw) {
    needMenuDraw = false;
    displayBuffer.clearWindow(0, 7, 96, 56);
    for (int i = 0; i < menuList[currentMenu].amtLines; i++) {
      if (i == currentSelectionLine) {
        displayBuffer.fontColor(defaultFontColor, ALPHA_COLOR);
      } else {
        displayBuffer.fontColor(inactiveFontColor, ALPHA_COLOR);
      }
      char buffer[20];
      strcpy_P(buffer, (PGM_P)pgm_read_word(&(menuList[currentMenu].strings[currentMenuLine + i])));
      int width = displayBuffer.getPrintWidth(buffer);
      displayBuffer.setCursor(96 / 2 - width / 2, menuTextY[i] + 5 + yChange - (currentSelectionLine * 8) + 16);
      displayBuffer.print(buffer);
    }

    writeArrows();
  }
  if (yChange != changeEnd) {
    if (abs(yChange - changeEnd) > 5) {
      yChange += changeDir;
    }
    if (abs(yChange - changeEnd) > 3) {
      yChange += changeDir;
    }
    yChange += changeDir;
    needMenuDraw = true;
  }
  displayBuffer.fontColor(0xFFFF, ALPHA_COLOR);
}



void viewMenu(uint8_t button) {
  //if (menu_debug_print)SerialMonitorInterface.print("viewMenu ");
  //if (menu_debug_print)SerialMonitorInterface.println(button);
  if (!button) {

  } else {
    if (button == upButton) {
      if (currentSelectionLine > 0) {
        currentSelectionLine--;
      }
    } else if (button == downButton) {
      
      if (currentSelectionLine < menuList[currentMenu].amtLines - 1) {
        currentSelectionLine++;
      }
    } else if (button == selectButton) {
      //if (menu_debug_print)SerialMonitorInterface.print("select ");
      //if (menu_debug_print)SerialMonitorInterface.println(currentMenuLine + currentSelectionLine);
      menuList[currentMenu].selectionHandler(currentMenuLine + currentSelectionLine);
    } else if (button == backButton) {
      newMenu(-1);
      if (!menuHistoryIndex)
        return;
    }
  }
  if (lastMenuLine != currentMenuLine || lastSelectionLine != currentSelectionLine) {
    if (menu_debug_print)SerialMonitorInterface.println("drawing menu ");
    if (menu_debug_print)SerialMonitorInterface.println(currentMenu);


    if (currentSelectionLine < lastSelectionLine) {
      changeDir = 1;
      changeEnd = 0;
      changeStart = -7;
    } else {
      changeDir = -1;
      changeEnd = 0;
      changeStart = 7;
    }
    if (lastSelectionLine == -1) {
      changeStart = changeEnd; //new menu, just draw once
    }
    yChange = changeStart;
    needMenuDraw = true;
  }
  lastMenuLine = currentMenuLine;
  lastSelectionLine = currentSelectionLine;
  menuSelectionLineHistory[menuHistoryIndex] = currentSelectionLine;


  displayBuffer.fontColor(0xFFFF, ALPHA_COLOR);
}


void writeArrows() {
  leftArrow(0, 15 + 2);
  rightArrow(0, 45 + 5);

  upArrow(90, 15 + 2);
  downArrow(90, 45 + 4);
}

void backArrow(){
  leftArrow(0, 15 + 2);
}


void leftArrow(int x, int y) {
  displayBuffer.drawLine(x + 2, y - 2, x + 2, y + 2, 0xFFFF);
  displayBuffer.drawLine(x + 1, y - 1, x + 1, y + 1, 0xFFFF);
  displayBuffer.drawLine(x + 0, y - 0, x + 0, y + 0, 0xFFFF);
}

void rightArrow(int x, int y) {
  displayBuffer.drawLine(x + 0, y - 2, x + 0, y + 2, 0xFFFF);
  displayBuffer.drawLine(x + 1, y - 1, x + 1, y + 1, 0xFFFF);
  displayBuffer.drawLine(x + 2, y - 0, x + 2, y + 0, 0xFFFF);
}

void upArrow(int x, int y) {
  displayBuffer.drawLine(x + 0, y - 0, x + 4, y - 0, 0xFFFF);
  displayBuffer.drawLine(x + 1, y - 1, x + 3, y - 1, 0xFFFF);
  displayBuffer.drawLine(x + 2, y - 2, x + 2, y - 2, 0xFFFF);
}
void downArrow(int x, int y) {
  displayBuffer.drawLine(x + 0, y + 0, x + 4, y + 0, 0xFFFF);
  displayBuffer.drawLine(x + 1, y + 1, x + 3, y + 1, 0xFFFF);
  displayBuffer.drawLine(x + 2, y + 2, x + 2, y + 2, 0xFFFF);
}
