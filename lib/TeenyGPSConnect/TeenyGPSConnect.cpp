/*
TeenyGPSConnect.h - Class file for the TeenyGPSConnect Arduino Library.
Copyright (C) *** Need copyright statement here ***

This program is free software: you can redistribute it and/or modify
it under the terms of the version 3 GNU General Public License as
published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <Arduino.h>
#include "TeenyGPSConnect.h"

/** @brief GNSS handler using UBX protocol library 
 * 
 * When activated, set auto receive for the UBX protocol commands used:
 * UBX-NAV-PVT: Position, velocity, time, PDOP, height, h/v accuracies, number of satellites. Navigation Position Velocity Time Solution.
 * 
*/

/********************************************************************/
TeenyGPSConnect::TeenyGPSConnect() { }

TeenyGPSConnect::~TeenyGPSConnect() { }

/********************************************************************/
// GPS SETUP
/********************************************************************/
bool TeenyGPSConnect::gnss_init(HardwareSerial &serialPort_, uint32_t baudRate_, uint8_t startMode_, uint8_t autoNAVPVTRate_, uint8_t autoNAVSATRate_, uint8_t autoNAVSTATUSRate_, bool enableDistanceBearingCalc_) {

  // Assign serial port
  serialPort = &serialPort_;

  // Set gnss baudRate
  baudRate = baudRate_;
  if(!gnss_setSerialRate()) return false;

  // Save gnss config
  autoNAVPVTRate = autoNAVPVTRate_;
  autoNAVSATRate = autoNAVSATRate_;
  autoNAVSTATUSRate = autoNAVSTATUSRate_;
  enableDistanceBearingCalc = enableDistanceBearingCalc_;

  // Optional startup mode
  if((startMode_ > 0) && (startMode_ < 5)) {
    switch(startMode_) {
      case 1: gnss.hotStart();
              break;
      case 2: gnss.warmStart();
              break;
      case 3: gnss.coldStart();
              break;
      case 4: gnss.hardwareReset();
              break;
    }
    // Re-establish comms after hot/warm/cold start and software/hardware reset
    delay(100); // recovery time for possible gnss module baud rate change
    if(!gnss_setSerialRate()) return false;
  }

  // Config gnss
  gnss_config();

  return true;
}

/********************************************************************/
bool TeenyGPSConnect::gnss_setSerialRate() {
  // Try the requested baud rate first
#ifdef CONFIG_IDF_TARGET_ESP32S3 // Core S3SE fix using the correct serial pins
  serialPort->begin(baudRate, SERIAL_8N1, RXD2, TXD2);
#else
  serialPort->begin(baudRate);
#endif
  if(gnss.begin(*serialPort)) {
    return true;
  }
  // Try 115200 baud rate next
#ifdef CONFIG_IDF_TARGET_ESP32S3 // Core S3SE fix using the correct serial pins
  serialPort->begin(115200, SERIAL_8N1, RXD2, TXD2);
#else
  serialPort->begin(115200);
#endif
  if(gnss.begin(*serialPort)) {
    gnss.setSerialRate(baudRate);
    delay(100); // recovery time for gnss module baud rate change
#ifdef CONFIG_IDF_TARGET_ESP32S3 // Core S3SE fix using the correct serial pins
    serialPort->begin(baudRate, SERIAL_8N1, RXD2, TXD2);
#else
    serialPort->begin(baudRate);
#endif
    // Wait up to 3000 msec in case module powers up with 10hz NEMA/UBLOX output frequency
    if(gnss.begin(*serialPort, 3000)) {
      return true;
    }
  }
  // Try 9600 baud rate last
#ifdef CONFIG_IDF_TARGET_ESP32S3 // Core S3SE fix using the correct serial pins
  serialPort->begin(9600, SERIAL_8N1, RXD2, TXD2);
#else
  serialPort->begin(9600);
#endif
  if(gnss.begin(*serialPort)) {
    gnss.setSerialRate(baudRate);
    delay(100); // recovery time for gnss module baud rate change
#ifdef CONFIG_IDF_TARGET_ESP32S3 // Core S3SE fix using the correct serial pins
    serialPort->begin(baudRate, SERIAL_8N1, RXD2, TXD2);
#else
    serialPort->begin(baudRate);
#endif
    if(gnss.begin(*serialPort)) {
      return true;
    }
  }
  return false;
}

/********************************************************************/
void TeenyGPSConnect::gnss_config() {

  gnss.setPortOutput(COM_PORT_UART1, COM_TYPE_UBX); //Set the UART port to output UBX only
  gnss.setMeasurementRate(1000);                //Produce a measurement every 1000ms
  gnss.setNavigationRate(1);                    //Produce a navigation solution every measurement
  gnss.setAutoNAVPVTRate(autoNAVPVTRate);       //Include NAV-PVT reports 
  gnss.setAutoNAVSATRate(autoNAVSATRate);       //Include NAV-SAT reports 
  gnss.setAutoNAVSTATUSRate(autoNAVSTATUSRate); //Include NAV-STATUS reports 

  // Mark the fix items invalid to start
  data.packet_valid = false;
  data.location_valid = false;
  data.location_fixType = 0;
  data.date_valid = false;
  data.time_valid = false;
  data.curr_gnssFixOk = false;
  data.prev_gnssFixOk = false;
  data.distance = -1;
  data.bearing  = -1;
}

/********************************************************************/
uint8_t TeenyGPSConnect::getUbloxModuleType() {
  return gnss.getUbloxModuleType();
}

/********************************************************************/
uint8_t TeenyGPSConnect::getProtocolVersionHigh() {
  return gnss.getProtocolVersionHigh();
}

/********************************************************************/
uint8_t TeenyGPSConnect::getProtocolVersionLow() {
  return gnss.getProtocolVersionLow();
}

/********************************************************************/
// GNSS SERIAL INPUT PROCESSING
/********************************************************************/
void TeenyGPSConnect::gnss_checkUblox() {
  gnss.checkUblox();
}

/********************************************************************/
// FACTORY RESET
/********************************************************************/
bool TeenyGPSConnect::factoryReset() {
  return gnss.factoryReset();
}

/********************************************************************/
// UBX-CFG-GNSS
/********************************************************************/
bool TeenyGPSConnect::pollGNSSSelection() {
  return gnss.pollGNSSSelection();
}

/********************************************************************/
ubloxMONGNSSInfo_t TeenyGPSConnect::getGNSSSelectionInfo() {
  return gnss.getGNSSSelectionInfo();
}

/********************************************************************/
bool TeenyGPSConnect::pollGNSSConfig() {
  return gnss.pollGNSSConfig();
}

/********************************************************************/
ubloxCFGGNSSInfo_t TeenyGPSConnect::getGNSSConfigInfo() {
  return gnss.getGNSSConfigInfo();
}

/********************************************************************/
ubloxCFGGNSSState_t TeenyGPSConnect::getGNSSConfigState() {
  return gnss.getGNSSConfigState();
}

/********************************************************************/
bool TeenyGPSConnect::setGNSSConfig(uint8_t gnssId, bool enable) {
  if(gnss.setGNSSConfig(gnssId, enable)) {
    // For M8 module - Re-establish comms after cold start and hardware reset
    if(gnss_setSerialRate()) {
      gnss_config();
      return true;
    }
  }
  return false;
}

/********************************************************************/
bool TeenyGPSConnect::setGNSSSignalConfig(uint8_t gnssId, const char* signalName, bool enable) {
  // Not used for Ublox M8 modules
  return gnss.setGNSSSignalConfig(gnssId, signalName, enable);
}

/********************************************************************/
bool TeenyGPSConnect::setGNSSConfigState(ubloxCFGGNSSState_t gnssConfigState) {
  return gnss.setGNSSConfigState(gnssConfigState);
}

/********************************************************************/
// UBX-NAV-PVT
/********************************************************************/
bool TeenyGPSConnect::getNAVPVT() {
  // getNAVPVT will return true if there actually is a fresh navigation solution available.
  // getNAVPVT() returns UTC date and time.
  // Do not use GNSS time, see u-blox spec section 9.
  if(gnss.getNAVPVT()) {
    time_getnavpvt.restart();
    gnss.getNAVPVTInfo(navpvtInfo);
    data.packet_valid = true;
    data.iTOW = navpvtInfo.iTOW;
    data.prev_gnssFixOk = data.curr_gnssFixOk;
    data.curr_gnssFixOk = navpvtInfo.gnssFixOk;

    if(data.curr_gnssFixOk) {
      data.location_valid = true;
      location_timer.restart();
      // save previous location data
      data.prev_latitude  = data.latitude;
      data.prev_longitude = data.longitude;
      // get current data
      data.location_fixType = navpvtInfo.fixType;
      data.latitude    = navpvtInfo.latitude * 1e-7;
      data.longitude   = navpvtInfo.longitude * 1e-7;
      data.altitude    = navpvtInfo.altitude / 1000;
      data.altitudeMSL = navpvtInfo.altitudeMSL / 1000;
      data.heading     = navpvtInfo.headMot * 1e-5;
      data.numSV       = navpvtInfo.numSV;
      data.hAcc        = navpvtInfo.hAcc / 1000;
      data.vAcc        = navpvtInfo.vAcc / 1000;
      data.pDOP        = navpvtInfo.pDOP * 1e-2;
      data.invalidLlh  = navpvtInfo.invalidLlh;
    } else {
      // No valid fix
      if(location_timer.isExpired()) {
        data.location_valid = false;
        data.location_fixType = 0;
      }
    }

    // compute distance between current and previous location
    if(enableDistanceBearingCalc) calcDistanceBearing();
 
    if(navpvtInfo.dateValid) {
      data.date_valid = true;
      date_timer.restart();
      data.year  = navpvtInfo.year;
      data.month = navpvtInfo.month;
      data.day   = navpvtInfo.day;
    } else if(date_timer.isExpired()) {
      data.date_valid = false;
    }

    if(navpvtInfo.timeValid) {
      data.time_valid = true;
      time_timer.restart();
      data.hour   = navpvtInfo.hour;
      data.minute = navpvtInfo.minute;
      data.second = navpvtInfo.second;
    } else if(time_timer.isExpired()) {
      data.time_valid = false;
    }
    return true;
  }
  // else lost packet(s)
  if(time_getnavpvt.isExpired()) {
    data.packet_valid = false;
    data.location_valid = false;
    data.location_fixType = 0;
    data.date_valid = false;
    data.time_valid = false;
    data.curr_gnssFixOk = false;
    data.prev_gnssFixOk = false;
    data.distance = -1;
    data.bearing  = -1;
  }
  return false;
}

/********************************************************************/
bool TeenyGPSConnect::pollNAVPVT() {
  if(gnss.pollNAVPVT()) {
    gnss.getNAVPVTInfo(navpvtInfo);
    data.iTOW = navpvtInfo.iTOW;
    // save previous location data
    data.prev_gnssFixOk = data.curr_gnssFixOk;
    data.prev_latitude  = data.latitude;
    data.prev_longitude = data.longitude;
    // get current data
    data.curr_gnssFixOk   = navpvtInfo.gnssFixOk;
    data.location_valid   = data.curr_gnssFixOk;
    data.location_fixType = navpvtInfo.fixType;
    data.latitude         = navpvtInfo.latitude * 1e-7;
    data.longitude        = navpvtInfo.longitude * 1e-7;
    data.altitude         = navpvtInfo.altitude / 1000;
    data.altitudeMSL      = navpvtInfo.altitudeMSL / 1000;
    data.heading          = navpvtInfo.headMot * 1e-5;
    data.numSV            = navpvtInfo.numSV;
    data.hAcc             = navpvtInfo.hAcc / 1000;
    data.vAcc             = navpvtInfo.vAcc / 1000;
    data.pDOP             = navpvtInfo.pDOP * 1e-2;
    data.invalidLlh       = navpvtInfo.invalidLlh;
    data.date_valid       = navpvtInfo.dateValid;
    data.year             = navpvtInfo.year;
    data.month            = navpvtInfo.month;
    data.day              = navpvtInfo.day;
    data.time_valid       = navpvtInfo.timeValid;
    data.hour             = navpvtInfo.hour;
    data.minute           = navpvtInfo.minute;
    data.second           = navpvtInfo.second;
    // compute distance between current and previous location
    if(enableDistanceBearingCalc) calcDistanceBearing();
    return true;
  }
  data.packet_valid = false;
  data.location_valid = false;
  data.location_fixType = 0;
  data.date_valid = false;
  data.time_valid = false;
  data.curr_gnssFixOk = false;
  data.prev_gnssFixOk = false;
  data.distance = -1;
  data.bearing  = -1;
  return false;
}

/********************************************************************/
void TeenyGPSConnect::getNAVPVTPacket(uint8_t *packet) {
  gnss.getNAVPVTPacket(packet);
}

/********************************************************************/
void TeenyGPSConnect::getNAVPVTInfo(ubloxNAVPVTInfo_t &info_) {
  info_ = navpvtInfo;
}

/********************************************************************/
void TeenyGPSConnect::calcDistanceBearing() {
//if(data.curr_gnssFixOk && data.prev_gnssFixOk) {
    // staring point
    double_t lat1R = data.prev_latitude * (PI/180.0);
    double_t lon1R = data.prev_longitude * (PI/180.0);
    // destination point
    double_t lat2R = data.latitude * (PI/180.0);
    double_t lon2R = data.longitude * (PI/180.0);
    // distance
    double_t a = sq(sin((lat2R-lat1R)/2.0)) + cos(lat1R) * cos(lat2R) * sq(sin((lon2R-lon1R)/2.0));
    double_t c = 2 * atan2(sqrt(a), sqrt(1-a));
    data.distance = round(6378137000.0 * c);
    // bearing
    double_t x = cos(lat2R) * sin(lon2R-lon1R);
    double_t y = cos(lat1R) * sin(lat2R) - sin(lat1R) * cos(lat2R) * cos(lon2R-lon1R);
    double_t bearingR = atan2(x, y);
    int16_t bearingD = round(57.295779 * bearingR);
    data.bearing = (bearingD + 360) % 360;
//} else {
//  data.distance = -1;
//  data.bearing  = -1;
//}
}

/********************************************************************/
bool TeenyGPSConnect::isPacketValid() {
  return data.packet_valid;
}
uint32_t TeenyGPSConnect::getITOW() {
  return data.iTOW;
}
bool TeenyGPSConnect::isLocationValid() {
  return data.location_valid;
}
uint8_t TeenyGPSConnect::getLocationFixType() {
  return data.location_fixType;
}
float_t TeenyGPSConnect::getLatitude() {
  return data.latitude;
}
float_t TeenyGPSConnect::getLongitude() {
  return data.longitude;
}
int32_t TeenyGPSConnect::getAltitude() {
  return data.altitude;
}
int32_t TeenyGPSConnect::getAltitudeMSL() {
  return data.altitudeMSL;
}
float_t TeenyGPSConnect::getHeading() {
  return data.heading;
}
int32_t TeenyGPSConnect::getDistance() {
  return data.distance;
}
int16_t TeenyGPSConnect::getBearing() {
  return data.bearing;
}

/********************************************************************/
uint8_t TeenyGPSConnect::getNumSV() {
  return data.numSV;
}
uint32_t TeenyGPSConnect::getHAccEst() {
  return data.hAcc;
}
uint32_t TeenyGPSConnect::getVAccEst() {
  return data.vAcc;
}
float_t TeenyGPSConnect::getPDOP() {
  return data.pDOP;
}
bool TeenyGPSConnect::getInvalidLlh() {
  return data.invalidLlh;
}

/********************************************************************/
bool TeenyGPSConnect::isDateValid() {
  return data.date_valid;
}
uint16_t TeenyGPSConnect::getYear() {
  return data.year;
}
uint8_t TeenyGPSConnect::getMonth() {
  return data.month;
}
uint8_t TeenyGPSConnect::getDay() {
  return data.day;
}
bool TeenyGPSConnect::isTimeValid() {
  return data.time_valid;
}
uint8_t TeenyGPSConnect::getHour() {
  return data.hour;
}
uint8_t TeenyGPSConnect::getMinute() {
  return data.minute;
}
uint8_t TeenyGPSConnect::getSecond() {
  return data.second;
}

/********************************************************************/
// UBX-NAV-SAT
/********************************************************************/
bool TeenyGPSConnect::getNAVSAT() {
  // getNAVSAT will return true if there actually is fresh
  // navigation satellite data.
  if(gnss.getNAVSAT()) {
    time_getnavsat.restart();
    gnss.getNAVSATInfo(navsatInfo);
    return true;
  }
  // else lost packet(s)
  if(time_getnavsat.isExpired()) {
    navsatInfo.validPacket = false;
    navsatInfo.numSvs = 0;
    navsatInfo.numSvsHealthy = 0;
    navsatInfo.numSvsUsed = 0;
  }
  return false;
}

/********************************************************************/
bool TeenyGPSConnect::pollNAVSAT() {
  if(gnss.pollNAVSAT()) {
    gnss.getNAVSATInfo(navsatInfo);
    return true;
  }
  return false;
}


/********************************************************************/
void TeenyGPSConnect::getNAVSATPacket(uint8_t *packet) {
  gnss.getNAVSATPacket(packet);
}

/********************************************************************/
uint16_t TeenyGPSConnect::getNAVSATPacketLength() {
  return gnss.getNAVSATPacketLength();
}

/********************************************************************/
void TeenyGPSConnect::getNAVSATInfo(ubloxNAVSATInfo_t &info_) {
  info_ = navsatInfo;
}

/********************************************************************/
uint8_t TeenyGPSConnect::getLostRxPacketCount() {
  return gnss.getLostRxPacketCount();
}

/********************************************************************/
uint8_t TeenyGPSConnect::getUnknownRxPacketCount() {
  return gnss.getUnknownRxPacketCount();
}

/********************************************************************/
// UBX-NAV-STATUS
/********************************************************************/
bool TeenyGPSConnect::getNAVSTATUS() {
  // getNAVSTATUS will return true if there actually is fresh
  // navigation satellite data.
  if(gnss.getNAVSTATUS()) {
    time_getnavstatus.restart();
    gnss.getNAVSTATUSInfo(navstatusInfo);
    return true;
  }
  // else lost packet(s)
  if(time_getnavstatus.isExpired()) {
    navstatusInfo.gpsFix = 0;
    navstatusInfo.gpsFixOk = false;
    navstatusInfo.psmState = 0;
    navstatusInfo.spoofDetState = 0;
    navstatusInfo.carrSoln = 0;
    navstatusInfo.ttff = 0;
    navstatusInfo.msss = 0;
  }
  return false;
}

/********************************************************************/
bool TeenyGPSConnect::pollNAVSTATUS() {
  if(gnss.pollNAVSTATUS()) {
    gnss.getNAVSTATUSInfo(navstatusInfo);
    return true;
  }
  return false;
}


/********************************************************************/
void TeenyGPSConnect::getNAVSTATUSPacket(uint8_t *packet) {
  gnss.getNAVSTATUSPacket(packet);
}

/********************************************************************/
void TeenyGPSConnect::getNAVSTATUSInfo(ubloxNAVSTATUSInfo_t &info_) {
  info_ = navstatusInfo;
}

/********************************************************************/
void TeenyGPSConnect::resetNAVSTATUSInfo() {
  gnss.resetNAVSTATUSInfo();
}

/********************************************************************/
// Lost packet count
/********************************************************************/
uint8_t TeenyGPSConnect::getLostNAVPVTPacketCount() {
  return gnss.getLostNAVPVTPacketCount();
}

/********************************************************************/
uint8_t TeenyGPSConnect::getLostNAVSATPacketCount() {
  return gnss.getLostNAVSATPacketCount();
}

/********************************************************************/
uint8_t TeenyGPSConnect::getLostNAVSTATUSPacketCount() {
  return gnss.getLostNAVSTATUSPacketCount();
}

