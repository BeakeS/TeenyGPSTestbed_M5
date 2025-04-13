/*
TeenyUbloxConnect.h - Header file for the TeenyUbloxConnect Arduino Library.
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

#ifndef TeenyUbloxConnect_h
#define TeenyUbloxConnect_h

/********************************************************************/
// GNSS Satellite Key
/********************************************************************/
// GNSS      gnssId  Type  #ofSats  isMajor  Ublox_M8_Compatible
// ----      ------  ----  -------  -------  -------------------
// GPS       0       G     31       YES      YES
// SBAS      1       S     WEB      NO       YES
// Galileo   2       E     27       YES      YES
// BeiDou    3       B     44       YES      YES
// IMES      4       I     0        NO       YES
// QZSS      5       Q     4        NO       YES
// GLONASS   6       R     24       YES      YES
// NAVIC     7       N     5        ?        NO
/********************************************************************/
// Ublox M8 GNSS Configuration
/********************************************************************/
// The Ublox 8 series (M8) supports reception from satellites on 3
// different frequency bands:
//   a) GPS+SBAS+GALILEO+QZSS (L1)
//   b) GLONASS (L1)
//   c) BEIDOU (B1I)
// ...BUT...
// M8 can only process 2 frequency bands concurrently.
// ...So...
// You can't enable GPS, GLONASS and BEIDOU at the same time (and the
// M8 module wouldn't let you if you tried).
//
// You need to choose which 2 (or 1) out of 3 groups (a, b, c) to
// enable in your M8 module.
// Notes:
// - Choosing your major GNSS (GPS, Galileo, BeiDou, GLONASS) should
//   based on your region.
// - Minor GNSS (SBAS, QZSS) choice should also be region-based.
//
// Example opinion from 'clive1' on ublox forum regarding M8 GNSS
// configuration for CONUS (continental US) with emphasis on power savings:
// - I wouldn't bother tracking QZSS within the CONUS, just grinding gears.
// - I'd suppose having many constellations to speed TTFF and your
//   ability to turn-off the receiver more quickly is where energy
//   would get saved. GALILEO tends to be slow to acquire on these designs.
// - Only SBAS covers CONUS, SBAS Only provides data for GPS Satellites
// - Skeptical of the overall value of SBAS in this scenario.
// - I'd perhaps lean to GPS+GLO.

/********************************************************************/
// UBX Receiver Types
/********************************************************************/
enum ubx_module_type_t : uint8_t {
  UBLOX_UNKNOWN_MODULE = 0,
  UBLOX_M8_MODULE      = 8,
  UBLOX_M9_MODULE      = 9,
  UBLOX_M10_MODULE     = 10
};

/********************************************************************/
// UBX Packet Frame Defines
/********************************************************************/
const uint8_t  COM_PORT_UART1 = 1;
const uint8_t  COM_TYPE_UBX  = 0x01;
const uint8_t  COM_TYPE_NMEA = 0x02;
const uint8_t  UBX_MAXNAVSATSATELLITES = 64; // Max payload determined by NAV-SAT max satellites
const uint16_t UBX_MAXPAYLOADLENGTH = 8 + (12 * (uint16_t)UBX_MAXNAVSATSATELLITES);
const uint8_t  UBX_SYNCH_1 = 0xB5;
const uint8_t  UBX_SYNCH_2 = 0x62;
const uint8_t  UBX_CLASS_NAV = 0x01;
const uint8_t    UBX_NAV_STATUS = 0x03;
const uint16_t   UBX_NAV_STATUS_PAYLOADLENGTH = 16;
const uint16_t   UBX_NAV_STATUS_PACKETLENGTH = 24;
const uint8_t    UBX_NAV_PVT    = 0x07;
const uint16_t   UBX_NAV_PVT_PAYLOADLENGTH = 92;
const uint16_t   UBX_NAV_PVT_PACKETLENGTH = 100;
const uint8_t    UBX_NAV_SAT    = 0x35;
const uint16_t   UBX_NAV_SAT_MINPAYLOADLENGTH = 8;
const uint16_t   UBX_NAV_SAT_MINPACKETLENGTH = UBX_NAV_SAT_MINPAYLOADLENGTH + 8;
const uint16_t   UBX_NAV_SAT_MAXPAYLOADLENGTH = UBX_MAXPAYLOADLENGTH;
const uint16_t   UBX_NAV_SAT_MAXPACKETLENGTH = UBX_NAV_SAT_MAXPAYLOADLENGTH + 8;
const uint8_t  UBX_CLASS_ACK = 0x05;
const uint8_t    UBX_ACK_NAK    = 0x00;
const uint8_t    UBX_ACK_ACK    = 0x01;
const uint16_t   UBX_ACKNAK_PAYLOADLENGTH = 2;
const uint8_t  UBX_CLASS_CFG = 0x06;
const uint8_t    UBX_CFG_PRT    = 0x00;              // M8 only
const uint16_t   UBX_CFG_PRT_PAYLOADLENGTH = 20;     // M8 only
const uint8_t    UBX_CFG_MSG    = 0x01;              // M8 only
const uint8_t    UBX_CFG_RST    = 0x04;
const uint16_t   UBX_CFG_RST_PAYLOADLENGTH = 4;
const uint8_t    UBX_CFG_RATE   = 0x08;              // M8 only
const uint16_t   UBX_CFG_RATE_PAYLOADLENGTH = 6;     // M8 only
const uint8_t    UBX_CFG_CFG    = 0x09;
const uint8_t    UBX_CFG_NAVX5  = 0x23;              // M8 only
const uint16_t   UBX_CFG_NAVX5_PAYLOADLENGTH = 40;   // M8 only
const uint8_t    UBX_CFG_GNSS   = 0x3E;              // M8 only
const uint16_t   UBX_CFG_GNSS_MINPAYLOADLENGTH = 4;  // M8 only
const uint16_t   UBX_CFG_GNSS_MAXPAYLOADLENGTH = 68; // M8 only
const uint8_t    UBX_CFG_VALSET = 0x8A;              // M9/M10 only
const uint8_t    UBX_CFG_VALGET = 0x8B;              // M9/M10 only
const uint8_t    UBX_CFG_VALDEL = 0x8C;              // M9/M10 only
const uint8_t  UBX_CLASS_MON = 0x0A;
const uint8_t    UBX_MON_VER    = 0x04;
const uint16_t   UBX_MON_VER_PAYLOADLENGTH = 160;
const uint8_t    UBX_MON_GNSS   = 0x28;
const uint16_t   UBX_MON_GNSS_PAYLOADLENGTH = 8;

/********************************************************************/
// UBX Configuration Layers
/********************************************************************/
const uint8_t VALDEL_LAYER_BBR     = 0x01;
const uint8_t VALDEL_LAYER_FLASH   = 0x02;
const uint8_t VALGET_LAYER_RAM     = 0x00;
const uint8_t VALGET_LAYER_BBR     = 0x01;
const uint8_t VALGET_LAYER_FLASH   = 0x02;
const uint8_t VALGET_LAYER_DEFAULT = 0x07;
const uint8_t VALSET_LAYER_RAM     = 0x01;
const uint8_t VALSET_LAYER_BBR     = 0x02;
const uint8_t VALSET_LAYER_FLASH   = 0x04;
const uint8_t VALSET_LAYER_RAM_BBR = 0x03;
const uint8_t VALSET_LAYER_ALL     = 0x07;

/********************************************************************/
// UBX Configuration Keys
/********************************************************************/
// port keys
const uint32_t UBLOX_CFG_UART1_ENABLED     = 0x10520005; // bool
const uint32_t UBLOX_CFG_UART1OUTPROT_UBX  = 0x10740001; // bool
const uint32_t UBLOX_CFG_UART1OUTPROT_NMEA = 0x10740002; // bool
const uint32_t UBLOX_CFG_UART1_BAUDRATE    = 0x40520001; // uint32_t
// gnss keys
// Note that changes to any items within this group will trigger a reset to the GNSS subsystem.
// The reset takes some time, so wait first for the acknowledgement from the receiver and then
// wait 0.5seconds before sending the next command.
const uint32_t UBLOX_CFG_SIGNAL_GPS_ENA       = 0x1031001F; // bool // GPS enable
const uint32_t UBLOX_CFG_SIGNAL_GPS_L1CA_ENA  = 0x10310001; // bool // GPS L1C/A
const uint32_t UBLOX_CFG_SIGNAL_SBAS_ENA      = 0x10310020; // bool // SBAS enable
const uint32_t UBLOX_CFG_SIGNAL_SBAS_L1CA_ENA = 0x10310005; // bool // SBAS L1C/A
const uint32_t UBLOX_CFG_SIGNAL_GAL_ENA       = 0x10310021; // bool // Galileo enable
const uint32_t UBLOX_CFG_SIGNAL_GAL_E1_ENA    = 0x10310007; // bool // Galileo E1
const uint32_t UBLOX_CFG_SIGNAL_BDS_ENA       = 0x10310022; // bool // BeiDou Enable
const uint32_t UBLOX_CFG_SIGNAL_BDS_B1_ENA    = 0x1031000D; // bool // BeiDou B1I
const uint32_t UBLOX_CFG_SIGNAL_BDS_B1C_ENA   = 0x1031000F; // bool // BeiDou B1C
const uint32_t UBLOX_CFG_SIGNAL_QZSS_ENA      = 0x10310024; // bool // QZSS enable
const uint32_t UBLOX_CFG_SIGNAL_QZSS_L1CA_ENA = 0x10310012; // bool // QZSS L1C/A
const uint32_t UBLOX_CFG_SIGNAL_QZSS_L1S_ENA  = 0x10310014; // bool // QZSS L1S
const uint32_t UBLOX_CFG_SIGNAL_GLO_ENA       = 0x10310025; // bool // GLONASS enable
const uint32_t UBLOX_CFG_SIGNAL_GLO_L1_ENA    = 0x10310018; // bool // GLONASS L1
// rate keys
const uint32_t UBLOX_CFG_RATE_MEAS = 0x30210001; // uint16_t
const uint32_t UBLOX_CFG_RATE_NAV  = 0x30210002; // uint16_t
const uint32_t UBLOX_CFG_MSGOUT_UBX_NAV_PVT_UART1    = 0x20910007; // uint8_t
const uint32_t UBLOX_CFG_MSGOUT_UBX_NAV_SAT_UART1    = 0x20910016; // uint8_t
const uint32_t UBLOX_CFG_MSGOUT_UBX_NAV_STATUS_UART1 = 0x2091001B; // uint8_t

/********************************************************************/
// UBX Packet Struct
/********************************************************************/
typedef struct {
  uint8_t  synch1 = UBX_SYNCH_1;
  uint8_t  synch2 = UBX_SYNCH_2;
  uint8_t  messageClass;
  uint8_t  messageID;
  uint16_t payloadLength;
  uint8_t  pad00a;
  uint8_t  pad00b;
  uint8_t  payload[UBX_MAXPAYLOADLENGTH];
  uint8_t  checksumA;
  uint8_t  checksumB;
  uint8_t  rollingChecksumA;
  uint8_t  rollingChecksumB;
  uint16_t packetCounter;
  uint16_t payloadCounter;
  bool     receivingPacket;
  bool     validPacket;
  uint8_t  pad01a;
  uint8_t  pad01b;
} ubloxPacket_t;

/********************************************************************/
// UBX ACK/NAK Packet Struct
/********************************************************************/
typedef struct {
  uint8_t  synch1 = UBX_SYNCH_1;
  uint8_t  synch2 = UBX_SYNCH_2;
  uint8_t  UBX_CLASS_ACK;
  uint8_t  messageID;
  uint16_t payloadLength = UBX_ACKNAK_PAYLOADLENGTH;
  uint8_t  pad00a;
  uint8_t  pad00b;
  uint8_t  payload[UBX_ACKNAK_PAYLOADLENGTH];
  uint8_t  pad01a;
  uint8_t  pad01b;
  uint8_t  checksumA;
  uint8_t  checksumB;
  bool     validPacket;
  uint8_t  pad02a;
} ubloxACKNAKPacket_t;

/********************************************************************/
// UBX-CFG-GNSS Info Struct
/********************************************************************/
typedef struct {
  int8_t   gnssId;
  uint8_t  gnssIdType;
  uint8_t  pad00;
  uint8_t  pad01;
  char     gnssIdName[8];
  uint8_t  resTrkCh;
  uint8_t  maxTrkCh;
  uint8_t  enable;
  uint8_t  sigCfgMask;
} ubloxM8CFGGNSSConfigBlock_t;
/********************************************************************/
typedef struct {
  uint8_t  numTrkChHw;
  uint8_t  numTrkChUse;
  uint8_t  numConfigBlocks;
  uint8_t  pad00;
  ubloxM8CFGGNSSConfigBlock_t configBlockList[8];
} ubloxM8CFGGNSSInfo_t;
/********************************************************************/
typedef struct {
  char     name[7];
  uint8_t  enable;
} ubloxM10CFGGNSSSignal_t;
/********************************************************************/
typedef struct {
  int8_t   gnssId;
  uint8_t  gnssIdType;
  uint8_t  pad00;
  uint8_t  pad01;
  char     gnssIdName[8];
  uint8_t  enable;
  uint8_t  numSigs;
  ubloxM10CFGGNSSSignal_t signalList[2];
} ubloxM10CFGGNSSConfigBlock_t;
/********************************************************************/
typedef struct {
  uint8_t  numConfigBlocks;
  uint8_t  pad00;
  uint8_t  pad01;
  uint8_t  pad02;
  ubloxM10CFGGNSSConfigBlock_t configBlockList[6];
} ubloxM10CFGGNSSInfo_t;
/********************************************************************/
typedef struct {
  ubloxM8CFGGNSSInfo_t M8;
  ubloxM10CFGGNSSInfo_t M10;
} ubloxCFGGNSSInfo_t;
/********************************************************************/
// GNSS State - Unknown=-1, Disabled=0, Enabled=1
typedef struct {
  int8_t GPS;
  int8_t GPS_L1CA;
  int8_t SBAS;
  int8_t SBAS_L1CA;
  int8_t Galileo;
  int8_t Galileo_E1;
  int8_t BeiDou;
  int8_t BeiDou_B1;
  int8_t BeiDou_B1C;
  int8_t IMES;
  int8_t QZSS;
  int8_t QZSS_L1CA;
  int8_t QZSS_L1S;
  int8_t GLONASS;
  int8_t GLONASS_L1;
  int8_t UNKNOWN;
} ubloxCFGGNSSState_t;

/********************************************************************/
// UBX-MON-GNSS Info Struct
/********************************************************************/
typedef struct {
  uint8_t  supportedGNSS;
  uint8_t  defaultGNSS;
  uint8_t  enabledGNSS;
  uint8_t  simultaneousGNSS;
} ubloxMONGNSSInfo_t;

/********************************************************************/
// UBX-NAV-PVT Info Struct
/********************************************************************/
typedef struct {
  uint32_t iTOW;
  uint16_t year;
  uint8_t  month;
  uint8_t  day;
  uint8_t  hour;
  uint8_t  minute;
  uint8_t  second;
  bool     dateValid;
  bool     timeValid;
  uint8_t  pad00a;
  uint8_t  pad00b;
  uint8_t  pad00c;
  uint32_t tAcc;
  uint8_t  fixType;
  bool     gnssFixOk;
  uint8_t  numSV;
  uint8_t  pad01a;
  int32_t  longitude;
  int32_t  latitude;
  int32_t  altitude;
  int32_t  altitudeMSL;
  uint32_t hAcc;
  uint32_t vAcc;
  int32_t  velN;
  int32_t  velE;
  int32_t  velD;
  int32_t  gSpeed;
  int32_t  headMot;
  uint32_t sAcc;
  uint32_t headAcc;
  uint16_t pDOP;
  bool     invalidLlh;
  bool     validPacket = false;
} ubloxNAVPVTInfo_t;

/********************************************************************/
// UBX-NAV-SAT Info Struct
/********************************************************************/
typedef struct {
  int8_t   gnssId;
  uint8_t  gnssIdType;
  int8_t   svId;
  uint8_t  cno;
  int16_t  azim;
  int16_t  prRes;
  int8_t   elev;
  bool     healthy;
  bool     ephValid;
  bool     svUsed;
} ubloxNAVSATSVInfo_t;
/********************************************************************/
typedef struct {
  uint32_t iTOW;
  uint8_t  numSvsReceived;
  uint8_t  numSvs;
  uint8_t  numSvsHealthy;
  uint8_t  numSvsEphValid;
  uint8_t  numSvsHealthyAndEphValid;
  uint8_t  numSvsUsed;
  bool     validPacket = false;
  uint8_t  pad00a;
  ubloxNAVSATSVInfo_t svSortList[UBX_MAXNAVSATSATELLITES];
} ubloxNAVSATInfo_t;

/********************************************************************/
// UBX-NAV-STATUS Info Struct
/********************************************************************/
typedef struct {
  uint32_t iTOW;
  uint8_t  gpsFix;
  bool     gpsFixOk;
  uint8_t  psmState;
  uint8_t  spoofDetState;
  bool     spoofingIndicated;
  bool     multipleSpoofingIndications;
  uint8_t  carrSoln;
  uint8_t  pad00a;
  uint32_t ttff;
  uint32_t msss;
  bool     validPacket = false;
  uint8_t  pad01a;
  uint8_t  pad01b;
  uint8_t  pad01c;
} ubloxNAVSTATUSInfo_t;

/********************************************************************/
// UBX-CFG-RST Payloads
/********************************************************************/
const uint8_t UBX_CFG_RST_HARDWARERESET_PAYLOAD[UBX_CFG_RST_PAYLOADLENGTH] = {
  0xFF,0xFF,0x00,0x00
};
const uint8_t UBX_CFG_RST_COLDSTART_PAYLOAD[UBX_CFG_RST_PAYLOADLENGTH] = {
  0xFF,0xFF,0x01,0x00
};
const uint8_t UBX_CFG_RST_WARMSTART_PAYLOAD[UBX_CFG_RST_PAYLOADLENGTH] = {
  0x01,0x00,0x01,0x00
};
const uint8_t UBX_CFG_RST_HOTSTART_PAYLOAD[UBX_CFG_RST_PAYLOADLENGTH] = {
  0x00,0x00,0x01,0x00
};

/********************************************************************/
// TeenyUbloxConnect Class
/********************************************************************/
class TeenyUbloxConnect {

  public:

    // Constructor / destructor / disallow copy and move
    TeenyUbloxConnect();
    virtual ~TeenyUbloxConnect();
    TeenyUbloxConnect(const TeenyUbloxConnect&);
    TeenyUbloxConnect& operator=(const TeenyUbloxConnect&);

// A default of 1100ms for maxWait Serial seems reasonable givin auto packet processing
#ifndef defaultMaxWait // Let's allow the user to define their own value if they want to
#define defaultMaxWait 1100
#endif

    // Ublox setup
    bool    begin(Stream &serialPort_, uint16_t maxWait_ = defaultMaxWait);

    uint8_t getUbloxModuleType();

    // Host methods for process incoming responses/acknowledges from ublox receiver
    // Can be called inside a timer ISR
    // Recommend calling ever 10-50ms - depends on queue size, baud rate and packets
    void    checkUblox();

    // Ublox command methods
    bool    pollUART1Port(uint16_t maxWait_ = defaultMaxWait);
    bool    setPortOutput(uint8_t portID_, uint8_t comSettings_, uint16_t maxWait_ = defaultMaxWait);
    void    setSerialRate(uint32_t baudrate_, uint8_t uartPort_ = COM_PORT_UART1, uint16_t maxWait_ = defaultMaxWait);
    bool    factoryReset();
    void    hardwareReset();
    void    coldStart();
    void    warmStart();
    void    hotStart();
    // ** Don't use clearConfiguration or saveConfiguration without first checking M8/M9/M10 documentation **
    bool    clearConfiguration(uint32_t configMask = 0xFFFF, uint16_t maxWait_ = defaultMaxWait);
    bool    saveConfiguration(uint32_t configMask = 0xFFFF, uint16_t maxWait_ = defaultMaxWait);
    bool    pollProtocolVersion(uint16_t maxWait_ = defaultMaxWait);
    uint8_t getProtocolVersionHigh(uint16_t maxWait_ = defaultMaxWait);
    uint8_t getProtocolVersionLow(uint16_t maxWait_ = defaultMaxWait);
    bool    pollGNSSSelection(uint16_t maxWait_ = defaultMaxWait);
    bool    pollGNSSConfig(uint16_t maxWait_ = defaultMaxWait);
    bool    setGNSSConfig(uint8_t gnssId, bool enable, uint16_t maxWait_ = defaultMaxWait);
    bool    setGNSSSignalConfig(uint8_t gnssId, const char* signalName, bool enable, uint16_t maxWait_ = defaultMaxWait);
    bool    setGNSSConfigState(ubloxCFGGNSSState_t gnssConfigState, uint16_t maxWait_ = defaultMaxWait);
    bool    setMeasurementRate(uint16_t rate_, uint16_t maxWait_ = defaultMaxWait);
    bool    setNavigationRate(uint16_t rate_, uint16_t maxWait_ = defaultMaxWait);
    bool    setAutoNAVPVT(bool enable_, uint16_t maxWait_ = defaultMaxWait);
    bool    setAutoNAVPVTRate(uint8_t rate_, uint16_t maxWait_ = defaultMaxWait);
    bool    setAutoNAVSAT(bool enable_, uint16_t maxWait_ = defaultMaxWait);
    bool    setAutoNAVSATRate(uint8_t rate_, uint16_t maxWait_ = defaultMaxWait);
    bool    setAutoNAVSTATUS(bool enable_, uint16_t maxWait_ = defaultMaxWait);
    bool    setAutoNAVSTATUSRate(uint8_t rate_, uint16_t maxWait_ = defaultMaxWait);

    // Get the latest Position/Velocity/Time solution and fill all global variables
    // Returns true when a packet has been received
    bool    getNAVPVT(); // Use only when autoNAVPVTRate > 0
    bool    pollNAVPVT(uint16_t maxWait_ = defaultMaxWait); // Use only when autoPVTRate = 0

    // Get the latest satellite information
    // Returns true when a packet has been received
    bool    getNAVSAT(); // Use only when autoNAVSATRate > 0
    bool    pollNAVSAT(uint16_t maxWait_ = defaultMaxWait); // Use only when autoNAVSATRate = 0

    // Get the latest navigation status
    // Returns true when a packet has been received
    bool    getNAVSTATUS(); // Use only when autoNAVSTATUSRate > 0
    bool    pollNAVSTATUS(uint16_t maxWait_ = defaultMaxWait); // Use only when autoNAVSTATUSRate = 0

    // Ublox GNSS info data access
    ubloxMONGNSSInfo_t  getGNSSSelectionInfo();
    ubloxCFGGNSSInfo_t  getGNSSConfigInfo();
    ubloxCFGGNSSState_t getGNSSConfigState();

    // Ublox navpvt data access
    void     getNAVPVTPacket(uint8_t *packet_); // Get the full NAV-PVT packet
    void     getNAVPVTInfo(ubloxNAVPVTInfo_t &info_); // summary
    uint32_t getITOW();
    uint16_t getYear();
    uint8_t  getMonth();
    uint8_t  getDay();
    uint8_t  getHour();
    uint8_t  getMinute();
    uint8_t  getSecond();
    bool     getDateValid();
    bool     getTimeValid();
    uint32_t getTimeAccEst();
    uint8_t  getFixType();
    bool     getGnssFixOk();
    uint8_t  getNumSV();
    int32_t  getLongitude();
    int32_t  getLatitude();
    int32_t  getAltitude();
    int32_t  getAltitudeMSL();
    uint32_t getHorizontalAccEst();
    uint32_t getVerticalAccEst();
    int32_t  getVelN();
    int32_t  getVelE();
    int32_t  getVelD();
    int32_t  getGroundSpeed();
    int32_t  getHeading();
    uint32_t getSpeedAccEst();
    uint32_t getHeadingAccEst();
    uint16_t getPDOP();
    bool     getInvalidLlh();
    void     resetNAVSTATUSInfo(); // reset spoofing flags

    // Ublox navsat data access
    void     getNAVSATPacket(uint8_t *packet_); // Get the full NAV-SAT packet
    uint16_t getNAVSATPacketLength(); // Get the actual NAV-SAT packet length
    void     getNAVSATInfo(ubloxNAVSATInfo_t &info_); // summary and sorted sat details

    // Ublox navstatus data access
    void     getNAVSTATUSPacket(uint8_t *packet_); // Get the full NAV-STATUS packet
    void     getNAVSTATUSInfo(ubloxNAVSTATUSInfo_t &info_); // summary

    // Access lost packet counts
    uint8_t  getLostRxPacketCount();
    uint8_t  getUnknownRxPacketCount();
    uint8_t  getLostNAVPVTPacketCount();
    uint8_t  getLostNAVSATPacketCount();
    uint8_t  getLostNAVSTATUSPacketCount();

  private:
    Stream   *serialPort;

    ubx_module_type_t ubloxModuleType = UBLOX_UNKNOWN_MODULE;

    bool pollUART1Port_M8(uint16_t maxWait_ = defaultMaxWait, bool flushPort_ = false);
    bool pollUART1Port_M10(uint16_t maxWait_ = defaultMaxWait, bool flushPort_ = false);
    bool setPortOutput_M8(uint8_t portID_, uint8_t comSettings_, uint16_t maxWait_ = defaultMaxWait);
    bool setPortOutput_M10(uint8_t portID_, uint8_t comSettings_, uint16_t maxWait_ = defaultMaxWait);
    void setSerialRate_M8(uint32_t baudrate_, uint8_t uartPort_ = COM_PORT_UART1, uint16_t maxWait_ = defaultMaxWait);
    void setSerialRate_M10(uint32_t baudrate_, uint8_t uartPort_ = COM_PORT_UART1, uint16_t maxWait_ = defaultMaxWait);
    void resetGNSSConfigState();
    bool pollGNSSConfig_M8(uint16_t maxWait_ = defaultMaxWait);
    bool pollGNSSConfig_M9(uint16_t maxWait_ = defaultMaxWait);
    bool pollGNSSConfig_M10(uint16_t maxWait_ = defaultMaxWait);
    bool setGNSSConfig_M8(uint8_t gnssId, bool enable, uint16_t maxWait_ = defaultMaxWait);
    bool setGNSSConfig_M10(uint8_t gnssId, bool enable, uint16_t maxWait_ = defaultMaxWait);
    bool setGNSSSignalConfig_M8(uint8_t gnssId, const char* signalName, bool enable, uint16_t maxWait_ = defaultMaxWait);
    bool setGNSSSignalConfig_M9(uint8_t gnssId, const char* signalName, bool enable, uint16_t maxWait_ = defaultMaxWait);
    bool setGNSSSignalConfig_M10(uint8_t gnssId, const char* signalName, bool enable, uint16_t maxWait_ = defaultMaxWait);
    bool setGNSSConfigState_M8(ubloxCFGGNSSState_t gnssConfigState, uint16_t maxWait_ = defaultMaxWait);
    bool setGNSSConfigState_M9(ubloxCFGGNSSState_t gnssConfigState, uint16_t maxWait_ = defaultMaxWait);
    bool setGNSSConfigState_M10(ubloxCFGGNSSState_t gnssConfigState, uint16_t maxWait_ = defaultMaxWait);
    bool setMeasurementRate_M8(uint16_t rate_, uint16_t maxWait_ = defaultMaxWait);
    bool setMeasurementRate_M10(uint16_t rate_, uint16_t maxWait_ = defaultMaxWait);
    bool setNavigationRate_M8(uint16_t rate_, uint16_t maxWait_ = defaultMaxWait);
    bool setNavigationRate_M10(uint16_t rate_, uint16_t maxWait_ = defaultMaxWait);
    bool setAutoNAVPVTRate_M8(uint8_t rate_, uint16_t maxWait_ = defaultMaxWait);
    bool setAutoNAVPVTRate_M10(uint8_t rate_, uint16_t maxWait_ = defaultMaxWait);
    bool setAutoNAVSATRate_M8(uint8_t rate_, uint16_t maxWait_ = defaultMaxWait);
    bool setAutoNAVSATRate_M10(uint8_t rate_, uint16_t maxWait_ = defaultMaxWait);
    bool setAutoNAVSTATUSRate_M8(uint8_t rate_, uint16_t maxWait_ = defaultMaxWait);
    bool setAutoNAVSTATUSRate_M10(uint8_t rate_, uint16_t maxWait_ = defaultMaxWait);

    ubloxPacket_t        commandPacket;
    ubloxPacket_t        incomingPacket;
    ubloxPacket_t        receivedPacket;
    ubloxPacket_t        responsePacket;
    ubloxACKNAKPacket_t  acknowledgePacket;
    ubloxMONGNSSInfo_t   ubloxMONGNSSInfo;
    bool                 ubloxCFGGNSSInfoValid = false;
    ubloxCFGGNSSInfo_t   ubloxCFGGNSSInfo;
    ubloxCFGGNSSState_t  ubloxCFGGNSSState;
    ubloxPacket_t        ubloxNAVPVTPacketBuffer;
    uint8_t              ubloxNAVPVTPacket[UBX_NAV_PVT_PACKETLENGTH];
    ubloxNAVPVTInfo_t    ubloxNAVPVTInfo;
    ubloxPacket_t        ubloxNAVSATPacketBuffer;
    uint8_t              ubloxNAVSATPacket[UBX_NAV_SAT_MAXPACKETLENGTH];
    uint16_t             ubloxNAVSATPacketLength;
    ubloxNAVSATInfo_t    ubloxNAVSATInfo;
    ubloxPacket_t        ubloxNAVSTATUSPacketBuffer;
    uint8_t              ubloxNAVSTATUSPacket[UBX_NAV_STATUS_PACKETLENGTH];
    ubloxNAVSTATUSInfo_t ubloxNAVSTATUSInfo;

    void     calcCommandPacketChecksum();
    bool     sendCommandPacket(bool expectResp_, bool expectAck_, uint16_t maxWait_);
    volatile bool processingUbloxCommand;
    void     checkUbloxInternal();
    void     processIncomingByte(uint8_t incomingByte_);
    // Do not call processIncomingPacket() in ISR - uses serial.write
    void     processIncomingPacket(uint8_t requestedClass_ = 0, uint8_t requestedID_ = 0);
    uint8_t  lostRxPacketCount;
    uint8_t  unknownRxPacketCount;
    uint8_t  protocolVersionHigh;
    uint8_t  protocolVersionLow;
    uint8_t  lostNAVPVTPacketCount;
    bool     processNAVPVTPacket();
    void     setNAVPVTPacketInfo();
    uint8_t  lostNAVSATPacketCount;
    bool     processNAVSATPacket();
    void     setNAVSATPacketInfo();
    uint8_t  lostNAVSTATUSPacketCount;
    bool     processNAVSTATUSPacket();
    void     setNAVSTATUSPacketInfo();

};

#endif //TeenyUbloxConnect_h

