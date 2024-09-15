/*
TeenyGPSConnect.h - Header file for the TeenyGPSConnect Arduino Library.
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

#ifndef TeenyGPSConnect_h
#define TeenyGPSConnect_h

/** @brief Manage position fix and date-time from GNSS receiver. 
 * 
 * Based on u-blox 8 receiver integrated Beitian BN-880 module, using the UBX communications
 * protocol instead of the usual NMEA-183. The problem with NMEA is that it was really 
 * designed for boats, where close enough to see with binoculars was good enough (well, not
 * really but it makes for a good story). The real issue is that it's hard to get consistent
 * fix and time data by parsing NMEA sentences.
 * 
*/

#include <Arduino.h>
#include <RBD_Timer.h>

#include "TeenyUbloxConnect.h"

// GNSS Config Variables
const uint32_t GPS_BAUD_RATE = 38400;
const uint32_t NAVPVT_FIX_AGE_LIMIT     = 1500;  // when autoNAVPVTrate=1second
const uint32_t NAVSTATUS_FIX_AGE_LIMIT  = 1500;  // when autoNAVSTATUSrate=10seconds
const uint32_t NAVSAT_FIX_AGE_LIMIT     = 11000; // when autoNAVSATrate=10seconds

struct GnssData {
  bool    packet_valid;
  uint8_t pad00a;
  uint8_t pad00b;
  uint8_t pad00c;
  uint8_t location_fixType;
  uint8_t pad01a;
  uint8_t pad01b;
  uint8_t pad01c;
  uint8_t location_valid;
  uint8_t pad02a;
  uint8_t pad02b;
  uint8_t pad02c;
  uint8_t date_valid;
  uint8_t pad03a;
  uint8_t pad03b;
  uint8_t pad03c;
  uint8_t time_valid;
  uint8_t pad04a;
  uint8_t pad04b;
  uint8_t pad04c;
  // Position
  float_t latitude;     // Longitude in degrees
  float_t longitude;    // Longitude in degrees
  int32_t altitude;     // Altitude above ellipsoid in meters
  int32_t altitudeMSL;  // Altitude above mean sea level in meters
  float_t heading;      // Heading of motion (2-D)
  // Confidence
  uint8_t numSV;        // Satellites used to calculate fix
  uint8_t pad05a;
  uint8_t pad05b;
  uint8_t pad05c;
  uint32_t hacc;        // Horizontal accuracy estimate in meters
  uint32_t vacc;        // Vertical accuracy estimate in meters
  float pdop;           // Positional Dilution of Precision
  // Date & Time
  uint16_t year;
  uint8_t pad06a;
  uint8_t pad06b;
  uint8_t month;
  uint8_t pad07a;
  uint8_t pad07b;
  uint8_t pad07c;
  uint8_t day;
  uint8_t pad08a;
  uint8_t pad08b;
  uint8_t pad08c;
  uint8_t hour;
  uint8_t pad09a;
  uint8_t pad09b;
  uint8_t pad09c;
  uint8_t minute;
  uint8_t pad10a;
  uint8_t pad10b;
  uint8_t pad10c;
  uint8_t second;
  uint8_t pad11a;
  uint8_t pad11b;
  uint8_t pad11c;
} __attribute__ ((__packed__));

/********************************************************************/
// GPS device worker, produces the current GPS location.
/********************************************************************/
class TeenyGPSConnect {

  public:

    // Constructor / destructor / disallow copy and move
    TeenyGPSConnect();
    virtual ~TeenyGPSConnect();
    TeenyGPSConnect(const TeenyGPSConnect&);
    TeenyGPSConnect& operator=(const TeenyGPSConnect&);

    // GPS SETUP
    bool gnss_init(HardwareSerial &serialPort_, uint32_t baudRate_, uint8_t startMode=0, uint8_t autoNAVPVTRate=1, uint8_t autoNAVSTATUSRate=0, uint8_t autoNAVSATRate=0);

    // Host methods for process incoming responses/acknowledges from ublox receiver
    // Can be called inside a timer ISR
    // Recommend calling ever 10-50ms - depends on queue size, baud rate and packets
    void gnss_checkUblox();

    // GPS ublox module type
    uint8_t getUbloxModuleType();

    // GPS protocol version
    uint8_t getProtocolVersionHigh();
    uint8_t getProtocolVersionLow();

    // Factory Reset - So we can make sure app can configure a factory default module
    bool factoryReset();

    // UBX-CFG-GNSS
    bool pollGNSSSelectionInfo();
    bool pollGNSSConfigInfo();
    bool setGNSSConfig(uint8_t gnssId, bool enable);
    bool setGNSSSignalConfig(uint8_t gnssId, const char* signalName, bool enable);
    // GNSS config info
    ubloxMONGNSSInfo_t getGNSSSelectionInfo();
    ubloxCFGGNSSInfo_t getGNSSConfigInfo();

    // UBX-NAV-PVT
    bool getNAVPVT();
    bool pollNAVPVT();
    // full NAVPVT packet
    void getNAVPVTPacket(uint8_t* packet);
    // NAVPVT packet info
    // Valid packet
    bool isPacketValid();
    // location fix
    uint8_t getLocationFixType();
    bool isLocationValid();
    // Position and orientation
    float getLatitude();       // Longitude in degrees
    float getLongitude();      // Longitude in degrees
    int32_t getAltitude();     // Altitude above ellipsoid in meters
    int32_t getAltitudeMSL();  // Altitude above mean sea level in meters
    float getHeading();        // Heading of motion (2-D)
    // Confidence
    uint8_t getNumSV();        // satellites used to calculate fix
    uint32_t getHAccEst();     // Horizontal accuracy estimate in meters
    uint32_t getVAccEst();     // Vertical accuracy estimate in meters
    float getPDOP();           // Positional Dilution of Precision
    // Date & Time
    bool isDateValid();
    uint16_t getYear();
    uint8_t getMonth();
    uint8_t getDay();
    bool isTimeValid();
    uint8_t getHour();
    uint8_t getMinute();
    uint8_t getSecond();

    // UBX-NAV-STATUS
    bool getNAVSTATUS();
    bool pollNAVSTATUS();
    // full NAVSTATUS packet
    void getNAVSTATUSPacket(uint8_t* packet);
    // NAVSTATUS packet info
    void getNAVSTATUSInfo(ubloxNAVSTATUSInfo_t &info_);
    void resetNAVSTATUSInfo();

    // UBX-NAV-SAT
    bool getNAVSAT();
    bool pollNAVSAT();
    // full NAVSAT packet
    void getNAVSATPacket(uint8_t* packet);
    uint16_t getNAVSATPacketLength();
    // NAVSAT packet info
    void getNAVSATInfo(ubloxNAVSATInfo_t &info_);

  private:
    /* u-blox UBX protocol query NAVPVT gets position, velocity & time in one call.
     * The call returns false if no new fix has been received. In other words,
     * each call only succeeds once until the next fix update.
     * Use pdop and numSV to determine acceptability.
    */
    HardwareSerial *serialPort;
    uint32_t baudRate;
    uint8_t autoNAVPVTRate;
    uint8_t autoNAVSTATUSRate;
    uint8_t autoNAVSATRate;
    TeenyUbloxConnect gnss;
    volatile GnssData data;

    // NAV-STATUS data
    ubloxNAVSTATUSInfo_t navstatusInfo;

    // NAV-SAT data
    ubloxNAVSATInfo_t navsatInfo;

    // GPS setup
    bool gnss_setSerialRate();
    void gnss_config();

    // Age each item. If the corresponding timer times out, it's stale.
    RBD::Timer time_getnavpvt{NAVPVT_FIX_AGE_LIMIT};  // if no response from getNAVPVT()
    RBD::Timer location_timer{NAVPVT_FIX_AGE_LIMIT};
    RBD::Timer date_timer{NAVPVT_FIX_AGE_LIMIT};
    RBD::Timer time_timer{NAVPVT_FIX_AGE_LIMIT};
    RBD::Timer time_getnavstatus{NAVSTATUS_FIX_AGE_LIMIT}; // if no response from getNAVSTATUS()
    RBD::Timer time_getnavsat{NAVSAT_FIX_AGE_LIMIT};       // if no response from getNAVSAT()

};

#endif //TeenyGPSConnect_h

