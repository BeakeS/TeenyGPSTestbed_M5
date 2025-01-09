
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

/********************************************************************/
// forward declarations
bool sdcard_openGNSSConfigFile();
void sdcard_writeGNSSConfigFile(const uint8_t *buf, size_t size);
uint16_t sdcard_closeGNSSConfigFile();

/********************************************************************/
bool gps_writeGNSSConfigFile() {
  if(!sdcard_openGNSSConfigFile()) return false;
  char _dispStr[80];
  uint8_t ubloxModuleType = gps.getUbloxModuleType();
  gps.pollGNSSConfigInfo();
  ubloxCFGGNSSInfo_t ubloxCFGGNSSInfo = gps.getGNSSConfigInfo();
  sprintf(_dispStr, "UbloxModule=M%d\n", ubloxModuleType);
  sdcard_writeGNSSConfigFile((uint8_t*)_dispStr, strlen(_dispStr));
  if(ubloxModuleType == UBLOX_M8_MODULE) {
    sprintf(_dispStr, "numTrkChHw=%02d\n", ubloxCFGGNSSInfo.M8.numTrkChHw);
    sdcard_writeGNSSConfigFile((uint8_t*)_dispStr, strlen(_dispStr));
    sprintf(_dispStr, "numTrkChUse=%02d\n", ubloxCFGGNSSInfo.M8.numTrkChUse);
    sdcard_writeGNSSConfigFile((uint8_t*)_dispStr, strlen(_dispStr));
    sprintf(_dispStr, "numConfigBlocks=%02d\n", ubloxCFGGNSSInfo.M8.numConfigBlocks);
    sdcard_writeGNSSConfigFile((uint8_t*)_dispStr, strlen(_dispStr));
    for(uint8_t i=0; i<min(ubloxCFGGNSSInfo.M8.numConfigBlocks, 7); i++) {
      sprintf(_dispStr, "GNSS=%c Enabled=%c Channels=%02d/%02d Mask=%02X\n",
              ubloxCFGGNSSInfo.M8.configBlockList[i].gnssIdType,
              (ubloxCFGGNSSInfo.M8.configBlockList[i].enable ? 'T' : 'F'),
              ubloxCFGGNSSInfo.M8.configBlockList[i].resTrkCh,
              ubloxCFGGNSSInfo.M8.configBlockList[i].maxTrkCh,
              ubloxCFGGNSSInfo.M8.configBlockList[i].sigCfgMask);
      sdcard_writeGNSSConfigFile((uint8_t*)_dispStr, strlen(_dispStr));
    }
  } else if((ubloxModuleType == UBLOX_M9_MODULE) || (ubloxModuleType == UBLOX_M10_MODULE)) {
    sprintf(_dispStr, "numConfigBlocks=%02d\n", ubloxCFGGNSSInfo.M10.numConfigBlocks);
    sdcard_writeGNSSConfigFile((uint8_t*)_dispStr, strlen(_dispStr));
    for(uint8_t i=0; i<min(ubloxCFGGNSSInfo.M10.numConfigBlocks, 6); i++) {
      sprintf(_dispStr, "GNSS:%c=%c Sats:%s%s%c%s%s%s%c\n",
              ubloxCFGGNSSInfo.M10.configBlockList[i].gnssIdType,
              (ubloxCFGGNSSInfo.M10.configBlockList[i].enable ? 'T' : 'F'),
              ubloxCFGGNSSInfo.M10.configBlockList[i].signalList[0].name,
              "=",
              (ubloxCFGGNSSInfo.M10.configBlockList[i].signalList[0].enable ? 'T' : 'F'),
              ((ubloxCFGGNSSInfo.M10.configBlockList[i].numSigs > 1) ? "," : ""),
              (ubloxCFGGNSSInfo.M10.configBlockList[i].numSigs > 1) ?
                ubloxCFGGNSSInfo.M10.configBlockList[i].signalList[1].name : "",
              (ubloxCFGGNSSInfo.M10.configBlockList[i].numSigs > 1) ? "=" : "",
              (ubloxCFGGNSSInfo.M10.configBlockList[i].numSigs > 1) ?
                (ubloxCFGGNSSInfo.M10.configBlockList[i].signalList[1].enable ? 'T' : 'F') : ' ');
      sdcard_writeGNSSConfigFile((uint8_t*)_dispStr, strlen(_dispStr));
    }
  }
  return sdcard_closeGNSSConfigFile() > 0;
}

