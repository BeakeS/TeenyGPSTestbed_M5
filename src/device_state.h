
/********************************************************************/
// Device Modes
enum device_mode_t : int16_t {
  DM_IDLE = 0,
  DM_GPSRCVR,
  DM_GPSLOGR,
  DM_GPSNSAT,
  DM_GPSSTAT,
  DM_GPSSCAL,
  DM_GPSSCFG,
  DM_GPSSSTP,
  DM_GPSEMU_M8,
  DM_GPSEMU_M10
};

/********************************************************************/
// EMULATOR UBX Packet Source
enum emuUbxPktSource_t : uint8_t {
  EMU_PGMINPUT = 0,
  EMU_SDCINPUT
};

/********************************************************************/
// Device State
typedef struct {
  int16_t  TIMEZONE = 0;
  int16_t  DEVICE_MODE = DM_IDLE;
  uint8_t  GPSRESET = GPS_NORESET;
  uint8_t  UBXPKTLOGMODE = UBXPKTLOG_ALL;
  bool     GPSLOG_GPX = false;
  bool     GPSLOG_KML = true;
  bool     GPSLOG_CSV = false;
  uint8_t  GPSCALIBRATIONPERIOD = 12;
  uint8_t  EMU_UBXPKTSOURCE = EMU_PGMINPUT;
  uint8_t  EMU_NUMCOLDSTARTPACKETS = 10;
  bool     EMU_UBXPKTLOOPENABLE = false;
  uint8_t  DISPLAYBRIGHTNESS = 100;
  uint8_t  DISPLAYTIMEOUT = 10;
  bool     STATUSLED = true;
  bool     DEBUGLOOPTIMING = false;
  bool     spare00;
  bool     spare01;
  bool     spare02;
} device_state_t;
device_state_t deviceState_defaults;
device_state_t deviceState;

/********************************************************************/
// KVS
/********************************************************************/
#include "TeenyKVS.h"
uint8_t deviceStateKVSArray[1024];
TeenyKVSArray deviceStateKVS(deviceStateKVSArray, sizeof(deviceStateKVSArray));

/********************************************************************/
bool writeDeviceStateKVS() {
  bool rc;
  rc = deviceStateKVS.reset();
  if(!rc) return false;
  rc = deviceStateKVS.set("TIMEZONE", strlen("TIMEZONE"),
                          (uint8_t*)&deviceState.TIMEZONE, sizeof(deviceState.TIMEZONE));
  if(!rc) return false;
  rc = deviceStateKVS.set("DEVICE_MODE", strlen("DEVICE_MODE"),
                          (uint8_t*)&deviceState.DEVICE_MODE, sizeof(deviceState.DEVICE_MODE));
  if(!rc) return false;
  rc = deviceStateKVS.set("GPSRESET", strlen("GPSRESET"),
                          (uint8_t*)&deviceState.GPSRESET, sizeof(deviceState.GPSRESET));
  if(!rc) return false;
  rc = deviceStateKVS.set("UBXPKTLOGMODE", strlen("UBXPKTLOGMODE"),
                          (uint8_t*)&deviceState.UBXPKTLOGMODE, sizeof(deviceState.UBXPKTLOGMODE));
  if(!rc) return false;
  rc = deviceStateKVS.set("GPSLOG_GPX", strlen("GPSLOG_GPX"),
                          (uint8_t*)&deviceState.GPSLOG_GPX, sizeof(deviceState.GPSLOG_GPX));
  if(!rc) return false;
  rc = deviceStateKVS.set("GPSLOG_KML", strlen("GPSLOG_KML"),
                          (uint8_t*)&deviceState.GPSLOG_KML, sizeof(deviceState.GPSLOG_KML));
  if(!rc) return false;
  rc = deviceStateKVS.set("GPSLOG_CSV", strlen("GPSLOG_CSV"),
                          (uint8_t*)&deviceState.GPSLOG_CSV, sizeof(deviceState.GPSLOG_CSV));
  if(!rc) return false;
  rc = deviceStateKVS.set("GPSCALIBRATIONPERIOD", strlen("GPSCALIBRATIONPERIOD"),
                          (uint8_t*)&deviceState.GPSCALIBRATIONPERIOD, sizeof(deviceState.GPSCALIBRATIONPERIOD));
  if(!rc) return false;
  rc = deviceStateKVS.set("EMU_UBXPKTSOURCE", strlen("EMU_UBXPKTSOURCE"),
                          (uint8_t*)&deviceState.EMU_UBXPKTSOURCE, sizeof(deviceState.EMU_UBXPKTSOURCE));
  if(!rc) return false;
  rc = deviceStateKVS.set("EMU_NUMCOLDSTARTPACKETS", strlen("EMU_NUMCOLDSTARTPACKETS"),
                          (uint8_t*)&deviceState.EMU_NUMCOLDSTARTPACKETS, sizeof(deviceState.EMU_NUMCOLDSTARTPACKETS));
  if(!rc) return false;
  rc = deviceStateKVS.set("EMU_UBXPKTLOOPENABLE", strlen("EMU_UBXPKTLOOPENABLE"),
                          (uint8_t*)&deviceState.EMU_UBXPKTLOOPENABLE, sizeof(deviceState.EMU_UBXPKTLOOPENABLE));
  if(!rc) return false;
  rc = deviceStateKVS.set("DISPLAYBRIGHTNESS", strlen("DISPLAYBRIGHTNESS"),
                          (uint8_t*)&deviceState.DISPLAYBRIGHTNESS, sizeof(deviceState.DISPLAYBRIGHTNESS));
  if(!rc) return false;
  rc = deviceStateKVS.set("DISPLAYTIMEOUT", strlen("DISPLAYTIMEOUT"),
                          (uint8_t*)&deviceState.DISPLAYTIMEOUT, sizeof(deviceState.DISPLAYTIMEOUT));
  if(!rc) return false;
  rc = deviceStateKVS.set("STATUSLED", strlen("STATUSLED"),
                          (uint8_t*)&deviceState.STATUSLED, sizeof(deviceState.STATUSLED));
  if(!rc) return false;
  rc = deviceStateKVS.set("DEBUGLOOPTIMING", strlen("DEBUGLOOPTIMING"),
                          (uint8_t*)&deviceState.DEBUGLOOPTIMING, sizeof(deviceState.DEBUGLOOPTIMING));
  if(!rc) return false;
  return true;
}

/********************************************************************/
bool readDeviceStateKVS() {
  bool rc;
  rc = deviceStateKVS.get("TIMEZONE", strlen("TIMEZONE"),
                          (uint8_t*)&deviceState.TIMEZONE, sizeof(deviceState.TIMEZONE));
  if(!rc) return false;
  rc = deviceStateKVS.get("DEVICE_MODE", strlen("DEVICE_MODE"),
                          (uint8_t*)&deviceState.DEVICE_MODE, sizeof(deviceState.DEVICE_MODE));
  if(!rc) return false;
  rc = deviceStateKVS.get("GPSRESET", strlen("GPSRESET"),
                          (uint8_t*)&deviceState.GPSRESET, sizeof(deviceState.GPSRESET));
  if(!rc) return false;
  rc = deviceStateKVS.get("UBXPKTLOGMODE", strlen("UBXPKTLOGMODE"),
                          (uint8_t*)&deviceState.UBXPKTLOGMODE, sizeof(deviceState.UBXPKTLOGMODE));
  if(!rc) return false;
  rc = deviceStateKVS.get("GPSLOG_GPX", strlen("GPSLOG_GPX"),
                          (uint8_t*)&deviceState.GPSLOG_GPX, sizeof(deviceState.GPSLOG_GPX));
  if(!rc) return false;
  rc = deviceStateKVS.get("GPSLOG_KML", strlen("GPSLOG_KML"),
                          (uint8_t*)&deviceState.GPSLOG_KML, sizeof(deviceState.GPSLOG_KML));
  if(!rc) return false;
  rc = deviceStateKVS.get("GPSLOG_CSV", strlen("GPSLOG_CSV"),
                          (uint8_t*)&deviceState.GPSLOG_CSV, sizeof(deviceState.GPSLOG_CSV));
  if(!rc) return false;
  rc = deviceStateKVS.get("GPSCALIBRATIONPERIOD", strlen("GPSCALIBRATIONPERIOD"),
                          (uint8_t*)&deviceState.GPSCALIBRATIONPERIOD, sizeof(deviceState.GPSCALIBRATIONPERIOD));
  if(!rc) return false;
  rc = deviceStateKVS.get("EMU_UBXPKTSOURCE", strlen("EMU_UBXPKTSOURCE"),
                          (uint8_t*)&deviceState.EMU_UBXPKTSOURCE, sizeof(deviceState.EMU_UBXPKTSOURCE));
  if(!rc) return false;
  rc = deviceStateKVS.get("EMU_NUMCOLDSTARTPACKETS", strlen("EMU_NUMCOLDSTARTPACKETS"),
                          (uint8_t*)&deviceState.EMU_NUMCOLDSTARTPACKETS, sizeof(deviceState.EMU_NUMCOLDSTARTPACKETS));
  if(!rc) return false;
  rc = deviceStateKVS.get("EMU_UBXPKTLOOPENABLE", strlen("EMU_UBXPKTLOOPENABLE"),
                          (uint8_t*)&deviceState.EMU_UBXPKTLOOPENABLE, sizeof(deviceState.EMU_UBXPKTLOOPENABLE));
  if(!rc) return false;
  rc = deviceStateKVS.get("DISPLAYBRIGHTNESS", strlen("DISPLAYBRIGHTNESS"),
                          (uint8_t*)&deviceState.DISPLAYBRIGHTNESS, sizeof(deviceState.DISPLAYBRIGHTNESS));
  if(!rc) return false;
  rc = deviceStateKVS.get("DISPLAYTIMEOUT", strlen("DISPLAYTIMEOUT"),
                          (uint8_t*)&deviceState.DISPLAYTIMEOUT, sizeof(deviceState.DISPLAYTIMEOUT));
  if(!rc) return false;
  rc = deviceStateKVS.get("STATUSLED", strlen("STATUSLED"),
                          (uint8_t*)&deviceState.STATUSLED, sizeof(deviceState.STATUSLED));
  if(!rc) return false;
  rc = deviceStateKVS.get("DEBUGLOOPTIMING", strlen("DEBUGLOOPTIMING"),
                          (uint8_t*)&deviceState.DEBUGLOOPTIMING, sizeof(deviceState.DEBUGLOOPTIMING));
  if(!rc) return false;
  return true;
}

