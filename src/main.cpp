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

  // debug loop timing
  static uint32_t _startLoopUS    = micros();
  static uint32_t _max_loop_check = _startLoopUS;
  static uint32_t _min_loop_time  = UINT32_MAX;
  static uint32_t _max_loop_time  = 0;
  static uint32_t _sum_loop_time  = 0;
  static uint32_t _sum_loop_count = 0;
  if(deviceState.DEBUGLOOPTIMING) {
    // Check/Update loop timing window
    if(_startLoopUS - _max_loop_check > 4000000) {
      _max_loop_check = _startLoopUS;
      _min_loop_time  = UINT32_MAX;
      _max_loop_time  = 0;
      _sum_loop_time  = 0;
      _sum_loop_count = 0;
    }
    // Capture loop time
    uint32_t _endLoopUS = micros();
    uint32_t _loopTime  = _endLoopUS - _startLoopUS;
    bool _updateLoopMsg = (_sum_loop_count == 0) ||
                          (_loopTime < _min_loop_time) ||
                          (_loopTime > _max_loop_time);
    _min_loop_time = min(_min_loop_time, _loopTime);
    _max_loop_time = max(_max_loop_time, _loopTime);
    _sum_loop_time += _loopTime;
    _sum_loop_count++;
    if(_updateLoopMsg) {
      char _tempStr[22];
      sprintf(_tempStr, "LT %04d/%04d/%06d",
              min(_min_loop_time, 9999),
              min((_sum_loop_time / _sum_loop_count), 9999),
              min(_max_loop_time, 999999));
      msg_update(_tempStr);
    }
    // Set start time for next loop
    _startLoopUS = _endLoopUS;
  }

}

