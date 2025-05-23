
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
// UBX Packet Logging Modes
enum ubxLogging_mode_t : uint8_t {
  GPS_LOGUBX_NONE = 0,
  GPS_LOGUBX_NAVPVT,
  GPS_LOGUBX_NAVSAT,
  GPS_LOGUBX_NAVSTATUS,
  GPS_LOGUBX_ALL
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
  char _tempStr[80];
  uint8_t ubloxModuleType = gps.getUbloxModuleType();
  gps.pollGNSSConfig();
  ubloxCFGGNSSInfo_t ubloxCFGGNSSInfo = gps.getGNSSConfigInfo();
  sprintf(_tempStr, "UbloxModule=M%d\n", ubloxModuleType);
  sdcard_writeGNSSConfigFile((uint8_t*)_tempStr, strlen(_tempStr));
  if(ubloxModuleType == UBLOX_M8_MODULE) {
    sprintf(_tempStr, "numTrkChHw=%02d\n", ubloxCFGGNSSInfo.M8.numTrkChHw);
    sdcard_writeGNSSConfigFile((uint8_t*)_tempStr, strlen(_tempStr));
    sprintf(_tempStr, "numTrkChUse=%02d\n", ubloxCFGGNSSInfo.M8.numTrkChUse);
    sdcard_writeGNSSConfigFile((uint8_t*)_tempStr, strlen(_tempStr));
    sprintf(_tempStr, "numConfigBlocks=%02d\n", ubloxCFGGNSSInfo.M8.numConfigBlocks);
    sdcard_writeGNSSConfigFile((uint8_t*)_tempStr, strlen(_tempStr));
    for(uint8_t i=0; i<min(ubloxCFGGNSSInfo.M8.numConfigBlocks, 7); i++) {
      sprintf(_tempStr, "GNSS=%s Enabled=%c Channels=%02d/%02d Mask=%02X\n",
              ubloxCFGGNSSInfo.M8.configBlockList[i].gnssIdName,
              (ubloxCFGGNSSInfo.M8.configBlockList[i].enable ? 'T' : 'F'),
              ubloxCFGGNSSInfo.M8.configBlockList[i].resTrkCh,
              ubloxCFGGNSSInfo.M8.configBlockList[i].maxTrkCh,
              ubloxCFGGNSSInfo.M8.configBlockList[i].sigCfgMask);
      sdcard_writeGNSSConfigFile((uint8_t*)_tempStr, strlen(_tempStr));
    }
  } else if((ubloxModuleType == UBLOX_M9_MODULE) || (ubloxModuleType == UBLOX_M10_MODULE)) {
    sprintf(_tempStr, "numConfigBlocks=%02d\n", ubloxCFGGNSSInfo.M10.numConfigBlocks);
    sdcard_writeGNSSConfigFile((uint8_t*)_tempStr, strlen(_tempStr));
    for(uint8_t i=0; i<min(ubloxCFGGNSSInfo.M10.numConfigBlocks, 6); i++) {
      sprintf(_tempStr, "GNSS:%s=%c Sats:%s%s%c%s%s%s%c\n",
              ubloxCFGGNSSInfo.M10.configBlockList[i].gnssIdName,
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
      sdcard_writeGNSSConfigFile((uint8_t*)_tempStr, strlen(_tempStr));
    }
  }
  return sdcard_closeGNSSConfigFile() > 0;
}

