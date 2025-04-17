
/********************************************************************/
// Satellite Configurations
/********************************************************************/
typedef struct {
  ubloxCFGGNSSState_t GPS        = {1, 1, 1, 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0, 1, -1};
  ubloxCFGGNSSState_t Galileo    = {0, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 1, -1};
  ubloxCFGGNSSState_t BeiDou_B1  = {0, 1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 1, -1};
  ubloxCFGGNSSState_t BeiDou_B1C = {0, 1, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 1, -1};
  ubloxCFGGNSSState_t GLONASS    = {0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 1, 1, 1, -1};
} satcalGNSSConfigList_t;
satcalGNSSConfigList_t satcalGNSSConfigList;

/********************************************************************/
// Satellite Calibration Steps
/********************************************************************/
enum satcal_step_t : uint8_t {
  SC_GPS_START = 0,
  SC_GPS,
  SC_GAL_START,
  SC_GAL,
  SC_BDB1_START,
  SC_BDB1,
  SC_BDB1C_START,
  SC_BDB1C,
  SC_GLO_START,
  SC_GLO,
  SC_FACTORY_START,
  SC_FACTORY,
  SC_CURRENT_START,
  SC_CURRENT,
  SC_END_START,
  SC_END
};

/********************************************************************/
// Global Variables
/********************************************************************/
uint8_t  satcalErrorCode;
uint8_t  satcalUbloxModuleType;
bool     satcalIsUbloxM10Module;
uint8_t  satcalStep;
uint32_t satcalStepPeriod;
uint32_t satcalStartTime;
uint32_t satcalStepStartTime;
uint32_t satcalTTF2DF;
uint32_t satcalTTF3DF;
const char* satcalStatusStr;
ubloxCFGGNSSState_t satcalGNSSConfigState_config;
ubloxCFGGNSSState_t satcalGNSSConfigState_factory;
bool     satcalIsCurrentConfigStateUnique;

/********************************************************************/
// forward declarations
/********************************************************************/
char* getGPSISO8601DateTimeStr();
char* getLatitudeStr(const float latitude);
char* getLongitudeStr(const float longitude);
void satcalCheckForFix();
void satCalibration_writeHeader();
void satCalibration_writeStats();
void satCalibration_setStatus(const char* status_);

/********************************************************************/
// Satellite Calibration
/********************************************************************/
bool satCalibration_enter() {
  satcalErrorCode = 0;
  satCalibration_setStatus("-Initializing GPS");
  satcalStepPeriod = deviceState.GPS_CALIBRATEPERIOD * 60000;
  // satcalStepPeriod = 10000; // for testing step sequence
  satcalStartTime = millis();
  if(gpsEnabled && gps.pollGNSSConfig()) {
    satcalUbloxModuleType = gps.getUbloxModuleType();
    satcalIsUbloxM10Module = satcalUbloxModuleType == UBLOX_M10_MODULE;
    satcalGNSSConfigState_config = gps.getGNSSConfigState();
    if(gps.factoryReset() &&
       gps.gnss_init(*gpsSerial, GPS_BAUD_RATE, GPS_COLDSTART, 0, 0, 0) &&
       gps.pollGNSSConfig()) {
      satcalGNSSConfigState_factory = gps.getGNSSConfigState();
      satcalIsCurrentConfigStateUnique = 
        (satcalGNSSConfigState_factory.GPS        != satcalGNSSConfigState_config.GPS) ||
        (satcalGNSSConfigState_factory.GPS_L1CA   != satcalGNSSConfigState_config.GPS_L1CA) ||
        (satcalGNSSConfigState_factory.SBAS       != satcalGNSSConfigState_config.SBAS) ||
        (satcalGNSSConfigState_factory.SBAS_L1CA  != satcalGNSSConfigState_config.SBAS_L1CA) ||
        (satcalGNSSConfigState_factory.Galileo    != satcalGNSSConfigState_config.Galileo) ||
        (satcalGNSSConfigState_factory.Galileo_E1 != satcalGNSSConfigState_config.Galileo_E1) ||
        (satcalGNSSConfigState_factory.BeiDou     != satcalGNSSConfigState_config.BeiDou) ||
        (satcalGNSSConfigState_factory.BeiDou_B1  != satcalGNSSConfigState_config.BeiDou_B1) ||
        (satcalGNSSConfigState_factory.BeiDou_B1C != satcalGNSSConfigState_config.BeiDou_B1C) ||
        (satcalGNSSConfigState_factory.IMES       != satcalGNSSConfigState_config.IMES) ||
        (satcalGNSSConfigState_factory.QZSS       != satcalGNSSConfigState_config.QZSS) ||
        (satcalGNSSConfigState_factory.QZSS_L1CA  != satcalGNSSConfigState_config.QZSS_L1CA) ||
        (satcalGNSSConfigState_factory.QZSS_L1S   != satcalGNSSConfigState_config.QZSS_L1S) ||
        (satcalGNSSConfigState_factory.GLONASS    != satcalGNSSConfigState_config.GLONASS) ||
        (satcalGNSSConfigState_factory.GLONASS_L1 != satcalGNSSConfigState_config.GLONASS_L1);
      if(sdcard_openGNSSCalibrateFile()) {
        satCalibration_writeHeader();
        satcalStep = SC_GPS_START;
        msg_update("GNSS CAL GPS SETUP");
        return true;
      } else {
        msg_update("GNSS CAL ERROR_03");
        satCalibration_setStatus("-SDCARD ERROR");
        satcalErrorCode = 3;
      }
    } else {
      msg_update("GNSS CAL ERROR_02");
      satCalibration_setStatus("-GPS RESET ERROR");
      satcalErrorCode = 2;
    }
  } else {
    satcalUbloxModuleType = UBLOX_UNKNOWN_MODULE;
    msg_update("GNSS CAL ERROR_01");
    satCalibration_setStatus("-GPS MODULE ERROR");
    satcalErrorCode = 1;
  }
  satcalStep = SC_END;
  return false;
}

/********************************************************************/
// call in main loop (not ISR safe)
void satCalibration_tick() {
  if(satcalStep == SC_END) return;
  switch(satcalStep) {
    case SC_GPS_START:
      if(gps.setGNSSConfigState(satcalGNSSConfigList.GPS) &&
         gps.gnss_init(*gpsSerial, GPS_BAUD_RATE, GPS_COLDSTART, 1, deviceState.GPS_UBXNAVSATRATE, 1)) {
        satcalStep = SC_GPS;
        satcalStepStartTime = millis();
        satcalTTF2DF = 0;
        satcalTTF3DF = 0;
        msg_update("GNSS CAL - GPS");
        satCalibration_setStatus("-GPS-Only SCAN");
      } else {
        msg_update("GNSS CAL ERROR_04");
        satCalibration_setStatus("-GPS CONFIG ERROR");
        satcalErrorCode = 4;
        satcalStep = SC_END_START;
      }
      break;
    case SC_GPS:
      if((millis() - satcalStepStartTime) < satcalStepPeriod) {
        satcalCheckForFix();
      } else {
        satCalibration_writeStats();
        satcalStep = SC_GAL_START;
      }
      break;
    case SC_GAL_START:
      if(gps.setGNSSConfigState(satcalGNSSConfigList.Galileo) &&
         gps.gnss_init(*gpsSerial, GPS_BAUD_RATE, GPS_COLDSTART, 1, deviceState.GPS_UBXNAVSATRATE, 1)) {
        satcalStep = SC_GAL;
        satcalStepStartTime = millis();
        satcalTTF2DF = 0;
        satcalTTF3DF = 0;
        msg_update("GNSS CAL - GAL");
        satCalibration_setStatus("-Galileo-Only SCAN");
      } else {
        msg_update("GNSS CAL ERROR_05");
        satCalibration_setStatus("-GAL CONFIG ERROR");
        satcalErrorCode = 5;
        satcalStep = SC_END_START;
      }
      break;
    case SC_GAL:
      if((millis() - satcalStepStartTime) < satcalStepPeriod) {
        satcalCheckForFix();
      } else {
        satCalibration_writeStats();
        satcalStep = SC_BDB1_START;
      }
      break;
    case SC_BDB1_START:
      if(gps.setGNSSConfigState(satcalGNSSConfigList.BeiDou_B1) &&
         gps.gnss_init(*gpsSerial, GPS_BAUD_RATE, GPS_COLDSTART, 1, deviceState.GPS_UBXNAVSATRATE, 1)) {
        satcalStep = SC_BDB1;
        satcalStepStartTime = millis();
        satcalTTF2DF = 0;
        satcalTTF3DF = 0;
        msg_update("GNSS CAL - BDB1");
        satCalibration_setStatus("-BeiDouB1-Only SCAN");
      } else {
        msg_update("GNSS CAL ERROR_06");
        satCalibration_setStatus("-BDB1 CONFIG ERROR");
        satcalErrorCode = 6;
        satcalStep = SC_END_START;
      }
      break;
    case SC_BDB1:
      if((millis() - satcalStepStartTime) < satcalStepPeriod) {
        satcalCheckForFix();
      } else {
        satCalibration_writeStats();
        if(satcalUbloxModuleType == UBLOX_M10_MODULE) {
          satcalStep = SC_BDB1C_START;
        } else {
          satcalStep = SC_GLO_START;
        }
      }
      break;
    case SC_BDB1C_START:
      if(gps.setGNSSConfigState(satcalGNSSConfigList.BeiDou_B1C) &&
         gps.gnss_init(*gpsSerial, GPS_BAUD_RATE, GPS_COLDSTART, 1, deviceState.GPS_UBXNAVSATRATE, 1)) {
        satcalStep = SC_BDB1C;
        satcalStepStartTime = millis();
        satcalTTF2DF = 0;
        satcalTTF3DF = 0;
        msg_update("GNSS CAL - BDB1C");
        satCalibration_setStatus("-BeiDouB1C-Only SCAN");
      } else {
        msg_update("GNSS CAL ERROR_07");
        satCalibration_setStatus("-BDB1C CONFIG ERROR");
        satcalErrorCode = 7;
        satcalStep = SC_END_START;
      }
      break;
    case SC_BDB1C:
      if((millis() - satcalStepStartTime) < satcalStepPeriod) {
        satcalCheckForFix();
      } else {
        satCalibration_writeStats();
        satcalStep = SC_GLO_START;
      }
      break;
    case SC_GLO_START:
      if(gps.setGNSSConfigState(satcalGNSSConfigList.GLONASS) &&
         gps.gnss_init(*gpsSerial, GPS_BAUD_RATE, GPS_COLDSTART, 1, deviceState.GPS_UBXNAVSATRATE, 1)) {
        satcalStep = SC_GLO;
        satcalStepStartTime = millis();
        satcalTTF2DF = 0;
        satcalTTF3DF = 0;
        msg_update("GNSS CAL - GLO");
        satCalibration_setStatus("-GLONASS-Only SCAN");
      } else {
        msg_update("GNSS CAL ERROR_08");
        satCalibration_setStatus("-GLO CONFIG ERROR");
        satcalErrorCode = 8;
        satcalStep = SC_END_START;
      }
      break;
    case SC_GLO:
      if((millis() - satcalStepStartTime) < satcalStepPeriod) {
        satcalCheckForFix();
      } else {
        satCalibration_writeStats();
        satcalStep = SC_FACTORY_START;
      }
      break;
    case SC_FACTORY_START:
      if(gps.setGNSSConfigState(satcalGNSSConfigState_factory) &&
         gps.gnss_init(*gpsSerial, GPS_BAUD_RATE, GPS_COLDSTART, 1, deviceState.GPS_UBXNAVSATRATE, 1)) {
        satcalGNSSConfigState_factory = gps.getGNSSConfigState();
        satcalStep = SC_FACTORY;
        satcalStepStartTime = millis();
        satcalTTF2DF = 0;
        satcalTTF3DF = 0;
        msg_update("GNSS CAL - FACTORY");
        satCalibration_setStatus("-FACTORY-DFLT SCAN");
      } else {
        msg_update("GNSS CAL ERROR_09");
        satCalibration_setStatus("-FACTORY CFG ERROR");
        satcalErrorCode = 9;
        satcalStep = SC_END_START;
      }
      break;
    case SC_FACTORY:
      if((millis() - satcalStepStartTime) < satcalStepPeriod) {
        satcalCheckForFix();
      } else {
        satCalibration_writeStats();
        if(satcalIsCurrentConfigStateUnique) {
          satcalStep = SC_CURRENT_START;
        } else {
          satcalStep = SC_CURRENT;
          satCalibration_writeStats();
          satcalStep = SC_END_START;
        }
      }
      break;
    case SC_CURRENT_START:
      if(gps.setGNSSConfigState(satcalGNSSConfigState_config) &&
         gps.gnss_init(*gpsSerial, GPS_BAUD_RATE, GPS_COLDSTART, 1, deviceState.GPS_UBXNAVSATRATE, 1)) {
        satcalStep = SC_CURRENT;
        satcalStepStartTime = millis();
        satcalTTF2DF = 0;
        satcalTTF3DF = 0;
        msg_update("GNSS CAL - CURRENT");
        satCalibration_setStatus("-CURRENT-CFG SCAN");
      } else {
        msg_update("GNSS CAL ERROR_10");
        satCalibration_setStatus("-CURRENT CFG ERROR");
        satcalErrorCode = 10;
        satcalStep = SC_END_START;
      }
      break;
    case SC_CURRENT:
      if((millis() - satcalStepStartTime) < satcalStepPeriod) {
        satcalCheckForFix();
      } else {
        satCalibration_writeStats();
        satcalStep = SC_END_START;
      }
      break;
    case SC_END_START:
      if(gps.setGNSSConfigState(satcalGNSSConfigState_config) &&
         gps.gnss_init(*gpsSerial, GPS_BAUD_RATE, GPS_COLDSTART, 0, 0, 0)) {
        if(satcalErrorCode == 0) {
          satCalibration_setStatus("-SCAN COMPLETE");
          msg_update("GNSS CAL END");
        }
      } else {
        msg_update("GNSS CAL ERROR_11");
        satCalibration_setStatus("-GNSS RESTORE ERROR");
        satcalErrorCode = 11;
      }
      sdcard_closeGNSSCalibrateFile();
      satcalStep = SC_END;
      break;
  }
}

/********************************************************************/
void satCalibration_exit() {
  if(satcalStep != SC_END) {
    if(satcalErrorCode == 0) {
      msg_update("GNSS CAL TERMINATED");
      satCalibration_setStatus("-SCAN TERMINATED");
    }
    satcalStep = SC_END_START;
    satCalibration_tick();
  }
}

/********************************************************************/
void satcalCheckForFix() {
  // don't use last step's NAVSTATUS data
  if((millis() - satcalStepStartTime) < 2000) return;
  if((satcalTTF2DF != 0) && (satcalTTF3DF !=0)) return;
  ubloxNAVSTATUSInfo_t navstatusInfo;
  gps.getNAVSTATUSInfo(navstatusInfo);
  if(!navstatusInfo.validPacket) return;
  if((satcalTTF2DF == 0) &&
     ((navstatusInfo.gpsFix == 2) || (navstatusInfo.gpsFix == 3))) {
    satcalTTF2DF = millis() - satcalStepStartTime;
  }
  if((satcalTTF3DF == 0) && (navstatusInfo.gpsFix == 3)) {
    satcalTTF3DF = millis() - satcalStepStartTime;
  }
}

/********************************************************************/
void satCalibration_writeHeader() {
  char _tempStr[81];
  sprintf(_tempStr, "UbloxModule,CalibrationTime,GNSSSystem,GMT,Latitude,Longitude,");
  sdcard_writeGNSSCalibrateFile((uint8_t*)_tempStr, strlen(_tempStr));
  sprintf(_tempStr, "GPSFixOK,GPSFix,TTFF,TTF2DF,TTF3DF,PSMState,CarrSoln,");
  sdcard_writeGNSSCalibrateFile((uint8_t*)_tempStr, strlen(_tempStr));
  sprintf(_tempStr, "SpoofDetState,SpoofIndicated,MultiSpoofInd,");
  sdcard_writeGNSSCalibrateFile((uint8_t*)_tempStr, strlen(_tempStr));
  sprintf(_tempStr, "TotalSatellites,HealthySignal,EphemerisValid,UsedForNav,Sats\n");
  sdcard_writeGNSSCalibrateFile((uint8_t*)_tempStr, strlen(_tempStr));
}

/********************************************************************/
void satCalibration_writeStats() {
  char _tempStr[81];
  sprintf(_tempStr, "M%d,", satcalUbloxModuleType);
  sdcard_writeGNSSCalibrateFile((uint8_t*)_tempStr, strlen(_tempStr));
  sprintf(_tempStr, "%d minutes,", (satcalStepPeriod / 60000));
  sdcard_writeGNSSCalibrateFile((uint8_t*)_tempStr, strlen(_tempStr));
  switch(satcalStep) {
    case SC_GPS:
      sprintf(_tempStr, "GPS,");
      break;
    case SC_GAL:
      sprintf(_tempStr, "Galileo,");
      break;
    case SC_BDB1:
      sprintf(_tempStr, "BeiDou_B1,");
      break;
    case SC_BDB1C:
      sprintf(_tempStr, "BeiDou_B1C,");
      break;
    case SC_GLO:
      sprintf(_tempStr, "GLONASS,");
      break;
    case SC_FACTORY:
      sprintf(_tempStr, "FACTORY,");
      break;
    case SC_CURRENT:
      sprintf(_tempStr, "CURRENT,");
      break;
  }
  sdcard_writeGNSSCalibrateFile((uint8_t*)_tempStr, strlen(_tempStr));
  // NAVPVT Data
  if(gps.isPacketValid()) {
    if(gps.isDateValid() && gps.isTimeValid()) {
      rtc_datetime_t dateTime;
      dateTime.year   = gps.getYear();
      dateTime.month  = gps.getMonth();
      dateTime.day    = gps.getDay();
      dateTime.hour   = gps.getHour();
      dateTime.minute = gps.getMinute();
      dateTime.second = gps.getSecond();
      sprintf(_tempStr, "%sZ,", rtc.dateTimeToISO8601Str(dateTime));
    } else {
      sprintf(_tempStr, ",");
    }
    sdcard_writeGNSSCalibrateFile((uint8_t*)_tempStr, strlen(_tempStr));
    if(gps.isLocationValid()) {
      sprintf(_tempStr, "%s,%s,",
              getLatitudeStr(gps.getLatitude()),
              getLongitudeStr(gps.getLongitude()));
    } else {
      sprintf(_tempStr, ",,");
    }
    sdcard_writeGNSSCalibrateFile((uint8_t*)_tempStr, strlen(_tempStr));
  }
  // NAVSTATUS Data
  ubloxNAVSTATUSInfo_t navstatusInfo;
  gps.getNAVSTATUSInfo(navstatusInfo);
  if(navstatusInfo.validPacket) {
    sprintf(_tempStr, "%c,%XD,%d,%d,%d,%X,%d,%d,%d,%d,",
            navstatusInfo.gpsFixOk ? 'T' : 'F',
            navstatusInfo.gpsFix,
            ((navstatusInfo.ttff == 0) ? 0 :
               ((navstatusInfo.ttff > 1000) ? (navstatusInfo.ttff / 1000) : 1)),
            ((satcalTTF2DF == 0) ? 0 :
               ((satcalTTF2DF > 1000) ? (satcalTTF2DF / 1000) : 1)),
            ((satcalTTF3DF == 0) ? 0 :
               ((satcalTTF3DF > 1000) ? (satcalTTF3DF / 1000) : 1)),
            navstatusInfo.psmState,
            navstatusInfo.carrSoln,
            navstatusInfo.spoofDetState,
            navstatusInfo.spoofingIndicated,
            navstatusInfo.multipleSpoofingIndications);
  } else {
    sprintf(_tempStr, ",,,,,,,,,,");
  }
  sdcard_writeGNSSCalibrateFile((uint8_t*)_tempStr, strlen(_tempStr));
  // NAVSAT Data
  ubloxNAVSATInfo_t navsatInfo;
  gps.getNAVSATInfo(navsatInfo);
  if(navsatInfo.validPacket) {
    sprintf(_tempStr, "%d,%d,%d,%d",
            navsatInfo.numSvs,
            navsatInfo.numSvsHealthy,
            navsatInfo.numSvsEphValid,
            navsatInfo.numSvsUsed);
    sdcard_writeGNSSCalibrateFile((uint8_t*)_tempStr, strlen(_tempStr));
    if(navsatInfo.numSvsHealthy > 0) {
      for(uint8_t i=0; i<navsatInfo.numSvsHealthy; i++) {
        sprintf(_tempStr, ",%c%02d/%02d/%02d\xB0/%03d\xB0",
                navsatInfo.svSortList[i].gnssIdType,
                navsatInfo.svSortList[i].svId,
                navsatInfo.svSortList[i].cno,
                navsatInfo.svSortList[i].elev,
                navsatInfo.svSortList[i].azim);
        sdcard_writeGNSSCalibrateFile((uint8_t*)_tempStr, strlen(_tempStr));
      }
    }
  } else {
    sprintf(_tempStr, ",,,");
    sdcard_writeGNSSCalibrateFile((uint8_t*)_tempStr, strlen(_tempStr));
  }
  sprintf(_tempStr, "\n");
  sdcard_writeGNSSCalibrateFile((uint8_t*)_tempStr, strlen(_tempStr));
}

/********************************************************************/
void satCalibration_setStatus(const char* status_) {
  satcalStatusStr = status_;
}

/********************************************************************/
const char* satCalibration_getStatus() {
  return satcalStatusStr;
}

/********************************************************************/
uint32_t satCalibration_getTimeRemaining() {
  uint32_t _timeUsed = millis() - satcalStepStartTime;
  uint32_t _timeRemaining = (_timeUsed <= satcalStepPeriod) ?
                              (satcalStepPeriod - _timeUsed) : 0;
  uint32_t _totalTime;
  uint32_t _remainingStepCount;
  switch(satcalStep) {
    case SC_GPS_START:
      _remainingStepCount = 5 + (satcalIsUbloxM10Module ? 1 : 0) +
                                (satcalIsCurrentConfigStateUnique ? 1 : 0);
      _timeRemaining = satcalStepPeriod * _remainingStepCount;
      break;
    case SC_GPS:
    case SC_GAL_START:
      _remainingStepCount = 4 + (satcalIsUbloxM10Module ? 1 : 0) +
                                (satcalIsCurrentConfigStateUnique ? 1 : 0);
      _timeRemaining = _timeRemaining + (satcalStepPeriod * _remainingStepCount);
      break;
    case SC_GAL:
    case SC_BDB1_START:
      _remainingStepCount = 3 + (satcalIsUbloxM10Module ? 1 : 0) +
                                (satcalIsCurrentConfigStateUnique ? 1 : 0);
      _timeRemaining = _timeRemaining + (satcalStepPeriod * _remainingStepCount);
      break;
    case SC_BDB1:
    case SC_BDB1C_START:
      _remainingStepCount = 2 + (satcalIsUbloxM10Module ? 1 : 0) +
                                (satcalIsCurrentConfigStateUnique ? 1 : 0);
      _timeRemaining = _timeRemaining + (satcalStepPeriod * _remainingStepCount);
      break;
    case SC_BDB1C:
    case SC_GLO_START:
      _remainingStepCount = 2 + (satcalIsCurrentConfigStateUnique ? 1 : 0);
      _timeRemaining = _timeRemaining + (satcalStepPeriod * _remainingStepCount);
      break;
    case SC_GLO:
    case SC_FACTORY_START:
      _remainingStepCount = 1 + (satcalIsCurrentConfigStateUnique ? 1 : 0);
      _timeRemaining = _timeRemaining + (satcalStepPeriod * _remainingStepCount);
      break;
    case SC_FACTORY:
    case SC_CURRENT_START:
      _remainingStepCount = 0 + (satcalIsCurrentConfigStateUnique ? 1 : 0);
      _timeRemaining = _timeRemaining + (satcalStepPeriod * _remainingStepCount);
      break;
    case SC_CURRENT:
      _timeRemaining = _timeRemaining;
      break;
    case SC_END_START:
    case SC_END:
      _timeRemaining = 0;
      break;
  }
  return _timeRemaining;
}

