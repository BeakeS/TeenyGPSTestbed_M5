
/********************************************************************/
// Device Modes
enum device_mode_t : int16_t {
  DM_IDLE = 0,
  DM_GPSRCVR,
  DM_GPSLOGR,
  DM_GPSSTAT,
  DM_GPSNSAT,
  DM_GPSSCFG,
  DM_GPSSSTP,
  DM_GPSEMU_M8,
  DM_GPSEMU_M10
};

/********************************************************************/
// Device State
typedef struct {
  int16_t  TIMEZONE = 0;
  int16_t  DEVICE_MODE = DM_IDLE;
  uint8_t  GPSRESET = GPS_NORESET;
  uint8_t  UBXPKTLOGMODE = UBXPKTLOG_ALL;
  uint8_t  GPSLOGMODE = GPSLOG_KML;
  uint8_t  EMUL_UBXPKTSOURCE = EMU_PGMINPUT;
  uint8_t  EMUL_NUMCOLDSTARTPACKETS = 10;
  uint8_t  DISPLAYBRIGHTNESS = 50;
  uint8_t  DISPLAYTIMEOUT = 10;
  bool     STATUSLED = true;
  uint8_t  spare00;
  uint8_t  spare01;
} device_state_t;
device_state_t deviceState_defaults;
device_state_t deviceState;

/********************************************************************/
// KVS
/********************************************************************/
#include "TeenyKVS.h"
uint8_t deviceStateKVSArray[256];
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
  rc = deviceStateKVS.set("EMUL_UBXPKTSOURCE", strlen("EMUL_UBXPKTSOURCE"),
                          (uint8_t*)&deviceState.EMUL_UBXPKTSOURCE, sizeof(deviceState.EMUL_UBXPKTSOURCE));
  if(!rc) return false;
  rc = deviceStateKVS.set("EMUL_NUMCOLDSTARTPACKETS", strlen("EMUL_NUMCOLDSTARTPACKETS"),
                          (uint8_t*)&deviceState.EMUL_NUMCOLDSTARTPACKETS, sizeof(deviceState.EMUL_NUMCOLDSTARTPACKETS));
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
  rc = deviceStateKVS.set("GPSRESET", strlen("GPSRESET"),
                          (uint8_t*)&deviceState.GPSRESET, sizeof(deviceState.GPSRESET));
  if(!rc) return false;
  rc = deviceStateKVS.set("UBXPKTLOGMODE", strlen("UBXPKTLOGMODE"),
                          (uint8_t*)&deviceState.UBXPKTLOGMODE, sizeof(deviceState.UBXPKTLOGMODE));
  if(!rc) return false;
  rc = deviceStateKVS.set("GPSLOGMODE", strlen("GPSLOGMODE"),
                          (uint8_t*)&deviceState.GPSLOGMODE, sizeof(deviceState.GPSLOGMODE));
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
  rc = deviceStateKVS.get("EMUL_UBXPKTSOURCE", strlen("EMUL_UBXPKTSOURCE"),
                          (uint8_t*)&deviceState.EMUL_UBXPKTSOURCE, sizeof(deviceState.EMUL_UBXPKTSOURCE));
  if(!rc) return false;
  rc = deviceStateKVS.get("EMUL_NUMCOLDSTARTPACKETS", strlen("EMUL_NUMCOLDSTARTPACKETS"),
                          (uint8_t*)&deviceState.EMUL_NUMCOLDSTARTPACKETS, sizeof(deviceState.EMUL_NUMCOLDSTARTPACKETS));
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
  rc = deviceStateKVS.get("GPSRESET", strlen("GPSRESET"),
                          (uint8_t*)&deviceState.GPSRESET, sizeof(deviceState.GPSRESET));
  if(!rc) return false;
  rc = deviceStateKVS.get("UBXPKTLOGMODE", strlen("UBXPKTLOGMODE"),
                          (uint8_t*)&deviceState.UBXPKTLOGMODE, sizeof(deviceState.UBXPKTLOGMODE));
  if(!rc) return false;
  rc = deviceStateKVS.get("GPSLOGMODE", strlen("GPSLOGMODE"),
                          (uint8_t*)&deviceState.GPSLOGMODE, sizeof(deviceState.GPSLOGMODE));
  if(!rc) return false;
  return true;
}
