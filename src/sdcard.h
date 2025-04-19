
/********************************************************************/
// SD Card
/********************************************************************/
bool sdcardEnabled;
#include <SPI.h>
#include <SD.h>
// m5stack core2 chip select = 4
const int sdChipSelect = 4;
File sdFile; // SD file pointer
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
//void sdDateTimeCB(uint16_t* date, uint16_t* time) {
//  rtc_datetime_t now = getRTCTime(); // get the RTC
//  // return date using FAT_DATE macro to format fields
//  *date = FAT_DATE(now.year, now.month, now.day);
//  // return time using FAT_TIME macro to format fields
//  *time = FAT_TIME(now.hour, now.minute, now.second);
//}

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
  //SdFile::dateTimeCallback(sdDateTimeCB);
  sdFile = SD.open("/TEENYGPS.cfg", FILE_WRITE);
  if(!sdFile) return false;
  sdFile.write(deviceStateKVSArray, deviceStateKVS.used_bytes());
  sdFile.close();
  return true;
}
/********************************************************************/
bool sdcard_deviceStateRestore() {
  if(sdcardEnabled && SD.exists("/TEENYGPS.cfg")) {
    sdFile = SD.open("/TEENYGPS.cfg");
    if(sdFile) {
      sdFile.read(deviceStateKVSArray, min(sdFile.size(), sizeof(deviceStateKVSArray)));
      sdFile.close();
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
// GNSS Config File Writer
/********************************************************************/
uint32_t gnssConfigFileWriteCount;
/********************************************************************/
bool sdcard_openGNSSConfigFile() {
  if(!sdcardEnabled) return false;
  if(SD.exists("/GNSSCNFG.txt")) {
    if(!SD.remove("/GNSSCNFG.txt")) return false;
  }
  //SdFile::dateTimeCallback(sdDateTimeCB);
  sdFile = SD.open("/GNSSCNFG.txt", FILE_WRITE);
  if(!sdFile) return false;
  gnssConfigFileWriteCount = 0;
  return true;
}
/********************************************************************/
void sdcard_writeGNSSConfigFile(const uint8_t *buf, size_t size) {
  sdFile.write(buf, size);
  gnssConfigFileWriteCount++;
}
/********************************************************************/
uint16_t sdcard_closeGNSSConfigFile() {
  sdFile.close();
  return gnssConfigFileWriteCount;
}

/********************************************************************/
// GPS Logging
/********************************************************************/
bool gpsLoggingInProgress = false;
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
  ubxLoggingFileNum = sdcard_getAvailableLogFileNumber("UBXLOG", "hex");
  if(ubxLoggingFileNum > 99) return false;
  sprintf(ubxLoggingFileName, "/UBXLOG%02d.hex", ubxLoggingFileNum);
  //SdFile::dateTimeCallback(sdDateTimeCB);
  sdFile_ubx = SD.open(ubxLoggingFileName, FILE_WRITE);
  if(!sdFile_ubx) return false;
  ubxLoggingFileWriteCount = 0;
  ubxLoggingFileWritePktCount = 0;
  ubxLoggingFileWriteLocValidCount = 0;
  return true;
}
/********************************************************************/
void sdcard_writeUBXLoggingFile(const uint8_t *buf, size_t size, bool locValid=false, bool append=false) {
  sdFile_ubx.write(buf, size);
  if(!append) ubxLoggingFileWriteCount++;
  ubxLoggingFileWritePktCount++;
  if(locValid) ubxLoggingFileWriteLocValidCount++;
}
/********************************************************************/
uint16_t sdcard_closeUBXLoggingFile() {
  sdFile_ubx.close();
  ubxLoggingFileNum++;
  if(ubxLoggingFileNum > 99) ubxLoggingFileNum = 0;
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
  //SdFile::dateTimeCallback(sdDateTimeCB);
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
  sdFile_gpx.write(buf, size);
  if(!append) gpxLoggingFileWriteCount++;
}
/********************************************************************/
uint16_t sdcard_closeGPXLoggingFile() {
  sdFile_gpx.write((uint8_t*)"    </trkseg>\n",
                   strlen("    </trkseg>\n"));
  sdFile_gpx.write((uint8_t*)"  </trk>\n",
                   strlen("  </trk>\n"));
  sdFile_gpx.write((uint8_t*)"</gpx>\n",
                   strlen("</gpx>\n"));
  sdFile_gpx.close();
  gpxLoggingFileNum++;
  if(gpxLoggingFileNum > 99) gpxLoggingFileNum = 0;
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
  //SdFile::dateTimeCallback(sdDateTimeCB);
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
  sdFile_kml.write(buf, size);
  if(!append) kmlLoggingFileWriteCount++;
}
/********************************************************************/
uint16_t sdcard_closeKMLLoggingFile() {
  sdFile_kml.write((uint8_t*)"    </gx:Track>\n",
                      strlen("    </gx:Track>\n"));
  sdFile_kml.write((uint8_t*)"  </Placemark>\n",
                      strlen("  </Placemark>\n"));
  sdFile_kml.write((uint8_t*)"</Folder>\n",
                      strlen("</Folder>\n"));
  sdFile_kml.write((uint8_t*)"</kml>\n",
                      strlen("</kml>\n"));
  sdFile_kml.close();
  kmlLoggingFileNum++;
  if(kmlLoggingFileNum > 99) kmlLoggingFileNum = 0;
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
  //SdFile::dateTimeCallback(sdDateTimeCB);
  sdFile_csv = SD.open(csvLoggingFileName, FILE_WRITE);
  if(!sdFile_csv) return false;
  // write header
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
  sdFile_csv.write(buf, size);
  if(!append) csvLoggingFileWriteCount++;
}
/********************************************************************/
uint16_t sdcard_closeCSVLoggingFile() {
  // write footer
  sdFile_csv.write((uint8_t*)"\n",
                      strlen("\n"));
  sdFile_csv.close();
  csvLoggingFileNum++;
  if(csvLoggingFileNum > 99) csvLoggingFileNum = 0;
  return csvLoggingFileWriteCount;
}

/********************************************************************/
// GPS Calibration
/********************************************************************/

/********************************************************************/
// GNSS Calibrate File Writer
/********************************************************************/
uint32_t gnssCalibrateFileWriteCount;
/********************************************************************/
bool sdcard_openGNSSCalibrateFile() {
  if(!sdcardEnabled) return false;
  if(SD.exists("/GNSSSCAL.csv")) {
    if(!SD.remove("/GNSSSCAL.csv")) return false;
  }
  //SdFile::dateTimeCallback(sdDateTimeCB);
  sdFile = SD.open("/GNSSSCAL.csv", FILE_WRITE);
  if(!sdFile) return false;
  gnssCalibrateFileWriteCount = 0;
  return true;
}
/********************************************************************/
void sdcard_writeGNSSCalibrateFile(const uint8_t *buf, size_t size) {
  sdFile.write(buf, size);
  gnssCalibrateFileWriteCount++;
}
/********************************************************************/
uint16_t sdcard_closeGNSSCalibrateFile() {
  sdFile.close();
  return gnssCalibrateFileWriteCount;
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
  sprintf(ubxInputFileName, "/UBXINPUT.hex");
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
  //SdFile::dateTimeCallback(sdDateTimeCB);
  sdFile = SD.open(rxPktFileName, FILE_WRITE);
  if(!sdFile) return false;
  return true;
}
/********************************************************************/
void sdcard_writeRxPktFile() {
  while(gpsSerial->available()) {
    sdFile.write(gpsSerial->read());
    rxPktWriteCount++;
  }
}
/********************************************************************/
uint16_t sdcard_closeRxPktFile() {
  sdFile.close();
  rxPktFileNum++;
  return rxPktWriteCount;
}

