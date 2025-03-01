
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
  SC_START = 0,
  SC_GPS,
  SC_GAL_START,
  SC_GAL,
  SC_BDB1_START,
  SC_BDB1,
  SC_BDB1C_START,
  SC_BDB1C,
  SC_GLO_START,
  SC_GLO,
  SC_END_START,
  SC_END
};

/********************************************************************/
// Global Variables
/********************************************************************/
uint8_t  satcalUbloxModuleType;
uint8_t  satcalStep;
uint32_t satcalStepPeriod;
uint32_t satcalStartTime;
uint32_t satcalStepStartTime;
const char* satcalStatusStr;
ubloxCFGGNSSState_t satcalGNSSConfigState_save;

/********************************************************************/
// forward declarations
/********************************************************************/
char* getGPSISO8601DateTimeStr();
char* getLatitudeStr(const float latitude);
char* getLongitudeStr(const float longitude);
void satCalibration_writeStats();
void satCalibration_setStatus(const char* status_);

/********************************************************************/
// Satellite Calibration
/********************************************************************/
bool satCalibration_enter() {
  satCalibration_setStatus("-Initializing GPS");
  satcalStepPeriod = deviceState.GPSCALIBRATIONPERIOD * 60000;
  satcalStartTime = millis();
  if(gpsEnabled && gps.pollGNSSConfig()) {
    satcalGNSSConfigState_save = gps.getGNSSConfigState();
    satcalUbloxModuleType = gps.getUbloxModuleType();
    if(sdcard_openGNSSCalibrateFile()) {
      satcalStep = SC_START;
      msg_update("GNSS CAL GPS SETUP");
      return true;
    } else {
      satcalStep = SC_END;
      msg_update("GNSS CAL ERROR_00");
    }
  } else {
    satcalUbloxModuleType = UBLOX_UNKNOWN_MODULE;
    satcalStep = SC_END;
    msg_update("GNSS CAL ERROR_01");
  }
  return false;
}

/********************************************************************/
// call in main loop (not ISR safe)
void satCalibration_tick() {
  char _tempStr[80];
  if(satcalStep == SC_END) return;
  switch(satcalStep) {
    case SC_START:
      if(gps.setGNSSConfigState(satcalGNSSConfigList.GPS) &&
         gps.gnss_init(*gpsSerial, GPS_BAUD_RATE, GPS_COLDSTART, 1, 1, 10)) {
        satcalStep = SC_GPS;
        satcalStepStartTime = millis();
        sprintf(_tempStr, "UbloxModule = M%d\n", satcalUbloxModuleType);
        sdcard_writeGNSSCalibrateFile((uint8_t*)_tempStr, strlen(_tempStr));
        sprintf(_tempStr, "Calibration Step Time = %d minutes\n",
                (satcalStepPeriod / 60000));
        sdcard_writeGNSSCalibrateFile((uint8_t*)_tempStr, strlen(_tempStr));
        sprintf(_tempStr, "**************** GNSS CAL - GPS ****************\n");
        sdcard_writeGNSSCalibrateFile((uint8_t*)_tempStr, strlen(_tempStr));
        msg_update("GNSS CAL - GPS");
        satCalibration_setStatus("-GPS-Only SCAN");
      } else {
        satCalibration_exit();
        msg_update("GNSS CAL ERROR_02");
      }
      break;
    case SC_GPS:
      if((millis() - satcalStepStartTime) >= satcalStepPeriod) {
        satCalibration_writeStats();
        satcalStep = SC_GAL_START;
      }
      break;
    case SC_GAL_START:
      if(gps.setGNSSConfigState(satcalGNSSConfigList.Galileo) &&
         gps.gnss_init(*gpsSerial, GPS_BAUD_RATE, GPS_COLDSTART, 1, 1, 10)) {
        satcalStep = SC_GAL;
        satcalStepStartTime = millis();
        sprintf(_tempStr, "**************** GNSS CAL - Galileo ****************\n");
        sdcard_writeGNSSCalibrateFile((uint8_t*)_tempStr, strlen(_tempStr));
        msg_update("GNSS CAL - GAL");
        satCalibration_setStatus("-Galileo-Only SCAN");
      } else {
        sprintf(_tempStr, "GNSS CAL ERROR_03\n");
        sdcard_writeGNSSCalibrateFile((uint8_t*)_tempStr, strlen(_tempStr));
        msg_update("GNSS CAL ERROR_03");
        satCalibration_exit();
      }
      break;
    case SC_GAL:
      if((millis() - satcalStepStartTime) >= satcalStepPeriod) {
        satCalibration_writeStats();
        satcalStep = SC_BDB1_START;
      }
      break;
    case SC_BDB1_START:
      if(gps.setGNSSConfigState(satcalGNSSConfigList.BeiDou_B1) &&
         gps.gnss_init(*gpsSerial, GPS_BAUD_RATE, GPS_COLDSTART, 1, 1, 10)) {
        satcalStep = SC_BDB1;
        satcalStepStartTime = millis();
        sprintf(_tempStr, "**************** GNSS CAL - BeiDou_B1 ****************\n");
        sdcard_writeGNSSCalibrateFile((uint8_t*)_tempStr, strlen(_tempStr));
        msg_update("GNSS CAL - BDB1");
        satCalibration_setStatus("-BeiDouB1-Only SCAN");
      } else {
        sprintf(_tempStr, "GNSS CAL ERROR_04\n");
        sdcard_writeGNSSCalibrateFile((uint8_t*)_tempStr, strlen(_tempStr));
        msg_update("GNSS CAL ERROR_04");
        satCalibration_exit();
      }
      break;
    case SC_BDB1:
      if((millis() - satcalStepStartTime) >= satcalStepPeriod) {
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
         gps.gnss_init(*gpsSerial, GPS_BAUD_RATE, GPS_COLDSTART, 1, 1, 10)) {
        satcalStep = SC_BDB1C;
        satcalStepStartTime = millis();
        sprintf(_tempStr, "**************** GNSS CAL - BeiDou_B1C ****************\n");
        sdcard_writeGNSSCalibrateFile((uint8_t*)_tempStr, strlen(_tempStr));
        msg_update("GNSS CAL - BDB1C");
        satCalibration_setStatus("-BeiDouB1C-Only SCAN");
      } else {
        sprintf(_tempStr, "GNSS CAL ERROR_05\n");
        sdcard_writeGNSSCalibrateFile((uint8_t*)_tempStr, strlen(_tempStr));
        msg_update("GNSS CAL ERROR_05");
        satCalibration_exit();
      }
      break;
    case SC_BDB1C:
      if((millis() - satcalStepStartTime) >= satcalStepPeriod) {
        satCalibration_writeStats();
        satcalStep = SC_GLO_START;
      }
      break;
    case SC_GLO_START:
      if(gps.setGNSSConfigState(satcalGNSSConfigList.GLONASS) &&
         gps.gnss_init(*gpsSerial, GPS_BAUD_RATE, GPS_COLDSTART, 1, 1, 10)) {
        satcalStep = SC_GLO;
        satcalStepStartTime = millis();
        sprintf(_tempStr, "**************** GNSS CAL - GLONASS ****************\n");
        sdcard_writeGNSSCalibrateFile((uint8_t*)_tempStr, strlen(_tempStr));
        msg_update("GNSS CAL - GLO");
        satCalibration_setStatus("-GLONASS-Only SCAN");
      } else {
        sprintf(_tempStr, "GNSS CAL ERROR_07\n");
        sdcard_writeGNSSCalibrateFile((uint8_t*)_tempStr, strlen(_tempStr));
        msg_update("GNSS CAL ERROR_07");
        satCalibration_exit();
      }
      break;
    case SC_GLO:
      if((millis() - satcalStepStartTime) >= satcalStepPeriod) {
        satCalibration_writeStats();
        satcalStep = SC_END_START;
      }
      break;
    case SC_END_START:
      if(gps.setGNSSConfigState(satcalGNSSConfigState_save) &&
         gps.gnss_init(*gpsSerial, GPS_BAUD_RATE, GPS_COLDSTART, 0, 0, 0)) {
        satcalStep = SC_END;
        sprintf(_tempStr, "**************** GNSS CAL END ****************\n");
        sdcard_writeGNSSCalibrateFile((uint8_t*)_tempStr, strlen(_tempStr));
        sdcard_closeGNSSCalibrateFile();
        msg_update("GNSS CAL END");
      } else {
        sprintf(_tempStr, "GNSS CAL ERROR_08\n");
        sdcard_writeGNSSCalibrateFile((uint8_t*)_tempStr, strlen(_tempStr));
        msg_update("GNSS CAL ERROR_08");
        satCalibration_exit();
      }
      satCalibration_setStatus("-SCAN COMPLETE");
      break;
  }
}

/********************************************************************/
void satCalibration_writeStats() {
  char _tempStr[80];
  // NAVPVT GPS Clock
  if(gps.isPacketValid()) {
    sprintf(_tempStr, "%s\n", getGPSISO8601DateTimeStr());
    sdcard_writeGNSSCalibrateFile((uint8_t*)_tempStr, strlen(_tempStr));
    if(gps.isLocationValid()) {
      sprintf(_tempStr, "Location: %s %s\n",
              getLatitudeStr(gps.getLatitude()),
              getLongitudeStr(gps.getLongitude()));
    } else {
      sprintf(_tempStr, "**  NO GNSS FIX  **\n");
    }
    sdcard_writeGNSSCalibrateFile((uint8_t*)_tempStr, strlen(_tempStr));
  } else {
    sprintf(_tempStr, "** NO NAVPVT DATA **\n");
    sdcard_writeGNSSCalibrateFile((uint8_t*)_tempStr, strlen(_tempStr));
  }
  // NAVSTATUS Data
  ubloxNAVSTATUSInfo_t navstatusInfo;
  gps.getNAVSTATUSInfo(navstatusInfo);
  if(navstatusInfo.validPacket) {
    sprintf(_tempStr, "**** NAVSTAT DATA ****\n");
    sdcard_writeGNSSCalibrateFile((uint8_t*)_tempStr, strlen(_tempStr));
    sprintf(_tempStr, "GPSFixOK=%c\n", navstatusInfo.gpsFixOk ? 'T' : 'F');
    sdcard_writeGNSSCalibrateFile((uint8_t*)_tempStr, strlen(_tempStr));
    sprintf(_tempStr, "GPSFix=%02X\n", navstatusInfo.gpsFix);
    sdcard_writeGNSSCalibrateFile((uint8_t*)_tempStr, strlen(_tempStr));
    sprintf(_tempStr, "PSMState=%02X\n", navstatusInfo.psmState);
    sdcard_writeGNSSCalibrateFile((uint8_t*)_tempStr, strlen(_tempStr));
    sprintf(_tempStr, "CarrSoln=%02d\n", navstatusInfo.carrSoln);
    sdcard_writeGNSSCalibrateFile((uint8_t*)_tempStr, strlen(_tempStr));
    sprintf(_tempStr, "TTFF=%08d\n", navstatusInfo.ttff / 1000);
    sdcard_writeGNSSCalibrateFile((uint8_t*)_tempStr, strlen(_tempStr));
    sprintf(_tempStr, "SpoofDetState=%d\n", navstatusInfo.spoofDetState);
    sdcard_writeGNSSCalibrateFile((uint8_t*)_tempStr, strlen(_tempStr));
    sprintf(_tempStr, "SpoofIndicated=%d\n", navstatusInfo.spoofingIndicated);
    sdcard_writeGNSSCalibrateFile((uint8_t*)_tempStr, strlen(_tempStr));
    sprintf(_tempStr, "MultiSpoofInd=%d\n", navstatusInfo.multipleSpoofingIndications);
    sdcard_writeGNSSCalibrateFile((uint8_t*)_tempStr, strlen(_tempStr));
  } else {
    sprintf(_tempStr, "** NO NAVSTAT DATA **\n");
    sdcard_writeGNSSCalibrateFile((uint8_t*)_tempStr, strlen(_tempStr));
  }
  // NAVSAT Data
  ubloxNAVSATInfo_t navsatInfo;
  gps.getNAVSATInfo(navsatInfo);
  if(navsatInfo.validPacket) {
    sprintf(_tempStr, "**** NAVSAT DATA ****\n");
    sdcard_writeGNSSCalibrateFile((uint8_t*)_tempStr, strlen(_tempStr));
    sprintf(_tempStr, "Total=%d/%d\n", navsatInfo.numSvs, navsatInfo.numSvsReceived);
    sdcard_writeGNSSCalibrateFile((uint8_t*)_tempStr, strlen(_tempStr));
    sprintf(_tempStr, "HealthySignal=%d\n", navsatInfo.numSvsHealthy);
    sdcard_writeGNSSCalibrateFile((uint8_t*)_tempStr, strlen(_tempStr));
    sprintf(_tempStr, "EphemerisValid=%d\n", navsatInfo.numSvsEphValid);
    sdcard_writeGNSSCalibrateFile((uint8_t*)_tempStr, strlen(_tempStr));
    sprintf(_tempStr, "UsedForNav=%d\n", navsatInfo.numSvsUsed);
    sdcard_writeGNSSCalibrateFile((uint8_t*)_tempStr, strlen(_tempStr));
    sprintf(_tempStr, "Satellites(id/snr):\n");
    sdcard_writeGNSSCalibrateFile((uint8_t*)_tempStr, strlen(_tempStr));
    if(navsatInfo.numSvsHealthy > 0) {
      for(uint8_t i=0; i<navsatInfo.numSvsHealthy; i++) {
        sprintf(_tempStr, "%c%02d/%02d ",
                navsatInfo.svSortList[i].gnssIdType,
                navsatInfo.svSortList[i].svId,
                navsatInfo.svSortList[i].cno);
        sdcard_writeGNSSCalibrateFile((uint8_t*)_tempStr, strlen(_tempStr));
        if(((i % 6) == 5) || i == (navsatInfo.numSvsHealthy-1)) {
          sprintf(_tempStr, "\n");
          sdcard_writeGNSSCalibrateFile((uint8_t*)_tempStr, strlen(_tempStr));
        }
      }
    } else {
      sprintf(_tempStr, "** NO HEALTHY SATELLITES **\n");
      sdcard_writeGNSSCalibrateFile((uint8_t*)_tempStr, strlen(_tempStr));
    }
  } else {
    sprintf(_tempStr, "** NO NAVSAT DATA **\n");
    sdcard_writeGNSSCalibrateFile((uint8_t*)_tempStr, strlen(_tempStr));
  }
}

/********************************************************************/
void satCalibration_exit() {
  char _tempStr[80];
  if(satcalStep != SC_END) {
    satcalStep = SC_END;
    if(gps.setGNSSConfigState(satcalGNSSConfigState_save) &&
       gps.gnss_init(*gpsSerial, GPS_BAUD_RATE, GPS_COLDSTART, 0, 0, 0)) {
      sprintf(_tempStr, "**************** GNSS CAL EARLY EXIT ****************\n");
      sdcard_writeGNSSCalibrateFile((uint8_t*)_tempStr, strlen(_tempStr));
      sdcard_closeGNSSCalibrateFile();
      msg_update("GNSS CAL END");
      satCalibration_setStatus("-SCAN TERMINATED");
    } else {
      sprintf(_tempStr, "**************** GNSS CAL EARLY EXIT ERROR ****************\n");
      sdcard_writeGNSSCalibrateFile((uint8_t*)_tempStr, strlen(_tempStr));
      sdcard_closeGNSSCalibrateFile();
      msg_update("GNSS CAL ERROR_02");
      satCalibration_setStatus("-SCAN ERROR");
    }
  }
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
  bool isUbloxM10Module = satcalUbloxModuleType == UBLOX_M10_MODULE;
  switch(satcalStep) {
    case SC_START:
      _timeRemaining = satcalStepPeriod * (isUbloxM10Module ? 5 : 4);
      break;
    case SC_GPS:
    case SC_GAL_START:
      _timeRemaining = _timeRemaining + (satcalStepPeriod * (isUbloxM10Module ? 4 : 3));
      break;
    case SC_GAL:
    case SC_BDB1_START:
      _timeRemaining = _timeRemaining + (satcalStepPeriod * (isUbloxM10Module ? 3 : 2));
      break;
    case SC_BDB1:
    case SC_BDB1C_START:
      _timeRemaining = _timeRemaining + (satcalStepPeriod * (isUbloxM10Module ? 2 : 1));
      break;
    case SC_BDB1C:
    case SC_GLO_START:
      _timeRemaining = _timeRemaining + (satcalStepPeriod * (isUbloxM10Module ? 1 : 0));
      break;
    case SC_GLO:
      _timeRemaining = _timeRemaining;
      break;
    case SC_END_START:
    case SC_END:
      _timeRemaining = 0;
      break;
  }
  return _timeRemaining;
}

