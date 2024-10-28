
/********************************************************************/
// GPS Startup Modes
enum gpsReset_mode_t : uint8_t {
  GPS_NORESET = 0,
  GPS_HOTSTART,
  GPS_WARMSTART,
  GPS_COLDSTART,
  GPS_HARDWARERESET
};

/********************************************************************/
// UBX/GPS Logging Modes
enum ubxLogging_mode_t : uint8_t {
  UBXPKTLOG_NAVPVT = 0,
  UBXPKTLOG_NAVSTATUS,
  UBXPKTLOG_NAVSAT,
  UBXPKTLOG_ALL
};
enum gpsLogging_mode_t : uint8_t {
  GPSLOG_NONE = 0,
  GPSLOG_GPX,
  GPSLOG_KML
};

/********************************************************************/
// GPS
bool gpsEnabled;
#include "TeenyGPSConnect.h"
TeenyGPSConnect gps;

