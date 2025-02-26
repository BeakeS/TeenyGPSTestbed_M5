
// When defined, displays button actions
//#define DEBUG_BUTTON_PRESSES

/********************************************************************/
// Buttons
#define SHORT_PRESS_DURATION 30
#define LONG_PRESS_DURATION  350
struct ButtonState {
  bool wasPressed = false;
  bool singleClickEvent = false;
  bool longClickEvent = false;
  bool doubleClickEvent = false;
};
volatile ButtonState buttonA_state;
volatile ButtonState buttonB_state;
volatile ButtonState buttonC_state;
volatile bool insideButtonsClick;
#ifdef DEBUG_BUTTON_PRESSES
volatile int16_t clickX;
volatile int16_t clickY;
#endif

/********************************************************************/
void buttonA_singleClick() {
  menuInputTime = millis();
  if(!menuDisplaySleepMode) {
#ifdef DEBUG_BUTTON_PRESSES
    static uint16_t count=0;
    char _tempStr[22];
    sprintf(_tempStr, "A.SC.%d %d:%d", ++count, clickX, clickY);
    msg_update(_tempStr);
#endif
    menu.registerKeyPress(TEENYMENU_KEY_DOWN);
  }
  displayRefresh = true;
}
void buttonA_longClick() {
  menuInputTime = millis();
  if(!menuDisplaySleepMode) {
#ifdef DEBUG_BUTTON_PRESSES
    static uint16_t count=0;
    char _tempStr[22];
    sprintf(_tempStr, "A.LC.%d %d:%d", ++count, clickX, clickY);
    msg_update(_tempStr);
#endif
    menu.registerKeyPress(TEENYMENU_KEY_DOWN);
  }
  displayRefresh = true;
}
void buttonA_doubleClick() {
  menuInputTime = millis();
  if(!menuDisplaySleepMode) {
#ifdef DEBUG_BUTTON_PRESSES
    static uint16_t count=0;
    char _tempStr[22];
    sprintf(_tempStr, "A.DC.%d %d:%d", ++count, clickX, clickY);
    msg_update(_tempStr);
#endif
  }
  displayRefresh = true;
}
void buttonB_singleClick() {
  menuInputTime = millis();
  if(!menuDisplaySleepMode) {
#ifdef DEBUG_BUTTON_PRESSES
    static uint16_t count=0;
    char _tempStr[22];
    sprintf(_tempStr, "B.SC.%d %d:%d", ++count, clickX, clickY);
    msg_update(_tempStr);
#endif
    menu.registerKeyPress(TEENYMENU_KEY_RIGHT);
  }
  displayRefresh = true;
}
void buttonB_longClick() {
  menuInputTime = millis();
  if(!menuDisplaySleepMode) {
#ifdef DEBUG_BUTTON_PRESSES
    static uint16_t count=0;
    char _tempStr[22];
    sprintf(_tempStr, "B.LC.%d %d:%d", ++count, clickX, clickY);
    msg_update(_tempStr);
#endif
    menu.registerKeyPress(TEENYMENU_KEY_LEFT);
  }
  displayRefresh = true;
}
void buttonB_doubleClick() {
  menuInputTime = millis();
  if(!menuDisplaySleepMode) {
#ifdef DEBUG_BUTTON_PRESSES
    static uint16_t count=0;
    char _tempStr[22];
    sprintf(_tempStr, "B.DC.%d %d:%d", ++count, clickX, clickY);
    msg_update(_tempStr);
#endif
    menu.registerKeyPress(TEENYMENU_KEY_LEFT);
  }
  displayRefresh = true;
}
void buttonC_singleClick() {
  menuInputTime = millis();
  if(!menuDisplaySleepMode) {
#ifdef DEBUG_BUTTON_PRESSES
    static uint16_t count=0;
    char _tempStr[22];
    sprintf(_tempStr, "C.SC.%d %d:%d", ++count, clickX, clickY);
    msg_update(_tempStr);
#endif
    menu.registerKeyPress(TEENYMENU_KEY_UP);
  }
  displayRefresh = true;
}
void buttonC_longClick() {
  menuInputTime = millis();
  if(!menuDisplaySleepMode) {
#ifdef DEBUG_BUTTON_PRESSES
    static uint16_t count=0;
    char _tempStr[22];
    sprintf(_tempStr, "C.LC.%d %d:%d", ++count, clickX, clickY);
    msg_update(_tempStr);
#endif
    menu.registerKeyPress(TEENYMENU_KEY_UP);
  }
  displayRefresh = true;
}
void buttonC_doubleClick() {
  menuInputTime = millis();
  if(!menuDisplaySleepMode) {
#ifdef DEBUG_BUTTON_PRESSES
    static uint16_t count=0;
    char _tempStr[22];
    sprintf(_tempStr, "C.DC.%d %d:%d", ++count, clickX, clickY);
    msg_update(_tempStr);
#endif
  }
  displayRefresh = true;
}

/********************************************************************/
void buttons_tick() {
  M5.update();
  if(insideButtonsClick) return;
#ifdef DEBUG_BUTTON_PRESSES
  if(M5.Touch.getCount() > 0) {
    m5::touch_detail_t touchDetail = M5.Touch.getDetail(1);
    if(touchDetail.x >=0) {
      clickX = touchDetail.x;
      clickY = touchDetail.y;
    }
  }
#endif
  if(buttonA_state.wasPressed &&
     !(buttonA_state.singleClickEvent || buttonA_state.longClickEvent)) {
    if(M5.BtnA.wasReleaseFor(LONG_PRESS_DURATION)) {
      buttonA_state.longClickEvent = true;
    } else if(M5.BtnA.wasReleaseFor(SHORT_PRESS_DURATION)) {
      buttonA_state.singleClickEvent = true;
    }
  }
  buttonA_state.wasPressed = M5.BtnA.pressedFor(SHORT_PRESS_DURATION);
  if(buttonB_state.wasPressed &&
     !(buttonB_state.singleClickEvent || buttonB_state.longClickEvent)) {
    if(M5.BtnB.wasReleaseFor(LONG_PRESS_DURATION)) {
      buttonB_state.longClickEvent = true;
    } else if(M5.BtnB.wasReleaseFor(SHORT_PRESS_DURATION)) {
      buttonB_state.singleClickEvent = true;
    }
  }
  buttonB_state.wasPressed = M5.BtnB.pressedFor(SHORT_PRESS_DURATION);
  if(buttonC_state.wasPressed &&
     !(buttonC_state.singleClickEvent || buttonC_state.longClickEvent)) {
    if(M5.BtnC.wasReleaseFor(LONG_PRESS_DURATION)) {
      buttonC_state.longClickEvent = true;
    } else if(M5.BtnC.wasReleaseFor(SHORT_PRESS_DURATION)) {
      buttonC_state.singleClickEvent = true;
    }
  }
  buttonC_state.wasPressed = M5.BtnC.pressedFor(SHORT_PRESS_DURATION);
}

/********************************************************************/
void buttons_click() {
  insideButtonsClick = true;
  if(buttonA_state.singleClickEvent) {
    buttonA_singleClick();
    buttonA_state.singleClickEvent = false;
    insideButtonsClick = false;
    return;
  }
  if(buttonA_state.longClickEvent) {
    buttonA_longClick();
    buttonA_state.longClickEvent = false;
    insideButtonsClick = false;
    return;
  }
  if(buttonA_state.doubleClickEvent) {
    buttonA_doubleClick();
    buttonA_state.doubleClickEvent = false;
    insideButtonsClick = false;
    return;
  }
  if(buttonB_state.singleClickEvent) {
    buttonB_singleClick();
    buttonB_state.singleClickEvent = false;
    insideButtonsClick = false;
    return;
  }
  if(buttonB_state.longClickEvent) {
    buttonB_longClick();
    buttonB_state.longClickEvent = false;
    insideButtonsClick = false;
    return;
  }
  if(buttonB_state.doubleClickEvent) {
    buttonB_doubleClick();
    buttonB_state.doubleClickEvent = false;
    insideButtonsClick = false;
    return;
  }
  if(buttonC_state.singleClickEvent) {
    buttonC_singleClick();
    buttonC_state.singleClickEvent = false;
    insideButtonsClick = false;
    return;
  }
  if(buttonC_state.longClickEvent) {
    buttonC_longClick();
    buttonC_state.longClickEvent = false;
    insideButtonsClick = false;
    return;
  }
  if(buttonC_state.doubleClickEvent) {
    buttonC_doubleClick();
    buttonC_state.doubleClickEvent = false;
    insideButtonsClick = false;
    return;
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

