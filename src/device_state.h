
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
  uint8_t  GPS_RESETMODE = GPS_NORESET;
  uint8_t  GPS_UBXNAVSATRATE = 10;
  uint8_t  GPS_LOGUBXMODE = GPS_LOGUBX_ALL;
  bool     GPS_LOGGPX = false;
  bool     GPS_LOGKML = true;
  bool     GPS_LOGCSV = true;
  uint8_t  GPS_CALIBRATEPERIOD = 12;
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
  rc = deviceStateKVS.set("GPS_RESETMODE", strlen("GPS_RESETMODE"),
                          (uint8_t*)&deviceState.GPS_RESETMODE, sizeof(deviceState.GPS_RESETMODE));
  if(!rc) return false;
  rc = deviceStateKVS.set("GPS_UBXNAVSATRATE", strlen("GPS_UBXNAVSATRATE"),
                          (uint8_t*)&deviceState.GPS_UBXNAVSATRATE, sizeof(deviceState.GPS_UBXNAVSATRATE));
  if(!rc) return false;
  rc = deviceStateKVS.set("GPS_LOGUBXMODE", strlen("GPS_LOGUBXMODE"),
                          (uint8_t*)&deviceState.GPS_LOGUBXMODE, sizeof(deviceState.GPS_LOGUBXMODE));
  if(!rc) return false;
  rc = deviceStateKVS.set("GPS_LOGGPX", strlen("GPS_LOGGPX"),
                          (uint8_t*)&deviceState.GPS_LOGGPX, sizeof(deviceState.GPS_LOGGPX));
  if(!rc) return false;
  rc = deviceStateKVS.set("GPS_LOGKML", strlen("GPS_LOGKML"),
                          (uint8_t*)&deviceState.GPS_LOGKML, sizeof(deviceState.GPS_LOGKML));
  if(!rc) return false;
  rc = deviceStateKVS.set("GPS_LOGCSV", strlen("GPS_LOGCSV"),
                          (uint8_t*)&deviceState.GPS_LOGCSV, sizeof(deviceState.GPS_LOGCSV));
  if(!rc) return false;
  rc = deviceStateKVS.set("GPS_CALIBRATEPERIOD", strlen("GPS_CALIBRATEPERIOD"),
                          (uint8_t*)&deviceState.GPS_CALIBRATEPERIOD, sizeof(deviceState.GPS_CALIBRATEPERIOD));
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
  rc = deviceStateKVS.get("GPS_RESETMODE", strlen("GPS_RESETMODE"),
                          (uint8_t*)&deviceState.GPS_RESETMODE, sizeof(deviceState.GPS_RESETMODE));
  if(!rc) return false;
  rc = deviceStateKVS.get("GPS_UBXNAVSATRATE", strlen("GPS_UBXNAVSATRATE"),
                          (uint8_t*)&deviceState.GPS_UBXNAVSATRATE, sizeof(deviceState.GPS_UBXNAVSATRATE));
  if(!rc) return false;
  rc = deviceStateKVS.get("GPS_LOGUBXMODE", strlen("GPS_LOGUBXMODE"),
                          (uint8_t*)&deviceState.GPS_LOGUBXMODE, sizeof(deviceState.GPS_LOGUBXMODE));
  if(!rc) return false;
  rc = deviceStateKVS.get("GPS_LOGGPX", strlen("GPS_LOGGPX"),
                          (uint8_t*)&deviceState.GPS_LOGGPX, sizeof(deviceState.GPS_LOGGPX));
  if(!rc) return false;
  rc = deviceStateKVS.get("GPS_LOGKML", strlen("GPS_LOGKML"),
                          (uint8_t*)&deviceState.GPS_LOGKML, sizeof(deviceState.GPS_LOGKML));
  if(!rc) return false;
  rc = deviceStateKVS.get("GPS_LOGCSV", strlen("GPS_LOGCSV"),
                          (uint8_t*)&deviceState.GPS_LOGCSV, sizeof(deviceState.GPS_LOGCSV));
  if(!rc) return false;
  rc = deviceStateKVS.get("GPS_CALIBRATEPERIOD", strlen("GPS_CALIBRATEPERIOD"),
                          (uint8_t*)&deviceState.GPS_CALIBRATEPERIOD, sizeof(deviceState.GPS_CALIBRATEPERIOD));
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

