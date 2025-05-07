
/********************************************************************/
// SD Card
/********************************************************************/
bool sdcardEnabled = false;
#include <SD.h>
// m5stack core2 chip select = 4
const int sdChipSelect = 4;
File sdFile; // SD file pointer
File sdFile_cfg; // SD file pointer
File sdFile_ubx; // SD file pointer
File sdFile_gpx; // SD file pointer
File sdFile_kml; // SD file pointer
File sdFile_csv; // SD file pointer

/********************************************************************/
bool sdcard_setup() {
  if(SD.begin(sdChipSelect)) {
    sdcardEnabled = true;
  } else {
    sdcardEnabled = false;
  }
  return sdcardEnabled;
}

/********************************************************************/
// Config File Save/restore
/********************************************************************/
bool sdcard_deviceStateReset() {
  if(!sdcardEnabled) return false;
  if(SD.exists("/TEENYGPS.cfg")) {
    if(!SD.remove("/TEENYGPS.cfg")) return false;
  }
  return true;
}
/********************************************************************/
bool sdcard_deviceStateSave() {
  if(!writeDeviceStateKVS()) return false;
  if(!sdcard_deviceStateReset()) return false;
  File _sdFile = SD.open("/TEENYGPS.cfg", FILE_WRITE);
  if(!_sdFile) return false;
  _sdFile.write(deviceStateKVSArray, deviceStateKVS.used_bytes());
  _sdFile.close();
  return true;
}
/********************************************************************/
bool sdcard_deviceStateRestore() {
  if(sdcardEnabled && SD.exists("/TEENYGPS.cfg")) {
    File _sdFile = SD.open("/TEENYGPS.cfg");
    if(_sdFile) {
      _sdFile.read(deviceStateKVSArray, min(_sdFile.size(), sizeof(deviceStateKVSArray)));
      _sdFile.close();
      if(readDeviceStateKVS()) {
        return true;
      }
      sdcard_deviceStateReset();
    }
  }
  deviceState = deviceState_defaults;
  return false;
}

/********************************************************************/
// GPS Logging
/********************************************************************/
uint8_t sdcard_getAvailableLogFileNumber(const char* prefix, const char* ext) {
  char tempFileName[14]={0};
  for(uint8_t fileNum=0; fileNum<100; fileNum++) {
    sprintf(tempFileName, "/%s%02d.%s", prefix, fileNum, ext);
    if(!SD.exists(tempFileName)) {
      return fileNum;
    }
  }
  return 100;
}

/********************************************************************/
// GNSS Config File Writer
/********************************************************************/
uint8_t  gnssConfigFileNum = 0;
char     gnssConfigFileName[14]={0};
uint32_t gnssConfigFileWriteCount;
/********************************************************************/
bool sdcard_openGNSSConfigFile() {
  if(!sdcardEnabled) return false;
  gnssConfigFileNum = sdcard_getAvailableLogFileNumber("GPSLOG", "cfg");
  if(gnssConfigFileNum > 99) return false;
  sprintf(gnssConfigFileName, "/GPSLOG%02d.cfg", gnssConfigFileNum);
  sdFile_cfg = SD.open(gnssConfigFileName, FILE_WRITE);
  if(!sdFile_cfg) return false;
  gnssConfigFileWriteCount = 0;
  return true;
}
/********************************************************************/
void sdcard_writeGNSSConfigFile(const uint8_t *buf, size_t size) {
  if(!sdFile_cfg) return;
  sdFile_cfg.write(buf, size);
  gnssConfigFileWriteCount++;
}
/********************************************************************/
uint16_t sdcard_closeGNSSConfigFile() {
  if(!sdFile_cfg) return 0;
  sdFile_cfg.close();
  return gnssConfigFileWriteCount;
}

/********************************************************************/
// UBX Logging File Writer
/********************************************************************/
uint8_t  ubxLoggingFileNum = 0;
char     ubxLoggingFileName[14]={0};
uint32_t ubxLoggingFileWriteCount;
uint32_t ubxLoggingFileWritePktCount;
uint32_t ubxLoggingFileWriteLocValidCount;
/********************************************************************/
bool sdcard_openUBXLoggingFile() {
  if(!sdcardEnabled) return false;
  ubxLoggingFileNum = sdcard_getAvailableLogFileNumber("GPSLOG", "ubx");
  if(ubxLoggingFileNum > 99) return false;
  sprintf(ubxLoggingFileName, "/GPSLOG%02d.ubx", ubxLoggingFileNum);
  sdFile_ubx = SD.open(ubxLoggingFileName, FILE_WRITE);
  if(!sdFile_ubx) return false;
  ubxLoggingFileWriteCount = 0;
  ubxLoggingFileWritePktCount = 0;
  ubxLoggingFileWriteLocValidCount = 0;
  return true;
}
/********************************************************************/
void sdcard_writeUBXLoggingFile(const uint8_t *buf, size_t size, bool locValid=false, bool append=false) {
  if(!sdFile_ubx) return;
  sdFile_ubx.write(buf, size);
  if(!append) ubxLoggingFileWriteCount++;
  ubxLoggingFileWritePktCount++;
  if(locValid) ubxLoggingFileWriteLocValidCount++;
}
/********************************************************************/
uint16_t sdcard_closeUBXLoggingFile() {
  if(!sdFile_ubx) return 0;
  sdFile_ubx.close();
  return ubxLoggingFileWriteCount;
}

/********************************************************************/
// GPX Logging File Writer
// See "https://www.topografix.com/GPX/1/1/" for details
/********************************************************************/
uint8_t  gpxLoggingFileNum = 0;
char     gpxLoggingFileName[14]={0};
uint16_t gpxLoggingFileWriteCount;
/********************************************************************/
bool sdcard_openGPXLoggingFile() {
  if(!sdcardEnabled) return false;
  gpxLoggingFileNum = sdcard_getAvailableLogFileNumber("GPSLOG", "gpx");
  if(gpxLoggingFileNum > 99) return false;
  sprintf(gpxLoggingFileName, "/GPSLOG%02d.gpx", gpxLoggingFileNum);
  sdFile_gpx = SD.open(gpxLoggingFileName, FILE_WRITE);
  if(!sdFile_gpx) return false;
  sdFile_gpx.write((uint8_t*)"<gpx version=\"1.1\" creator=\"TeenyGPSTestbed\">\n",
                      strlen("<gpx version=\"1.1\" creator=\"TeenyGPSTestbed\">\n"));
  sdFile_gpx.write((uint8_t*)"  <trk>\n",
                      strlen("  <trk>\n"));
  sdFile_gpx.write((uint8_t*)"    <trkseg>\n",
                      strlen("    <trkseg>\n"));
  gpxLoggingFileWriteCount = 0;
  return true;
}
/********************************************************************/
void sdcard_writeGPXLoggingFile(const uint8_t *buf, size_t size, bool append=false) {
  if(!sdFile_gpx) return;
  sdFile_gpx.write(buf, size);
  if(!append) gpxLoggingFileWriteCount++;
}
/********************************************************************/
uint16_t sdcard_closeGPXLoggingFile() {
  if(!sdFile_gpx) return 0;
  sdFile_gpx.write((uint8_t*)"    </trkseg>\n",
                      strlen("    </trkseg>\n"));
  sdFile_gpx.write((uint8_t*)"  </trk>\n",
                      strlen("  </trk>\n"));
  sdFile_gpx.write((uint8_t*)"</gpx>\n",
                      strlen("</gpx>\n"));
  sdFile_gpx.close();
  return gpxLoggingFileWriteCount;
}

/********************************************************************/
// KML Logging File Writer
// See "https://developers.google.com/kml/documentation/kmlreference#gxtrack" for details 
/********************************************************************/
uint8_t  kmlLoggingFileNum = 0;
char     kmlLoggingFileName[14]={0};
uint16_t kmlLoggingFileWriteCount;
/********************************************************************/
bool sdcard_openKMLLoggingFile() {
  if(!sdcardEnabled) return false;
  kmlLoggingFileNum = sdcard_getAvailableLogFileNumber("GPSLOG", "kml");
  if(kmlLoggingFileNum > 99) return false;
  sprintf(kmlLoggingFileName, "/GPSLOG%02d.kml", kmlLoggingFileNum);
  sdFile_kml = SD.open(kmlLoggingFileName, FILE_WRITE);
  if(!sdFile_kml) return false;
  sdFile_kml.write((uint8_t*)"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n",
                      strlen("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"));
  sdFile_kml.write((uint8_t*)"<kml xmlns=\"http://www.opengis.net/kml/2.2\" xmlns:gx=\"http://www.google.com/kml/ext/2.2\">\n",
                      strlen("<kml xmlns=\"http://www.opengis.net/kml/2.2\" xmlns:gx=\"http://www.google.com/kml/ext/2.2\">\n"));
  sdFile_kml.write((uint8_t*)"<Folder>\n",
                      strlen("<Folder>\n"));
  sdFile_kml.write((uint8_t*)"  <Placemark>\n",
                      strlen("  <Placemark>\n"));
  sdFile_kml.write((uint8_t*)"    <gx:Track>\n",
                      strlen("    <gx:Track>\n"));
  kmlLoggingFileWriteCount = 0;
  return true;
}
/********************************************************************/
void sdcard_writeKMLLoggingFile(const uint8_t *buf, size_t size, bool append=false) {
  if(!sdFile_kml) return;
  sdFile_kml.write(buf, size);
  if(!append) kmlLoggingFileWriteCount++;
}
/********************************************************************/
uint16_t sdcard_closeKMLLoggingFile() {
  if(!sdFile_kml) return 0;
  sdFile_kml.write((uint8_t*)"    </gx:Track>\n",
                      strlen("    </gx:Track>\n"));
  sdFile_kml.write((uint8_t*)"  </Placemark>\n",
                      strlen("  </Placemark>\n"));
  sdFile_kml.write((uint8_t*)"</Folder>\n",
                      strlen("</Folder>\n"));
  sdFile_kml.write((uint8_t*)"</kml>\n",
                      strlen("</kml>\n"));
  sdFile_kml.close();
  return kmlLoggingFileWriteCount;
}

/********************************************************************/
// CSV Logging File Writer
/********************************************************************/
uint8_t  csvLoggingFileNum = 0;
char     csvLoggingFileName[14]={0};
uint16_t csvLoggingFileWriteCount;
/********************************************************************/
bool sdcard_openCSVLoggingFile() {
  if(!sdcardEnabled) return false;
  csvLoggingFileNum = sdcard_getAvailableLogFileNumber("GPSLOG", "csv");
  if(csvLoggingFileNum > 99) return false;
  sprintf(csvLoggingFileName, "/GPSLOG%02d.csv", csvLoggingFileNum);
  sdFile_csv = SD.open(csvLoggingFileName, FILE_WRITE);
  if(!sdFile_csv) return false;
  sdFile_csv.write((uint8_t*)"INDEX,GMT,VALIDLOC,LATITUDE,LONGITUDE,ALTITUDE,",
                      strlen("INDEX,GMT,VALIDLOC,LATITUDE,LONGITUDE,ALTITUDE,"));
  sdFile_csv.write((uint8_t*)"HEADING,HACCEST,VACCEST,FIXTYPE,",
                      strlen("HEADING,HACCEST,VACCEST,FIXTYPE,"));
  sdFile_csv.write((uint8_t*)"NUMSV,PDOP,INVALIDL1H,DISTANCE,BEARING,",
                      strlen("NUMSV,PDOP,INVALIDL1H,DISTANCE,BEARING,"));
  sdFile_csv.write((uint8_t*)"GPSFIXOK,GPSFIX,PSMSTATE,CARRSOLN,",
                      strlen("GPSFIXOK,GPSFIX,PSMSTATE,CARRSOLN,"));
  sdFile_csv.write((uint8_t*)"TTFF,SPOOFDETSTATE,SPOOFIND,MULTISPOOF,",
                      strlen("TTFF,SPOOFDETSTATE,SPOOFIND,MULTISPOOF,"));
  sdFile_csv.write((uint8_t*)"TOTALSATS,RCVDSATS,HEALTHYSIGNAL,EPHEMERISVALID,USEDFORNAV,SATS",
                      strlen("TOTALSATS,RCVDSATS,HEALTHYSIGNAL,EPHEMERISVALID,USEDFORNAV,SATS"));
  csvLoggingFileWriteCount = 0;
  return true;
}
/********************************************************************/
void sdcard_writeCSVLoggingFile(const uint8_t *buf, size_t size, bool append=false) {
  if(!sdFile_csv) return;
  sdFile_csv.write(buf, size);
  if(!append) csvLoggingFileWriteCount++;
}
/********************************************************************/
uint16_t sdcard_closeCSVLoggingFile() {
  if(!sdFile_csv) return 0;
  sdFile_csv.write((uint8_t*)"\n", strlen("\n"));
  sdFile_csv.close();
  return csvLoggingFileWriteCount;
}

/********************************************************************/
// GPS Calibration
/********************************************************************/

/********************************************************************/
// GNSS Calibrate File Writer
/********************************************************************/
uint8_t  gnssCalibrateFileNum = 0;
char     gnssCalibrateFileName[14]={0};
uint32_t gnssCalibrateFileWriteCount;
/********************************************************************/
bool sdcard_openGNSSCalibrateFile() {
  if(!sdcardEnabled) return false;
  gnssCalibrateFileNum = sdcard_getAvailableLogFileNumber("GPSCAL", "csv");
  if(gnssCalibrateFileNum > 99) return false;
  sprintf(gnssCalibrateFileName, "/GPSCAL%02d.csv", gnssCalibrateFileNum);
  sdFile = SD.open(gnssCalibrateFileName, FILE_WRITE);
  if(!sdFile) return false;
  gnssCalibrateFileWriteCount = 0;
  return true;
}
/********************************************************************/
void sdcard_writeGNSSCalibrateFile(const uint8_t *buf, size_t size) {
  if(!sdFile) return;
  sdFile.write(buf, size);
  gnssCalibrateFileWriteCount++;
}
/********************************************************************/
uint16_t sdcard_closeGNSSCalibrateFile() {
  if(!sdFile) return 0;
  sdFile.write((uint8_t*)"\n", strlen("\n"));
  sdFile.close();
  return gnssCalibrateFileWriteCount;
}

/********************************************************************/
// Delete Log Files
/********************************************************************/
int16_t sdcard_deleteLogFiles() {
  if(!sdcardEnabled) return -1;
  int16_t deleteCount = 0;
  char tempFileName[14]={0};
  for(uint8_t fileNum=0; fileNum<100; fileNum++) {
    sprintf(tempFileName, "/%s%02d.%s", "GPSLOG", fileNum, "cfg");
    if(SD.exists(tempFileName)) {
      if(!SD.remove(tempFileName)) return -1;
      deleteCount++;
    }
    sprintf(tempFileName, "/%s%02d.%s", "GPSLOG", fileNum, "ubx");
    if(SD.exists(tempFileName)) {
      if(!SD.remove(tempFileName)) return -1;
      deleteCount++;
    }
    sprintf(tempFileName, "/%s%02d.%s", "GPSLOG", fileNum, "gpx");
    if(SD.exists(tempFileName)) {
      if(!SD.remove(tempFileName)) return -1;
      deleteCount++;
    }
    sprintf(tempFileName, "/%s%02d.%s", "GPSLOG", fileNum, "kml");
    if(SD.exists(tempFileName)) {
      if(!SD.remove(tempFileName)) return -1;
      deleteCount++;
    }
    sprintf(tempFileName, "/%s%02d.%s", "GPSLOG", fileNum, "csv");
    if(SD.exists(tempFileName)) {
      if(!SD.remove(tempFileName)) return -1;
      deleteCount++;
    }
    sprintf(tempFileName, "/%s%02d.%s", "GPSCAL", fileNum, "csv");
    if(SD.exists(tempFileName)) {
      if(!SD.remove(tempFileName)) return -1;
      deleteCount++;
    }
  }
  if(SD.exists("/GNSSCNFG.log")) {
    if(!SD.remove("/GNSSCNFG.log")) return -1;
    deleteCount++;
  }
  return deleteCount;
}

/********************************************************************/
// GPS Emulation
/********************************************************************/

/********************************************************************/
// UBX Emulation Loop File Reader
/********************************************************************/
char     ubxInputFileName[14]={0};
/********************************************************************/
bool sdcard_openUBXInputFile() {
  if(!sdcardEnabled) return false;
  sprintf(ubxInputFileName, "/EMUINPUT.ubx");
  if(!SD.exists(ubxInputFileName)) {
    return false;
  }
  sdFile_ubx = SD.open(ubxInputFileName, FILE_READ);
  if(!sdFile_ubx) return false;
  return true;
}
/********************************************************************/
bool sdcard_readUBXInputFile(uint8_t* value) {
  if(sdcardEnabled && sdFile_ubx) {
    if(!sdFile_ubx.available()) {
      if(!deviceState.EMU_UBXPKTLOOPENABLE) {
        return false;
      }
      sdFile_ubx.seek(0);
    }
    *value = sdFile_ubx.read();
    return true;
  }
  return false;
}
/********************************************************************/
void sdcard_closeUBXInputFile() {
  if(!sdFile_ubx) return;
  sdFile_ubx.close();
}

/********************************************************************/
// GPS Packet Debugging
/********************************************************************/

/********************************************************************/
// Rx Pkt File Writer
/********************************************************************/
uint8_t rxPktFileNum = 0;
char rxPktFileName[14]={0};
uint16_t rxPktWriteCount;
/********************************************************************/
bool sdcard_openRxPktFile() {
  if(!sdcardEnabled) return false;
  rxPktWriteCount = 0;
  while(gpsSerial->available()) gpsSerial->read();
  sprintf(rxPktFileName, "/RXPKT%03d.hex", rxPktFileNum);
  if(SD.exists(rxPktFileName)) {
    if(!SD.remove(rxPktFileName)) return false;
  }
  sdFile = SD.open(rxPktFileName, FILE_WRITE);
  if(!sdFile) return false;
  return true;
}
/********************************************************************/
void sdcard_writeRxPktFile() {
  if(!sdFile) return;
  while(gpsSerial->available()) {
    sdFile.write(gpsSerial->read());
    rxPktWriteCount++;
  }
}
/********************************************************************/
uint16_t sdcard_closeRxPktFile() {
  if(!sdFile) return 0;
  sdFile.close();
  rxPktFileNum++;
  return rxPktWriteCount;
}

