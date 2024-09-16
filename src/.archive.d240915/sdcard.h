
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
// UBX Logging File Writer
/********************************************************************/
bool     ubxLoggingInProgress = false;
uint8_t  ubxLoggingFileNum = 0;
char     ubxLoggingFileName[14]={0};
uint32_t ubxLoggingFileWriteCount;
uint32_t ubxLoggingFileWriteValidCount;
/********************************************************************/
bool sdcard_openUBXLoggingFile() {
  if(!sdcardEnabled) return false;
  sprintf(ubxLoggingFileName, "/UBXLOG%02d.hex", ubxLoggingFileNum);
  if(SD.exists(ubxLoggingFileName)) {
    if(!SD.remove(ubxLoggingFileName)) return false;
  }
  //SdFile::dateTimeCallback(sdDateTimeCB);
  sdFile_ubx = SD.open(ubxLoggingFileName, FILE_WRITE);
  if(!sdFile_ubx) return false;
  ubxLoggingFileWriteCount = 0;
  ubxLoggingFileWriteValidCount = 0;
  return true;
}
/********************************************************************/
void sdcard_writeUBXLoggingFile(const uint8_t *buf, size_t size) {
  sdFile_ubx.write(buf, size);
  //ubxLoggingFileWriteCount++; //done in loop along with fixed count
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
  sprintf(gpxLoggingFileName, "/GPSLOG%02d.gpx", gpxLoggingFileNum);
  if(SD.exists(gpxLoggingFileName)) {
    if(!SD.remove(gpxLoggingFileName)) return false;
  }
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
void sdcard_writeGPXLoggingFile(const uint8_t *buf, size_t size) {
  sdFile_gpx.write(buf, size);
  gpxLoggingFileWriteCount++;
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
  sprintf(kmlLoggingFileName, "/GPSLOG%02d.kml", kmlLoggingFileNum);
  if(SD.exists(kmlLoggingFileName)) {
    if(!SD.remove(kmlLoggingFileName)) return false;
  }
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
void sdcard_writeKMLLoggingFile(const uint8_t *buf, size_t size) {
  sdFile_kml.write(buf, size);
  kmlLoggingFileWriteCount++;
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
uint8_t sdcard_readUBXInputFile() {
  if(sdcardEnabled && sdFile_ubx) {
    if(!sdFile_ubx.available()) {
      sdFile_ubx.seek(0);
    }
    return sdFile_ubx.read();
  }
  return 0;
}
/********************************************************************/
void sdcard_closeUBXInputFile() {
  sdFile_ubx.close();
}

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

