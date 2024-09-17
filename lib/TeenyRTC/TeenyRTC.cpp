/*
TeenyRTC - a small library for Arduino providing RTC support for multiple platforms

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <Arduino.h>
#include "TeenyRTC.h"


/********************************************************************/
TeenyRTC::TeenyRTC() { }

/********************************************************************/
void TeenyRTC::setValid(bool valid) {
  _valid = valid;
}
bool TeenyRTC::isValid() {
  return _valid;
}

/********************************************************************/
// Check that dateTime is valid and in range
// Minimum uint32_t dateTime is 1970-01-01T00:00:00Z
// Maximum int32_t dateTime is 2038-01-19T03:14:07Z
bool TeenyRTC::isValidDateTime(rtc_datetime_t dateTime) {
  if((dateTime.year<1970) || (dateTime.year>2037)) return false;
  if((dateTime.month<1) || (dateTime.month>12)) return false;
  uint8_t maxDay;
  if((dateTime.month == 4) || (dateTime.month == 6) ||
     (dateTime.month == 9) || (dateTime.month == 11)) {
    maxDay = 30;
  } else if(dateTime.month == 2) {
    if((dateTime.year % 4) == 0) {
      maxDay = 29;
    } else {
      maxDay = 28;
    }
  } else {
    maxDay = 31;
  }
  if((dateTime.day<1) || (dateTime.day>maxDay)) return false;
  if(dateTime.hour>23) return false;
  if(dateTime.minute>59) return false;
  if(dateTime.second>59) return false;
  return true;
}

/********************************************************************/
// Convert unixTime to dateTime
rtc_datetime_t TeenyRTC::unixTimeToDateTime(uint32_t unixTime) {
  uint32_t _unixTime = unixTime;
  rtc_datetime_t _dateTime;
  _dateTime.second = _unixTime % 60ul;
  _unixTime /= 60ul;
  _dateTime.minute = _unixTime % 60ul;
  _unixTime /= 60ul;
  _dateTime.hour = _unixTime % 24ul;
  _unixTime /= 24ul;
  uint32_t z = _unixTime + 719468;
  uint8_t era = z / 146097ul;
  uint16_t doe = z - era * 146097ul;
  uint16_t yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
  uint16_t y = yoe + era * 400;
  uint16_t doy = doe - (yoe * 365 + yoe / 4 - yoe / 100);
  uint16_t mp = (doy * 5 + 2) / 153;
  _dateTime.day = doy - (mp * 153 + 2) / 5 + 1;
  _dateTime.month = mp + (mp < 10 ? 3 : -9);
  y += (_dateTime.month <= 2);
  _dateTime.year = y;
  return _dateTime;
}
 
/********************************************************************/
// Convert unixTime to dateTime with time zone offset
rtc_datetime_t TeenyRTC::unixTimeToDateTime(uint32_t unixTime, int16_t timeZoneOffset) {
  return getOffsetDateTime(unixTimeToDateTime(unixTime), timeZoneOffset);
}

/********************************************************************/
// Convert dateTime to unixTime
uint32_t TeenyRTC::dateTimeToUnixTime(rtc_datetime_t dateTime) {
  int8_t my = (dateTime.month >= 3) ? 1 : 0;
  uint16_t y = dateTime.year + my - 1970;
  uint16_t dm = 0;
  for(int i = 0; i < dateTime.month - 1; i++) dm += (i<7)?((i==1)?28:((i&1)?30:31)):((i&1)?31:30);
  return (((dateTime.day-1+dm+((y+1)>>2)-((y+69)/100)+((y+369)/100/4)+365*(y-my))*24ul+dateTime.hour)*60ul+dateTime.minute)*60ul+dateTime.second;
}
 
/********************************************************************/
// Convert dateTime to unixTime with time zone offset
uint32_t TeenyRTC::dateTimeToUnixTime(rtc_datetime_t dateTime, int16_t timeZoneOffset) {
  return dateTimeToUnixTime(getOffsetDateTime(dateTime, timeZoneOffset));
}

/********************************************************************/
// Convert dateTime to ISO8601 string
char* TeenyRTC::dateTimeToISO8601Str(rtc_datetime_t dateTime) {
  //e.g. "2020-06-25T15:29:37"
  static char _itdStr[20];
  sprintf(_itdStr, "%u-%02d-%02dT%02d:%02d:%02d",
          dateTime.year, dateTime.month, dateTime.day,
          dateTime.hour, dateTime.minute, dateTime.second);
  return _itdStr;
}

/********************************************************************/
// Convert dateTime to ISO8601 string with time zone offset
char* TeenyRTC::dateTimeToISO8601Str(rtc_datetime_t dateTime,
                                      int16_t timeZoneOffset) {
  return dateTimeToISO8601Str(getOffsetDateTime(dateTime, timeZoneOffset));
}

/********************************************************************/
// Convert unixTime to ISO8601 string
char* TeenyRTC::unixTimeToISO8601Str(uint32_t unixTime) {
  return dateTimeToISO8601Str(unixTimeToDateTime(unixTime));
}

/********************************************************************/
// Convert unixTime to ISO8601 string with time zone offset
char* TeenyRTC::unixTimeToISO8601Str(uint32_t unixTime,
                                      int16_t timeZoneOffset) {
  return dateTimeToISO8601Str(getOffsetDateTime(unixTimeToDateTime(unixTime), timeZoneOffset));
}

/********************************************************************/
// Extract dateTime from ISO8601 string
rtc_datetime_t TeenyRTC::extractISO8601DateTime(const char* iso8601DateTimeStr) {
  char ref[] = "2000-01-01T00:00:00";
  rtc_datetime_t _dateTime;
  memcpy(ref, iso8601DateTimeStr, min(strlen(ref), strlen(iso8601DateTimeStr)));
  _dateTime.year   = (ref[0]  - '0') * 1000 +
                     (ref[1]  - '0') * 100 +
                     (ref[2]  - '0') * 10 +
                     (ref[3]  - '0');
  _dateTime.month  = (ref[5]  - '0') * 10 +
                     (ref[6]  - '0');
  _dateTime.day    = (ref[8]  - '0') * 10 +
                     (ref[9]  - '0');
  _dateTime.hour   = (ref[11] - '0') * 10 +
                     (ref[12] - '0');
  _dateTime.minute = (ref[14] - '0') * 10 +
                     (ref[15] - '0');
  _dateTime.second = (ref[17] - '0') * 10 +
                     (ref[18] - '0');
  return _dateTime;
}

/********************************************************************/
// Extract dateTime from ISO8601 string with time zone offset
rtc_datetime_t TeenyRTC::extractISO8601DateTime(const char* iso8601DateTimeStr,
                                                int16_t timeZoneOffset) {
  return getOffsetDateTime(extractISO8601DateTime(iso8601DateTimeStr), timeZoneOffset);
}

/********************************************************************/
// Convert dateTime based on time zone offset in minutes
rtc_datetime_t TeenyRTC::getOffsetDateTime(rtc_datetime_t dateTime, int16_t timeZoneOffset) {
  rtc_datetime_t _localTime = dateTime;
  // No offset - GMT
  if(timeZoneOffset == 0) return _localTime;
  // Negative offset
  if(timeZoneOffset < 0) {
    int16_t _hourminutes = (_localTime.hour * 60) + _localTime.minute;
    // To avoid negative numbers in the calculation,
    // add hourminutes and subtract one day
    _hourminutes += (1440 + timeZoneOffset); // remember - offset is negative!
    _localTime.day--;
    // twenty-four hours (1440 minutes) make one day
    if(_hourminutes >= 1440) {
      _hourminutes -= 1440;
      _localTime.day++;
    }
    _localTime.minute = _hourminutes % 60;
    _localTime.hour = _hourminutes / 60;
    // change "zeroth" day of month to last day of previous month
    if(_localTime.day == 0) {
      _localTime.month--;
      if(_localTime.month == 0) {
        _localTime.month = 12;
        _localTime.year--;
      }
      if((_localTime.month == 4) || (_localTime.month == 6) ||
         (_localTime.month == 9) || (_localTime.month == 11)) {
        _localTime.day = 30;
      } else if(_localTime.month == 2) {
        if((_localTime.year % 4) == 0) {
          // this will fail in the year 2100
          _localTime.day = 29;
        } else {
          _localTime.day = 28;
        }
      } else {
        _localTime.day = 31;
      }
    }
  // Positive offset
  } else {
    _localTime.minute += timeZoneOffset % 60;
    if(_localTime.minute >= 60) {
      _localTime.minute -= 60;
      _localTime.hour++;
    }
    _localTime.hour += timeZoneOffset / 60;
    if(_localTime.hour >= 24) {
      _localTime.hour -= 24;
      _localTime.day++;
    }
    if((_localTime.month == 4) || (_localTime.month == 6) ||
       (_localTime.month == 9) || (_localTime.month == 11)) {
      if(_localTime.day == 31) {
        _localTime.month++;
        _localTime.day = 1;
      }
    } else if(_localTime.month == 2) {
      if((_localTime.year % 4) == 0) {
        // this will fail in the year 2100
        if(_localTime.day == 30) {
          _localTime.month++;
          _localTime.day = 1;
        }
      } else {
        if(_localTime.day == 29) {
          _localTime.month++;
          _localTime.day = 1;
        }
      }
    } else {
      if(_localTime.day == 32) {
        _localTime.month++;
        _localTime.day = 1;
      }
      if(_localTime.month == 13) {
        _localTime.month = 1;
        _localTime.year++;
      }
    }
  }
  return _localTime;
}


#ifdef M5_CORE2_RTC
#include <M5Core2.h>
/********************************************************************/
// Core2 RTC
/********************************************************************/
TeenyCore2RTC::TeenyCore2RTC() { }

TeenyCore2RTC::~TeenyCore2RTC() { }

/********************************************************************/
bool TeenyCore2RTC::setup(bool hasBatteryBackup) {
  _valid = hasBatteryBackup;
  return true;
}

/********************************************************************/
void TeenyCore2RTC::resetRTCTime() {
  RTC_TimeTypeDef rtcTime;
  RTC_DateTypeDef rtcDate;
  rtcDate.Year    = RTC_DATETIME_RESET.year;
  rtcDate.Month   = RTC_DATETIME_RESET.month;
  rtcDate.Date    = RTC_DATETIME_RESET.day;
  rtcTime.Hours   = RTC_DATETIME_RESET.hour;
  rtcTime.Minutes = RTC_DATETIME_RESET.minute;
  rtcTime.Seconds = RTC_DATETIME_RESET.second;
  M5.Rtc.SetTime(&rtcTime);
  M5.Rtc.SetDate(&rtcDate);
  _valid = false;
}

/********************************************************************/
bool TeenyCore2RTC::setRTCTime(rtc_datetime_t dateTime) {
  if(!isValidDateTime(dateTime)) {
    resetRTCTime();
    return false;
  }
  RTC_TimeTypeDef rtcTime;
  RTC_DateTypeDef rtcDate;
  rtcDate.Year    = dateTime.year;
  rtcDate.Month   = dateTime.month;
  rtcDate.Date    = dateTime.day;
  rtcTime.Hours   = dateTime.hour;
  rtcTime.Minutes = dateTime.minute;
  rtcTime.Seconds = dateTime.second;
  M5.Rtc.SetTime(&rtcTime);
  M5.Rtc.SetDate(&rtcDate);
  _valid = true;
  return _valid;
}
/********************************************************************/
bool TeenyCore2RTC::setRTCTime(uint32_t unixTime) {
  return setRTCTime(unixTimeToDateTime(unixTime));
}
/********************************************************************/
bool TeenyCore2RTC::setRTCTime(uint16_t year, uint8_t month, uint8_t day,
                               uint8_t hour, uint8_t minute, uint8_t second) {
  rtc_datetime_t _dateTime;
  _dateTime.year   = year;
  _dateTime.month  = month;
  _dateTime.day    = day;
  _dateTime.hour   = hour;
  _dateTime.minute = minute;
  _dateTime.second = second;
  return setRTCTime(_dateTime);
}

/********************************************************************/
bool TeenyCore2RTC::setRTCTime(rtc_datetime_t dateTime, int16_t timeZoneOffset) {
  return setRTCTime(getOffsetDateTime(dateTime, -timeZoneOffset));
}
/********************************************************************/
bool TeenyCore2RTC::setRTCTime(uint32_t unixTime, int16_t timeZoneOffset) {
  return setRTCTime(getOffsetDateTime(unixTimeToDateTime(unixTime), -timeZoneOffset));
}
/********************************************************************/
bool TeenyCore2RTC::setRTCTime(uint16_t year, uint8_t month, uint8_t day,
                               uint8_t hour, uint8_t minute, uint8_t second,
                               int16_t timeZoneOffset) {
  rtc_datetime_t _dateTime;
  _dateTime.year   = year;
  _dateTime.month  = month;
  _dateTime.day    = day;
  _dateTime.hour   = hour;
  _dateTime.minute = minute;
  _dateTime.second = second;
  return setRTCTime(getOffsetDateTime(_dateTime, -timeZoneOffset));
}

/********************************************************************/
rtc_datetime_t TeenyCore2RTC::getRTCTime() {
  rtc_datetime_t _dateTime;
  RTC_TimeTypeDef rtcTime;
  RTC_DateTypeDef rtcDate;
  M5.Rtc.GetTime(&rtcTime);
  M5.Rtc.GetDate(&rtcDate);
  _dateTime.year   = rtcDate.Year;
  _dateTime.month  = rtcDate.Month;
  _dateTime.day    = rtcDate.Date;
  _dateTime.hour   = rtcTime.Hours;
  _dateTime.minute = rtcTime.Minutes;
  _dateTime.second = rtcTime.Seconds;
  if(!isValidDateTime(_dateTime)) {
    resetRTCTime();
    _dateTime = RTC_DATETIME_RESET;
  }
  return _dateTime;
}
/********************************************************************/
// Get RTC dateTime with time zone offset
rtc_datetime_t TeenyCore2RTC::getRTCTime(int16_t timeZoneOffset) {
   return getOffsetDateTime(getRTCTime(), timeZoneOffset);
}
/********************************************************************/
// Get RTC unixTime
uint32_t TeenyCore2RTC::getRTCUnixTime() {
   return dateTimeToUnixTime(getRTCTime());
}
/********************************************************************/
// Get RTC unixTime with time zone offset
uint32_t TeenyCore2RTC::getRTCUnixTime(int16_t timeZoneOffset) {
   return dateTimeToUnixTime(getOffsetDateTime(getRTCTime(), timeZoneOffset));
}
/********************************************************************/
// Get RTC ISO8601 string
char* TeenyCore2RTC::getRTCISO8601DateTimeStr() {
  return dateTimeToISO8601Str(getRTCTime());
}
/********************************************************************/
// Get RTC ISO8601 string with time zone offset
char* TeenyCore2RTC::getRTCISO8601DateTimeStr(int16_t timeZoneOffset) {
  return dateTimeToISO8601Str(getRTCTime(timeZoneOffset));
}
#endif


#ifdef TEENSY41_RTC
#include <TimeLib.h>
/********************************************************************/
// Teensy41 RTC
/********************************************************************/
// static function wrapper for getting Teensy's RTC clock
time_t getTeensy3Time() {
  return Teensy3Clock.get();
}

/********************************************************************/
TeenyTeensy41RTC::TeenyTeensy41RTC() { }

TeenyTeensy41RTC::~TeenyTeensy41RTC() { }

/********************************************************************/
bool TeenyTeensy41RTC::setup(bool hasBatteryBackup) {
  _valid = hasBatteryBackup;
  // set the Time library to use Teensy's RTC to keep time
  setSyncProvider(getTeensy3Time);
  return true;
}

/********************************************************************/
void TeenyTeensy41RTC::resetRTCTime() {
  setTime(RTC_DATETIME_RESET.hour, RTC_DATETIME_RESET.minute,
          RTC_DATETIME_RESET.second, RTC_DATETIME_RESET.day,
          RTC_DATETIME_RESET.month, RTC_DATETIME_RESET.year);
  Teensy3Clock.set(now());
  _valid = false;
}

/********************************************************************/
bool TeenyTeensy41RTC::setRTCTime(rtc_datetime_t dateTime) {
  if(!isValidDateTime(dateTime)) {
    resetRTCTime();
    return false;
  }
  setTime(dateTime.hour, dateTime.minute, dateTime.second,
          dateTime.day, dateTime.month, dateTime.year);
  Teensy3Clock.set(now());
  _valid = true;
  return _valid;
}
/********************************************************************/
bool TeenyTeensy41RTC::setRTCTime(uint32_t unixTime) {
  return setRTCTime(unixTimeToDateTime(unixTime));
}
/********************************************************************/
bool TeenyTeensy41RTC::setRTCTime(uint16_t year, uint8_t month, uint8_t day,
                                  uint8_t hour, uint8_t minute, uint8_t second) {
  rtc_datetime_t _dateTime;
  _dateTime.year   = year;
  _dateTime.month  = month;
  _dateTime.day    = day;
  _dateTime.hour   = hour;
  _dateTime.minute = minute;
  _dateTime.second = second;
  return setRTCTime(_dateTime);
}

/********************************************************************/
bool TeenyTeensy41RTC::setRTCTime(rtc_datetime_t dateTime, int16_t timeZoneOffset) {
  return setRTCTime(getOffsetDateTime(dateTime, -timeZoneOffset));
}
/********************************************************************/
bool TeenyTeensy41RTC::setRTCTime(uint32_t unixTime, int16_t timeZoneOffset) {
  return setRTCTime(getOffsetDateTime(unixTimeToDateTime(unixTime), -timeZoneOffset));
}
/********************************************************************/
bool TeenyTeensy41RTC::setRTCTime(uint16_t year, uint8_t month, uint8_t day,
                                  uint8_t hour, uint8_t minute, uint8_t second,
                                  int16_t timeZoneOffset) {
  rtc_datetime_t _dateTime;
  _dateTime.year   = year;
  _dateTime.month  = month;
  _dateTime.day    = day;
  _dateTime.hour   = hour;
  _dateTime.minute = minute;
  _dateTime.second = second;
  return setRTCTime(getOffsetDateTime(_dateTime, -timeZoneOffset));
}

/********************************************************************/
rtc_datetime_t TeenyTeensy41RTC::getRTCTime() {
  time_t _now = getTeensy3Time();
  rtc_datetime_t _dateTime;
  _dateTime.year   = year(_now);
  _dateTime.month  = month(_now);
  _dateTime.day    = day(_now);
  _dateTime.hour   = hour(_now);
  _dateTime.minute = minute(_now);
  _dateTime.second = second(_now);
  if(!isValidDateTime(_dateTime)) {
    resetRTCTime();
    _dateTime = RTC_DATETIME_RESET;
  }
  return _dateTime;
}
/********************************************************************/
// Get RTC dateTime with time zone offset
rtc_datetime_t TeenyTeensy41RTC::getRTCTime(int16_t timeZoneOffset) {
   return getOffsetDateTime(getRTCTime(), timeZoneOffset);
}
/********************************************************************/
// Get RTC unixTime
uint32_t TeenyTeensy41RTC::getRTCUnixTime() {
   return dateTimeToUnixTime(getRTCTime());
}
/********************************************************************/
// Get RTC unixTime with time zone offset
uint32_t TeenyTeensy41RTC::getRTCUnixTime(int16_t timeZoneOffset) {
   return dateTimeToUnixTime(getOffsetDateTime(getRTCTime(), timeZoneOffset));
}
/********************************************************************/
// Get RTC ISO8601 string
char* TeenyTeensy41RTC::getRTCISO8601DateTimeStr() {
  return dateTimeToISO8601Str(getRTCTime());
}
/********************************************************************/
// Get RTC ISO8601 string with time zone offset
char* TeenyTeensy41RTC::getRTCISO8601DateTimeStr(int16_t timeZoneOffset) {
  return dateTimeToISO8601Str(getRTCTime(timeZoneOffset));
}
#endif


#ifdef RP2040_RTC
#include "hardware/rtc.h"
#include <TimeLib.h>
/********************************************************************/
// RP2040 RTC
/********************************************************************/
TeenyRP2040RTC::TeenyRP2040RTC() { }

TeenyRP2040RTC::~TeenyRP2040RTC() { }

/********************************************************************/
bool TeenyRP2040RTC::setup(bool hasBatteryBackup) {
  _valid = hasBatteryBackup;
  rtc_init();
  return true;
}

/********************************************************************/
void TeenyRP2040RTC::resetRTCTime() {
  datetime_t t = {
    .year  = RTC_DATETIME_RESET.year,
    .month = RTC_DATETIME_RESET.month,
    .day   = RTC_DATETIME_RESET.day,
    .dotw  = 0,
    .hour  = RTC_DATETIME_RESET.hour,
    .min   = RTC_DATETIME_RESET.minute,
    .sec   = RTC_DATETIME_RESET.second
  };
  rtc_set_datetime(&t);
  _valid = false;
}

/********************************************************************/
bool TeenyRP2040RTC::setRTCTime(rtc_datetime_t dateTime) {
  if(!isValidDateTime(dateTime)) {
    resetRTCTime();
    return false;
  }
  datetime_t t = {
    .year  = dateTime.year,
    .month = dateTime.month,
    .day   = dateTime.day,
    .dotw  = 0,
    .hour  = dateTime.hour,
    .min   = dateTime.minute,
    .sec   = dateTime.second
  };
  rtc_set_datetime(&t);
  _valid = true;
  return _valid;
}
/********************************************************************/
bool TeenyRP2040RTC::setRTCTime(uint32_t unixTime) {
  return setRTCTime(unixTimeToDateTime(unixTime));
}
/********************************************************************/
bool TeenyRP2040RTC::setRTCTime(uint16_t year, uint8_t month, uint8_t day,
                                uint8_t hour, uint8_t minute, uint8_t second) {
  rtc_datetime_t _dateTime;
  _dateTime.year   = year;
  _dateTime.month  = month;
  _dateTime.day    = day;
  _dateTime.hour   = hour;
  _dateTime.minute = minute;
  _dateTime.second = second;
  return setRTCTime(_dateTime);
}

/********************************************************************/
bool TeenyRP2040RTC::setRTCTime(rtc_datetime_t dateTime, int16_t timeZoneOffset) {
  return setRTCTime(getOffsetDateTime(dateTime, -timeZoneOffset));
}
/********************************************************************/
bool TeenyRP2040RTC::setRTCTime(uint32_t unixTime, int16_t timeZoneOffset) {
  return setRTCTime(getOffsetDateTime(unixTimeToDateTime(unixTime), -timeZoneOffset));
}
/********************************************************************/
bool TeenyRP2040RTC::setRTCTime(uint16_t year, uint8_t month, uint8_t day,
                                uint8_t hour, uint8_t minute, uint8_t second,
                                int16_t timeZoneOffset) {
  rtc_datetime_t _dateTime;
  _dateTime.year   = year;
  _dateTime.month  = month;
  _dateTime.day    = day;
  _dateTime.hour   = hour;
  _dateTime.minute = minute;
  _dateTime.second = second;
  return setRTCTime(getOffsetDateTime(_dateTime, -timeZoneOffset));
}

/********************************************************************/
rtc_datetime_t TeenyRP2040RTC::getRTCTime() {
  datetime_t t;
  rtc_get_datetime(&t);
  rtc_datetime_t _dateTime;
  _dateTime.year   = t.year;
  _dateTime.month  = t.month;
  _dateTime.day    = t.day;
  _dateTime.hour   = t.hour;
  _dateTime.minute = t.min;
  _dateTime.second = t.sec;
  if(!isValidDateTime(_dateTime)) {
    resetRTCTime();
    _dateTime = RTC_DATETIME_RESET;
  }
  return _dateTime;
}
/********************************************************************/
// Get RTC dateTime with time zone offset
rtc_datetime_t TeenyRP2040RTC::getRTCTime(int16_t timeZoneOffset) {
   return getOffsetDateTime(getRTCTime(), timeZoneOffset);
}
/********************************************************************/
// Get RTC unixTime
uint32_t TeenyRP2040RTC::getRTCUnixTime() {
   return dateTimeToUnixTime(getRTCTime());
}
/********************************************************************/
// Get RTC unixTime with time zone offset
uint32_t TeenyRP2040RTC::getRTCUnixTime(int16_t timeZoneOffset) {
   return dateTimeToUnixTime(getOffsetDateTime(getRTCTime(), timeZoneOffset));
}
/********************************************************************/
// Get RTC ISO8601 string
char* TeenyRP2040RTC::getRTCISO8601DateTimeStr() {
  return dateTimeToISO8601Str(getRTCTime());
}
/********************************************************************/
// Get RTC ISO8601 string with time zone offset
char* TeenyRP2040RTC::getRTCISO8601DateTimeStr(int16_t timeZoneOffset) {
  return dateTimeToISO8601Str(getRTCTime(timeZoneOffset));
}
#endif


#ifdef FEATHERM0_RTC
#include <RTCZero.h>
/********************************************************************/
// SAMD21 ARM Cortex M0 RTC
/********************************************************************/
TeenyZeroRTC::TeenyZeroRTC() { }

TeenyZeroRTC::~TeenyZeroRTC() { }

/********************************************************************/
bool TeenyZeroRTC::setup(bool hasBatteryBackup) {
  _valid = hasBatteryBackup;
  rtc.begin();
  return true;
}

/********************************************************************/
void TeenyZeroRTC::resetRTCTime() {
  rtc.setTime(RTC_DATETIME_RESET.hour, RTC_DATETIME_RESET.minute,
              RTC_DATETIME_RESET.second);
  rtc.setDate(RTC_DATETIME_RESET.day, RTC_DATETIME_RESET.month,
              (uint8_t)(RTC_DATETIME_RESET.year - 2000));
  _valid = false;
}

/********************************************************************/
bool TeenyZeroRTC::setRTCTime(rtc_datetime_t dateTime) {
  if(!isValidDateTime(dateTime)) {
    resetRTCTime();
    return false;
  }
  rtc.setTime(dateTime.hour, dateTime.minute, dateTime.second);
  rtc.setDate(dateTime.day, dateTime.month, (uint8_t)(dateTime.year - 2000));
  _valid = true;
  return _valid;
}
/********************************************************************/
bool TeenyZeroRTC::setRTCTime(uint32_t unixTime) {
  return setRTCTime(unixTimeToDateTime(unixTime));
}
/********************************************************************/
bool TeenyZeroRTC::setRTCTime(uint16_t year, uint8_t month, uint8_t day,
                              uint8_t hour, uint8_t minute, uint8_t second) {
  rtc_datetime_t _dateTime;
  _dateTime.year   = year;
  _dateTime.month  = month;
  _dateTime.day    = day;
  _dateTime.hour   = hour;
  _dateTime.minute = minute;
  _dateTime.second = second;
  return setRTCTime(_dateTime);
}

/********************************************************************/
bool TeenyZeroRTC::setRTCTime(rtc_datetime_t dateTime, int16_t timeZoneOffset) {
  return setRTCTime(getOffsetDateTime(dateTime, -timeZoneOffset));
}
/********************************************************************/
bool TeenyZeroRTC::setRTCTime(uint32_t unixTime, int16_t timeZoneOffset) {
  return setRTCTime(getOffsetDateTime(unixTimeToDateTime(unixTime), -timeZoneOffset));
}
/********************************************************************/
bool TeenyZeroRTC::setRTCTime(uint16_t year, uint8_t month, uint8_t day,
                              uint8_t hour, uint8_t minute, uint8_t second,
                              int16_t timeZoneOffset) {
  rtc_datetime_t _dateTime;
  _dateTime.year   = year;
  _dateTime.month  = month;
  _dateTime.day    = day;
  _dateTime.hour   = hour;
  _dateTime.minute = minute;
  _dateTime.second = second;
  return setRTCTime(getOffsetDateTime(_dateTime, -timeZoneOffset));
}

/********************************************************************/
rtc_datetime_t TeenyZeroRTC::getRTCTime() {
  rtc_datetime_t _dateTime;
  _dateTime.year   = (uint16_t)rtc.getYear() + 2000;
  _dateTime.month  = rtc.getMonth();
  _dateTime.day    = rtc.getDay();
  _dateTime.hour   = rtc.getHours();
  _dateTime.minute = rtc.getMinutes();
  _dateTime.second = rtc.getSeconds();
  if(!isValidDateTime(_dateTime)) {
    resetRTCTime();
    _dateTime = RTC_DATETIME_RESET;
  }
  return _dateTime;
}
/********************************************************************/
// Get RTC dateTime with time zone offset
rtc_datetime_t TeenyZeroRTC::getRTCTime(int16_t timeZoneOffset) {
   return getOffsetDateTime(getRTCTime(), timeZoneOffset);
}
/********************************************************************/
// Get RTC unixTime
uint32_t TeenyZeroRTC::getRTCUnixTime() {
   return dateTimeToUnixTime(getRTCTime());
}
/********************************************************************/
// Get RTC unixTime with time zone offset
uint32_t TeenyZeroRTC::getRTCUnixTime(int16_t timeZoneOffset) {
   return dateTimeToUnixTime(getOffsetDateTime(getRTCTime(), timeZoneOffset));
}
/********************************************************************/
// Get RTC ISO8601 string
char* TeenyZeroRTC::getRTCISO8601DateTimeStr() {
  return dateTimeToISO8601Str(getRTCTime());
}
/********************************************************************/
// Get RTC ISO8601 string with time zone offset
char* TeenyZeroRTC::getRTCISO8601DateTimeStr(int16_t timeZoneOffset) {
  return dateTimeToISO8601Str(getRTCTime(timeZoneOffset));
}
#endif


#ifdef DS3231_RTC
#include <RTClib.h>
/********************************************************************/
// DS3231 RTC
/********************************************************************/
TeenyDS3231RTC::TeenyDS3231RTC() { }

TeenyDS3231RTC::~TeenyDS3231RTC() { }

/********************************************************************/
bool TeenyDS3231RTC::setup(bool hasBatteryBackup) {
  _valid = hasBatteryBackup;
  if(rtc.begin()) {
    isValid();   // run this to set RTC _valid flag
    return true; // setup is successful even if RTC is not valid
  }
  return false;
}

/********************************************************************/
bool TeenyDS3231RTC::isValid() {
  if(rtc.lostPower()) {
    // need to init RTC to prevent wacky date/time progression
    resetRTCTime();
    _valid = false;
  }
  return _valid;
}

/********************************************************************/
void TeenyDS3231RTC::resetRTCTime() {
  rtc.adjust(DateTime(RTC_DATETIME_RESET.year, RTC_DATETIME_RESET.month,
                      RTC_DATETIME_RESET.day, RTC_DATETIME_RESET.hour,
                      RTC_DATETIME_RESET.minute, RTC_DATETIME_RESET.second));
  _valid = false;
}

/********************************************************************/
bool TeenyDS3231RTC::setRTCTime(rtc_datetime_t dateTime) {
  if(!isValidDateTime(dateTime)) {
    resetRTCTime();
    return false;
  }
  rtc.adjust(DateTime(dateTime.year, dateTime.month, dateTime.day,
                      dateTime.hour, dateTime.minute, dateTime.second));
  _valid = true;
  return _valid;
}
/********************************************************************/
bool TeenyDS3231RTC::setRTCTime(uint32_t unixTime) {
  return setRTCTime(unixTimeToDateTime(unixTime));
}
/********************************************************************/
bool TeenyDS3231RTC::setRTCTime(uint16_t year, uint8_t month, uint8_t day,
                                uint8_t hour, uint8_t minute, uint8_t second) {
  rtc_datetime_t _dateTime;
  _dateTime.year   = year;
  _dateTime.month  = month;
  _dateTime.day    = day;
  _dateTime.hour   = hour;
  _dateTime.minute = minute;
  _dateTime.second = second;
  return setRTCTime(_dateTime);
}

/********************************************************************/
bool TeenyDS3231RTC::setRTCTime(rtc_datetime_t dateTime, int16_t timeZoneOffset) {
  return setRTCTime(getOffsetDateTime(dateTime, -timeZoneOffset));
}
/********************************************************************/
bool TeenyDS3231RTC::setRTCTime(uint32_t unixTime, int16_t timeZoneOffset) {
  return setRTCTime(getOffsetDateTime(unixTimeToDateTime(unixTime), -timeZoneOffset));
}
/********************************************************************/
bool TeenyDS3231RTC::setRTCTime(uint16_t year, uint8_t month, uint8_t day,
                                uint8_t hour, uint8_t minute, uint8_t second,
                                int16_t timeZoneOffset) {
  rtc_datetime_t _dateTime;
  _dateTime.year   = year;
  _dateTime.month  = month;
  _dateTime.day    = day;
  _dateTime.hour   = hour;
  _dateTime.minute = minute;
  _dateTime.second = second;
  return setRTCTime(getOffsetDateTime(_dateTime, -timeZoneOffset));
}

/********************************************************************/
rtc_datetime_t TeenyDS3231RTC::getRTCTime() {
  DateTime _now = rtc.now();
  rtc_datetime_t _dateTime;
  _dateTime.year   = _now.year();
  _dateTime.month  = _now.month();
  _dateTime.day    = _now.day();
  _dateTime.hour   = _now.hour();
  _dateTime.minute = _now.minute();
  _dateTime.second = _now.second();
  if(!isValidDateTime(_dateTime)) {
    resetRTCTime();
    _dateTime = RTC_DATETIME_RESET;
  }
  return _dateTime;
}
/********************************************************************/
// Get RTC dateTime with time zone offset
rtc_datetime_t TeenyDS3231RTC::getRTCTime(int16_t timeZoneOffset) {
   return getOffsetDateTime(getRTCTime(), timeZoneOffset);
}
/********************************************************************/
// Get RTC unixTime
uint32_t TeenyDS3231RTC::getRTCUnixTime() {
   return dateTimeToUnixTime(getRTCTime());
}
/********************************************************************/
// Get RTC unixTime with time zone offset
uint32_t TeenyDS3231RTC::getRTCUnixTime(int16_t timeZoneOffset) {
   return dateTimeToUnixTime(getOffsetDateTime(getRTCTime(), timeZoneOffset));
}
/********************************************************************/
// Get RTC ISO8601 string
char* TeenyDS3231RTC::getRTCISO8601DateTimeStr() {
  return dateTimeToISO8601Str(getRTCTime());
}
/********************************************************************/
// Get RTC ISO8601 string with time zone offset
char* TeenyDS3231RTC::getRTCISO8601DateTimeStr(int16_t timeZoneOffset) {
  return dateTimeToISO8601Str(getRTCTime(timeZoneOffset));
}
#endif


#ifdef M5_BM8563
#include <M5Unified.h>
/********************************************************************/
// M5 BM8563
/********************************************************************/
TeenyM5BM8563::TeenyM5BM8563() { }

TeenyM5BM8563::~TeenyM5BM8563() { }

/********************************************************************/
bool TeenyM5BM8563::setup(bool hasBatteryBackup) {
  _valid = hasBatteryBackup;
  if(M5.Rtc.begin() && M5.Rtc.isEnabled()) {
    isValid();   // run this to set RTC _valid flag
    return true; // setup is successful even if RTC is not valid
  }
  return false;
}

/********************************************************************/
bool TeenyM5BM8563::isValid() {
  if(M5.Rtc.getVoltLow()) {
    // need to init RTC to prevent wacky date/time progression
    resetRTCTime();
    _valid = false;
  }
  return _valid;
}

/********************************************************************/
void TeenyM5BM8563::resetRTCTime() {
  M5.Rtc.setDateTime({{RTC_DATETIME_RESET.year, RTC_DATETIME_RESET.month, RTC_DATETIME_RESET.day},
                      {RTC_DATETIME_RESET.hour, RTC_DATETIME_RESET.minute, RTC_DATETIME_RESET.second}});
  _valid = false;
}

/********************************************************************/
bool TeenyM5BM8563::setRTCTime(rtc_datetime_t dateTime) {
  if(!isValidDateTime(dateTime)) {
    resetRTCTime();
    return false;
  }
  M5.Rtc.setDateTime({{dateTime.year, dateTime.month, dateTime.day} ,
                      {dateTime.hour, dateTime.minute, dateTime.second}});
  _valid = true;
  return _valid;
}
/********************************************************************/
bool TeenyM5BM8563::setRTCTime(uint32_t unixTime) {
  return setRTCTime(unixTimeToDateTime(unixTime));
}
/********************************************************************/
bool TeenyM5BM8563::setRTCTime(uint16_t year, uint8_t month, uint8_t day,
                                uint8_t hour, uint8_t minute, uint8_t second) {
  rtc_datetime_t _dateTime;
  _dateTime.year   = year;
  _dateTime.month  = month;
  _dateTime.day    = day;
  _dateTime.hour   = hour;
  _dateTime.minute = minute;
  _dateTime.second = second;
  return setRTCTime(_dateTime);
}

/********************************************************************/
bool TeenyM5BM8563::setRTCTime(rtc_datetime_t dateTime, int16_t timeZoneOffset) {
  return setRTCTime(getOffsetDateTime(dateTime, -timeZoneOffset));
}
/********************************************************************/
bool TeenyM5BM8563::setRTCTime(uint32_t unixTime, int16_t timeZoneOffset) {
  return setRTCTime(getOffsetDateTime(unixTimeToDateTime(unixTime), -timeZoneOffset));
}
/********************************************************************/
bool TeenyM5BM8563::setRTCTime(uint16_t year, uint8_t month, uint8_t day,
                                uint8_t hour, uint8_t minute, uint8_t second,
                                int16_t timeZoneOffset) {
  rtc_datetime_t _dateTime;
  _dateTime.year   = year;
  _dateTime.month  = month;
  _dateTime.day    = day;
  _dateTime.hour   = hour;
  _dateTime.minute = minute;
  _dateTime.second = second;
  return setRTCTime(getOffsetDateTime(_dateTime, -timeZoneOffset));
}

/********************************************************************/
rtc_datetime_t TeenyM5BM8563::getRTCTime() {
  m5::rtc_datetime_t rtc_datetime;
  //rtc_datetime = M5.Rtc.getDateTime();
  M5.Rtc.getDateTime(&rtc_datetime);
  rtc_datetime_t _dateTime;
  _dateTime.year   = rtc_datetime.date.year;
  _dateTime.month  = rtc_datetime.date.month;
  _dateTime.day    = rtc_datetime.date.date;
  _dateTime.hour   = rtc_datetime.time.hours;
  _dateTime.minute = rtc_datetime.time.minutes;
  _dateTime.second = rtc_datetime.time.seconds;
  if(!isValidDateTime(_dateTime)) {
    resetRTCTime();
    _dateTime = RTC_DATETIME_RESET;
  }
  return _dateTime;
}
/********************************************************************/
// Get RTC dateTime with time zone offset
rtc_datetime_t TeenyM5BM8563::getRTCTime(int16_t timeZoneOffset) {
   return getOffsetDateTime(getRTCTime(), timeZoneOffset);
}
/********************************************************************/
// Get RTC unixTime
uint32_t TeenyM5BM8563::getRTCUnixTime() {
   return dateTimeToUnixTime(getRTCTime());
}
/********************************************************************/
// Get RTC unixTime with time zone offset
uint32_t TeenyM5BM8563::getRTCUnixTime(int16_t timeZoneOffset) {
   return dateTimeToUnixTime(getOffsetDateTime(getRTCTime(), timeZoneOffset));
}
/********************************************************************/
// Get RTC ISO8601 string
char* TeenyM5BM8563::getRTCISO8601DateTimeStr() {
  return dateTimeToISO8601Str(getRTCTime());
}
/********************************************************************/
// Get RTC ISO8601 string with time zone offset
char* TeenyM5BM8563::getRTCISO8601DateTimeStr(int16_t timeZoneOffset) {
  return dateTimeToISO8601Str(getRTCTime(timeZoneOffset));
}
#endif

