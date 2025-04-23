// TeenyGPSEmulator Arduino Sketch
// 2023-12-08 by Michael Beakes <beakes@bezwax.com>
//
// Changelog:
// 2023-03-17 - Initial release
//
// To-Do:
//

#include <M5Unified.hpp>

#include <Arduino.h>

/********************************************************************/
// Reset
void device_reset() {
  // send reboot command -----
  ESP.restart();
}

/********************************************************************/
// Prototypes
#include "prototypes.h"

/********************************************************************/
// RTC
#include "rtc.h"

/********************************************************************/
// GPS
HardwareSerial *gpsSerial;
#include "gps.h"

/********************************************************************/
// Compass
#include "compass.h"

/********************************************************************/
// Device State
#include "device_state.h"

/********************************************************************/
// Emulator
HardwareSerial *emulatorSerial;
#include "emulator.h"

/********************************************************************/
// SD Card
#include "sdcard.h"

/********************************************************************/
// UBX Logger
#include "logger.h"

/********************************************************************/
// Satellite Calibration
#include "calibration.h"

/********************************************************************/
// Display
#include "display_menu.h"
#include "msg.h"

/********************************************************************/
// Satellite Constellation
#include "constellation.h"

/********************************************************************/
// GNSS Utilites
#include "gnss.h"

/********************************************************************/
// Battery
#include "battery.h"

/********************************************************************/
// Buttons
#include "buttons.h"

/********************************************************************/
// Pulsed Outputs
//#include <TeenyPulser.h>
// Status LED Defines
//TeenyPulser statusLED(13, true);

/********************************************************************/
// Device Mode
#include "mode.h"

/********************************************************************/
// Periodic Timer
#include "timer.h"

/********************************************************************/
// Display Draw
#include "display_draw.h"


/********************************************************************/
void setup() {

  // setup M5
  //auto cfg = M5.config();
  //cfg.external_rtc  = true;  // default=false. use Unit RTC.
  //M5.begin(cfg);
  M5.begin();

  // setup statusLED
//  statusLED.init(20, 50, 4000);

  // Setup display
  display_setup();
  if(displayEnabled) {
    displayPV.prt_str("TeenyGPSTestbed_M5", 20, 0, 0);
    display_display();
    delay(200);
  }

  //Setup buttons
  if(displayEnabled) {
    buttons_setup();
    if(displayEnabled) {
      displayPV.prt_str("- Buttons Enabled", 20, 0, 16);
      display_display();
      delay(200);
    }
  } else {
    if(displayEnabled) {
      displayPV.prt_str("- Buttons Missing", 20, 0, 16);
      display_display();
      delay(200);
    }
  }

  //Setup rtc
  if(rtc.setup()) {
    if(displayEnabled) {
      displayPV.prt_str("- RTC Enabled", 20, 0, 32);
      display_display();
      delay(200);
    }
  } else {
    if(displayEnabled) {
      displayPV.prt_str("- RTC Missing", 20, 0, 32);
      display_display();
      delay(200);
    }
  }

  //Setup compass
  if(compass_setup()) {
    if(displayEnabled) {
      displayPV.prt_str("- Compass Enabled", 20, 0, 48);
      display_display();
      delay(200);
    }
  } else {
    if(displayEnabled) {
      displayPV.prt_str("- Compass Missing", 20, 0, 48);
      display_display();
      delay(200);
    }
  }

  //Setup sdcard
  if(sdcard_setup()) {
    if(displayEnabled) {
      displayPV.prt_str("- SD Card Enabled", 20, 0, 64);
      display_display();
      delay(200);
    }
  } else {
    if(displayEnabled) {
      displayPV.prt_str("- SD Card Missing", 20, 0, 64);
      display_display();
      delay(200);
    }
  }

  //Select device mode
  sdcard_deviceStateRestore();

  // Initalize device mode
  deviceMode_init();

  // setup timer
  itimer0_setup();

  if(displayEnabled) {
    displayPV.prt_str("Setup Complete", 20, 0, 112);
    display_display();
    delay(1000);
    display_clearDisplay();
    display_display();
  }

  // setup and init menu
  if(displayEnabled) {
    menu_setup(); // must be done after device_state_restore()
    menu_init();
    menu_enter();
  }

}


/********************************************************************/
void loop() {

  uint32_t _nowMS = millis();

  // Update clock
  rtc_datetime_t _rtcTime = rtc.getRTCTime(); // get the RTC
  uint32_t _clockTime = (uint32_t)(_rtcTime.hour*3600) + (uint32_t)(_rtcTime.minute*60) + _rtcTime.second;
  static uint32_t _prevClockTime = 86400; // This is 24hr rollover seconds so it will never match _clockTime
  bool _clockTick_1sec = false;
  if(_prevClockTime != _clockTime) {
    displayRefresh = true;
    _prevClockTime = _clockTime;
    _clockTick_1sec = true;
  }

  // Update device based on mode
  switch(deviceState.DEVICE_MODE) {
    case DM_GPSRCVR:
      if(gps.getNAVPVT()) {
        if((!rtc.isValid()) && gps.isDateValid() && gps.isTimeValid()) {
          rtc.setRTCTime(gps.getYear(), gps.getMonth(), gps.getDay(),
                         gps.getHour(), gps.getMinute(), gps.getSecond());
        }
        displayRefresh = true;
      }
      break;
    case DM_GPSLOGR:
      if(logger_update()) {
        displayRefresh = true;
      }
      break;
    case DM_GPSNSAT:
      if(gps.getNAVPVT()) {
        if((!rtc.isValid()) && gps.isDateValid() && gps.isTimeValid()) {
          rtc.setRTCTime(gps.getYear(), gps.getMonth(), gps.getDay(),
                         gps.getHour(), gps.getMinute(), gps.getSecond());
        }
        displayRefresh = true;
      } else if(gps.getNAVSAT()) {
        displayRefresh = true;
      }
      break;
    case DM_GPSSTAT:
      if(gps.getNAVPVT()) {
        if((!rtc.isValid()) && gps.isDateValid() && gps.isTimeValid()) {
          rtc.setRTCTime(gps.getYear(), gps.getMonth(), gps.getDay(),
                         gps.getHour(), gps.getMinute(), gps.getSecond());
        }
        displayRefresh = true;
      } else if(gps.getNAVSTATUS()) {
        displayRefresh = true;
      }
      break;
    case DM_GPSSCAL:
      if(gps.getNAVPVT()) {
        if((!rtc.isValid()) && gps.isDateValid() && gps.isTimeValid()) {
          rtc.setRTCTime(gps.getYear(), gps.getMonth(), gps.getDay(),
                         gps.getHour(), gps.getMinute(), gps.getSecond());
        }
        displayRefresh = true;
      } else if(gps.getNAVSAT()) {
        displayRefresh = true;
      } else if(gps.getNAVSTATUS()) {
        displayRefresh = true;
      }
      if(_clockTick_1sec) satCalibration_tick();
      break;
    case DM_GPSSCFG:
      break;
    case DM_GPSEMU_M8:
    case DM_GPSEMU_M10:
      if(emulator_update()) {
        displayRefresh = true;
      }
      break;
  }

  ////CHECKBUTTONACTIVITY////
  buttons_tick(); ////NOT ISR SAFE - Moved from itimer0_handler() to loop()////
  buttons_click();
  ////CHECKBUTTONACTIVITY////

  if(displayEnabled) {
    menu_idle_timer();
    display_refresh(); // refresh sprite buffer
    display_update();  // push sprite to display
  }

  // Check loop timing
  static uint32_t max_loop_check=_nowMS;
  static uint32_t min_loop_time=999;
  static uint32_t max_loop_time=0;
  static uint32_t sum_loop_time=0;
  static uint32_t sum_loop_count=0;
  if(deviceState.DEBUGLOOPTIMING) {
    if(_nowMS-max_loop_check > 4000) {
      max_loop_check = _nowMS;
      min_loop_time=999;
      max_loop_time = 0;
      sum_loop_time=0;
      sum_loop_count=0;
    }
    if(millis()-_nowMS > max_loop_time) {
      min_loop_time = min(min_loop_time, millis()-_nowMS);
      max_loop_time = max(max_loop_time, millis()-_nowMS);
      sum_loop_time += millis()-_nowMS;
      sum_loop_count++;
      char _tempStr[22];
      sprintf(_tempStr, "LoopTime %d/%d/%d",
              min_loop_time, sum_loop_time/sum_loop_count, max_loop_time);
      msg_update(_tempStr);
    }
  }

}

