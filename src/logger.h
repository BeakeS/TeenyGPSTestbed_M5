
/********************************************************************/
// LOGGER
/********************************************************************/
// global variables
bool gpsLoggingInProgress = false;
struct LogData_struct {
  // NAVPVT
  uint8_t  navpvtPacket[UBX_NAV_PVT_PACKETLENGTH];
  bool     navpvtIsLocationValid = false;
  uint8_t  pad00a;
  uint8_t  pad00b;
  uint8_t  pad00c;
  bool     enqueueWriteNAVPVTPkt = false;
  uint8_t  pad01a;
  uint8_t  pad01b;
  uint8_t  pad01c;
  char     navpvtGPXStr[256];
  bool     enqueueWriteNAVPVTGPXRecord = false;
  uint8_t  pad02a;
  uint8_t  pad02b;
  uint8_t  pad02c;
  char     navpvtKMLStr[256];
  bool     enqueueWriteNAVPVTKMLRecord = false;
  uint8_t  pad03a;
  uint8_t  pad03b;
  uint8_t  pad03c;
  char     navpvtCSVStr[256];
  bool     enqueueWriteNAVPVTCSVRecord = false;
  uint8_t  pad04a;
  uint8_t  pad04b;
  uint8_t  pad04c;
  bool     sentNAVPVTCSVRecord = false;
  uint8_t  pad05a;
  uint8_t  pad05b;
  uint8_t  pad05c;
  // NAVSAT
  uint8_t  navsatPacket[UBX_NAV_SAT_MAXPACKETLENGTH];
  uint16_t navsatPacketLength;
  uint8_t  pad06a;
  uint8_t  pad06b;
  bool     enqueueWriteNAVSATPkt = false;
  uint8_t  pad07a;
  uint8_t  pad07b;
  uint8_t  pad07c;
  char     navsatCSVStr[2048];
  bool     enqueueWriteNAVSATCSVRecord = false;
  uint8_t  pad08a;
  uint8_t  pad08b;
  uint8_t  pad08c;
  bool     sentNAVSATCSVRecord = false;
  uint8_t  pad09a;
  uint8_t  pad09b;
  uint8_t  pad09c;
  // NAVSTATUS
  uint8_t  navstatusPacket[UBX_NAV_STATUS_PACKETLENGTH];
  bool     enqueueWriteNAVSTATUSPkt = false;
  uint8_t  pad10a;
  uint8_t  pad10b;
  uint8_t  pad10c;
  char     navstatusCSVStr[256];
  bool     enqueueWriteNAVSTATUSCSVRecord = false;
  uint8_t  pad11a;
  uint8_t  pad11b;
  uint8_t  pad11c;
  bool     sentNAVSTATUSCSVRecord = false;
  uint8_t  pad12a;
  uint8_t  pad12b;
  uint8_t  pad12c;
} __attribute__ ((__packed__));
LogData_struct logData;

/********************************************************************/
// forward declarations

/********************************************************************/
bool logger_update() {
  bool _displayRefresh = false;

  // Log packets and records received (so packets are recorded in order received)
  if(logData.enqueueWriteNAVPVTPkt) {
    //SATNAVPVT Packet
    logData.enqueueWriteNAVPVTPkt = false;
    sdcard_writeUBXLoggingFile(logData.navpvtPacket, UBX_NAV_PVT_PACKETLENGTH, logData.navpvtIsLocationValid);
  } else if(logData.enqueueWriteNAVSATPkt) {
    // NAVSAT Packet
    logData.enqueueWriteNAVSATPkt = false;
    sdcard_writeUBXLoggingFile(logData.navsatPacket, logData.navsatPacketLength, false, true);
  } else if(logData.enqueueWriteNAVSTATUSPkt) {
    // NAVSTATUS Packet
    logData.enqueueWriteNAVSTATUSPkt = false;
    sdcard_writeUBXLoggingFile(logData.navstatusPacket, UBX_NAV_STATUS_PACKETLENGTH, false, true);
  } else if(logData.enqueueWriteNAVPVTGPXRecord) {
    // NAVPVT GPX
    logData.enqueueWriteNAVPVTGPXRecord = false;
    sdcard_writeGPXLoggingFile((uint8_t*)logData.navpvtGPXStr, strlen(logData.navpvtGPXStr));
  } else if(logData.enqueueWriteNAVPVTKMLRecord) {
    // NAVPVT KML
    logData.enqueueWriteNAVPVTKMLRecord = false;
    sdcard_writeKMLLoggingFile((uint8_t*)logData.navpvtKMLStr, strlen(logData.navpvtKMLStr));
  } else if(logData.enqueueWriteNAVPVTCSVRecord) {
    // NAVPVT CSV
    logData.enqueueWriteNAVPVTCSVRecord = false;
    sdcard_writeCSVLoggingFile((uint8_t*)logData.navpvtCSVStr, strlen(logData.navpvtCSVStr));
    logData.sentNAVPVTCSVRecord = true;
  } else if(logData.enqueueWriteNAVSTATUSCSVRecord && logData.sentNAVPVTCSVRecord) {
    // NAVSTATUS CSV - Send NAVSTATUS before NAVSAT because NAVSAT has variable packet length
    logData.enqueueWriteNAVSTATUSCSVRecord = false;
    sdcard_writeCSVLoggingFile((uint8_t*)logData.navstatusCSVStr, strlen(logData.navstatusCSVStr), true);
    logData.sentNAVSATCSVRecord = true;
  } else if(logData.enqueueWriteNAVSATCSVRecord && logData.sentNAVSATCSVRecord) {
    // NAVSAT CSV
    logData.enqueueWriteNAVSATCSVRecord = false;
    sdcard_writeCSVLoggingFile((uint8_t*)logData.navsatCSVStr, strlen(logData.navsatCSVStr), true);
    logData.sentNAVSTATUSCSVRecord = true;
  // Check/receive packets
  } else if(gps.getNAVPVT()) {
    // NAVPVT
    if((!rtc.isValid()) && gps.isDateValid() && gps.isTimeValid()) {
      rtc.setRTCTime(gps.getYear(), gps.getMonth(), gps.getDay(),
                     gps.getHour(), gps.getMinute(), gps.getSecond());
    }
    if(gpsLoggingInProgress &&
       ((deviceState.GPS_LOGUBXMODE == GPS_LOGUBX_NAVPVT) ||
        (deviceState.GPS_LOGUBXMODE == GPS_LOGUBX_ALL))) {
      // UBX packet logging
      gps.getNAVPVTPacket(logData.navpvtPacket);
      logData.navpvtIsLocationValid = gps.isLocationValid();
      logData.enqueueWriteNAVPVTPkt = true;
    }
    if(gpsLoggingInProgress &&
       (deviceState.GPS_LOGGPX || deviceState.GPS_LOGKML || deviceState.GPS_LOGCSV)) {
      // GPS Logging
      char _latStr[11];
      dtostrf(gps.getLatitude(), -9, 6, _latStr);
      char _lonStr[11];
      dtostrf(gps.getLongitude(), -9, 6, _lonStr);
      rtc_datetime_t dateTime;
      dateTime.year   = gps.getYear();
      dateTime.month  = gps.getMonth();
      dateTime.day    = gps.getDay();
      dateTime.hour   = gps.getHour();
      dateTime.minute = gps.getMinute();
      dateTime.second = gps.getSecond();
      char* _itdStr = rtc.dateTimeToISO8601Str(dateTime);
      // GPX Logging
      if(deviceState.GPS_LOGGPX) {
        // trkpt - "      <trkpt lat=\"45.4431641\" lon=\"-121.7295456\"><ele>122</ele><time>2001-06-02T00:18:15Z</time></trkpt>\n"
        sprintf(logData.navpvtGPXStr, "      <trkpt lat=\"%s\" lon=\"%s\"><ele>%d</ele><time>%sZ</time></trkpt>\n",
                _latStr, _lonStr, gps.getAltitude(), _itdStr);
        logData.enqueueWriteNAVPVTGPXRecord = true;
      }
      // KML Logging
      if(deviceState.GPS_LOGKML) {
        // trkpt - "      <when>2010-05-28T02:02:09Z</when>"
        // trkpt - "      <gx:coord>-122.207881 37.371915 156.000000</gx:coord>"
        sprintf(logData.navpvtKMLStr,   "      <when>%sZ</when>\n", _itdStr);
        sprintf(logData.navpvtKMLStr, "%s      <gx:coord>%s %s %d.000000</gx:coord>\n",
                logData.navpvtKMLStr,
                _lonStr, _latStr, gps.getAltitude());
        logData.enqueueWriteNAVPVTKMLRecord = true;
      }
      // CSV Logging
      if(deviceState.GPS_LOGCSV) {
        char _headingStr[11];
        dtostrf(gps.getHeading(), -9, 6, _headingStr);
        char _pdopStr[11];
        dtostrf(gps.getPDOP(), -9, 6, _pdopStr);
        sprintf(logData.navpvtCSVStr, "\n%d,%sZ,%s,%s,%s,%d,%s,%d,%d,%dD,%d,%s,%s,%d,%d",
                csvLoggingFileWriteCount, _itdStr, (gps.isLocationValid() ? "TRUE" : "FALSE"),
                _latStr, _lonStr, gps.getAltitude(), _headingStr,
                gps.getHAccEst(), gps.getVAccEst(), gps.getLocationFixType(),
                gps.getNumSV(), _pdopStr, (gps.getInvalidLlh() ? "TRUE" : "FALSE"),
                gps.getDistance(), gps.getBearing());
        logData.enqueueWriteNAVPVTCSVRecord = true;
	logData.sentNAVPVTCSVRecord = false;
      }
    }
    _displayRefresh = true;
  } else if(gps.getNAVSAT()) {
    // NAVSAT
    if(gpsLoggingInProgress &&
       ((deviceState.GPS_LOGUBXMODE == GPS_LOGUBX_NAVSAT) ||
        (deviceState.GPS_LOGUBXMODE == GPS_LOGUBX_ALL))) {
      // UBX packet logging
      gps.getNAVSATPacket(logData.navsatPacket);
      logData.navsatPacketLength = gps.getNAVSATPacketLength();
      logData.enqueueWriteNAVSATPkt = true;
    }
    // CSV Logging
    if(gpsLoggingInProgress && deviceState.GPS_LOGCSV) {
      ubloxNAVSATInfo_t navsatInfo;
      gps.getNAVSATInfo(navsatInfo);
      sprintf(logData.navsatCSVStr, ",%d,%d,%d,%d,%d",
              navsatInfo.numSvs, navsatInfo.numSvsReceived,
              navsatInfo.numSvsHealthy, navsatInfo.numSvsEphValid,
              navsatInfo.numSvsUsed);
      if(navsatInfo.numSvsHealthy > 0) {
        for(uint8_t i=0; i<navsatInfo.numSvsHealthy; i++) {
          sprintf(logData.navsatCSVStr, "%s,%c%02d/%02d/%02d\xB0/%03d\xB0",
                  logData.navsatCSVStr,
                  navsatInfo.svSortList[i].gnssIdType,
                  navsatInfo.svSortList[i].svId,
                  navsatInfo.svSortList[i].cno,
                  navsatInfo.svSortList[i].elev,
                  navsatInfo.svSortList[i].azim);
        }
      }
      logData.enqueueWriteNAVSATCSVRecord = true;
      logData.sentNAVSATCSVRecord = false;
    }
    _displayRefresh = true;
  } else if(gps.getNAVSTATUS()) {
    // NAVSTATUS
    if(gpsLoggingInProgress &&
       ((deviceState.GPS_LOGUBXMODE == GPS_LOGUBX_NAVSTATUS) ||
        (deviceState.GPS_LOGUBXMODE == GPS_LOGUBX_ALL))) {
      // UBX packet logging
      gps.getNAVSTATUSPacket(logData.navstatusPacket);
      logData.enqueueWriteNAVSTATUSPkt = true;
    }
    // CSV Logging
    if(gpsLoggingInProgress && deviceState.GPS_LOGCSV) {
      ubloxNAVSTATUSInfo_t navstatusInfo;
      gps.getNAVSTATUSInfo(navstatusInfo);
      sprintf(logData.navstatusCSVStr, ",%s,%02X,%02X,%02d,%08d,%d,%d,%d",
              (navstatusInfo.gpsFixOk ? "TRUE" : "FALSE"),
              navstatusInfo.gpsFix, navstatusInfo.psmState, navstatusInfo.carrSoln,
              (navstatusInfo.ttff / 1000), navstatusInfo.spoofDetState,
              navstatusInfo.spoofingIndicated,navstatusInfo.multipleSpoofingIndications);
      logData.enqueueWriteNAVSTATUSCSVRecord = true;
      logData.sentNAVSTATUSCSVRecord = false;
    }
    _displayRefresh = true;
  }
  return _displayRefresh;
}

