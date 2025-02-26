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

// When defined, measures and displays max loop iteration time
//#define DEBUG_LOOP_TIMING
//#define DEBUG_LOOP_PERIOD 2500

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
// Emulator
HardwareSerial *emulatorSerial;
#include "emulator.h"

/********************************************************************/
// Device State
#include "device_state.h"

/********************************************************************/
// SD Card
#include "sdcard.h"

/********************************************************************/
// Display
#include "display_menu.h"
#include "msg.h"

/********************************************************************/
// Satellite Constellation
#include "constellation.h"

/********************************************************************/
// Satellite Calibration
#include "calibration.h"

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

#ifdef DEBUG_LOOP_TIMING
  static uint32_t max_loop_check=_nowMS;
  static uint32_t min_loop_time=999;
  static uint32_t max_loop_time=0;
  static uint32_t sum_loop_time=0;
  static uint32_t sum_loop_count=0;
#endif

  // Update clock
  rtc_datetime_t _rtcTime = rtc.getRTCTime(); // get the RTC
  uint32_t _clockTime = (uint32_t)(_rtcTime.hour*3600) + (uint32_t)(_rtcTime.minute*60) + _rtcTime.second;
  static uint32_t _prevClockTime = 86400; // This is 24hr rollover seconds so it will never match _clockTime
  bool _clockTick_1sec = false;
  static uint8_t _clockTick_1sec_count = 0;
  bool _clockTick_10sec = false;
  if(_prevClockTime != _clockTime) {
    displayRefresh = true;
    _prevClockTime = _clockTime;
    _clockTick_1sec = true;
    _clockTick_1sec_count++;
    if(_clockTick_1sec_count >= 10) {
      _clockTick_1sec_count = 0;
      _clockTick_10sec = true;
    }
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
      if(gps.getNAVPVT()) {
        if((!rtc.isValid()) && gps.isDateValid() && gps.isTimeValid()) {
          rtc.setRTCTime(gps.getYear(), gps.getMonth(), gps.getDay(),
                         gps.getHour(), gps.getMinute(), gps.getSecond());
        }
        if(ubxLoggingInProgress &&
           ((deviceState.UBXPKTLOGMODE == UBXPKTLOG_NAVPVT) ||
            (deviceState.UBXPKTLOGMODE == UBXPKTLOG_ALL))) {
          // UBX packet logging
          uint8_t navpvtPacket[UBX_NAV_PVT_PACKETLENGTH];
          gps.getNAVPVTPacket(navpvtPacket);
          sdcard_writeUBXLoggingFile(navpvtPacket, UBX_NAV_PVT_PACKETLENGTH);
          ubxLoggingFileWriteCount++;
          if(gps.isLocationValid()) ubxLoggingFileWriteValidCount++;
        }
        if(deviceState.GPSLOGMODE != GPSLOG_NONE) {
          // GPS track logging
          char _latStr[11];
          dtostrf(gps.getLatitude(), -9, 6, _latStr);
          char _lonStr[11];
          dtostrf(gps.getLongitude(), -9, 6, _lonStr);
          rtc_datetime_t dateTime;
          dateTime.year   = gps.getYear();
          dateTime.month  = gps.getMonth();
          dateTime.day    = gps.getDay();
          dateTime.hour   = gps.getHour();
          dateTime.minute = gps.getMinute();
          dateTime.second = gps.getSecond();
          char* _itdStr = rtc.dateTimeToISO8601Str(dateTime);
          // GPX track logging
          if(deviceState.GPSLOGMODE == GPSLOG_GPX) {
            // trkpt - "      <trkpt lat=\"45.4431641\" lon=\"-121.7295456\"><ele>122</ele><time>2001-06-02T00:18:15Z</time></trkpt>\n"
            char _trkptStr[256];
            sprintf(_trkptStr, "      <trkpt lat=\"%s\" lon=\"%s\"><ele>%d</ele><time>%sZ</time></trkpt>\n",
                    _latStr, _lonStr, gps.getAltitude(), _itdStr);
            sdcard_writeGPXLoggingFile((uint8_t*)_trkptStr, strlen(_trkptStr));
          }
          // KML track logging
          if(deviceState.GPSLOGMODE == GPSLOG_KML) {
            // trkpt - "      <when>2010-05-28T02:02:09Z</when>"
            // trkpt - "      <gx:coord>-122.207881 37.371915 156.000000</gx:coord>"
            char _trkptStr[256];
            sprintf(_trkptStr, "      <when>%sZ</when>\n", _itdStr);
            sdcard_writeKMLLoggingFile((uint8_t*)_trkptStr, strlen(_trkptStr));
            sprintf(_trkptStr, "      <gx:coord>%s %s %d.000000</gx:coord>\n",
                    _lonStr, _latStr, gps.getAltitude());
            sdcard_writeKMLLoggingFile((uint8_t*)_trkptStr, strlen(_trkptStr));
          }
        }
        displayRefresh = true;
      } else if(gps.getNAVSTATUS()) {
        if(ubxLoggingInProgress &&
           ((deviceState.UBXPKTLOGMODE == UBXPKTLOG_NAVSTATUS) ||
            (deviceState.UBXPKTLOGMODE == UBXPKTLOG_ALL))) {
          // UBX packet logging
          uint8_t navstatusPacket[UBX_NAV_STATUS_PACKETLENGTH];
          gps.getNAVSTATUSPacket(navstatusPacket);
          sdcard_writeUBXLoggingFile(navstatusPacket, UBX_NAV_STATUS_PACKETLENGTH);
          ubxLoggingFileWriteCount++;
          if(gps.isLocationValid()) ubxLoggingFileWriteValidCount++;
        }
        displayRefresh = true;
      } else if(gps.getNAVSAT()) {
        if(ubxLoggingInProgress &&
           ((deviceState.UBXPKTLOGMODE == UBXPKTLOG_NAVSAT) ||
            (deviceState.UBXPKTLOGMODE == UBXPKTLOG_ALL))) {
          // UBX packet logging
          uint8_t navsatPacket[UBX_NAV_SAT_MAXPACKETLENGTH];
          gps.getNAVSATPacket(navsatPacket);
          sdcard_writeUBXLoggingFile(navsatPacket, gps.getNAVSATPacketLength());
          ubxLoggingFileWriteCount++;
          if(gps.isLocationValid()) ubxLoggingFileWriteValidCount++;
        }
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
    case DM_GPSSCAL:
      if(gps.getNAVPVT()) {
        if((!rtc.isValid()) && gps.isDateValid() && gps.isTimeValid()) {
          rtc.setRTCTime(gps.getYear(), gps.getMonth(), gps.getDay(),
                         gps.getHour(), gps.getMinute(), gps.getSecond());
        }
        displayRefresh = true;
      } else if(gps.getNAVSTATUS()) {
        displayRefresh = true;
      } else if(gps.getNAVSAT()) {
        displayRefresh = true;
      }
      satCalibration_tick();
      break;
    case DM_GPSSCFG:
      break;
    case DM_GPSEMU_M8:
    case DM_GPSEMU_M10:
      if(emulatorEnabled) {
        // Process host commands
        emulator.processIncomingPacket();
//*** NEED TO INCORPORATE TRANSMISSION RATE INTO sendNAVPVTPacket()
//*** ALSO NEED TO FACTOR IN LOG RATE VS TRANSMISSION RATE
//uint32_t getNAVPVTTransmissionRate();
//uint32_t getNAVSATTransmissionRate();
        uint8_t _ubxNAVPVTBuf[100];
        ubxNAVPVTInfo_t _ubxNAVPVTInfo;
        // Wait for loop to be enabled by auto* command or packet request
        if(!emulatorLoopEnabled) {
          if(emulator.isAutoNAVPVTEnabled() ||
             emulator.isNAVPVTPacketRequested() ||
             emulator.isAutoNAVSATEnabled() ||
             emulator.isNAVSATPacketRequested()) {
            emulatorLoopEnabled = true;
          }
        }
        if(!emulatorLoopEnabled) break;
        // Update loop event every second
        if(_clockTick_1sec) {
          if(emulatorColdStartPacketCount < deviceState.EMUL_NUMCOLDSTARTPACKETS) {
            emulatorColdStartPacketCount++;
            emulator.setEmuColdOutputPackets(); // Sets cold NAVPVT, NAVSTATUS, and NAVSAT packets
            //statusLED.pulse(1);
          } else {
            emulator.setEmuLoopOutputPackets(); // Sets NAVPVT packet and possible adjacent NAVSAT packet
            if(!rtc.isValid()) {
              _ubxNAVPVTInfo = emulator.getNAVPVTPacketInfo();
              if(_ubxNAVPVTInfo.dateValid && _ubxNAVPVTInfo.timeValid) {
                rtc.setRTCTime(_ubxNAVPVTInfo.year, _ubxNAVPVTInfo.month, _ubxNAVPVTInfo.day,
                               _ubxNAVPVTInfo.hour, _ubxNAVPVTInfo.minute, _ubxNAVPVTInfo.second);
                _prevClockTime = (uint32_t)(_ubxNAVPVTInfo.hour*3600) +
                                 (uint32_t)(_ubxNAVPVTInfo.minute*60) +
                                 _ubxNAVPVTInfo.second;
              }
              _clockTick_1sec_count = 0;
              _clockTick_10sec = true;
              //statusLED.pulse(_ubxNAVPVTInfo.locationValid ? 2 : 1);
            } else {
              _ubxNAVPVTInfo = emulator.getNAVPVTPacketInfo();
              emulator.setNAVPVTPacketDateTime(_rtcTime.year, _rtcTime.month, _rtcTime.day,
                                               _rtcTime.hour, _rtcTime.minute, _rtcTime.second);
              //statusLED.pulse(_ubxNAVPVTInfo.locationValid ? 2 : 1);
            }
          }
        }
        if((_clockTick_1sec && emulator.isAutoNAVPVTEnabled()) ||
           emulator.isNAVPVTPacketRequested()) {
          emulator.sendNAVPVTPacket();
          displayRefresh = true;
        }
        if((_clockTick_1sec && emulator.isAutoNAVSTATUSEnabled()) ||
           emulator.isNAVSTATUSPacketRequested()) {
          emulator.sendNAVSTATUSPacket();
          displayRefresh = true;
        }
        if((_clockTick_10sec && emulator.isAutoNAVSATEnabled()) ||
           emulator.isNAVSATPacketRequested()) {
          emulator.sendNAVSATPacket();
          displayRefresh = true;
        }
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

#ifdef DEBUG_LOOP_TIMING
  if(_nowMS-max_loop_check > DEBUG_LOOP_PERIOD) {
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
#endif

}

