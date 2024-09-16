
// When defined, displays button actions
//#define DEBUG_BUTTON_PRESSES

/********************************************************************/
// Buttons
#define SHORT_PRESS_DURATION 10
#define LONG_PRESS_DURATION  350
struct ButtonState {
  bool currentlyPressed = false;
  bool shortPress = false;
  bool longPress = false;
};
ButtonState buttonA_state;
ButtonState buttonB_state;
ButtonState buttonC_state;
volatile bool buttonA_singleClickEvent;
volatile bool buttonA_longClickEvent;
volatile bool buttonA_doubleClickEvent;
volatile bool buttonB_singleClickEvent;
volatile bool buttonB_longClickEvent;
volatile bool buttonB_doubleClickEvent;
volatile bool buttonC_singleClickEvent;
volatile bool buttonC_longClickEvent;
volatile bool buttonC_doubleClickEvent;
volatile bool insideButtonsClick;
#ifdef DEBUG_BUTTON_PRESSES
volatile int16_t clickX;
volatile int16_t clickY;
#endif

/********************************************************************/
void buttonA_singleClick() {
  menuInputTime = millis();
  if(!menuDisplaySleepMode) {
    menu.registerKeyPress(TEENYMENU_KEY_DOWN);
#ifdef DEBUG_BUTTON_PRESSES
    static uint16_t count=0;
    char _tempStr[22];
    sprintf(_tempStr, "BtnA.SC.%d %d:%d", ++count, clickX, clickY);
    msg_update(_tempStr);
#endif
  }
  displayRefresh = true;
}
void buttonA_longClick() {
  menuInputTime = millis();
  if(!menuDisplaySleepMode) {
    menu.registerKeyPress(TEENYMENU_KEY_DOWN);
#ifdef DEBUG_BUTTON_PRESSES
    static uint16_t count=0;
    char _tempStr[22];
    sprintf(_tempStr, "BtnA.LC.%d %d:%d", ++count, clickX, clickY);
    msg_update(_tempStr);
#endif
  }
  displayRefresh = true;
}
void buttonA_doubleClick() {
  menuInputTime = millis();
  if(!menuDisplaySleepMode) {
#ifdef DEBUG_BUTTON_PRESSES
    static uint16_t count=0;
    char _tempStr[22];
    sprintf(_tempStr, "BtnA.DC.%d %d:%d", ++count, clickX, clickY);
    msg_update(_tempStr);
#endif
  }
  displayRefresh = true;
}
void buttonB_singleClick() {
  menuInputTime = millis();
  if(!menuDisplaySleepMode) {
    menu.registerKeyPress(TEENYMENU_KEY_RIGHT);
#ifdef DEBUG_BUTTON_PRESSES
    static uint16_t count=0;
    char _tempStr[22];
    sprintf(_tempStr, "BtnB.SC.%d %d:%d", ++count, clickX, clickY);
    msg_update(_tempStr);
#endif
  }
  displayRefresh = true;
}
void buttonB_longClick() {
  menuInputTime = millis();
  if(!menuDisplaySleepMode) {
    menu.registerKeyPress(TEENYMENU_KEY_LEFT);
#ifdef DEBUG_BUTTON_PRESSES
    static uint16_t count=0;
    char _tempStr[22];
    sprintf(_tempStr, "BtnB.LC.%d %d:%d", ++count, clickX, clickY);
    msg_update(_tempStr);
#endif
  }
  displayRefresh = true;
}
void buttonB_doubleClick() {
  menuInputTime = millis();
  if(!menuDisplaySleepMode) {
    menu.registerKeyPress(TEENYMENU_KEY_LEFT);
#ifdef DEBUG_BUTTON_PRESSES
    static uint16_t count=0;
    char _tempStr[22];
    sprintf(_tempStr, "BtnB.DC.%d %d:%d", ++count, clickX, clickY);
    msg_update(_tempStr);
#endif
  }
  displayRefresh = true;
}
void buttonC_singleClick() {
  menuInputTime = millis();
  if(!menuDisplaySleepMode) {
    menu.registerKeyPress(TEENYMENU_KEY_UP);
#ifdef DEBUG_BUTTON_PRESSES
    static uint16_t count=0;
    char _tempStr[22];
    sprintf(_tempStr, "BtnC.SC.%d %d:%d", ++count, clickX, clickY);
    msg_update(_tempStr);
#endif
  }
  displayRefresh = true;
}
void buttonC_longClick() {
  menuInputTime = millis();
  if(!menuDisplaySleepMode) {
    menu.registerKeyPress(TEENYMENU_KEY_UP);
#ifdef DEBUG_BUTTON_PRESSES
    static uint16_t count=0;
    char _tempStr[22];
    sprintf(_tempStr, "BtnC.LC.%d %d:%d", ++count, clickX, clickY);
    msg_update(_tempStr);
#endif
  }
  displayRefresh = true;
}
void buttonC_doubleClick() {
  menuInputTime = millis();
  if(!menuDisplaySleepMode) {
#ifdef DEBUG_BUTTON_PRESSES
    static uint16_t count=0;
    char _tempStr[22];
    sprintf(_tempStr, "BtnC.DC.%d %d:%d", ++count, clickX, clickY);
    msg_update(_tempStr);
#endif
  }
  displayRefresh = true;
}

/********************************************************************/
void buttons_tick() {
  if(insideButtonsClick) return;
  M5.update();
#ifdef DEBUG_BUTTON_PRESSES
  if(M5.Touch.ispressed()) {
    TouchPoint_t touchPt = M5.Touch.getPressPoint();
    if(touchPt.x >=0) {
      clickX = touchPt.x;
      clickY = touchPt.y;
    }
  }
#endif
  buttonA_state.longPress        = buttonA_state.currentlyPressed && M5.BtnA.wasReleasefor(LONG_PRESS_DURATION);
  buttonA_state.shortPress       = buttonA_state.currentlyPressed && M5.BtnA.wasReleasefor(SHORT_PRESS_DURATION) && !buttonA_state.longPress;
  buttonA_state.currentlyPressed = M5.BtnA.pressedFor(SHORT_PRESS_DURATION);
  if(buttonA_state.shortPress) buttonA_singleClickEvent=true;
  if(buttonA_state.longPress)  buttonA_longClickEvent=true;
  buttonB_state.longPress        = buttonB_state.currentlyPressed && M5.BtnB.wasReleasefor(LONG_PRESS_DURATION);
  buttonB_state.shortPress       = buttonB_state.currentlyPressed && M5.BtnB.wasReleasefor(SHORT_PRESS_DURATION) && !buttonB_state.longPress;
  buttonB_state.currentlyPressed = M5.BtnB.pressedFor(SHORT_PRESS_DURATION);
  if(buttonB_state.shortPress) buttonB_singleClickEvent=true;
  if(buttonB_state.longPress)  buttonB_longClickEvent=true;
  buttonC_state.longPress        = buttonC_state.currentlyPressed && M5.BtnC.wasReleasefor(LONG_PRESS_DURATION);
  buttonC_state.shortPress       = buttonC_state.currentlyPressed && M5.BtnC.wasReleasefor(SHORT_PRESS_DURATION) && !buttonC_state.longPress;
  buttonC_state.currentlyPressed = M5.BtnC.pressedFor(SHORT_PRESS_DURATION);
  if(buttonC_state.shortPress) buttonC_singleClickEvent=true;
  if(buttonC_state.longPress)  buttonC_longClickEvent=true;
}

/********************************************************************/
void buttons_click() {
  insideButtonsClick = true;
  if(buttonA_singleClickEvent) {
    buttonA_singleClick();
    buttonA_singleClickEvent = false;
  }
  if(buttonA_longClickEvent) {
    buttonA_longClick();
    buttonA_longClickEvent = false;
  }
  if(buttonA_doubleClickEvent) {
    buttonA_doubleClick();
    buttonA_doubleClickEvent = false;
  }
  if(buttonB_singleClickEvent) {
    buttonB_singleClick();
    buttonB_singleClickEvent = false;
  }
  if(buttonB_longClickEvent) {
    buttonB_longClick();
    buttonB_longClickEvent = false;
  }
  if(buttonB_doubleClickEvent) {
    buttonB_doubleClick();
    buttonB_doubleClickEvent = false;
  }
  if(buttonC_singleClickEvent) {
    buttonC_singleClick();
    buttonC_singleClickEvent = false;
  }
  if(buttonC_longClickEvent) {
    buttonC_longClick();
    buttonC_longClickEvent = false;
  }
  if(buttonC_doubleClickEvent) {
    buttonC_doubleClick();
    buttonC_doubleClickEvent = false;
  }
  insideButtonsClick = false; 
}

/********************************************************************/
void buttons_setup() {
  insideButtonsClick = false; 
  // Button locations
  //M5.BtnA.set( 10, 240, 110, 40); // M5Core2.h default for rotation=1
  //M5.BtnB.set(130, 240,  70, 40); // M5Core2.h default for rotation=1
  //M5.BtnC.set(230, 240,  80, 40); // M5Core2.h default for rotation=1
  //M5.BtnA.set(240,  10, 40, 110); // For rotation=2
  //M5.BtnB.set(240, 130, 40,  70); // For rotation=2
  //M5.BtnC.set(240, 230, 40,  80); // For rotation=2
  // Tuned using m5stackcore2_demo to identify off-screen touch coord.
  //M5.BtnA.set(230,  40, 50,  40); // Slight extension into display area
  //M5.BtnB.set(230, 120, 50,  80); // Slight extension into display area
  //M5.BtnC.set(230, 220, 50,  80); // Slight extension into display area
}

