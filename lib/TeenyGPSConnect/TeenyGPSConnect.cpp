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
bool TeenyGPSConnect::gnss_init(HardwareSerial &serialPort_, uint32_t baudRate_, uint8_t startMode_, uint8_t autoNAVPVTRate_, uint8_t autoNAVSTATUSRate_, uint8_t autoNAVSATRate_) {

  // Assign serial port
  serialPort = &serialPort_;

  // Set gnss baudRate
  baudRate = baudRate_;
  if(!gnss_setSerialRate()) return false;

  // Save gnss config
  autoNAVPVTRate = autoNAVPVTRate_;
  autoNAVSTATUSRate = autoNAVSTATUSRate_;
  autoNAVSATRate = autoNAVSATRate_;

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
  serialPort->begin(baudRate);
  if(gnss.begin(*serialPort)) {
    return true;
  } else {
    serialPort->begin(9600); // default for many gps modules
    if(gnss.begin(*serialPort)) {
      gnss.setSerialRate(baudRate);
      delay(100); // recovery time for gnss module baud rate change
      serialPort->begin(baudRate);
      return gnss.begin(*serialPort);
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
  gnss.setAutoNAVSTATUSRate(autoNAVSTATUSRate); //Include NAV-STATUS reports 
  gnss.setAutoNAVSATRate(autoNAVSATRate);       //Include NAV-SAT reports 

  // Mark the fix items invalid to start
  data.packet_valid = false;
  data.location_fixType = 0;
  data.location_valid = false;
  data.date_valid = false;
  data.time_valid = false;
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
bool TeenyGPSConnect::pollGNSSSelectionInfo() {
  return gnss.pollGNSSSelectionInfo();
}

/********************************************************************/
ubloxMONGNSSInfo_t TeenyGPSConnect::getGNSSSelectionInfo() {
  return gnss.getGNSSSelectionInfo();
}

/********************************************************************/
bool TeenyGPSConnect::pollGNSSConfigInfo() {
  return gnss.pollGNSSConfigInfo();
}

/********************************************************************/
ubloxCFGGNSSInfo_t TeenyGPSConnect::getGNSSConfigInfo() {
  return gnss.getGNSSConfigInfo();
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
  // Only used for M10 modules
  return gnss.setGNSSSignalConfig(gnssId, signalName, enable);
}

/********************************************************************/
// UBX-NAV-PVT
/********************************************************************/
bool TeenyGPSConnect::getNAVPVT() {
  // getNAVPVT will return true if there actually is a fresh
  // navigation solution available. "LLH" is longitude, latitude, height.
  // getNAVPVT() returns UTC date and time.
  // Do not use GNSS time, see u-blox spec section 9.
  if(gnss.getNAVPVT()) {
    time_getnavpvt.restart();
    data.packet_valid = true;

    if(gnss.getGnssFixOk()) {
      data.location_fixType = gnss.getFixType();
      data.location_valid = true;
      location_timer.restart();
      data.latitude = (float)(gnss.getLatitude() * 1e-7);
      data.longitude = (float)(gnss.getLongitude() * 1e-7);
      data.altitude = gnss.getAltitude() / 1000;
      data.altitudeMSL = gnss.getAltitudeMSL() / 1000;
      data.heading = (float)(gnss.getHeading() * 1e-5);
      data.numSV = gnss.getSIV();
      data.hacc = gnss.getHorizontalAccEst() / 1000;
      data.vacc = gnss.getVerticalAccEst() / 1000;
      data.pdop = ((float)gnss.getPDOP()) * 0.01;
    }
    else {
      // No valid fix
      if(location_timer.isExpired()) {
        data.location_fixType = 0;
        data.location_valid = false;
      }
    }
 
    if(gnss.getDateValid()) {
      data.date_valid = true;
      date_timer.restart();
      data.year = gnss.getYear();
      data.month = gnss.getMonth();
      data.day = gnss.getDay();
    } else if(date_timer.isExpired()) {
      data.date_valid = false;
    }

    if(gnss.getTimeValid()) {
      data.time_valid = true;
      time_timer.restart();
      data.hour = gnss.getHour();
      data.minute = gnss.getMinute();
      data.second = gnss.getSecond();
    } else if(time_timer.isExpired()) {
      data.time_valid = false;
    }
    return true;
  }
  // else lost packet(s)
  if(time_getnavpvt.isExpired()) {
    data.packet_valid = false;
    data.location_fixType = 0;
    data.location_valid = false;
    data.date_valid = false;
    data.time_valid = false;
  }
  return false;
}

/********************************************************************/
bool TeenyGPSConnect::pollNAVPVT() {
  if(gnss.pollNAVPVT()) {
    data.location_fixType = gnss.getFixType();
    data.location_valid = gnss.getGnssFixOk();
    data.longitude = (float)(gnss.getLongitude() * 1e-7);
    data.latitude = (float)(gnss.getLatitude() * 1e-7);
    data.altitude = gnss.getAltitude() / 1000;
    data.altitudeMSL = gnss.getAltitudeMSL() / 1000;
    data.heading = (float)(gnss.getHeading() * 1e-5);
    data.numSV = gnss.getSIV();
    data.hacc = gnss.getHorizontalAccEst() / 1000;
    data.vacc = gnss.getVerticalAccEst() / 1000;
    data.pdop = ((float)gnss.getPDOP()) * 0.01;
    data.date_valid = gnss.getDateValid();
    data.year = gnss.getYear();
    data.month = gnss.getMonth();
    data.day = gnss.getDay();
    data.time_valid = gnss.getTimeValid();
    data.hour = gnss.getHour();
    data.minute = gnss.getMinute();
    data.second = gnss.getSecond();
    return true;
  }
  return false;
}

/********************************************************************/
void TeenyGPSConnect::getNAVPVTPacket(uint8_t *packet) {
  gnss.getNAVPVTPacket(packet);
}

/********************************************************************/
bool TeenyGPSConnect::isPacketValid() {
  return data.packet_valid;
}
uint8_t TeenyGPSConnect::getLocationFixType() {
  return data.location_fixType;
}
bool TeenyGPSConnect::isLocationValid() {
  return data.location_valid;
}
float TeenyGPSConnect::getLatitude() {
  return data.latitude;
}
float TeenyGPSConnect::getLongitude() {
  return data.longitude;
}
int32_t TeenyGPSConnect::getAltitude() {
  return data.altitude;
}
int32_t TeenyGPSConnect::getAltitudeMSL() {
  return data.altitudeMSL;
}
float TeenyGPSConnect::getHeading() {
  return data.heading;
}

/********************************************************************/
uint8_t TeenyGPSConnect::getNumSV() {
  return data.numSV;
}
uint32_t TeenyGPSConnect::getHAccEst() {
  return data.hacc;
}
uint32_t TeenyGPSConnect::getVAccEst() {
  return data.vacc;
}
float TeenyGPSConnect::getPDOP() {
  return data.pdop;
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

