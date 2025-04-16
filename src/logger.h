
/********************************************************************/
// forward declarations

/********************************************************************/
// LOGGER
/********************************************************************/
bool logger_update() {
  bool _displayRefresh = false;
  // NAVPVT
  static uint8_t  _navpvtPacket[UBX_NAV_PVT_PACKETLENGTH];
  static bool     _navpvtIsLocationValid = false;
  static bool     _enqueueWriteNAVPVTPkt = false;
  static char     _navpvtGPXStr[256];
  static bool     _enqueueWriteNAVPVTGPXRecord = false;
  static char     _navpvtKMLStr[256];
  static bool     _enqueueWriteNAVPVTKMLRecord = false;
  static char     _navpvtCSVStr[256];
  static bool     _enqueueWriteNAVPVTCSVRecord = false;
  static bool     _sentNAVPVTCSVRecord = false;
  // NAVSAT
  static uint8_t  _navsatPacket[UBX_NAV_SAT_MAXPACKETLENGTH];
  static uint16_t _navsatPacketLength;
  static bool     _enqueueWriteNAVSATPkt = false;
  static char     _navsatCSVStr[256];
  static bool     _enqueueWriteNAVSATCSVRecord = false;
  static bool     _sentNAVSATCSVRecord = false;
  // NAVSTATUS
  static uint8_t  _navstatusPacket[UBX_NAV_STATUS_PACKETLENGTH];
  static bool     _enqueueWriteNAVSTATUSPkt = false;
  static char     _navstatusCSVStr[256];
  static bool     _enqueueWriteNAVSTATUSCSVRecord = false;
  static bool     _sentNAVSTATUSCSVRecord = false;

  // Log packets and records received (so packets are recorded in order received)
  if(_enqueueWriteNAVPVTPkt) {
    //SATNAVPVT Packet
    _enqueueWriteNAVPVTPkt = false;
    sdcard_writeUBXLoggingFile(_navpvtPacket, UBX_NAV_PVT_PACKETLENGTH, _navpvtIsLocationValid);
  } else if(_enqueueWriteNAVSATPkt) {
    // NAVSAT Packet
    _enqueueWriteNAVSATPkt = false;
    sdcard_writeUBXLoggingFile(_navsatPacket, _navsatPacketLength, false, true);
  } else if(_enqueueWriteNAVSTATUSPkt) {
    // NAVSTATUS Packet
    _enqueueWriteNAVSTATUSPkt = false;
    sdcard_writeUBXLoggingFile(_navstatusPacket, UBX_NAV_STATUS_PACKETLENGTH, false, true);
  } else if(_enqueueWriteNAVPVTGPXRecord) {
    // NAVPVT GPX
    _enqueueWriteNAVPVTGPXRecord = false;
    sdcard_writeGPXLoggingFile((uint8_t*)_navpvtGPXStr, strlen(_navpvtGPXStr));
  } else if(_enqueueWriteNAVPVTKMLRecord) {
    // NAVPVT KML
    _enqueueWriteNAVPVTKMLRecord = false;
    sdcard_writeKMLLoggingFile((uint8_t*)_navpvtKMLStr, strlen(_navpvtKMLStr));
  } else if(_enqueueWriteNAVPVTCSVRecord) {
    // NAVPVT CSV
    _enqueueWriteNAVPVTCSVRecord = false;
    sdcard_writeCSVLoggingFile((uint8_t*)_navpvtCSVStr, strlen(_navpvtCSVStr));
    _sentNAVPVTCSVRecord = true;
  } else if(_enqueueWriteNAVSTATUSCSVRecord && _sentNAVPVTCSVRecord) {
    // NAVSTATUS CSV - Send NAVSTATUS before NAVSAT because NAVSAT has variable packet length
    _enqueueWriteNAVSTATUSCSVRecord = false;
    sdcard_writeCSVLoggingFile((uint8_t*)_navstatusCSVStr, strlen(_navstatusCSVStr), true);
    _sentNAVSATCSVRecord = true;
  } else if(_enqueueWriteNAVSATCSVRecord && _sentNAVSATCSVRecord) {
    // NAVSAT CSV
    _enqueueWriteNAVSATCSVRecord = false;
    sdcard_writeCSVLoggingFile((uint8_t*)_navsatCSVStr, strlen(_navsatCSVStr), true);
    _sentNAVSTATUSCSVRecord = true;
  // Check/receive packets
  } else if(gps.getNAVPVT()) {
    // NAVPVT
    if((!rtc.isValid()) && gps.isDateValid() && gps.isTimeValid()) {
      rtc.setRTCTime(gps.getYear(), gps.getMonth(), gps.getDay(),
                     gps.getHour(), gps.getMinute(), gps.getSecond());
    }
    if(gpsLoggingInProgress &&
       ((deviceState.UBXPKTLOGMODE == UBXPKTLOG_NAVPVT) ||
        (deviceState.UBXPKTLOGMODE == UBXPKTLOG_ALL))) {
      // UBX packet logging
      gps.getNAVPVTPacket(_navpvtPacket);
      _navpvtIsLocationValid = gps.isLocationValid();
      _enqueueWriteNAVPVTPkt = true;
    }
    if(gpsLoggingInProgress &&
       (deviceState.GPSLOG_GPX || deviceState.GPSLOG_KML || deviceState.GPSLOG_CSV)) {
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
      if(deviceState.GPSLOG_GPX) {
        // trkpt - "      <trkpt lat=\"45.4431641\" lon=\"-121.7295456\"><ele>122</ele><time>2001-06-02T00:18:15Z</time></trkpt>\n"
        sprintf(_navpvtGPXStr, "      <trkpt lat=\"%s\" lon=\"%s\"><ele>%d</ele><time>%sZ</time></trkpt>\n",
                _latStr, _lonStr, gps.getAltitude(), _itdStr);
        _enqueueWriteNAVPVTGPXRecord = true;
      }
      // KML Logging
      if(deviceState.GPSLOG_KML) {
        // trkpt - "      <when>2010-05-28T02:02:09Z</when>"
        // trkpt - "      <gx:coord>-122.207881 37.371915 156.000000</gx:coord>"
        sprintf(_navpvtKMLStr,   "      <when>%sZ</when>\n", _itdStr);
        sprintf(_navpvtKMLStr, "%s      <gx:coord>%s %s %d.000000</gx:coord>\n",
                _navpvtKMLStr,
                _lonStr, _latStr, gps.getAltitude());
        _enqueueWriteNAVPVTKMLRecord = true;
      }
      // CSV Logging
      if(deviceState.GPSLOG_CSV) {
        char _headingStr[11];
        dtostrf(gps.getHeading(), -9, 6, _headingStr);
        char _pdopStr[11];
        dtostrf(gps.getPDOP(), -9, 6, _pdopStr);
        sprintf(_navpvtCSVStr, "\n%sZ,%s,%s,%s,%d,%s,%d,%d,%dD,%d,%s,%s,%d,%d",
                _itdStr, (gps.isLocationValid() ? "TRUE" : "FALSE"),
                _latStr, _lonStr, gps.getAltitude(), _headingStr,
                gps.getHAccEst(), gps.getVAccEst(), gps.getLocationFixType(),
                gps.getNumSV(), _pdopStr, (gps.getInvalidLlh() ? "TRUE" : "FALSE"),
                gps.getDistance(), gps.getBearing());
        _enqueueWriteNAVPVTCSVRecord = true;
	_sentNAVPVTCSVRecord = false;
      }
    }
    _displayRefresh = true;
  } else if(gps.getNAVSAT()) {
    // NAVSAT
    if(gpsLoggingInProgress &&
       ((deviceState.UBXPKTLOGMODE == UBXPKTLOG_NAVSAT) ||
        (deviceState.UBXPKTLOGMODE == UBXPKTLOG_ALL))) {
      // UBX packet logging
      gps.getNAVSATPacket(_navsatPacket);
      _navsatPacketLength = gps.getNAVSATPacketLength();
      _enqueueWriteNAVSATPkt = true;
    }
    // CSV Logging
    if(gpsLoggingInProgress && deviceState.GPSLOG_CSV) {
      ubloxNAVSATInfo_t _navsatInfo;
      gps.getNAVSATInfo(_navsatInfo);
      sprintf(_navsatCSVStr, ",%d,%d,%d,%d,%d",
              _navsatInfo.numSvs, _navsatInfo.numSvsReceived,
              _navsatInfo.numSvsHealthy, _navsatInfo.numSvsEphValid,
              _navsatInfo.numSvsUsed);
      if(_navsatInfo.numSvsHealthy > 0) {
        for(uint8_t i=0; i<_navsatInfo.numSvsHealthy; i++) {
          sprintf(_navsatCSVStr, "%s,%c%02d/%02d/%02d\xB0/%03d\xB0",
                  _navsatCSVStr,
                  _navsatInfo.svSortList[i].gnssIdType,
                  _navsatInfo.svSortList[i].svId,
                  _navsatInfo.svSortList[i].cno,
                  _navsatInfo.svSortList[i].elev,
                  _navsatInfo.svSortList[i].azim);
        }
      }
      _enqueueWriteNAVSATCSVRecord = true;
      _sentNAVSATCSVRecord = false;
    }
    _displayRefresh = true;
  } else if(gps.getNAVSTATUS()) {
    // NAVSTATUS
    if(gpsLoggingInProgress &&
       ((deviceState.UBXPKTLOGMODE == UBXPKTLOG_NAVSTATUS) ||
        (deviceState.UBXPKTLOGMODE == UBXPKTLOG_ALL))) {
      // UBX packet logging
      gps.getNAVSTATUSPacket(_navstatusPacket);
      _enqueueWriteNAVSTATUSPkt = true;
    }
    // CSV Logging
    if(gpsLoggingInProgress && deviceState.GPSLOG_CSV) {
      ubloxNAVSTATUSInfo_t _navstatusInfo;
      gps.getNAVSTATUSInfo(_navstatusInfo);
      sprintf(_navstatusCSVStr, ",%s,%02X,%02X,%02d,%08d,%d,%d,%d",
              (_navstatusInfo.gpsFixOk ? "TRUE" : "FALSE"),
              _navstatusInfo.gpsFix, _navstatusInfo.psmState, _navstatusInfo.carrSoln,
              (_navstatusInfo.ttff / 1000), _navstatusInfo.spoofDetState,
              _navstatusInfo.spoofingIndicated,_navstatusInfo.multipleSpoofingIndications);
      _enqueueWriteNAVSTATUSCSVRecord = true;
      _sentNAVSTATUSCSVRecord = false;
    }
    _displayRefresh = true;
  }
  return _displayRefresh;
}

