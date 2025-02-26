/*
TeenyUbloxConnect.h - Class file for the TeenyUbloxConnect Arduino Library.
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
#include "TeenyUbloxConnect.h"

/********************************************************************/
TeenyUbloxConnect::TeenyUbloxConnect() { }

TeenyUbloxConnect::~TeenyUbloxConnect() { }

/********************************************************************/
/********************************************************************/
// begin
/********************************************************************/
/********************************************************************/
bool TeenyUbloxConnect::begin(Stream &serialPort_, uint16_t maxWait_) {
  serialPort = &serialPort_;
  resetNAVSTATUSInfo(); // reset spoofing flags
  ubloxModuleType = UBLOX_UNKNOWN_MODULE;
  ubloxCFGGNSSInfoValid = false;
  // Try three times
  bool connected = false;
  if(pollUART1Port_M8(maxWait_, true)) {
    connected = true;
  } else if(pollUART1Port_M8(maxWait_, true)) {
    connected = true;
  } else if(pollUART1Port_M8(maxWait_, true)) {
    connected = true;
  }
  if(connected) {
    if(pollProtocolVersion(maxWait_)) {
      if((protocolVersionHigh == 18) && (protocolVersionLow == 00)) {
        ubloxModuleType = UBLOX_M8_MODULE;
        return true;
      } else if((protocolVersionHigh == 32) && (protocolVersionLow == 01)) {
        ubloxModuleType = UBLOX_M9_MODULE;
        return true;
      } else if((protocolVersionHigh == 34) && (protocolVersionLow == 10)) {
        ubloxModuleType = UBLOX_M10_MODULE;
        return true;
      }
    }
  }
  return false;
}

/********************************************************************/
/********************************************************************/
// ublox commands
/********************************************************************/
/********************************************************************/
uint8_t TeenyUbloxConnect::getUbloxModuleType() {
  return ubloxModuleType;
}

/********************************************************************/
bool TeenyUbloxConnect::pollUART1Port(uint16_t maxWait_) {
  if(ubloxModuleType == UBLOX_M8_MODULE) {
    return pollUART1Port_M8(maxWait_);
  } else if(ubloxModuleType == UBLOX_M9_MODULE) {
    return pollUART1Port_M10(maxWait_);
  } else if(ubloxModuleType == UBLOX_M10_MODULE) {
    return pollUART1Port_M10(maxWait_);
  }
  return false;
}
/********************************************************************/
bool TeenyUbloxConnect::pollUART1Port_M8(uint16_t maxWait_, bool flushPort_) {
  commandPacket.messageClass = UBX_CLASS_CFG;
  commandPacket.messageID = UBX_CFG_PRT;
  commandPacket.payloadLength = 1;
  commandPacket.payload[0] = COM_PORT_UART1;
  commandPacket.validPacket = true;
  if(flushPort_) while(serialPort->available()) serialPort->read();
  return sendCommandPacket(true, true, maxWait_);
}
/********************************************************************/
bool TeenyUbloxConnect::pollUART1Port_M10(uint16_t maxWait_, bool flushPort_) {
  commandPacket.messageClass = UBX_CLASS_CFG;
  commandPacket.messageID = UBX_CFG_VALGET;
  commandPacket.payloadLength = 8;
  commandPacket.payload[0] = 0;
  commandPacket.payload[1] = VALGET_LAYER_RAM;
  commandPacket.payload[2] = 0;
  commandPacket.payload[3] = 0;
  for(uint8_t i = 0; i < 4; i++)
    commandPacket.payload[4 + i] = UBLOX_CFG_UART1_ENABLED >> (8 * i);
  commandPacket.validPacket = true;
  if(flushPort_) while(serialPort->available()) serialPort->read();
  return sendCommandPacket(true, true, maxWait_);
}

/********************************************************************/
bool TeenyUbloxConnect::setPortOutput(uint8_t portID_, uint8_t comSettings_, uint16_t maxWait_) {
  if(ubloxModuleType == UBLOX_M8_MODULE) {
    return setPortOutput_M8(portID_, comSettings_, maxWait_);
  } else if(ubloxModuleType == UBLOX_M9_MODULE) {
    return setPortOutput_M10(portID_, comSettings_, maxWait_);
  } else if(ubloxModuleType == UBLOX_M10_MODULE) {
    return setPortOutput_M10(portID_, comSettings_, maxWait_);
  }
  return false;
}
/********************************************************************/
bool TeenyUbloxConnect::setPortOutput_M8(uint8_t portID_, uint8_t comSettings_, uint16_t maxWait_) {
  commandPacket.messageClass = UBX_CLASS_CFG;
  commandPacket.messageID = UBX_CFG_PRT;
  commandPacket.payloadLength = 1;
  commandPacket.payload[0] = portID_;
  commandPacket.validPacket = true;
  if(sendCommandPacket(true, true, maxWait_)) {
    commandPacket = responsePacket;
    commandPacket.payload[14] = comSettings_;
    commandPacket.validPacket = true;
    return sendCommandPacket(false, true, maxWait_);
  }
  return false;
}
/********************************************************************/
bool TeenyUbloxConnect::setPortOutput_M10(uint8_t portID_, uint8_t comSettings_, uint16_t maxWait_) {
  commandPacket.messageClass = UBX_CLASS_CFG;
  commandPacket.messageID = UBX_CFG_VALSET;
  commandPacket.payloadLength = 9;
  commandPacket.payload[0] = 0;
  commandPacket.payload[1] = VALSET_LAYER_RAM;
  commandPacket.payload[2] = 0;
  commandPacket.payload[3] = 0;
  if(portID_ == COM_PORT_UART1) {
    for(uint8_t i = 0; i < 4; i++)
      commandPacket.payload[4 + i] = UBLOX_CFG_UART1OUTPROT_UBX >> (8 * i);
    commandPacket.payload[8] = (comSettings_ & COM_TYPE_UBX) == 0 ? false : true;
    commandPacket.validPacket = true;
    if(sendCommandPacket(false, true, maxWait_)) {
      for(uint8_t i = 0; i < 4; i++)
        commandPacket.payload[4 + i] = UBLOX_CFG_UART1OUTPROT_NMEA >> (8 * i);
      commandPacket.payload[8] = (comSettings_ & COM_TYPE_NMEA) == 0 ? false : true;
      commandPacket.validPacket = true;
      return sendCommandPacket(false, true, maxWait_);
    }
  }
  return false;
}

/********************************************************************/
void TeenyUbloxConnect::setSerialRate(uint32_t baudrate_, uint8_t uartPort_, uint16_t maxWait_) {
  if(ubloxModuleType == UBLOX_M8_MODULE) {
    setSerialRate_M8(baudrate_, uartPort_, maxWait_);
  } else if(ubloxModuleType == UBLOX_M9_MODULE) {
    setSerialRate_M10(baudrate_, uartPort_, maxWait_);
  } else if(ubloxModuleType == UBLOX_M10_MODULE) {
    setSerialRate_M10(baudrate_, uartPort_, maxWait_);
  }
}
/********************************************************************/
void TeenyUbloxConnect::setSerialRate_M8(uint32_t baudrate_, uint8_t uartPort_, uint16_t maxWait_) {
  commandPacket.messageClass = UBX_CLASS_CFG;
  commandPacket.messageID = UBX_CFG_PRT;
  commandPacket.payloadLength = 1;
  commandPacket.payload[0] = uartPort_;
  commandPacket.validPacket = true;
  if(sendCommandPacket(true, true, maxWait_)) {
    commandPacket = responsePacket;
    for(uint8_t i = 0; i < 4; i++)
      commandPacket.payload[8 + i] = baudrate_ >> (8 * i);
    commandPacket.validPacket = true;
    sendCommandPacket(false, false, maxWait_); // ACK lost due to baudrate change
  }
}
/********************************************************************/
void TeenyUbloxConnect::setSerialRate_M10(uint32_t baudrate_, uint8_t uartPort_, uint16_t maxWait_) {
  commandPacket.messageClass = UBX_CLASS_CFG;
  commandPacket.messageID = UBX_CFG_VALSET;
  commandPacket.payloadLength = 12;
  commandPacket.payload[0] = 0;
  commandPacket.payload[1] = VALSET_LAYER_RAM;
  commandPacket.payload[2] = 0;
  commandPacket.payload[3] = 0;
  if(uartPort_ == COM_PORT_UART1) {
    for(uint8_t i = 0; i < 4; i++)
      commandPacket.payload[4 + i] = UBLOX_CFG_UART1_BAUDRATE >> (8 * i);
    for(uint8_t i = 0; i < 4; i++)
      commandPacket.payload[8 + i] = baudrate_ >> (8 * i);
    commandPacket.validPacket = true;
    sendCommandPacket(false, false, maxWait_); // ACK lost due to baudrate change
  }
}

/********************************************************************/
bool TeenyUbloxConnect::factoryReset() {
  if(clearConfiguration(0x0000FFFF)) {
    // Will need system reboot to access GPS module
    hardwareReset();
    return true;
  }
  return false;
}

/********************************************************************/
void TeenyUbloxConnect::hardwareReset() {
  resetNAVSTATUSInfo(); // reset spoofing flags
  commandPacket.messageClass = UBX_CLASS_CFG;
  commandPacket.messageID = UBX_CFG_RST;
  commandPacket.payloadLength = UBX_CFG_RST_PAYLOADLENGTH;
  memcpy(commandPacket.payload, UBX_CFG_RST_HARDWARERESET_PAYLOAD, UBX_CFG_RST_PAYLOADLENGTH);
  commandPacket.validPacket = true;
  sendCommandPacket(false, false, 0);
}

/********************************************************************/
void TeenyUbloxConnect::coldStart() {
  resetNAVSTATUSInfo(); // reset spoofing flags
  commandPacket.messageClass = UBX_CLASS_CFG;
  commandPacket.messageID = UBX_CFG_RST;
  commandPacket.payloadLength = UBX_CFG_RST_PAYLOADLENGTH;
  memcpy(commandPacket.payload, UBX_CFG_RST_COLDSTART_PAYLOAD, UBX_CFG_RST_PAYLOADLENGTH);
  commandPacket.validPacket = true;
  sendCommandPacket(false, false, 0);
}

/********************************************************************/
void TeenyUbloxConnect::warmStart() {
  resetNAVSTATUSInfo(); // reset spoofing flags
  commandPacket.messageClass = UBX_CLASS_CFG;
  commandPacket.messageID = UBX_CFG_RST;
  commandPacket.payloadLength = UBX_CFG_RST_PAYLOADLENGTH;
  memcpy(commandPacket.payload, UBX_CFG_RST_WARMSTART_PAYLOAD, UBX_CFG_RST_PAYLOADLENGTH);
  commandPacket.validPacket = true;
  sendCommandPacket(false, false, 0);
}

/********************************************************************/
void TeenyUbloxConnect::hotStart() {
  resetNAVSTATUSInfo(); // reset spoofing flags
  commandPacket.messageClass = UBX_CLASS_CFG;
  commandPacket.messageID = UBX_CFG_RST;
  commandPacket.payloadLength = UBX_CFG_RST_PAYLOADLENGTH;
  memcpy(commandPacket.payload, UBX_CFG_RST_HOTSTART_PAYLOAD, UBX_CFG_RST_PAYLOADLENGTH);
  commandPacket.validPacket = true;
  sendCommandPacket(false, false, 0);
}

/********************************************************************/
bool TeenyUbloxConnect::clearConfiguration(uint32_t configMask, uint16_t maxWait_) {
  commandPacket.messageClass = UBX_CLASS_CFG;
  commandPacket.messageID = UBX_CFG_CFG;
  commandPacket.payloadLength = 12;
  memset(commandPacket.payload, 0, commandPacket.payloadLength);
  commandPacket.payload[0] = configMask & 0xFF;
  commandPacket.payload[1] = (configMask >> 8) & 0xFF;
  commandPacket.payload[2] = (configMask >> 16) & 0xFF;
  commandPacket.payload[3] = (configMask >> 24) & 0xFF;
  commandPacket.validPacket = true;
  return sendCommandPacket(false, true, maxWait_);
}

/********************************************************************/
bool TeenyUbloxConnect::saveConfiguration(uint32_t configMask, uint16_t maxWait_) {
  commandPacket.messageClass = UBX_CLASS_CFG;
  commandPacket.messageID = UBX_CFG_CFG;
  commandPacket.payloadLength = 12;
  memset(commandPacket.payload, 0, commandPacket.payloadLength);
  commandPacket.payload[4] = configMask & 0xFF;
  commandPacket.payload[5] = (configMask >> 8) & 0xFF;
  commandPacket.payload[6] = (configMask >> 16) & 0xFF;
  commandPacket.payload[7] = (configMask >> 24) & 0xFF;
  commandPacket.validPacket = true;
  return sendCommandPacket(false, true, maxWait_);
}

/********************************************************************/
bool TeenyUbloxConnect::pollProtocolVersion(uint16_t maxWait_) {
  commandPacket.messageClass = UBX_CLASS_MON;
  commandPacket.messageID = UBX_MON_VER;
  commandPacket.payloadLength = 0;
  commandPacket.validPacket = true;
  if(sendCommandPacket(true, false, maxWait_)) {
    protocolVersionHigh = ((responsePacket.payload[78] - '0') * 10) +
                          (responsePacket.payload[79] - '0');
    protocolVersionLow  = ((responsePacket.payload[81] - '0') * 10) +
                          (responsePacket.payload[82] - '0');
    return true;
  }
  protocolVersionHigh = 0;
  protocolVersionLow = 0;
  return false;
}

/********************************************************************/
uint8_t TeenyUbloxConnect::getProtocolVersionHigh(uint16_t maxWait_) {
  return protocolVersionHigh;
}

/********************************************************************/
uint8_t TeenyUbloxConnect::getProtocolVersionLow(uint16_t maxWait_) {
  return protocolVersionLow;
}

/********************************************************************/
bool TeenyUbloxConnect::pollGNSSSelection(uint16_t maxWait_) {
  commandPacket.messageClass = UBX_CLASS_MON;
  commandPacket.messageID = UBX_MON_GNSS;
  commandPacket.payloadLength = 0;
  commandPacket.validPacket = true;
  if(sendCommandPacket(true, false, maxWait_)) {
    ubloxMONGNSSInfo.supportedGNSS    = responsePacket.payload[1];
    ubloxMONGNSSInfo.defaultGNSS      = responsePacket.payload[2];
    ubloxMONGNSSInfo.enabledGNSS      = responsePacket.payload[3];
    ubloxMONGNSSInfo.simultaneousGNSS = responsePacket.payload[4];
    return true;
  }
  ubloxMONGNSSInfo.supportedGNSS = 0;
  ubloxMONGNSSInfo.defaultGNSS = 0;
  ubloxMONGNSSInfo.enabledGNSS = 0;
  ubloxMONGNSSInfo.simultaneousGNSS = 0;
  return false;
}

/********************************************************************/
bool TeenyUbloxConnect::pollGNSSConfig(uint16_t maxWait_) {
  ubloxCFGGNSSInfoValid = false;
  resetGNSSConfigState();
  if(ubloxModuleType == UBLOX_M8_MODULE) {
    ubloxCFGGNSSInfoValid = pollGNSSConfig_M8(maxWait_);
  } else if(ubloxModuleType == UBLOX_M9_MODULE) {
    ubloxCFGGNSSInfoValid = pollGNSSConfig_M9(maxWait_);
  } else if(ubloxModuleType == UBLOX_M10_MODULE) {
    ubloxCFGGNSSInfoValid = pollGNSSConfig_M10(maxWait_);
  }
  return ubloxCFGGNSSInfoValid;
}
/********************************************************************/
bool TeenyUbloxConnect::pollGNSSConfig_M8(uint16_t maxWait_) {
  commandPacket.messageClass = UBX_CLASS_CFG;
  commandPacket.messageID = UBX_CFG_GNSS;
  commandPacket.payloadLength = 0;
  commandPacket.validPacket = true;
  if(sendCommandPacket(true, true, maxWait_)) {
    ubloxCFGGNSSInfo.M8.numTrkChHw      = responsePacket.payload[1];
    ubloxCFGGNSSInfo.M8.numTrkChUse     = responsePacket.payload[2];
    ubloxCFGGNSSInfo.M8.numConfigBlocks = responsePacket.payload[3];
    char gnssIdTypeMap[8]={'G','S','E','B','I','Q','R','N'};
    char gnssIdNameMap[8][8]={"GPS", "SBAS", "Galileo", "BeiDou", "IMES", "QZSS", "GLONASS", "NAVIC"};
    for(uint8_t i = 0; i < ubloxCFGGNSSInfo.M8.numConfigBlocks; i++) {
      ubloxCFGGNSSInfo.M8.configBlockList[i].gnssId = responsePacket.payload[4+(i*8)];
      if((ubloxCFGGNSSInfo.M8.configBlockList[i].gnssId < 0) ||
         (ubloxCFGGNSSInfo.M8.configBlockList[i].gnssId > 7)) {
        ubloxCFGGNSSInfo.M8.configBlockList[i].gnssId = -1;
        ubloxCFGGNSSInfo.M8.configBlockList[i].gnssIdType = '?';
      } else {
        ubloxCFGGNSSInfo.M8.configBlockList[i].gnssIdType =
          gnssIdTypeMap[ubloxCFGGNSSInfo.M8.configBlockList[i].gnssId];
        strcpy(ubloxCFGGNSSInfo.M8.configBlockList[i].gnssIdName,
               gnssIdNameMap[ubloxCFGGNSSInfo.M8.configBlockList[i].gnssId]);
      }
      ubloxCFGGNSSInfo.M8.configBlockList[i].resTrkCh = responsePacket.payload[4+(i*8)+1];
      ubloxCFGGNSSInfo.M8.configBlockList[i].maxTrkCh = responsePacket.payload[4+(i*8)+2];
      ubloxCFGGNSSInfo.M8.configBlockList[i].enable = responsePacket.payload[4+(i*8)+4];
      ubloxCFGGNSSInfo.M8.configBlockList[i].sigCfgMask = responsePacket.payload[4+(i*8)+6];
      int8_t gnssId = responsePacket.payload[4+(i*8)];
      if((gnssId >= 0) && (gnssId <= 7)) {
        uint8_t gnssIdType = gnssIdTypeMap[gnssId];
        int8_t  gnssEnable = responsePacket.payload[4+(i*8)+4];
        switch(gnssIdType) {
          case 'G':
            ubloxCFGGNSSState.GPS = gnssEnable;
            break;
          case 'S':
            ubloxCFGGNSSState.SBAS = gnssEnable;
            break;
          case 'E':
            ubloxCFGGNSSState.Galileo = gnssEnable;
            break;
          case 'B':
            ubloxCFGGNSSState.BeiDou = gnssEnable;
            break;
          case 'I':
            ubloxCFGGNSSState.IMES = gnssEnable;
            break;
          case 'Q':
            ubloxCFGGNSSState.QZSS = gnssEnable;
            break;
          case 'R':
            ubloxCFGGNSSState.GLONASS = gnssEnable;
            break;
        }
      }
    }
    return true;
  }
  ubloxCFGGNSSInfo.M8.numTrkChHw = 0;
  ubloxCFGGNSSInfo.M8.numTrkChUse = 0;
  ubloxCFGGNSSInfo.M8.numConfigBlocks = 0;
  return false;
}
/********************************************************************/
bool TeenyUbloxConnect::pollGNSSConfig_M9(uint16_t maxWait_) {
  commandPacket.messageClass = UBX_CLASS_CFG;
  commandPacket.messageID = UBX_CFG_VALGET;
  commandPacket.payloadLength = 52;
  commandPacket.payload[0] = 0;
  commandPacket.payload[1] = VALGET_LAYER_RAM;
  commandPacket.payload[2] = 0;
  commandPacket.payload[3] = 0;
  for(uint8_t i = 0; i < 4; i++) commandPacket.payload[4 + i]  = UBLOX_CFG_SIGNAL_GPS_ENA >> (8 * i);
  for(uint8_t i = 0; i < 4; i++) commandPacket.payload[8 + i]  = UBLOX_CFG_SIGNAL_GPS_L1CA_ENA >> (8 * i);
  for(uint8_t i = 0; i < 4; i++) commandPacket.payload[12 + i] = UBLOX_CFG_SIGNAL_SBAS_ENA >> (8 * i);
  for(uint8_t i = 0; i < 4; i++) commandPacket.payload[16 + i] = UBLOX_CFG_SIGNAL_SBAS_L1CA_ENA >> (8 * i);
  for(uint8_t i = 0; i < 4; i++) commandPacket.payload[20 + i] = UBLOX_CFG_SIGNAL_GAL_ENA >> (8 * i);
  for(uint8_t i = 0; i < 4; i++) commandPacket.payload[24 + i] = UBLOX_CFG_SIGNAL_GAL_E1_ENA >> (8 * i);
  for(uint8_t i = 0; i < 4; i++) commandPacket.payload[28 + i] = UBLOX_CFG_SIGNAL_BDS_ENA >> (8 * i);
  for(uint8_t i = 0; i < 4; i++) commandPacket.payload[32 + i] = UBLOX_CFG_SIGNAL_BDS_B1_ENA >> (8 * i);
  for(uint8_t i = 0; i < 4; i++) commandPacket.payload[36 + i] = UBLOX_CFG_SIGNAL_QZSS_ENA >> (8 * i);
  for(uint8_t i = 0; i < 4; i++) commandPacket.payload[40 + i] = UBLOX_CFG_SIGNAL_QZSS_L1CA_ENA >> (8 * i);
  for(uint8_t i = 0; i < 4; i++) commandPacket.payload[44 + i] = UBLOX_CFG_SIGNAL_GLO_ENA >> (8 * i);
  for(uint8_t i = 0; i < 4; i++) commandPacket.payload[48 + i] = UBLOX_CFG_SIGNAL_GLO_L1_ENA >> (8 * i);
  commandPacket.validPacket = true;
  if(sendCommandPacket(true, true, maxWait_) &&
     (responsePacket.payloadLength == 64)) {
    uint32_t gnssKey;
    bool gnssEnable;
    uint8_t cfgBlkNum = 0;
    uint8_t cfgSigNum = 0;
    for(uint8_t i = 0; i < 13; i++) {
      gnssKey =  responsePacket.payload[4+(i*5)+0];
      gnssKey |= responsePacket.payload[4+(i*5)+1] << 8;
      gnssKey |= responsePacket.payload[4+(i*5)+2] << 16;
      gnssKey |= responsePacket.payload[4+(i*5)+3] << 24;
      gnssEnable = responsePacket.payload[4+(i*5)+4];
      switch(gnssKey) {
        case UBLOX_CFG_SIGNAL_GPS_ENA:
          ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum].gnssId = 0;
          ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum].gnssIdType = 'G';
          strcpy(ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum].gnssIdName, "GPS");
          ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum].enable = gnssEnable;
          cfgSigNum = 0;
          ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum].numSigs = cfgSigNum;
          cfgBlkNum++;
          ubloxCFGGNSSInfo.M10.numConfigBlocks = cfgBlkNum;
          ubloxCFGGNSSState.GPS = gnssEnable;
          break;
        case UBLOX_CFG_SIGNAL_GPS_L1CA_ENA:
          strncpy(ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum-1].signalList[cfgSigNum].name, "L1CA", 5);
          ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum-1].signalList[cfgSigNum].enable = gnssEnable;
          cfgSigNum++;
          ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum-1].numSigs = cfgSigNum;
          ubloxCFGGNSSState.GPS_L1CA = gnssEnable;
          break;
        case UBLOX_CFG_SIGNAL_SBAS_ENA:
          ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum].gnssId = 1;
          ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum].gnssIdType = 'S';
          strcpy(ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum].gnssIdName, "SBAS");
          ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum].enable = gnssEnable;
          cfgSigNum = 0;
          ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum].numSigs = cfgSigNum;
          cfgBlkNum++;
          ubloxCFGGNSSInfo.M10.numConfigBlocks = cfgBlkNum;
          ubloxCFGGNSSState.SBAS = gnssEnable;
          break;
        case UBLOX_CFG_SIGNAL_SBAS_L1CA_ENA:
          strncpy(ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum-1].signalList[cfgSigNum].name, "L1CA", 5);
          ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum-1].signalList[cfgSigNum].enable = gnssEnable;
          cfgSigNum++;
          ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum-1].numSigs = cfgSigNum;
          ubloxCFGGNSSState.SBAS_L1CA = gnssEnable;
          break;
        case UBLOX_CFG_SIGNAL_GAL_ENA:
          ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum].gnssId = 2;
          ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum].gnssIdType = 'E';
          strcpy(ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum].gnssIdName, "Galileo");
          ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum].enable = gnssEnable;
          cfgSigNum = 0;
          ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum].numSigs = cfgSigNum;
          cfgBlkNum++;
          ubloxCFGGNSSInfo.M10.numConfigBlocks = cfgBlkNum;
          ubloxCFGGNSSState.Galileo = gnssEnable;
          break;
        case UBLOX_CFG_SIGNAL_GAL_E1_ENA:
          strncpy(ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum-1].signalList[cfgSigNum].name, "E1", 3);
          ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum-1].signalList[cfgSigNum].enable = gnssEnable;
          cfgSigNum++;
          ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum-1].numSigs = cfgSigNum;
          ubloxCFGGNSSState.Galileo_E1 = gnssEnable;
          break;
        case UBLOX_CFG_SIGNAL_BDS_ENA:
          ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum].gnssId = 3;
          ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum].gnssIdType = 'B';
          strcpy(ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum].gnssIdName, "BeiDou");
          ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum].enable = gnssEnable;
          cfgSigNum = 0;
          ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum].numSigs = cfgSigNum;
          cfgBlkNum++;
          ubloxCFGGNSSInfo.M10.numConfigBlocks = cfgBlkNum;
          ubloxCFGGNSSState.BeiDou = gnssEnable;
          break;
        case UBLOX_CFG_SIGNAL_BDS_B1_ENA:
          strncpy(ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum-1].signalList[cfgSigNum].name, "B1", 3);
          ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum-1].signalList[cfgSigNum].enable = gnssEnable;
          cfgSigNum++;
          ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum-1].numSigs = cfgSigNum;
          ubloxCFGGNSSState.BeiDou_B1 = gnssEnable;
          break;
        case UBLOX_CFG_SIGNAL_QZSS_ENA:
          ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum].gnssId = 5;
          ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum].gnssIdType = 'Q';
          strcpy(ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum].gnssIdName, "QZSS");
          ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum].enable = gnssEnable;
          cfgSigNum = 0;
          ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum].numSigs = cfgSigNum;
          cfgBlkNum++;
          ubloxCFGGNSSInfo.M10.numConfigBlocks = cfgBlkNum;
          ubloxCFGGNSSState.QZSS = gnssEnable;
          break;
        case UBLOX_CFG_SIGNAL_QZSS_L1CA_ENA:
          strncpy(ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum-1].signalList[cfgSigNum].name, "L1CA", 5);
          ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum-1].signalList[cfgSigNum].enable = gnssEnable;
          cfgSigNum++;
          ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum-1].numSigs = cfgSigNum;
          ubloxCFGGNSSState.QZSS_L1CA = gnssEnable;
          break;
        case UBLOX_CFG_SIGNAL_GLO_ENA:
          ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum].gnssId = 6;
          ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum].gnssIdType = 'R';
          strcpy(ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum].gnssIdName, "GLONASS");
          ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum].enable = gnssEnable;
          cfgSigNum = 0;
          ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum].numSigs = cfgSigNum;
          cfgBlkNum++;
          ubloxCFGGNSSInfo.M10.numConfigBlocks = cfgBlkNum;
          ubloxCFGGNSSState.GLONASS = gnssEnable;
          break;
        case UBLOX_CFG_SIGNAL_GLO_L1_ENA:
          strncpy(ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum-1].signalList[cfgSigNum].name, "L1", 3);
          ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum-1].signalList[cfgSigNum].enable = gnssEnable;
          cfgSigNum++;
          ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum-1].numSigs = cfgSigNum;
          ubloxCFGGNSSState.GLONASS_L1 = gnssEnable;
          break;
      }
    }
    return true;
  }
  ubloxCFGGNSSInfo.M10.numConfigBlocks = 0;
  return false;
}
/********************************************************************/
bool TeenyUbloxConnect::pollGNSSConfig_M10(uint16_t maxWait_) {
  commandPacket.messageClass = UBX_CLASS_CFG;
  commandPacket.messageID = UBX_CFG_VALGET;
  commandPacket.payloadLength = 60;
  commandPacket.payload[0] = 0;
  commandPacket.payload[1] = VALGET_LAYER_RAM;
  commandPacket.payload[2] = 0;
  commandPacket.payload[3] = 0;
  for(uint8_t i = 0; i < 4; i++) commandPacket.payload[4 + i]  = UBLOX_CFG_SIGNAL_GPS_ENA >> (8 * i);
  for(uint8_t i = 0; i < 4; i++) commandPacket.payload[8 + i]  = UBLOX_CFG_SIGNAL_GPS_L1CA_ENA >> (8 * i);
  for(uint8_t i = 0; i < 4; i++) commandPacket.payload[12 + i] = UBLOX_CFG_SIGNAL_SBAS_ENA >> (8 * i);
  for(uint8_t i = 0; i < 4; i++) commandPacket.payload[16 + i] = UBLOX_CFG_SIGNAL_SBAS_L1CA_ENA >> (8 * i);
  for(uint8_t i = 0; i < 4; i++) commandPacket.payload[20 + i] = UBLOX_CFG_SIGNAL_GAL_ENA >> (8 * i);
  for(uint8_t i = 0; i < 4; i++) commandPacket.payload[24 + i] = UBLOX_CFG_SIGNAL_GAL_E1_ENA >> (8 * i);
  for(uint8_t i = 0; i < 4; i++) commandPacket.payload[28 + i] = UBLOX_CFG_SIGNAL_BDS_ENA >> (8 * i);
  for(uint8_t i = 0; i < 4; i++) commandPacket.payload[32 + i] = UBLOX_CFG_SIGNAL_BDS_B1_ENA >> (8 * i);
  for(uint8_t i = 0; i < 4; i++) commandPacket.payload[36 + i] = UBLOX_CFG_SIGNAL_BDS_B1C_ENA >> (8 * i);
  for(uint8_t i = 0; i < 4; i++) commandPacket.payload[40 + i] = UBLOX_CFG_SIGNAL_QZSS_ENA >> (8 * i);
  for(uint8_t i = 0; i < 4; i++) commandPacket.payload[44 + i] = UBLOX_CFG_SIGNAL_QZSS_L1CA_ENA >> (8 * i);
  for(uint8_t i = 0; i < 4; i++) commandPacket.payload[48 + i] = UBLOX_CFG_SIGNAL_QZSS_L1S_ENA >> (8 * i);
  for(uint8_t i = 0; i < 4; i++) commandPacket.payload[52 + i] = UBLOX_CFG_SIGNAL_GLO_ENA >> (8 * i);
  for(uint8_t i = 0; i < 4; i++) commandPacket.payload[56 + i] = UBLOX_CFG_SIGNAL_GLO_L1_ENA >> (8 * i);
  commandPacket.validPacket = true;
  if(sendCommandPacket(true, true, maxWait_) &&
     (responsePacket.payloadLength == 74)) {
    uint32_t gnssKey;
    bool gnssEnable;
    uint8_t cfgBlkNum = 0;
    uint8_t cfgSigNum = 0;
    for(uint8_t i = 0; i < 14; i++) {
      gnssKey =  responsePacket.payload[4+(i*5)+0];
      gnssKey |= responsePacket.payload[4+(i*5)+1] << 8;
      gnssKey |= responsePacket.payload[4+(i*5)+2] << 16;
      gnssKey |= responsePacket.payload[4+(i*5)+3] << 24;
      gnssEnable = responsePacket.payload[4+(i*5)+4];
      switch(gnssKey) {
        case UBLOX_CFG_SIGNAL_GPS_ENA:
          ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum].gnssId = 0;
          ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum].gnssIdType = 'G';
          strcpy(ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum].gnssIdName, "GPS");
          ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum].enable = gnssEnable;
          cfgSigNum = 0;
          ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum].numSigs = cfgSigNum;
          cfgBlkNum++;
          ubloxCFGGNSSInfo.M10.numConfigBlocks = cfgBlkNum;
          ubloxCFGGNSSState.GPS = gnssEnable;
          break;
        case UBLOX_CFG_SIGNAL_GPS_L1CA_ENA:
          strncpy(ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum-1].signalList[cfgSigNum].name, "L1CA", 5);
          ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum-1].signalList[cfgSigNum].enable = gnssEnable;
          cfgSigNum++;
          ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum-1].numSigs = cfgSigNum;
          ubloxCFGGNSSState.GPS_L1CA = gnssEnable;
          break;
        case UBLOX_CFG_SIGNAL_SBAS_ENA:
          ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum].gnssId = 1;
          ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum].gnssIdType = 'S';
          strcpy(ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum].gnssIdName, "SBAS");
          ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum].enable = gnssEnable;
          cfgSigNum = 0;
          ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum].numSigs = cfgSigNum;
          cfgBlkNum++;
          ubloxCFGGNSSInfo.M10.numConfigBlocks = cfgBlkNum;
          ubloxCFGGNSSState.SBAS = gnssEnable;
          break;
        case UBLOX_CFG_SIGNAL_SBAS_L1CA_ENA:
          strncpy(ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum-1].signalList[cfgSigNum].name, "L1CA", 5);
          ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum-1].signalList[cfgSigNum].enable = gnssEnable;
          cfgSigNum++;
          ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum-1].numSigs = cfgSigNum;
          ubloxCFGGNSSState.SBAS_L1CA = gnssEnable;
          break;
        case UBLOX_CFG_SIGNAL_GAL_ENA:
          ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum].gnssId = 2;
          ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum].gnssIdType = 'E';
          strcpy(ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum].gnssIdName, "Galileo");
          ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum].enable = gnssEnable;
          cfgSigNum = 0;
          ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum].numSigs = cfgSigNum;
          cfgBlkNum++;
          ubloxCFGGNSSInfo.M10.numConfigBlocks = cfgBlkNum;
          ubloxCFGGNSSState.Galileo = gnssEnable;
          break;
        case UBLOX_CFG_SIGNAL_GAL_E1_ENA:
          strncpy(ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum-1].signalList[cfgSigNum].name, "E1", 3);
          ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum-1].signalList[cfgSigNum].enable = gnssEnable;
          cfgSigNum++;
          ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum-1].numSigs = cfgSigNum;
          ubloxCFGGNSSState.Galileo_E1 = gnssEnable;
          break;
        case UBLOX_CFG_SIGNAL_BDS_ENA:
          ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum].gnssId = 3;
          ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum].gnssIdType = 'B';
          strcpy(ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum].gnssIdName, "BeiDou");
          ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum].enable = gnssEnable;
          cfgSigNum = 0;
          ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum].numSigs = cfgSigNum;
          cfgBlkNum++;
          ubloxCFGGNSSInfo.M10.numConfigBlocks = cfgBlkNum;
          ubloxCFGGNSSState.BeiDou = gnssEnable;
          break;
        case UBLOX_CFG_SIGNAL_BDS_B1_ENA:
          strncpy(ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum-1].signalList[cfgSigNum].name, "B1", 3);
          ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum-1].signalList[cfgSigNum].enable = gnssEnable;
          cfgSigNum++;
          ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum-1].numSigs = cfgSigNum;
          ubloxCFGGNSSState.BeiDou_B1 = gnssEnable;
          break;
        case UBLOX_CFG_SIGNAL_BDS_B1C_ENA:
          strncpy(ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum-1].signalList[cfgSigNum].name, "B1C", 4);
          ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum-1].signalList[cfgSigNum].enable = gnssEnable;
          cfgSigNum++;
          ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum-1].numSigs = cfgSigNum;
          ubloxCFGGNSSState.BeiDou_B1C = gnssEnable;
          break;
        case UBLOX_CFG_SIGNAL_QZSS_ENA:
          ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum].gnssId = 5;
          ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum].gnssIdType = 'Q';
          strcpy(ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum].gnssIdName, "QZSS");
          ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum].enable = gnssEnable;
          cfgSigNum = 0;
          ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum].numSigs = cfgSigNum;
          cfgBlkNum++;
          ubloxCFGGNSSInfo.M10.numConfigBlocks = cfgBlkNum;
          ubloxCFGGNSSState.QZSS = gnssEnable;
          break;
        case UBLOX_CFG_SIGNAL_QZSS_L1CA_ENA:
          strncpy(ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum-1].signalList[cfgSigNum].name, "L1CA", 5);
          ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum-1].signalList[cfgSigNum].enable = gnssEnable;
          cfgSigNum++;
          ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum-1].numSigs = cfgSigNum;
          ubloxCFGGNSSState.QZSS_L1CA = gnssEnable;
          break;
        case UBLOX_CFG_SIGNAL_QZSS_L1S_ENA:
          strncpy(ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum-1].signalList[cfgSigNum].name, "L1S", 4);
          ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum-1].signalList[cfgSigNum].enable = gnssEnable;
          cfgSigNum++;
          ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum-1].numSigs = cfgSigNum;
          ubloxCFGGNSSState.QZSS_L1S = gnssEnable;
          break;
        case UBLOX_CFG_SIGNAL_GLO_ENA:
          ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum].gnssId = 6;
          ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum].gnssIdType = 'R';
          strcpy(ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum].gnssIdName, "GLONASS");
          ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum].enable = gnssEnable;
          cfgSigNum = 0;
          ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum].numSigs = cfgSigNum;
          cfgBlkNum++;
          ubloxCFGGNSSInfo.M10.numConfigBlocks = cfgBlkNum;
          ubloxCFGGNSSState.GLONASS = gnssEnable;
          break;
        case UBLOX_CFG_SIGNAL_GLO_L1_ENA:
          strncpy(ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum-1].signalList[cfgSigNum].name, "L1", 3);
          ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum-1].signalList[cfgSigNum].enable = gnssEnable;
          cfgSigNum++;
          ubloxCFGGNSSInfo.M10.configBlockList[cfgBlkNum-1].numSigs = cfgSigNum;
          ubloxCFGGNSSState.GLONASS_L1 = gnssEnable;
          break;
      }
    }
    return true;
  }
  ubloxCFGGNSSInfo.M10.numConfigBlocks = 0;
  return false;
}

/********************************************************************/
bool TeenyUbloxConnect::setGNSSConfig(uint8_t gnssId, bool enable, uint16_t maxWait_) {
  if(ubloxModuleType == UBLOX_M8_MODULE) {
    return setGNSSConfig_M8(gnssId, enable, maxWait_);
  } else if(ubloxModuleType == UBLOX_M9_MODULE) {
    return setGNSSConfig_M10(gnssId, enable, maxWait_);
  } else if(ubloxModuleType == UBLOX_M10_MODULE) {
    return setGNSSConfig_M10(gnssId, enable, maxWait_);
  }
  return false;
}
/********************************************************************/
bool TeenyUbloxConnect::setGNSSConfig_M8(uint8_t gnssId, bool enable, uint16_t maxWait_) {
  commandPacket.messageClass = UBX_CLASS_CFG;
  commandPacket.messageID = UBX_CFG_GNSS;
  commandPacket.payloadLength = 0;
  commandPacket.validPacket = true;
  if(sendCommandPacket(true, true, maxWait_)) {
    commandPacket = responsePacket;
    uint8_t numConfigBlocks = responsePacket.payload[3];
    bool _blockFound = false;
    for(uint8_t i = 0; i < numConfigBlocks; i++) {
      if(commandPacket.payload[4+(i*8)] == gnssId) {
        _blockFound = true;
        if(enable) {
          commandPacket.payload[4+(i*8) + 4] |= 0x01;
        } else {
          commandPacket.payload[4+(i*8) + 4] &= 0xFE;
        }
        break;
      }
    }
    if(_blockFound) {
      commandPacket.validPacket = true;
      if(sendCommandPacket(false, true, maxWait_)) {
        //Applying the GNSS system configuration takes some time.
        //After issuing UBX-CFG-GNSS, wait first for the acknowledgement
        //from the receiver and then 0.5 seconds before sending the next command.
        delay(500);
        //If Galileo is enabled, UBX-CFG-GNSS must be followed by
        //UBX-CFG-CFG to save current configuration to BBR and then
        //by UBX-CFG-RST with resetMode set to Hardware reset.
        if(saveConfiguration(0x00000010)) {
          hardwareReset();
          delay(100); // recovery time for possible gnss module baud rate change;
          return true;
        }
      }
    }
  }
  return false;
}
/********************************************************************/
bool TeenyUbloxConnect::setGNSSConfig_M10(uint8_t gnssId, bool enable, uint16_t maxWait_) {
  commandPacket.messageClass = UBX_CLASS_CFG;
  commandPacket.messageID = UBX_CFG_VALSET;
  commandPacket.payloadLength = 9;
  commandPacket.payload[0] = 0;
  commandPacket.payload[1] = VALSET_LAYER_ALL;
  commandPacket.payload[2] = 0;
  commandPacket.payload[3] = 0;
  uint32_t gnssKey;
  switch(gnssId) {
    case 0: gnssKey = UBLOX_CFG_SIGNAL_GPS_ENA; break;
    case 1: gnssKey = UBLOX_CFG_SIGNAL_SBAS_ENA; break;
    case 2: gnssKey = UBLOX_CFG_SIGNAL_GAL_ENA; break;
    case 3: gnssKey = UBLOX_CFG_SIGNAL_BDS_ENA; break;
    case 5: gnssKey = UBLOX_CFG_SIGNAL_QZSS_ENA; break;
    case 6: gnssKey = UBLOX_CFG_SIGNAL_GLO_ENA; break;
    default: return false;
  }
  for(uint8_t i = 0; i < 4; i++)
    commandPacket.payload[4 + i] = gnssKey >> (8 * i);
  commandPacket.payload[8] = enable;
  commandPacket.validPacket = true;
  if(sendCommandPacket(false, true, maxWait_)) {
    //Note that changes to any items within CFG-SIGNAL will
    //trigger a reset to the GNSS subsystem. The reset takes
    //some time, so wait ﬁrst for the acknowledgement from the
    //receiver and then 0.5 seconds before sending the next command.
    delay(500);
    return true;
  }
  delay(500);
  return false;
}

/********************************************************************/
bool TeenyUbloxConnect::setGNSSSignalConfig(uint8_t gnssId, const char* signalName, bool enable, uint16_t maxWait_) {
  if(ubloxModuleType == UBLOX_M8_MODULE) {
    return setGNSSSignalConfig_M8(gnssId, signalName, enable, maxWait_);
  } else if(ubloxModuleType == UBLOX_M9_MODULE) {
    return setGNSSSignalConfig_M9(gnssId, signalName, enable, maxWait_);
  } else if(ubloxModuleType == UBLOX_M10_MODULE) {
    return setGNSSSignalConfig_M10(gnssId, signalName, enable, maxWait_);
  }
  return false;
}
/********************************************************************/
bool TeenyUbloxConnect::setGNSSSignalConfig_M8(uint8_t gnssId, const char* signalName, bool enable, uint16_t maxWait_) {
  // Only used for M10 modules
  return false;
}
/********************************************************************/
bool TeenyUbloxConnect::setGNSSSignalConfig_M9(uint8_t gnssId, const char* signalName, bool enable, uint16_t maxWait_) {
  commandPacket.messageClass = UBX_CLASS_CFG;
  commandPacket.messageID = UBX_CFG_VALSET;
  commandPacket.payloadLength = 9;
  commandPacket.payload[0] = 0;
  commandPacket.payload[1] = VALSET_LAYER_ALL;
  commandPacket.payload[2] = 0;
  commandPacket.payload[3] = 0;
  uint32_t gnssKey;
  switch(gnssId) {
    case 0:
      if(strcmp(signalName, "L1CA") == 0) {
        gnssKey =  UBLOX_CFG_SIGNAL_GPS_L1CA_ENA;
        break;
      } else {
        return false;
      }
    case 1:
      if(strcmp(signalName, "L1CA") == 0) {
        gnssKey = UBLOX_CFG_SIGNAL_SBAS_L1CA_ENA;
        break;
      } else {
        return false;
      }
    case 2:
      if(strcmp(signalName, "E1") == 0) {
        gnssKey = UBLOX_CFG_SIGNAL_GAL_E1_ENA;
        break;
      } else {
        return false;
      }
    case 3:
      if(strcmp(signalName, "B1") == 0) {
        gnssKey = UBLOX_CFG_SIGNAL_BDS_B1_ENA;
        break;
      } else {
        return false;
      }
    case 5:
      if(strcmp(signalName, "L1CA") == 0) {
        gnssKey = UBLOX_CFG_SIGNAL_QZSS_L1CA_ENA;
        break;
      } else {
        return false;
      }
    case 6:
      if(strcmp(signalName, "L1") == 0) {
        gnssKey = UBLOX_CFG_SIGNAL_GLO_L1_ENA;
        break;
      } else {
        return false;
      }
    default: return false;
  }
  for(uint8_t i = 0; i < 4; i++)
    commandPacket.payload[4 + i] = gnssKey >> (8 * i);
  commandPacket.payload[8] = enable;
  commandPacket.validPacket = true;
  if(sendCommandPacket(false, true, maxWait_)) {
    //Note that changes to any items within CFG-SIGNAL will
    //trigger a reset to the GNSS subsystem. The reset takes
    //some time, so wait ﬁrst for the acknowledgement from the
    //receiver and then 0.5 seconds before sending the next command.
    delay(500);
    return true;
  }
  delay(500);
  return false;
}
/********************************************************************/
bool TeenyUbloxConnect::setGNSSSignalConfig_M10(uint8_t gnssId, const char* signalName, bool enable, uint16_t maxWait_) {
  commandPacket.messageClass = UBX_CLASS_CFG;
  commandPacket.messageID = UBX_CFG_VALSET;
  commandPacket.payloadLength = 9;
  commandPacket.payload[0] = 0;
  commandPacket.payload[1] = VALSET_LAYER_ALL;
  commandPacket.payload[2] = 0;
  commandPacket.payload[3] = 0;
  uint32_t gnssKey;
  switch(gnssId) {
    case 0:
      if(strcmp(signalName, "L1CA") == 0) {
        gnssKey =  UBLOX_CFG_SIGNAL_GPS_L1CA_ENA;
        break;
      } else {
        return false;
      }
    case 1:
      if(strcmp(signalName, "L1CA") == 0) {
        gnssKey = UBLOX_CFG_SIGNAL_SBAS_L1CA_ENA;
        break;
      } else {
        return false;
      }
    case 2:
      if(strcmp(signalName, "E1") == 0) {
        gnssKey = UBLOX_CFG_SIGNAL_GAL_E1_ENA;
        break;
      } else {
        return false;
      }
    case 3:
      if(strcmp(signalName, "B1") == 0) {
        gnssKey = UBLOX_CFG_SIGNAL_BDS_B1_ENA;
        break;
      } else if(strcmp(signalName, "B1C") == 0) {
        gnssKey = UBLOX_CFG_SIGNAL_BDS_B1C_ENA;
        break;
      } else {
        return false;
      }
    case 5:
      if(strcmp(signalName, "L1CA") == 0) {
        gnssKey = UBLOX_CFG_SIGNAL_QZSS_L1CA_ENA;
        break;
      } else if(strcmp(signalName, "L1S") == 0) {
        gnssKey = UBLOX_CFG_SIGNAL_QZSS_L1S_ENA;
        break;
      } else {
        return false;
      }
    case 6:
      if(strcmp(signalName, "L1") == 0) {
        gnssKey = UBLOX_CFG_SIGNAL_GLO_L1_ENA;
        break;
      } else {
        return false;
      }
    default: return false;
  }
  for(uint8_t i = 0; i < 4; i++)
    commandPacket.payload[4 + i] = gnssKey >> (8 * i);
  commandPacket.payload[8] = enable;
  commandPacket.validPacket = true;
  if(sendCommandPacket(false, true, maxWait_)) {
    //Note that changes to any items within CFG-SIGNAL will
    //trigger a reset to the GNSS subsystem. The reset takes
    //some time, so wait ﬁrst for the acknowledgement from the
    //receiver and then 0.5 seconds before sending the next command.
    delay(500);
    return true;
  }
  delay(500);
  return false;
}

/********************************************************************/
bool TeenyUbloxConnect::setGNSSConfigState(ubloxCFGGNSSState_t gnssConfigState, uint16_t maxWait_) {
  if(ubloxModuleType == UBLOX_M8_MODULE) {
    return setGNSSConfigState_M8(gnssConfigState, maxWait_);
  } else if(ubloxModuleType == UBLOX_M9_MODULE) {
    return setGNSSConfigState_M9(gnssConfigState, maxWait_);
  } else if(ubloxModuleType == UBLOX_M10_MODULE) {
    return setGNSSConfigState_M10(gnssConfigState, maxWait_);
  }
  return false;
}
/********************************************************************/
bool TeenyUbloxConnect::setGNSSConfigState_M8(ubloxCFGGNSSState_t gnssConfigState, uint16_t maxWait_) {
  commandPacket.messageClass = UBX_CLASS_CFG;
  commandPacket.messageID = UBX_CFG_GNSS;
  commandPacket.payloadLength = 0;
  commandPacket.validPacket = true;
  if(sendCommandPacket(true, true, maxWait_)) {
    commandPacket = responsePacket;
    uint8_t numConfigBlocks = responsePacket.payload[3];
    int8_t _enable = false;
    for(uint8_t i = 0; i < numConfigBlocks; i++) {
      switch(commandPacket.payload[4+(i*8)]) {
        case 0:
          _enable = gnssConfigState.GPS == 1;
          break;
        case 1:
          _enable = gnssConfigState.SBAS == 1;
          break;
        case 2:
          _enable = gnssConfigState.Galileo == 1;
          break;
        case 3:
          _enable = gnssConfigState.BeiDou == 1;
          break;
        case 4:
          _enable = gnssConfigState.IMES == 1;
          break;
        case 5:
          _enable = gnssConfigState.QZSS == 1;
          break;
        case 6:
          _enable = gnssConfigState.GLONASS == 1;
          break;
        default:
          _enable = -1;
          break;
      }
      if(_enable > 0) {
        commandPacket.payload[4+(i*8) + 4] |= 0x01;
      } else if(_enable == 0) {
        commandPacket.payload[4+(i*8) + 4] &= 0xFE;
      }
    }
    commandPacket.validPacket = true;
    if(sendCommandPacket(false, true, maxWait_)) {
      //Applying the GNSS system configuration takes some time.
      //After issuing UBX-CFG-GNSS, wait first for the acknowledgement
      //from the receiver and then 0.5 seconds before sending the next command.
      delay(500);
      //If Galileo is enabled, UBX-CFG-GNSS must be followed by
      //UBX-CFG-CFG to save current configuration to BBR and then
      //by UBX-CFG-RST with resetMode set to Hardware reset.
      if(saveConfiguration(0x00000010)) {
        hardwareReset();
        delay(100); // recovery time for possible gnss module baud rate change;
        return true;
      }
    }
  }
  return false;
}
/********************************************************************/
bool TeenyUbloxConnect::setGNSSConfigState_M9(ubloxCFGGNSSState_t gnssConfigState, uint16_t maxWait_) {
  commandPacket.messageClass = UBX_CLASS_CFG;
  commandPacket.messageID = UBX_CFG_VALSET;
  commandPacket.payloadLength = 64;
  commandPacket.payload[0] = 0;
  commandPacket.payload[1] = VALSET_LAYER_ALL;
  commandPacket.payload[2] = 0;
  commandPacket.payload[3] = 0;
  uint32_t gnssKey;
  bool enable;
  uint8_t kvIndex = 4;
  gnssKey = UBLOX_CFG_SIGNAL_GPS_ENA;
  enable = gnssConfigState.GPS == 1;
  for(uint8_t i = 0; i < 4; i++) commandPacket.payload[kvIndex + i] = gnssKey >> (8 * i);
  commandPacket.payload[kvIndex + 4] = enable;
  kvIndex += 5;
  gnssKey = UBLOX_CFG_SIGNAL_GPS_L1CA_ENA;
  enable = gnssConfigState.GPS_L1CA == 1;
  for(uint8_t i = 0; i < 4; i++) commandPacket.payload[kvIndex + i] = gnssKey >> (8 * i);
  commandPacket.payload[kvIndex + 4] = enable;
  kvIndex += 5;
  gnssKey = UBLOX_CFG_SIGNAL_SBAS_ENA;
  enable = gnssConfigState.SBAS == 1;
  for(uint8_t i = 0; i < 4; i++) commandPacket.payload[kvIndex + i] = gnssKey >> (8 * i);
  commandPacket.payload[kvIndex + 4] = enable;
  kvIndex += 5;
  gnssKey = UBLOX_CFG_SIGNAL_SBAS_L1CA_ENA;
  enable = gnssConfigState.SBAS_L1CA == 1;
  for(uint8_t i = 0; i < 4; i++) commandPacket.payload[kvIndex + i] = gnssKey >> (8 * i);
  commandPacket.payload[kvIndex + 4] = enable;
  kvIndex += 5;
  gnssKey = UBLOX_CFG_SIGNAL_GAL_ENA;
  enable = gnssConfigState.Galileo == 1;
  for(uint8_t i = 0; i < 4; i++) commandPacket.payload[kvIndex + i] = gnssKey >> (8 * i);
  commandPacket.payload[kvIndex + 4] = enable;
  kvIndex += 5;
  gnssKey = UBLOX_CFG_SIGNAL_GAL_E1_ENA;
  enable = gnssConfigState.Galileo_E1 == 1;
  for(uint8_t i = 0; i < 4; i++) commandPacket.payload[kvIndex + i] = gnssKey >> (8 * i);
  commandPacket.payload[kvIndex + 4] = enable;
  kvIndex += 5;
  gnssKey = UBLOX_CFG_SIGNAL_BDS_ENA;
  enable = gnssConfigState.BeiDou == 1;
  for(uint8_t i = 0; i < 4; i++) commandPacket.payload[kvIndex + i] = gnssKey >> (8 * i);
  commandPacket.payload[kvIndex + 4] = enable;
  kvIndex += 5;
  gnssKey = UBLOX_CFG_SIGNAL_BDS_B1_ENA;
  enable = gnssConfigState.BeiDou_B1 == 1;
  for(uint8_t i = 0; i < 4; i++) commandPacket.payload[kvIndex + i] = gnssKey >> (8 * i);
  commandPacket.payload[kvIndex + 4] = enable;
  kvIndex += 5;
  gnssKey = UBLOX_CFG_SIGNAL_QZSS_ENA;
  enable = gnssConfigState.QZSS == 1;
  for(uint8_t i = 0; i < 4; i++) commandPacket.payload[kvIndex + i] = gnssKey >> (8 * i);
  commandPacket.payload[kvIndex + 4] = enable;
  kvIndex += 5;
  gnssKey = UBLOX_CFG_SIGNAL_QZSS_L1CA_ENA;
  enable = gnssConfigState.QZSS_L1CA == 1;
  for(uint8_t i = 0; i < 4; i++) commandPacket.payload[kvIndex + i] = gnssKey >> (8 * i);
  commandPacket.payload[kvIndex + 4] = enable;
  kvIndex += 5;
  gnssKey = UBLOX_CFG_SIGNAL_GLO_ENA;
  enable = gnssConfigState.GLONASS == 1;
  for(uint8_t i = 0; i < 4; i++) commandPacket.payload[kvIndex + i] = gnssKey >> (8 * i);
  commandPacket.payload[kvIndex + 4] = enable;
  kvIndex += 5;
  gnssKey = UBLOX_CFG_SIGNAL_GLO_L1_ENA;
  enable = gnssConfigState.GLONASS_L1 == 1;
  for(uint8_t i = 0; i < 4; i++) commandPacket.payload[kvIndex + i] = gnssKey >> (8 * i);
  commandPacket.payload[kvIndex + 4] = enable;
  commandPacket.validPacket = true;
  if(sendCommandPacket(false, true, maxWait_)) {
    //Note that changes to any items within CFG-SIGNAL will
    //trigger a reset to the GNSS subsystem. The reset takes
    //some time, so wait ﬁrst for the acknowledgement from the
    //receiver and then 0.5 seconds before sending the next command.
    delay(500);
    return true;
  }
  delay(500);
  return false;
}
/********************************************************************/
bool TeenyUbloxConnect::setGNSSConfigState_M10(ubloxCFGGNSSState_t gnssConfigState, uint16_t maxWait_) {
  commandPacket.messageClass = UBX_CLASS_CFG;
  commandPacket.messageID = UBX_CFG_VALSET;
  commandPacket.payloadLength = 74;
  commandPacket.payload[0] = 0;
  commandPacket.payload[1] = VALSET_LAYER_ALL;
  commandPacket.payload[2] = 0;
  commandPacket.payload[3] = 0;
  uint32_t gnssKey;
  bool enable;
  uint8_t kvIndex = 4;
  gnssKey = UBLOX_CFG_SIGNAL_GPS_ENA;
  enable = gnssConfigState.GPS == 1;
  for(uint8_t i = 0; i < 4; i++) commandPacket.payload[kvIndex + i] = gnssKey >> (8 * i);
  commandPacket.payload[kvIndex + 4] = enable;
  kvIndex += 5;
  gnssKey = UBLOX_CFG_SIGNAL_GPS_L1CA_ENA;
  enable = gnssConfigState.GPS_L1CA == 1;
  for(uint8_t i = 0; i < 4; i++) commandPacket.payload[kvIndex + i] = gnssKey >> (8 * i);
  commandPacket.payload[kvIndex + 4] = enable;
  kvIndex += 5;
  gnssKey = UBLOX_CFG_SIGNAL_SBAS_ENA;
  enable = gnssConfigState.SBAS == 1;
  for(uint8_t i = 0; i < 4; i++) commandPacket.payload[kvIndex + i] = gnssKey >> (8 * i);
  commandPacket.payload[kvIndex + 4] = enable;
  kvIndex += 5;
  gnssKey = UBLOX_CFG_SIGNAL_SBAS_L1CA_ENA;
  enable = gnssConfigState.SBAS_L1CA == 1;
  for(uint8_t i = 0; i < 4; i++) commandPacket.payload[kvIndex + i] = gnssKey >> (8 * i);
  commandPacket.payload[kvIndex + 4] = enable;
  kvIndex += 5;
  gnssKey = UBLOX_CFG_SIGNAL_GAL_ENA;
  enable = gnssConfigState.Galileo == 1;
  for(uint8_t i = 0; i < 4; i++) commandPacket.payload[kvIndex + i] = gnssKey >> (8 * i);
  commandPacket.payload[kvIndex + 4] = enable;
  kvIndex += 5;
  gnssKey = UBLOX_CFG_SIGNAL_GAL_E1_ENA;
  enable = gnssConfigState.Galileo_E1 == 1;
  for(uint8_t i = 0; i < 4; i++) commandPacket.payload[kvIndex + i] = gnssKey >> (8 * i);
  commandPacket.payload[kvIndex + 4] = enable;
  kvIndex += 5;
  gnssKey = UBLOX_CFG_SIGNAL_BDS_ENA;
  enable = gnssConfigState.BeiDou == 1;
  for(uint8_t i = 0; i < 4; i++) commandPacket.payload[kvIndex + i] = gnssKey >> (8 * i);
  commandPacket.payload[kvIndex + 4] = enable;
  kvIndex += 5;
  gnssKey = UBLOX_CFG_SIGNAL_BDS_B1_ENA;
  enable = gnssConfigState.BeiDou_B1 == 1;
  for(uint8_t i = 0; i < 4; i++) commandPacket.payload[kvIndex + i] = gnssKey >> (8 * i);
  commandPacket.payload[kvIndex + 4] = enable;
  kvIndex += 5;
  gnssKey = UBLOX_CFG_SIGNAL_BDS_B1C_ENA;
  enable = gnssConfigState.BeiDou_B1C == 1;
  for(uint8_t i = 0; i < 4; i++) commandPacket.payload[kvIndex + i] = gnssKey >> (8 * i);
  commandPacket.payload[kvIndex + 4] = enable;
  kvIndex += 5;
  gnssKey = UBLOX_CFG_SIGNAL_QZSS_ENA;
  enable = gnssConfigState.QZSS == 1;
  for(uint8_t i = 0; i < 4; i++) commandPacket.payload[kvIndex + i] = gnssKey >> (8 * i);
  commandPacket.payload[kvIndex + 4] = enable;
  kvIndex += 5;
  gnssKey = UBLOX_CFG_SIGNAL_QZSS_L1CA_ENA;
  enable = gnssConfigState.QZSS_L1CA == 1;
  for(uint8_t i = 0; i < 4; i++) commandPacket.payload[kvIndex + i] = gnssKey >> (8 * i);
  commandPacket.payload[kvIndex + 4] = enable;
  kvIndex += 5;
  gnssKey = UBLOX_CFG_SIGNAL_QZSS_L1S_ENA;
  enable = gnssConfigState.QZSS_L1S == 1;
  for(uint8_t i = 0; i < 4; i++) commandPacket.payload[kvIndex + i] = gnssKey >> (8 * i);
  commandPacket.payload[kvIndex + 4] = enable;
  kvIndex += 5;
  gnssKey = UBLOX_CFG_SIGNAL_GLO_ENA;
  enable = gnssConfigState.GLONASS == 1;
  for(uint8_t i = 0; i < 4; i++) commandPacket.payload[kvIndex + i] = gnssKey >> (8 * i);
  commandPacket.payload[kvIndex + 4] = enable;
  kvIndex += 5;
  gnssKey = UBLOX_CFG_SIGNAL_GLO_L1_ENA;
  enable = gnssConfigState.GLONASS_L1 == 1;
  for(uint8_t i = 0; i < 4; i++) commandPacket.payload[kvIndex + i] = gnssKey >> (8 * i);
  commandPacket.payload[kvIndex + 4] = enable;
  commandPacket.validPacket = true;
  if(sendCommandPacket(false, true, maxWait_)) {
    //Note that changes to any items within CFG-SIGNAL will
    //trigger a reset to the GNSS subsystem. The reset takes
    //some time, so wait ﬁrst for the acknowledgement from the
    //receiver and then 0.5 seconds before sending the next command.
    delay(500);
    return true;
  }
  delay(500);
  return false;
}

/********************************************************************/
bool TeenyUbloxConnect::setMeasurementRate(uint16_t rate_, uint16_t maxWait_) {
  if(ubloxModuleType == UBLOX_M8_MODULE) {
    return setMeasurementRate_M8(rate_, maxWait_);
  } else if(ubloxModuleType == UBLOX_M9_MODULE) {
    return setMeasurementRate_M10(rate_, maxWait_);
  } else if(ubloxModuleType == UBLOX_M10_MODULE) {
    return setMeasurementRate_M10(rate_, maxWait_);
  }
  return false;
}
/********************************************************************/
bool TeenyUbloxConnect::setMeasurementRate_M8(uint16_t rate_, uint16_t maxWait_) {
  commandPacket.messageClass = UBX_CLASS_CFG;
  commandPacket.messageID = UBX_CFG_RATE;
  commandPacket.payloadLength = 0;
  commandPacket.validPacket = true;
  if(sendCommandPacket(true, true, maxWait_)) {
    commandPacket = responsePacket;
    commandPacket.payload[0] = rate_;
    commandPacket.payload[1] = rate_ >> 8;
    commandPacket.validPacket = true;
    return sendCommandPacket(false, true, maxWait_);
  }
  return false;
}
/********************************************************************/
bool TeenyUbloxConnect::setMeasurementRate_M10(uint16_t rate_, uint16_t maxWait_) {
  commandPacket.messageClass = UBX_CLASS_CFG;
  commandPacket.messageID = UBX_CFG_VALSET;
  commandPacket.payloadLength = 10;
  commandPacket.payload[0] = 0;
  commandPacket.payload[1] = VALSET_LAYER_RAM;
  commandPacket.payload[2] = 0;
  commandPacket.payload[3] = 0;
  for(uint8_t i = 0; i < 4; i++)
    commandPacket.payload[4 + i] = UBLOX_CFG_RATE_MEAS >> (8 * i);
  commandPacket.payload[8] = rate_;
  commandPacket.payload[9] = rate_ >> 8;
  commandPacket.validPacket = true;
  return sendCommandPacket(false, true, maxWait_);
}

/********************************************************************/
bool TeenyUbloxConnect::setNavigationRate(uint16_t rate_, uint16_t maxWait_) {
  if(ubloxModuleType == UBLOX_M8_MODULE) {
    return setNavigationRate_M8(rate_, maxWait_);
  } else if(ubloxModuleType == UBLOX_M9_MODULE) {
    return setNavigationRate_M10(rate_, maxWait_);
  } else if(ubloxModuleType == UBLOX_M10_MODULE) {
    return setNavigationRate_M10(rate_, maxWait_);
  }
  return false;
}
/********************************************************************/
bool TeenyUbloxConnect::setNavigationRate_M8(uint16_t rate_, uint16_t maxWait_) {
  commandPacket.messageClass = UBX_CLASS_CFG;
  commandPacket.messageID = UBX_CFG_RATE;
  commandPacket.payloadLength = 0;
  commandPacket.validPacket = true;
  if(sendCommandPacket(true, true, maxWait_)) {
    commandPacket = responsePacket;
    commandPacket.payload[2] = rate_;
    commandPacket.payload[3] = rate_ >> 8;
    commandPacket.validPacket = true;
    return sendCommandPacket(false, true, maxWait_);
  }
  return false;
}
/********************************************************************/
bool TeenyUbloxConnect::setNavigationRate_M10(uint16_t rate_, uint16_t maxWait_) {
  commandPacket.messageClass = UBX_CLASS_CFG;
  commandPacket.messageID = UBX_CFG_VALSET;
  commandPacket.payloadLength = 10;
  commandPacket.payload[0] = 0;
  commandPacket.payload[1] = VALSET_LAYER_RAM;
  commandPacket.payload[2] = 0;
  commandPacket.payload[3] = 0;
  for(uint8_t i = 0; i < 4; i++)
    commandPacket.payload[4 + i] = UBLOX_CFG_RATE_NAV >> (8 * i);
  commandPacket.payload[8] = rate_;
  commandPacket.payload[9] = rate_ >> 8;
  commandPacket.validPacket = true;
  return sendCommandPacket(false, true, maxWait_);
}

/********************************************************************/
bool TeenyUbloxConnect::setAutoNAVPVT(bool enable_, uint16_t maxWait_) {
  return setAutoNAVPVTRate(enable_ ? 1 : 0, maxWait_);
}

/********************************************************************/
bool TeenyUbloxConnect::setAutoNAVPVTRate(uint8_t rate_, uint16_t maxWait_) {
  if(ubloxModuleType == UBLOX_M8_MODULE) {
    return setAutoNAVPVTRate_M8(rate_, maxWait_);
  } else if(ubloxModuleType == UBLOX_M9_MODULE) {
    return setAutoNAVPVTRate_M10(rate_, maxWait_);
  } else if(ubloxModuleType == UBLOX_M10_MODULE) {
    return setAutoNAVPVTRate_M10(rate_, maxWait_);
  }
  return false;
}
/********************************************************************/
bool TeenyUbloxConnect::setAutoNAVPVTRate_M8(uint8_t rate_, uint16_t maxWait_) {
  commandPacket.messageClass = UBX_CLASS_CFG;
  commandPacket.messageID = UBX_CFG_MSG;
  commandPacket.payloadLength = 3;
  commandPacket.payload[0] = UBX_CLASS_NAV;
  commandPacket.payload[1] = UBX_NAV_PVT;
  commandPacket.payload[2] = (rate_ <= 127) ? rate_ : 127;
  commandPacket.validPacket = true;
  return sendCommandPacket(false, true, maxWait_);
}
/********************************************************************/
bool TeenyUbloxConnect::setAutoNAVPVTRate_M10(uint8_t rate_, uint16_t maxWait_) {
  commandPacket.messageClass = UBX_CLASS_CFG;
  commandPacket.messageID = UBX_CFG_VALSET;
  commandPacket.payloadLength = 9;
  commandPacket.payload[0] = 0;
  commandPacket.payload[1] = VALSET_LAYER_RAM;
  commandPacket.payload[2] = 0;
  commandPacket.payload[3] = 0;
  for(uint8_t i = 0; i < 4; i++)
    commandPacket.payload[4 + i] = UBLOX_CFG_MSGOUT_UBX_NAV_PVT_UART1 >> (8 * i);
  commandPacket.payload[8] = (rate_ <= 127) ? rate_ : 127;
  commandPacket.validPacket = true;
  return sendCommandPacket(false, true, maxWait_);
}

/********************************************************************/
bool TeenyUbloxConnect::setAutoPVT(bool enable_, uint16_t maxWait_) {
  return setAutoNAVPVTRate(enable_ ? 1 : 0, maxWait_);
}

/********************************************************************/
bool TeenyUbloxConnect::setAutoPVTRate(uint8_t rate_, uint16_t maxWait_) {
  return setAutoNAVPVTRate(rate_, maxWait_);
}

/********************************************************************/
bool TeenyUbloxConnect::setAutoNAVSTATUS(bool enable_, uint16_t maxWait_) {
  return setAutoNAVSTATUSRate(enable_ ? 1 : 0, maxWait_);
}

/********************************************************************/
bool TeenyUbloxConnect::setAutoNAVSTATUSRate(uint8_t rate_, uint16_t maxWait_) {
  if(ubloxModuleType == UBLOX_M8_MODULE) {
    return setAutoNAVSTATUSRate_M8(rate_, maxWait_);
  } else if(ubloxModuleType == UBLOX_M9_MODULE) {
    return setAutoNAVSTATUSRate_M10(rate_, maxWait_);
  } else if(ubloxModuleType == UBLOX_M10_MODULE) {
    return setAutoNAVSTATUSRate_M10(rate_, maxWait_);
  }
  return false;
}
/********************************************************************/
bool TeenyUbloxConnect::setAutoNAVSTATUSRate_M8(uint8_t rate_, uint16_t maxWait_) {
  commandPacket.messageClass = UBX_CLASS_CFG;
  commandPacket.messageID = UBX_CFG_MSG;
  commandPacket.payloadLength = 3;
  commandPacket.payload[0] = UBX_CLASS_NAV;
  commandPacket.payload[1] = UBX_NAV_STATUS;
  commandPacket.payload[2] = (rate_ <= 127) ? rate_ : 127;
  commandPacket.validPacket = true;
  return sendCommandPacket(false, true, maxWait_);
}
/********************************************************************/
bool TeenyUbloxConnect::setAutoNAVSTATUSRate_M10(uint8_t rate_, uint16_t maxWait_) {
  commandPacket.messageClass = UBX_CLASS_CFG;
  commandPacket.messageID = UBX_CFG_VALSET;
  commandPacket.payloadLength = 9;
  commandPacket.payload[0] = 0;
  commandPacket.payload[1] = VALSET_LAYER_RAM;
  commandPacket.payload[2] = 0;
  commandPacket.payload[3] = 0;
  for(uint8_t i = 0; i < 4; i++)
    commandPacket.payload[4 + i] = UBLOX_CFG_MSGOUT_UBX_NAV_STATUS_UART1 >> (8 * i);
  commandPacket.payload[8] = (rate_ <= 127) ? rate_ : 127;
  commandPacket.validPacket = true;
  return sendCommandPacket(false, true, maxWait_);
}

/********************************************************************/
bool TeenyUbloxConnect::setAutoNAVSAT(bool enable_, uint16_t maxWait_) {
  return setAutoNAVSATRate(enable_ ? 1 : 0, maxWait_);
}

/********************************************************************/
bool TeenyUbloxConnect::setAutoNAVSATRate(uint8_t rate_, uint16_t maxWait_) {
  if(ubloxModuleType == UBLOX_M8_MODULE) {
    return setAutoNAVSATRate_M8(rate_, maxWait_);
  } else if(ubloxModuleType == UBLOX_M9_MODULE) {
    return setAutoNAVSATRate_M10(rate_, maxWait_);
  } else if(ubloxModuleType == UBLOX_M10_MODULE) {
    return setAutoNAVSATRate_M10(rate_, maxWait_);
  }
  return false;
}
/********************************************************************/
bool TeenyUbloxConnect::setAutoNAVSATRate_M8(uint8_t rate_, uint16_t maxWait_) {
  commandPacket.messageClass = UBX_CLASS_CFG;
  commandPacket.messageID = UBX_CFG_MSG;
  commandPacket.payloadLength = 3;
  commandPacket.payload[0] = UBX_CLASS_NAV;
  commandPacket.payload[1] = UBX_NAV_SAT;
  commandPacket.payload[2] = (rate_ <= 127) ? rate_ : 127;
  commandPacket.validPacket = true;
  return sendCommandPacket(false, true, maxWait_);
}
/********************************************************************/
bool TeenyUbloxConnect::setAutoNAVSATRate_M10(uint8_t rate_, uint16_t maxWait_) {
  commandPacket.messageClass = UBX_CLASS_CFG;
  commandPacket.messageID = UBX_CFG_VALSET;
  commandPacket.payloadLength = 9;
  commandPacket.payload[0] = 0;
  commandPacket.payload[1] = VALSET_LAYER_RAM;
  commandPacket.payload[2] = 0;
  commandPacket.payload[3] = 0;
  for(uint8_t i = 0; i < 4; i++)
    commandPacket.payload[4 + i] = UBLOX_CFG_MSGOUT_UBX_NAV_SAT_UART1 >> (8 * i);
  commandPacket.payload[8] = (rate_ <= 127) ? rate_ : 127;
  commandPacket.validPacket = true;
  return sendCommandPacket(false, true, maxWait_);
}

/********************************************************************/
bool TeenyUbloxConnect::pollNAVPVT(uint16_t maxWait_) {
  commandPacket.messageClass = UBX_CLASS_NAV;
  commandPacket.messageID = UBX_NAV_PVT;
  commandPacket.payloadLength = 0;
  commandPacket.validPacket = true;
  if(sendCommandPacket(true, false, maxWait_)) {
    if(ubloxNAVPVTPacketBuffer.validPacket) {
      // Lost rx packet
      lostNAVPVTPacketCount += (lostNAVPVTPacketCount < 99) ? 1 : 0;
      processNAVPVTPacket(); // processing clears validPacket
      return false;
    }
    ubloxNAVPVTPacketBuffer = responsePacket;
    ubloxNAVPVTPacketBuffer.validPacket = true;
    return processNAVPVTPacket();
  }
  return false;
}

/********************************************************************/
bool TeenyUbloxConnect::pollNAVSTATUS(uint16_t maxWait_) {
  commandPacket.messageClass = UBX_CLASS_NAV;
  commandPacket.messageID = UBX_NAV_STATUS;
  commandPacket.payloadLength = 0;
  commandPacket.validPacket = true;
  if(sendCommandPacket(true, false, maxWait_)) {
    if(ubloxNAVSTATUSPacketBuffer.validPacket) {
      // Lost rx packet
      lostNAVSTATUSPacketCount += (lostNAVSTATUSPacketCount < 99) ? 1 : 0;
      processNAVSTATUSPacket(); // processing clears validPacket
      return false;
    }
    ubloxNAVSTATUSPacketBuffer = responsePacket;
    ubloxNAVSTATUSPacketBuffer.validPacket = true;
    return processNAVSTATUSPacket();
  }
  return false;
}

/********************************************************************/
bool TeenyUbloxConnect::pollNAVSAT(uint16_t maxWait_) {
  commandPacket.messageClass = UBX_CLASS_NAV;
  commandPacket.messageID = UBX_NAV_SAT;
  commandPacket.payloadLength = 0;
  commandPacket.validPacket = true;
  if(sendCommandPacket(true, false, maxWait_)) {
    if(ubloxNAVSATPacketBuffer.validPacket) {
      // Lost rx packet
      lostNAVSATPacketCount += (lostNAVSATPacketCount < 99) ? 1 : 0;
      processNAVSATPacket(); // processing clears validPacket
      return false;
    }
    ubloxNAVSATPacketBuffer = responsePacket;
    ubloxNAVSATPacketBuffer.validPacket = true;
    return processNAVSATPacket();
  }
  return false;
}

/********************************************************************/
/********************************************************************/
// send command packet
/********************************************************************/
/********************************************************************/
bool TeenyUbloxConnect::sendCommandPacket(bool expectResp_, bool expectAck_, uint16_t maxWait_) {
  if(!commandPacket.validPacket) return false;
  calcCommandPacketChecksum();
  serialPort->write(commandPacket.synch1);
  serialPort->write(commandPacket.synch2);
  serialPort->write(commandPacket.messageClass);
  serialPort->write(commandPacket.messageID);
  serialPort->write(commandPacket.payloadLength & 0xFF);
  serialPort->write(commandPacket.payloadLength >> 8);
  //for(uint16_t i = 0; i < commandPacket.payloadLength; i++) {
  //  serialPort->write(commandPacket.payload[i]);
  //}
  serialPort->write(commandPacket.payload, commandPacket.payloadLength);
  serialPort->write(commandPacket.checksumA);
  serialPort->write(commandPacket.checksumB);
  commandPacket.validPacket = false;
  if(!(expectResp_ || expectAck_)) {
    return true;
  }
  processingUbloxCommand = true;
  responsePacket.validPacket = false;
  acknowledgePacket.validPacket = false;
  uint32_t startTime = millis();
  while((millis() - startTime) < maxWait_) {
    checkUbloxInternal();
    processIncomingPacket(commandPacket.messageClass, commandPacket.messageID);
    if(expectResp_ && expectAck_ && responsePacket.validPacket && acknowledgePacket.validPacket) {
      processingUbloxCommand = false;
      return acknowledgePacket.messageID;
    } else if(expectResp_ && (!expectAck_) && responsePacket.validPacket) {
      processingUbloxCommand = false;
      return true;
    } else if((!expectResp_) && expectAck_ && acknowledgePacket.validPacket) {
      processingUbloxCommand = false;
      return acknowledgePacket.messageID;
    }
  }
  processingUbloxCommand = false;
  return false;
}

/********************************************************************/
/********************************************************************/
// checksum
/********************************************************************/
/********************************************************************/
// Given a message, calc and store the two byte "8-Bit Fletcher"
// checksum over the entirety of the message
// This is called before we send a command packet
void TeenyUbloxConnect::calcCommandPacketChecksum() {
  commandPacket.checksumA = 0;
  commandPacket.checksumB = 0;

  commandPacket.checksumA += commandPacket.messageClass;
  commandPacket.checksumB += commandPacket.checksumA;

  commandPacket.checksumA += commandPacket.messageID;
  commandPacket.checksumB += commandPacket.checksumA;

  commandPacket.checksumA += (commandPacket.payloadLength & 0xFF);
  commandPacket.checksumB += commandPacket.checksumA;

  commandPacket.checksumA += (commandPacket.payloadLength >> 8);
  commandPacket.checksumB += commandPacket.checksumA;

  for(uint16_t i = 0; i < commandPacket.payloadLength; i++) {
    commandPacket.checksumA += commandPacket.payload[i];
    commandPacket.checksumB += commandPacket.checksumA;
  }
}

/********************************************************************/
/********************************************************************/
// Methods for receiving ublox packets
/********************************************************************/
/********************************************************************/
void TeenyUbloxConnect::checkUblox() {
  if(!processingUbloxCommand) {
    checkUbloxInternal();
  }
}

/********************************************************************/
void TeenyUbloxConnect::checkUbloxInternal() {
  while(serialPort->available()) {
    if(incomingPacket.validPacket) return;
    processIncomingByte(serialPort->read());
  }
}

/********************************************************************/
void TeenyUbloxConnect::processIncomingByte(uint8_t incomingByte_) {

  // synch1
  if(!incomingPacket.receivingPacket) {
    if(incomingByte_ == incomingPacket.synch1) {
      // This is the start of a binary sentence. Reset flags.
      // Reset the packet byte counter
      incomingPacket.receivingPacket = true;
      incomingPacket.packetCounter = 1;
    } else {
      // This character is unknown or we missed the previous start of a sentence
    }

  // synch2
  } else if(incomingPacket.receivingPacket &&
            (incomingPacket.packetCounter == 1)) {
    if(incomingByte_ == incomingPacket.synch2) {
      incomingPacket.packetCounter++;
      incomingPacket.rollingChecksumA = 0;
      incomingPacket.rollingChecksumB = 0;
    } else {
      // This character is unknown or we missed the previous start of a sentence
      incomingPacket.receivingPacket = false;
    }

  // messageClass, messageID, payloadLength
  } else if(incomingPacket.receivingPacket &&
            (incomingPacket.packetCounter < 6)) {
    if(incomingPacket.packetCounter == 2) {
      incomingPacket.messageClass = incomingByte_;
    } else if(incomingPacket.packetCounter == 3) {
      incomingPacket.messageID = incomingByte_;
    } else if(incomingPacket.packetCounter == 4) {
      incomingPacket.payloadLength = incomingByte_;
    } else if(incomingPacket.packetCounter == 5) {
      incomingPacket.payloadLength |= (incomingByte_ << 8);
      incomingPacket.payloadCounter = 0;
      memset(incomingPacket.payload, 0, UBX_MAXPAYLOADLENGTH);
    }
    incomingPacket.packetCounter++;
    incomingPacket.rollingChecksumA += incomingByte_;
    incomingPacket.rollingChecksumB += incomingPacket.rollingChecksumA;

  // payload
  } else if(incomingPacket.receivingPacket &&
            (incomingPacket.payloadCounter < incomingPacket.payloadLength)) {
    if(incomingPacket.payloadCounter < UBX_MAXPAYLOADLENGTH) {
      // Not storing packet data in excess of UBX_MAXPAYLOADLENGTH - Just deal with it!
      incomingPacket.payload[incomingPacket.payloadCounter] = incomingByte_;
    }
    incomingPacket.payloadCounter++;
    incomingPacket.packetCounter++;
    incomingPacket.rollingChecksumA += incomingByte_;
    incomingPacket.rollingChecksumB += incomingPacket.rollingChecksumA;

  // checksumA
  } else if(incomingPacket.receivingPacket &&
            (incomingPacket.packetCounter == (incomingPacket.payloadLength + 6))) {
    incomingPacket.checksumA = incomingByte_;
    incomingPacket.packetCounter++;

  // checksumB
  } else if(incomingPacket.receivingPacket &&
            (incomingPacket.packetCounter == (incomingPacket.payloadLength + 7))) {
    incomingPacket.checksumB = incomingByte_;
    incomingPacket.packetCounter++;
    if((incomingPacket.checksumA == incomingPacket.rollingChecksumA) &&
       (incomingPacket.checksumB == incomingPacket.rollingChecksumB)) {
      incomingPacket.validPacket = true;
    }
    incomingPacket.receivingPacket = false;
  }
}

/********************************************************************/
void TeenyUbloxConnect::processIncomingPacket(uint8_t requestedClass_, uint8_t requestedID_) {
  if(incomingPacket.validPacket) {
    if(receivedPacket.validPacket) {
      // Lost rx packet
      lostRxPacketCount += (lostRxPacketCount < 99) ? 1 : 0;
    } else {
      receivedPacket = incomingPacket;
    }
    incomingPacket.validPacket = false;
  }

  if(receivedPacket.validPacket) {

    // Command response and/or acknowledge
    if(requestedClass_) {
      if((receivedPacket.messageClass == requestedClass_) &&
         (receivedPacket.messageID == requestedID_)) {
        responsePacket = receivedPacket;
        receivedPacket.validPacket = false;
        return;
      }
      if((receivedPacket.messageClass == UBX_CLASS_ACK) &&
         ((receivedPacket.messageID == UBX_ACK_ACK) ||
          (receivedPacket.messageID == UBX_ACK_NAK)) &&
         (receivedPacket.payloadLength == UBX_ACKNAK_PAYLOADLENGTH) &&
         (receivedPacket.payload[0] == requestedClass_) &&
         (receivedPacket.payload[1] == requestedID_)) {
        acknowledgePacket.messageID = receivedPacket.messageID;
        acknowledgePacket.payload[0] = receivedPacket.payload[0];
        acknowledgePacket.payload[1] = receivedPacket.payload[1];
        acknowledgePacket.checksumA = receivedPacket.checksumA;
        acknowledgePacket.checksumB = receivedPacket.checksumB;
        acknowledgePacket.validPacket = receivedPacket.validPacket;
        receivedPacket.validPacket = false;
        return;
      }
    }

    // NAV-PVT packet
    if((receivedPacket.messageClass == UBX_CLASS_NAV) &&
       (receivedPacket.messageID == UBX_NAV_PVT) &&
       (receivedPacket.payloadLength == UBX_NAV_PVT_PAYLOADLENGTH)) {
      if(ubloxNAVPVTPacketBuffer.validPacket) {
        // Lost rx packet
        lostNAVPVTPacketCount += (lostNAVPVTPacketCount < 99) ? 1 : 0;
      } else {
        ubloxNAVPVTPacketBuffer = receivedPacket;
      }
      receivedPacket.validPacket = false;
      return;
    }

    // NAV-STATUS packet
    if((receivedPacket.messageClass == UBX_CLASS_NAV) &&
       (receivedPacket.messageID == UBX_NAV_STATUS) &&
       (receivedPacket.payloadLength == UBX_NAV_STATUS_PAYLOADLENGTH)) {
      if(ubloxNAVSTATUSPacketBuffer.validPacket) {
        // Lost rx packet
        lostNAVSTATUSPacketCount += (lostNAVSTATUSPacketCount < 99) ? 1 : 0;
      } else {
        ubloxNAVSTATUSPacketBuffer = receivedPacket;
      }
      receivedPacket.validPacket = false;
      return;
    }

    // NAV-SAT packet
    if((receivedPacket.messageClass == UBX_CLASS_NAV) &&
       (receivedPacket.messageID == UBX_NAV_SAT) &&
       (receivedPacket.payloadLength >= UBX_NAV_SAT_MINPAYLOADLENGTH) &&
       (((receivedPacket.payloadLength - 8) % 12) == 0)) {
      if(ubloxNAVSATPacketBuffer.validPacket) {
        // Lost rx packet
        lostNAVSATPacketCount += (lostNAVSATPacketCount < 99) ? 1 : 0;
      } else {
        ubloxNAVSATPacketBuffer = receivedPacket;
      }
      receivedPacket.validPacket = false;
      return;
    }

    // Unknown packet
    unknownRxPacketCount += (unknownRxPacketCount < 99) ? 1 : 0;
    receivedPacket.validPacket = false;
    return;

  }
}

/********************************************************************/
uint8_t TeenyUbloxConnect::getLostRxPacketCount() {
  uint8_t _count = lostRxPacketCount;
  lostRxPacketCount = 0;
  return _count;
}

/********************************************************************/
uint8_t TeenyUbloxConnect::getUnknownRxPacketCount() {
  uint8_t _count = unknownRxPacketCount;
  unknownRxPacketCount = 0;
  return _count;
}

/********************************************************************/
uint8_t TeenyUbloxConnect::getLostNAVPVTPacketCount() {
  uint8_t _count = lostNAVPVTPacketCount;
  lostNAVPVTPacketCount = 0;
  return _count;
}

/********************************************************************/
uint8_t TeenyUbloxConnect::getLostNAVSTATUSPacketCount() {
  uint8_t _count = lostNAVSTATUSPacketCount;
  lostNAVSTATUSPacketCount = 0;
  return _count;
}

/********************************************************************/
uint8_t TeenyUbloxConnect::getLostNAVSATPacketCount() {
  uint8_t _count = lostNAVSATPacketCount;
  lostNAVSATPacketCount = 0;
  return _count;
}


/********************************************************************/
/********************************************************************/
// Ublox GNSS info data access
/********************************************************************/
/********************************************************************/
ubloxMONGNSSInfo_t TeenyUbloxConnect::getGNSSSelectionInfo() {
  return ubloxMONGNSSInfo;
}
ubloxCFGGNSSInfo_t TeenyUbloxConnect::getGNSSConfigInfo() {
  return ubloxCFGGNSSInfo;
}
void TeenyUbloxConnect::resetGNSSConfigState() {
  // GNSS State - Unknown=-1, Disabled=0, Enabled=1
  ubloxCFGGNSSState.GPS        = -1;
  ubloxCFGGNSSState.GPS_L1CA   = -1;
  ubloxCFGGNSSState.SBAS       = -1;
  ubloxCFGGNSSState.SBAS_L1CA  = -1;
  ubloxCFGGNSSState.Galileo    = -1;
  ubloxCFGGNSSState.Galileo_E1 = -1;
  ubloxCFGGNSSState.BeiDou     = -1;
  ubloxCFGGNSSState.BeiDou_B1  = -1;
  ubloxCFGGNSSState.BeiDou_B1C = -1;
  ubloxCFGGNSSState.IMES       = -1;
  ubloxCFGGNSSState.QZSS       = -1;
  ubloxCFGGNSSState.QZSS_L1CA  = -1;
  ubloxCFGGNSSState.QZSS_L1S   = -1;
  ubloxCFGGNSSState.GLONASS    = -1;
  ubloxCFGGNSSState.GLONASS_L1 = -1;
}
ubloxCFGGNSSState_t TeenyUbloxConnect::getGNSSConfigState() {
  return ubloxCFGGNSSState;
}


/********************************************************************/
/********************************************************************/
// Methods for NAVPVT packet processing
/********************************************************************/
/********************************************************************/
bool TeenyUbloxConnect::getNAVPVT() {
  processIncomingPacket();
  return processNAVPVTPacket();
}

/********************************************************************/
// For compatibility with SparkFun_u-blox_GNSS_Arduino_Library
bool TeenyUbloxConnect::getPVT() {
  return getNAVPVT();
}

/********************************************************************/
bool TeenyUbloxConnect::processNAVPVTPacket() {
  if(ubloxNAVPVTPacketBuffer.validPacket) {
    // Save buffer to packet
    ubloxNAVPVTPacket[0] = ubloxNAVPVTPacketBuffer.synch1;
    ubloxNAVPVTPacket[1] = ubloxNAVPVTPacketBuffer.synch2;
    ubloxNAVPVTPacket[2] = ubloxNAVPVTPacketBuffer.messageClass;
    ubloxNAVPVTPacket[3] = ubloxNAVPVTPacketBuffer.messageID;
    ubloxNAVPVTPacket[4] = ubloxNAVPVTPacketBuffer.payloadLength;
    ubloxNAVPVTPacket[5] = ubloxNAVPVTPacketBuffer.payloadLength >> 8;
    memcpy(ubloxNAVPVTPacket+6, ubloxNAVPVTPacketBuffer.payload, 92);
    ubloxNAVPVTPacket[98] = ubloxNAVPVTPacketBuffer.checksumA;
    ubloxNAVPVTPacket[99] = ubloxNAVPVTPacketBuffer.checksumB;
    // Parse packet fields
    setNAVPVTPacketInfo();
    ubloxNAVPVTPacketBuffer.validPacket = false;
    return true;
  }
  return false;
}

/********************************************************************/
void TeenyUbloxConnect::setNAVPVTPacketInfo() {
  ubloxNAVPVTInfo.year = ubloxNAVPVTPacketBuffer.payload[4];
  ubloxNAVPVTInfo.year |= ubloxNAVPVTPacketBuffer.payload[5] << 8;
  ubloxNAVPVTInfo.month = ubloxNAVPVTPacketBuffer.payload[6];
  ubloxNAVPVTInfo.day = ubloxNAVPVTPacketBuffer.payload[7];
  ubloxNAVPVTInfo.hour = ubloxNAVPVTPacketBuffer.payload[8];
  ubloxNAVPVTInfo.minute = ubloxNAVPVTPacketBuffer.payload[9];
  ubloxNAVPVTInfo.second = ubloxNAVPVTPacketBuffer.payload[10];
  ubloxNAVPVTInfo.dateValid = ubloxNAVPVTPacketBuffer.payload[11] & 0x01;
  ubloxNAVPVTInfo.timeValid = (ubloxNAVPVTPacketBuffer.payload[11] & 0x02) >> 1;
  ubloxNAVPVTInfo.tAcc = ubloxNAVPVTPacketBuffer.payload[12];
  ubloxNAVPVTInfo.tAcc |= ubloxNAVPVTPacketBuffer.payload[13] << 8;
  ubloxNAVPVTInfo.tAcc |= ubloxNAVPVTPacketBuffer.payload[14] << 16;
  ubloxNAVPVTInfo.tAcc |= ubloxNAVPVTPacketBuffer.payload[15] << 24;
  ubloxNAVPVTInfo.fixType = ubloxNAVPVTPacketBuffer.payload[20];
  ubloxNAVPVTInfo.locationValid = ubloxNAVPVTPacketBuffer.payload[21] & 0x01;
  ubloxNAVPVTInfo.numSV = ubloxNAVPVTPacketBuffer.payload[23];
  ubloxNAVPVTInfo.longitude = ubloxNAVPVTPacketBuffer.payload[24];
  ubloxNAVPVTInfo.longitude |= ubloxNAVPVTPacketBuffer.payload[25] << 8;
  ubloxNAVPVTInfo.longitude |= ubloxNAVPVTPacketBuffer.payload[26] << 16;
  ubloxNAVPVTInfo.longitude |= ubloxNAVPVTPacketBuffer.payload[27] << 24;
  ubloxNAVPVTInfo.latitude = ubloxNAVPVTPacketBuffer.payload[28];
  ubloxNAVPVTInfo.latitude |= ubloxNAVPVTPacketBuffer.payload[29] << 8;
  ubloxNAVPVTInfo.latitude |= ubloxNAVPVTPacketBuffer.payload[30] << 16;
  ubloxNAVPVTInfo.latitude |= ubloxNAVPVTPacketBuffer.payload[31] << 24;
  ubloxNAVPVTInfo.altitude = ubloxNAVPVTPacketBuffer.payload[32];
  ubloxNAVPVTInfo.altitude |= ubloxNAVPVTPacketBuffer.payload[33] << 8;
  ubloxNAVPVTInfo.altitude |= ubloxNAVPVTPacketBuffer.payload[34] << 16;
  ubloxNAVPVTInfo.altitude |= ubloxNAVPVTPacketBuffer.payload[35] << 24;
  ubloxNAVPVTInfo.altitudeMSL = ubloxNAVPVTPacketBuffer.payload[36];
  ubloxNAVPVTInfo.altitudeMSL |= ubloxNAVPVTPacketBuffer.payload[37] << 8;
  ubloxNAVPVTInfo.altitudeMSL |= ubloxNAVPVTPacketBuffer.payload[38] << 16;
  ubloxNAVPVTInfo.altitudeMSL |= ubloxNAVPVTPacketBuffer.payload[39] << 24;
  ubloxNAVPVTInfo.hAcc = ubloxNAVPVTPacketBuffer.payload[40];
  ubloxNAVPVTInfo.hAcc |= ubloxNAVPVTPacketBuffer.payload[41] << 8;
  ubloxNAVPVTInfo.hAcc |= ubloxNAVPVTPacketBuffer.payload[42] << 16;
  ubloxNAVPVTInfo.hAcc |= ubloxNAVPVTPacketBuffer.payload[43] << 24;
  ubloxNAVPVTInfo.vAcc = ubloxNAVPVTPacketBuffer.payload[44];
  ubloxNAVPVTInfo.vAcc |= ubloxNAVPVTPacketBuffer.payload[45] << 8;
  ubloxNAVPVTInfo.vAcc |= ubloxNAVPVTPacketBuffer.payload[46] << 16;
  ubloxNAVPVTInfo.vAcc |= ubloxNAVPVTPacketBuffer.payload[47] << 24;
  ubloxNAVPVTInfo.velN = ubloxNAVPVTPacketBuffer.payload[48];
  ubloxNAVPVTInfo.velN |= ubloxNAVPVTPacketBuffer.payload[49] << 8;
  ubloxNAVPVTInfo.velN |= ubloxNAVPVTPacketBuffer.payload[50] << 16;
  ubloxNAVPVTInfo.velN |= ubloxNAVPVTPacketBuffer.payload[51] << 24;
  ubloxNAVPVTInfo.velE = ubloxNAVPVTPacketBuffer.payload[52];
  ubloxNAVPVTInfo.velE |= ubloxNAVPVTPacketBuffer.payload[53] << 8;
  ubloxNAVPVTInfo.velE |= ubloxNAVPVTPacketBuffer.payload[54] << 16;
  ubloxNAVPVTInfo.velE |= ubloxNAVPVTPacketBuffer.payload[55] << 24;
  ubloxNAVPVTInfo.velD = ubloxNAVPVTPacketBuffer.payload[56];
  ubloxNAVPVTInfo.velD |= ubloxNAVPVTPacketBuffer.payload[57] << 8;
  ubloxNAVPVTInfo.velD |= ubloxNAVPVTPacketBuffer.payload[58] << 16;
  ubloxNAVPVTInfo.velD |= ubloxNAVPVTPacketBuffer.payload[59] << 24;
  ubloxNAVPVTInfo.gSpeed = ubloxNAVPVTPacketBuffer.payload[60];
  ubloxNAVPVTInfo.gSpeed |= ubloxNAVPVTPacketBuffer.payload[61] << 8;
  ubloxNAVPVTInfo.gSpeed |= ubloxNAVPVTPacketBuffer.payload[62] << 16;
  ubloxNAVPVTInfo.gSpeed |= ubloxNAVPVTPacketBuffer.payload[63] << 24;
  ubloxNAVPVTInfo.headMot = ubloxNAVPVTPacketBuffer.payload[64];
  ubloxNAVPVTInfo.headMot |= ubloxNAVPVTPacketBuffer.payload[65] << 8;
  ubloxNAVPVTInfo.headMot |= ubloxNAVPVTPacketBuffer.payload[66] << 16;
  ubloxNAVPVTInfo.headMot |= ubloxNAVPVTPacketBuffer.payload[67] << 24;
  ubloxNAVPVTInfo.sAcc = ubloxNAVPVTPacketBuffer.payload[68];
  ubloxNAVPVTInfo.sAcc |= ubloxNAVPVTPacketBuffer.payload[69] << 8;
  ubloxNAVPVTInfo.sAcc |= ubloxNAVPVTPacketBuffer.payload[70] << 16;
  ubloxNAVPVTInfo.sAcc |= ubloxNAVPVTPacketBuffer.payload[71] << 24;
  ubloxNAVPVTInfo.headAcc = ubloxNAVPVTPacketBuffer.payload[72];
  ubloxNAVPVTInfo.headAcc |= ubloxNAVPVTPacketBuffer.payload[73] << 8;
  ubloxNAVPVTInfo.headAcc |= ubloxNAVPVTPacketBuffer.payload[74] << 16;
  ubloxNAVPVTInfo.headAcc |= ubloxNAVPVTPacketBuffer.payload[75] << 24;
  ubloxNAVPVTInfo.pDOP = ubloxNAVPVTPacketBuffer.payload[76];
  ubloxNAVPVTInfo.pDOP |= ubloxNAVPVTPacketBuffer.payload[77] << 8;
  ubloxNAVPVTInfo.invalidLlh = ubloxNAVPVTPacketBuffer.payload[78] & 0x01;
}

/********************************************************************/
void TeenyUbloxConnect::getNAVPVTPacket(uint8_t *packet_) {
  memcpy(packet_, ubloxNAVPVTPacket, UBX_NAV_PVT_PACKETLENGTH);
}

/********************************************************************/
bool TeenyUbloxConnect::getDateValid() {
  return ubloxNAVPVTInfo.dateValid;
}
bool TeenyUbloxConnect::getTimeValid() {
  return ubloxNAVPVTInfo.timeValid;
}
uint32_t TeenyUbloxConnect::getTimeAccEst() {
  return ubloxNAVPVTInfo.tAcc;
}
uint16_t TeenyUbloxConnect::getYear() {
  return ubloxNAVPVTInfo.year;
}
uint8_t TeenyUbloxConnect::getMonth() {
  return ubloxNAVPVTInfo.month;
}
uint8_t TeenyUbloxConnect::getDay() {
  return ubloxNAVPVTInfo.day;
}
uint8_t TeenyUbloxConnect::getHour() {
  return ubloxNAVPVTInfo.hour;
}
uint8_t TeenyUbloxConnect::getMinute() {
  return ubloxNAVPVTInfo.minute;
}
uint8_t TeenyUbloxConnect::getSecond() {
  return ubloxNAVPVTInfo.second;
}
/********************************************************************/
uint8_t TeenyUbloxConnect::getFixType() {
  return ubloxNAVPVTInfo.fixType;
}
bool TeenyUbloxConnect::getGnssFixOk() {
  return ubloxNAVPVTInfo.locationValid;
}
uint8_t TeenyUbloxConnect::getSIV() {
  return ubloxNAVPVTInfo.numSV;
}
int32_t TeenyUbloxConnect::getLongitude() {
  return ubloxNAVPVTInfo.longitude;
}
int32_t TeenyUbloxConnect::getLatitude() {
  return ubloxNAVPVTInfo.latitude;
}
int32_t TeenyUbloxConnect::getAltitude() {
  return ubloxNAVPVTInfo.altitude;
}
int32_t TeenyUbloxConnect::getAltitudeMSL() {
  return ubloxNAVPVTInfo.altitudeMSL;
}
uint32_t TeenyUbloxConnect::getHorizontalAccEst() {
  return ubloxNAVPVTInfo.hAcc;
}
uint32_t TeenyUbloxConnect::getVerticalAccEst() {
  return ubloxNAVPVTInfo.vAcc;
}
int32_t TeenyUbloxConnect::getVelN() {
  return ubloxNAVPVTInfo.velN;
}
int32_t TeenyUbloxConnect::getVelE() {
  return ubloxNAVPVTInfo.velE;
}
int32_t TeenyUbloxConnect::getVelD() {
  return ubloxNAVPVTInfo.velD;
}
int32_t TeenyUbloxConnect::getGroundSpeed() {
  return ubloxNAVPVTInfo.gSpeed;
}
int32_t TeenyUbloxConnect::getHeading() {
  return ubloxNAVPVTInfo.headMot;
}
uint32_t TeenyUbloxConnect::getSpeedAccEst() {
  return ubloxNAVPVTInfo.sAcc;
}
uint32_t TeenyUbloxConnect::getHeadingAccEst() {
  return ubloxNAVPVTInfo.headAcc;
}
uint16_t TeenyUbloxConnect::getPDOP() {
  return ubloxNAVPVTInfo.pDOP;
}
bool TeenyUbloxConnect::getInvalidLlh() {
  return ubloxNAVPVTInfo.invalidLlh;
}


/********************************************************************/
/********************************************************************/
// Methods for NAVSTATUS packet processing
/********************************************************************/
/********************************************************************/
bool TeenyUbloxConnect::getNAVSTATUS() {
  processIncomingPacket();
  return processNAVSTATUSPacket();
}

/********************************************************************/
bool TeenyUbloxConnect::processNAVSTATUSPacket() {
  if(ubloxNAVSTATUSPacketBuffer.validPacket) {
    // Save buffer to packet
    ubloxNAVSTATUSPacket[0] = ubloxNAVSTATUSPacketBuffer.synch1;
    ubloxNAVSTATUSPacket[1] = ubloxNAVSTATUSPacketBuffer.synch2;
    ubloxNAVSTATUSPacket[2] = ubloxNAVSTATUSPacketBuffer.messageClass;
    ubloxNAVSTATUSPacket[3] = ubloxNAVSTATUSPacketBuffer.messageID;
    ubloxNAVSTATUSPacket[4] = ubloxNAVSTATUSPacketBuffer.payloadLength;
    ubloxNAVSTATUSPacket[5] = ubloxNAVSTATUSPacketBuffer.payloadLength >> 8;
    memcpy(ubloxNAVSTATUSPacket+6, ubloxNAVSTATUSPacketBuffer.payload, 16);
    ubloxNAVSTATUSPacket[22] = ubloxNAVSTATUSPacketBuffer.checksumA;
    ubloxNAVSTATUSPacket[23] = ubloxNAVSTATUSPacketBuffer.checksumB;
    // Parse packet fields
    setNAVSTATUSPacketInfo();
    ubloxNAVSTATUSPacketBuffer.validPacket = false;
    return true;
  }
  return false;
}

/********************************************************************/
void TeenyUbloxConnect::setNAVSTATUSPacketInfo() {
  ubloxNAVSTATUSInfo.validPacket = true;
  ubloxNAVSTATUSInfo.gpsFix = ubloxNAVSTATUSPacketBuffer.payload[4];
  ubloxNAVSTATUSInfo.gpsFixOk = ubloxNAVSTATUSPacketBuffer.payload[5] & 0x01;
  ubloxNAVSTATUSInfo.psmState = ubloxNAVSTATUSPacketBuffer.payload[7] & 0x03;
  ubloxNAVSTATUSInfo.spoofDetState = (ubloxNAVSTATUSPacketBuffer.payload[7] & 0x18) >> 3;
  if(ubloxNAVSTATUSInfo.spoofDetState == 2) {
    ubloxNAVSTATUSInfo.spoofingIndicated = true;
  }
  if(ubloxNAVSTATUSInfo.spoofDetState == 3) {
    ubloxNAVSTATUSInfo.multipleSpoofingIndications = true;
  }
  ubloxNAVSTATUSInfo.carrSoln = (ubloxNAVSTATUSPacketBuffer.payload[7] & 0xC0) >> 6;
  ubloxNAVSTATUSInfo.ttff = ubloxNAVSTATUSPacketBuffer.payload[8];
  ubloxNAVSTATUSInfo.ttff |= ubloxNAVSTATUSPacketBuffer.payload[9] << 8;
  ubloxNAVSTATUSInfo.ttff |= ubloxNAVSTATUSPacketBuffer.payload[10] << 16;
  ubloxNAVSTATUSInfo.ttff |= ubloxNAVSTATUSPacketBuffer.payload[11] << 24;
  ubloxNAVSTATUSInfo.msss = ubloxNAVSTATUSPacketBuffer.payload[12];
  ubloxNAVSTATUSInfo.msss |= ubloxNAVSTATUSPacketBuffer.payload[13] << 8;
  ubloxNAVSTATUSInfo.msss |= ubloxNAVSTATUSPacketBuffer.payload[14] << 16;
  ubloxNAVSTATUSInfo.msss |= ubloxNAVSTATUSPacketBuffer.payload[15] << 24;
}

/********************************************************************/
void TeenyUbloxConnect::getNAVSTATUSPacket(uint8_t *packet_) {
  memcpy(packet_, ubloxNAVSTATUSPacket, UBX_NAV_STATUS_PACKETLENGTH);
}

/********************************************************************/
void TeenyUbloxConnect::getNAVSTATUSInfo(ubloxNAVSTATUSInfo_t &info_) {
  info_ = ubloxNAVSTATUSInfo;
}

/********************************************************************/
void TeenyUbloxConnect::resetNAVSTATUSInfo() {
  ubloxNAVSTATUSInfo.spoofingIndicated = false;
  ubloxNAVSTATUSInfo.multipleSpoofingIndications = false;
}


/********************************************************************/
/********************************************************************/
// Methods for NAVSAT packet processing
/********************************************************************/
/********************************************************************/
bool TeenyUbloxConnect::getNAVSAT() {
  processIncomingPacket();
  return processNAVSATPacket();
}

/********************************************************************/
bool TeenyUbloxConnect::processNAVSATPacket() {
  if(ubloxNAVSATPacketBuffer.validPacket) {
    // Save buffer to packet
    uint16_t payloadLength = min(ubloxNAVSATPacketBuffer.payloadLength, UBX_NAV_SAT_MAXPAYLOADLENGTH);
    ubloxNAVSATPacketLength = 8 + payloadLength;
    ubloxNAVSATPacket[0] = ubloxNAVSATPacketBuffer.synch1;
    ubloxNAVSATPacket[1] = ubloxNAVSATPacketBuffer.synch2;
    ubloxNAVSATPacket[2] = ubloxNAVSATPacketBuffer.messageClass;
    ubloxNAVSATPacket[3] = ubloxNAVSATPacketBuffer.messageID;
    ubloxNAVSATPacket[4] = payloadLength & 0xFF;
    ubloxNAVSATPacket[5] = payloadLength >> 8;
    memcpy(ubloxNAVSATPacket+6, ubloxNAVSATPacketBuffer.payload, payloadLength);
    // Adjust numSvs based on max packet/payload size
    ubloxNAVSATPacket[6+5] = min(ubloxNAVSATPacket[6+5], UBX_MAXNAVSATSATELLITES);
    if(ubloxNAVSATPacketBuffer.payloadLength <= UBX_NAV_SAT_MAXPAYLOADLENGTH) {
      // Save checksum
      ubloxNAVSATPacket[ubloxNAVSATPacketLength-2] = ubloxNAVSATPacketBuffer.checksumA;
      ubloxNAVSATPacket[ubloxNAVSATPacketLength-1] = ubloxNAVSATPacketBuffer.checksumB;
    } else {
      // Calculate new checksum
      uint8_t checksumA = 0;
      uint8_t checksumB = 0;
      checksumA += ubloxNAVSATPacket[2];
      checksumB += checksumA;
      checksumA += ubloxNAVSATPacket[3];
      checksumB += checksumA;
      checksumA += ubloxNAVSATPacket[4];
      checksumB += checksumA;
      checksumA += ubloxNAVSATPacket[5];
      checksumB += checksumA;
      for(uint16_t i = 0; i < payloadLength; i++) {
        checksumA += ubloxNAVSATPacket[6 + i];
        checksumB += checksumA;
      }
      ubloxNAVSATPacket[ubloxNAVSATPacketLength-2] = checksumA;
      ubloxNAVSATPacket[ubloxNAVSATPacketLength-1] = checksumB;
    }
    // Parse packet fields
    setNAVSATPacketInfo();
    ubloxNAVSATPacketBuffer.validPacket = false;
    return true;
  }
  return false;
}

/********************************************************************/
void TeenyUbloxConnect::setNAVSATPacketInfo() {
  ubloxNAVSATInfo.validPacket = true;
  ubloxNAVSATInfo.numSvsReceived = ubloxNAVSATPacketBuffer.payload[5];
  ubloxNAVSATInfo.numSvs = min(ubloxNAVSATInfo.numSvsReceived, UBX_MAXNAVSATSATELLITES);
  ubloxNAVSATInfo.numSvsHealthy = 0;
  ubloxNAVSATInfo.numSvsEphValid = 0;
  ubloxNAVSATInfo.numSvsHealthyAndEphValid = 0;
  ubloxNAVSATInfo.numSvsUsed = 0;
  // Reset sort list
  for(uint8_t i=0; i<UBX_MAXNAVSATSATELLITES; i++) {
    ubloxNAVSATInfo.svSortList[i].gnssId = -1;
    ubloxNAVSATInfo.svSortList[i].gnssIdType = '?';
    ubloxNAVSATInfo.svSortList[i].svId = -1;
    ubloxNAVSATInfo.svSortList[i].cno = 0;
    ubloxNAVSATInfo.svSortList[i].elev = -91;
    ubloxNAVSATInfo.svSortList[i].azim = 0;
    ubloxNAVSATInfo.svSortList[i].prRes = 0;
    ubloxNAVSATInfo.svSortList[i].ephValid = false;
    ubloxNAVSATInfo.svSortList[i].healthy = false;
    ubloxNAVSATInfo.svSortList[i].svUsed = false;
  }
  // Using cno to filter unusable or filtered svs
  // Zero out cno of satellites with no signal or unknown/unhealthy signal
  for(uint8_t i=0; i<ubloxNAVSATInfo.numSvs; i++) {
    if((ubloxNAVSATPacketBuffer.payload[(i*12)+16] & 0x30) != 0x10) {
      ubloxNAVSATPacketBuffer.payload[(i*12)+10] = 0;
    }
  }
  // Find and sort up to UBX_MAXNAVSATSATELLITES
  char gnssIdTypeMap[8]={'G','S','E','B','I','Q','R','N'};
  for(uint8_t i=0; i<UBX_MAXNAVSATSATELLITES; i++) {
    bool foundSat = false;
    uint8_t foundSatIndex;
    ubloxNAVSATSVInfo_t foundSatInfo, compareSatInfo; 
    for(uint8_t j=0; j<ubloxNAVSATInfo.numSvs; j++) {
      if(ubloxNAVSATPacketBuffer.payload[(j*12)+10]) {
        // compare all the fields to see which is a 'better' satellite and replace if better
        compareSatInfo.gnssId = ubloxNAVSATPacketBuffer.payload[(j*12)+8];
        if((compareSatInfo.gnssId < 0) || (compareSatInfo.gnssId > 7)) {
          compareSatInfo.gnssId = -1;
          compareSatInfo.gnssIdType = '?';
          compareSatInfo.svId = -1;
        } else {
          compareSatInfo.gnssIdType = gnssIdTypeMap[compareSatInfo.gnssId];
          compareSatInfo.svId = ubloxNAVSATPacketBuffer.payload[(j*12)+9];
        }
        compareSatInfo.cno = ubloxNAVSATPacketBuffer.payload[(j*12)+10];
        compareSatInfo.elev = ubloxNAVSATPacketBuffer.payload[(j*12)+11];
        compareSatInfo.ephValid = (compareSatInfo.elev >= -90) && (compareSatInfo.elev <= 90);
        compareSatInfo.azim = ubloxNAVSATPacketBuffer.payload[(j*12)+12];
        compareSatInfo.azim |= ubloxNAVSATPacketBuffer.payload[(j*12)+13] << 8;
        compareSatInfo.prRes = ubloxNAVSATPacketBuffer.payload[(j*12)+14];
        compareSatInfo.prRes |= ubloxNAVSATPacketBuffer.payload[(j*12)+15] << 8;
        compareSatInfo.healthy = ((ubloxNAVSATPacketBuffer.payload[(j*12)+16] & 0x30) == 0x10) ? true : false;
        compareSatInfo.svUsed = (ubloxNAVSATPacketBuffer.payload[(j*12)+16] & 0x08) ? true : false;
        bool updateFoundSat = false;
        if(!foundSat) {
          updateFoundSat = true;
        } else if(compareSatInfo.svUsed && (!foundSatInfo.svUsed)) {
          updateFoundSat = true;
        } else if((compareSatInfo.svUsed == foundSatInfo.svUsed) &&
                  (compareSatInfo.ephValid && (!foundSatInfo.ephValid))) {
          updateFoundSat = true;
        } else if((compareSatInfo.svUsed == foundSatInfo.svUsed) &&
                  (compareSatInfo.ephValid == foundSatInfo.ephValid) &&
                  (compareSatInfo.healthy && (!foundSatInfo.healthy))) {
          updateFoundSat = true;
        } else if((compareSatInfo.svUsed == foundSatInfo.svUsed) &&
                  (compareSatInfo.ephValid == foundSatInfo.ephValid) &&
                  (compareSatInfo.healthy == foundSatInfo.healthy) &&
                  (compareSatInfo.cno > foundSatInfo.cno)) {
          updateFoundSat = true;
        }
        if(updateFoundSat) {
          foundSat = true;
          foundSatIndex = j;
          foundSatInfo = compareSatInfo;
        }
      }
    }
    if(foundSat) {
      // remove found satellite from buffer sort by changing cno to 0
      ubloxNAVSATPacketBuffer.payload[(foundSatIndex*12)+10] = 0;
      // add found satellite to sort list
      ubloxNAVSATInfo.svSortList[i] = foundSatInfo;
      // update satellites stats
      if(foundSatInfo.healthy) ubloxNAVSATInfo.numSvsHealthy++;
      if(foundSatInfo.ephValid) ubloxNAVSATInfo.numSvsEphValid++;
      if(foundSatInfo.healthy && foundSatInfo.ephValid) ubloxNAVSATInfo.numSvsHealthyAndEphValid++;
      if(foundSatInfo.svUsed) ubloxNAVSATInfo.numSvsUsed++;
    } else {
      // no more satellites with cno > 0
      break;
    }
  }
}

/********************************************************************/
void TeenyUbloxConnect::getNAVSATPacket(uint8_t *packet_) {
  memcpy(packet_, ubloxNAVSATPacket, UBX_NAV_SAT_MAXPACKETLENGTH);
}

/********************************************************************/
uint16_t TeenyUbloxConnect::getNAVSATPacketLength() {
  return ubloxNAVSATPacketLength;
}

/********************************************************************/
void TeenyUbloxConnect::getNAVSATInfo(ubloxNAVSATInfo_t &info_) {
  info_ = ubloxNAVSATInfo;
}

