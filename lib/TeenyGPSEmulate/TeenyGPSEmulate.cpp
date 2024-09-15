/*
TeenyGPSEmulate.h - Class file for the TeenyGPSEmulate Arduino Library.
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
#include "TeenyGPSEmulate.h"

/********************************************************************/
// UBX Emulation Loop Packets
/********************************************************************/
//#include "TeenyGPSEmulate.navPvtLoop.h"
//#include "TeenyGPSEmulate.navPvtSatLoop.h"
#include "TeenyGPSEmulate.navPvtStatusSatLoop.h"

/********************************************************************/
TeenyGPSEmulate::TeenyGPSEmulate() { }

TeenyGPSEmulate::~TeenyGPSEmulate() { }

/********************************************************************/
bool TeenyGPSEmulate::init(HardwareSerial &serialPort_,
                           uint32_t baudRate_,
                           tgpse_ubx_module_type_t ubxModuleType_,
                           uint8_t (*fetch)()) {
  reset();
  serialPort = &serialPort_;
  emulatorSettings.baudRate = baudRate_;
  ubxModuleType = ubxModuleType_;
  ubxFetch = fetch;
  if((ubxModuleType != TGPSE_UBX_M8_MODULE) &&
     (ubxModuleType != TGPSE_UBX_M10_MODULE)) {
    return false;
  }
  serialPort->begin(emulatorSettings.baudRate);
  return true;
}

/********************************************************************/
bool TeenyGPSEmulate::reset() {
  emulatorSettings = emulatorSettings_default;
  incomingPacket.validPacket = false;
  receivedPacket.validPacket = false;
  responsePacket.validPacket = false;
  acknowledgePacket.validPacket = false;
  unknownPacket.validPacket = false;
  lostRxPacketCount = 0;
  requestNAVPVTPacket = false;
  lostNAVPVTRequestCount = 0;
  requestNAVSTATUSPacket = false;
  lostNAVSTATUSRequestCount = 0;
  requestNAVSATPacket = false;
  lostNAVSATRequestCount = 0;
  ubxLoopPacket.validPacket = false;
  ubxLoopPacketIndex = 0;
  setEmuColdOutputPackets();
  return true;
}

/********************************************************************/
/********************************************************************/
// Methods for receiving host commands and sending replies and ACK/NAK
/********************************************************************/
/********************************************************************/
void TeenyGPSEmulate::tick() {
  while(serialPort->available()) {
    if(incomingPacket.validPacket) return;
    processIncomingByte(serialPort->read());
  }
}

/********************************************************************/
void TeenyGPSEmulate::processIncomingByte(uint8_t incomingByte) {

  // synch1
  if(!incomingPacket.receivingPacket) {
    if(incomingByte == incomingPacket.synch1) {
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
    if(incomingByte == incomingPacket.synch2) {
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
      incomingPacket.messageClass = incomingByte;
    } else if(incomingPacket.packetCounter == 3) {
      incomingPacket.messageID = incomingByte;
    } else if(incomingPacket.packetCounter == 4) {
      incomingPacket.payloadLength = incomingByte;
    } else if(incomingPacket.packetCounter == 5) {
      incomingPacket.payloadLength |= (incomingByte << 8);
      incomingPacket.payloadCounter = 0;
    }
    incomingPacket.packetCounter++;
    incomingPacket.rollingChecksumA += incomingByte;
    incomingPacket.rollingChecksumB += incomingPacket.rollingChecksumA;

  // payload
  } else if(incomingPacket.receivingPacket &&
            (incomingPacket.payloadCounter < incomingPacket.payloadLength)) {
    if(incomingPacket.payloadCounter < sizeof(incomingPacket.payload)) {
      // Not storing large full packets, just checking and responding with NAK
      incomingPacket.payload[incomingPacket.payloadCounter] = incomingByte;
    }
    incomingPacket.payloadCounter++;
    incomingPacket.packetCounter++;
    incomingPacket.rollingChecksumA += incomingByte;
    incomingPacket.rollingChecksumB += incomingPacket.rollingChecksumA;

  // checksumA
  } else if(incomingPacket.receivingPacket &&
            (incomingPacket.packetCounter == (incomingPacket.payloadLength + 6))) {
    incomingPacket.checksumA = incomingByte;
    incomingPacket.packetCounter++;

  // checksumB
  } else if(incomingPacket.receivingPacket &&
            (incomingPacket.packetCounter == (incomingPacket.payloadLength + 7))) {
    incomingPacket.checksumB = incomingByte;
    incomingPacket.packetCounter++;
    if((incomingPacket.checksumA == incomingPacket.rollingChecksumA) &&
       (incomingPacket.checksumB == incomingPacket.rollingChecksumB)) {
      incomingPacket.validPacket = true;
    }
    incomingPacket.receivingPacket = false;
  }
}

/********************************************************************/
void TeenyGPSEmulate::processIncomingPacket() {
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

    // Poll CFG-PRT COM_PORT_UART1
    if((receivedPacket.messageClass == TGPSE_UBX_CLASS_CFG) &&
       (receivedPacket.messageID == TGPSE_UBX_CFG_PRT) &&
       (receivedPacket.payloadLength == 1) &&
       (receivedPacket.payload[0] == TGPSE_COM_PORT_UART1)) {
      // Return configuration for COM_PORT_UART1
      responsePacket.messageClass = receivedPacket.messageClass;
      responsePacket.messageID = receivedPacket.messageID;
      responsePacket.payloadLength = TGPSE_UBX_CFG_PRT_PAYLOADLENGTH;
      memcpy(responsePacket.payload, TGPSE_UBX_CFG_PRT_PAYLOAD, TGPSE_UBX_CFG_PRT_PAYLOADLENGTH);
      responsePacket.payload[0] = TGPSE_COM_PORT_UART1;
      responsePacket.payload[8] = emulatorSettings.baudRate & 0xFF;
      responsePacket.payload[9] = emulatorSettings.baudRate >> 8;
      responsePacket.payload[10] = emulatorSettings.baudRate >> 16;
      responsePacket.payload[11] = emulatorSettings.baudRate >> 24;
      responsePacket.payload[14] = emulatorSettings.outputUBX ? 0x01 : 0x00;
      calcChecksum(&responsePacket);
      responsePacket.validPacket = true;
      buildAcknowledgePacket(receivedPacket.messageClass, receivedPacket.messageID, true);

    // Set CFG-PRT COM_PORT_UART1
    } else if((ubxModuleType == TGPSE_UBX_M8_MODULE) &&
              (receivedPacket.messageClass == TGPSE_UBX_CLASS_CFG) &&
              (receivedPacket.messageID == TGPSE_UBX_CFG_PRT) &&
              (receivedPacket.payloadLength == TGPSE_UBX_CFG_PRT_PAYLOADLENGTH) &&
              (receivedPacket.payload[0] == TGPSE_COM_PORT_UART1)) {
      // Update configuration for COM_PORT_UART1 and ACK if not changing baudRate
      requestedBaudRate = receivedPacket.payload[8];
      requestedBaudRate |= receivedPacket.payload[9] << 8;
      requestedBaudRate |= receivedPacket.payload[10] << 16;
      requestedBaudRate |= receivedPacket.payload[11] << 24;
      if(requestedBaudRate != emulatorSettings.baudRate) {
        if((requestedBaudRate == 9600) ||
           (requestedBaudRate == 38400) ||
           (requestedBaudRate == 115200)) {
          init(*serialPort, requestedBaudRate, ubxModuleType);
          receivedPacket.validPacket = false;
          return;
        } else {
          buildAcknowledgePacket(receivedPacket.messageClass, receivedPacket.messageID, false);
        }
      } else if(((receivedPacket.payload[14] & 0xFE) != 0x00) ||
                (receivedPacket.payload[15] != 0x00)) {
        buildAcknowledgePacket(receivedPacket.messageClass, receivedPacket.messageID, false);
      } else {
        emulatorSettings.outputUBX = receivedPacket.payload[14] & 0x01;
        buildAcknowledgePacket(receivedPacket.messageClass, receivedPacket.messageID, true);
      }

    // Poll CFG-RATE (measurementRate and navigationRate)
    } else if((ubxModuleType == TGPSE_UBX_M8_MODULE) &&
              (receivedPacket.messageClass == TGPSE_UBX_CLASS_CFG) &&
              (receivedPacket.messageID == TGPSE_UBX_CFG_RATE) &&
              (receivedPacket.payloadLength == 0)) {
      // Return measurementRate and navigationRate
      responsePacket.messageClass = receivedPacket.messageClass;
      responsePacket.messageID = receivedPacket.messageID;
      responsePacket.payloadLength = TGPSE_UBX_CFG_RATE_PAYLOADLENGTH;
      memcpy(responsePacket.payload, TGPSE_UBX_CFG_RATE_PAYLOAD, TGPSE_UBX_CFG_RATE_PAYLOADLENGTH);
      responsePacket.payload[0] = emulatorSettings.measurementRate & 0xFF;
      responsePacket.payload[1] = emulatorSettings.measurementRate >> 8;
      responsePacket.payload[2] = emulatorSettings.navigationRate & 0xFF;
      responsePacket.payload[3] = emulatorSettings.navigationRate >> 8;
      calcChecksum(&responsePacket);
      responsePacket.validPacket = true;
      buildAcknowledgePacket(receivedPacket.messageClass, receivedPacket.messageID, true);

    // Set CFG-RATE (measurementRate and navigationRate)
    } else if((ubxModuleType == TGPSE_UBX_M8_MODULE) &&
              (receivedPacket.messageClass == TGPSE_UBX_CLASS_CFG) &&
              (receivedPacket.messageID == TGPSE_UBX_CFG_RATE) &&
              (receivedPacket.payloadLength == TGPSE_UBX_CFG_RATE_PAYLOADLENGTH)) {
      // Update measurementRate and navigationRate
      emulatorSettings.measurementRate = receivedPacket.payload[0];
      emulatorSettings.measurementRate |= receivedPacket.payload[1] << 8;
      emulatorSettings.navigationRate = receivedPacket.payload[2];
      emulatorSettings.navigationRate |= receivedPacket.payload[3] << 8;
      buildAcknowledgePacket(receivedPacket.messageClass, receivedPacket.messageID, true);

    // Poll CFG-MSG (autoNAVPVTRate)
    } else if((ubxModuleType == TGPSE_UBX_M8_MODULE) &&
              (receivedPacket.messageClass == TGPSE_UBX_CLASS_CFG) &&
              (receivedPacket.messageID == TGPSE_UBX_CFG_MSG) &&
              (receivedPacket.payloadLength == 2) &&
              (receivedPacket.payload[0] == TGPSE_UBX_CLASS_NAV) &&
              (receivedPacket.payload[1] == TGPSE_UBX_NAV_PVT)) {
      // Return autoNAVPVTRate setting
      responsePacket.messageClass = receivedPacket.messageClass;
      responsePacket.messageID = receivedPacket.messageID;
      responsePacket.payloadLength = 3;
      responsePacket.payload[0] = TGPSE_UBX_CLASS_NAV;
      responsePacket.payload[1] = TGPSE_UBX_NAV_PVT;
      responsePacket.payload[2] = emulatorSettings.autoNAVPVTRate;
      calcChecksum(&responsePacket);
      responsePacket.validPacket = true;
      buildAcknowledgePacket(receivedPacket.messageClass, receivedPacket.messageID, true);

    // Set CFG-MSG (autoNAVPVTRate)
    } else if((ubxModuleType == TGPSE_UBX_M8_MODULE) &&
              (receivedPacket.messageClass == TGPSE_UBX_CLASS_CFG) &&
              (receivedPacket.messageID == TGPSE_UBX_CFG_MSG) &&
              (receivedPacket.payloadLength == 3) &&
              (receivedPacket.payload[0] == TGPSE_UBX_CLASS_NAV) &&
              (receivedPacket.payload[1] == TGPSE_UBX_NAV_PVT)) {
      emulatorSettings.autoNAVPVTRate = receivedPacket.payload[2];
      buildAcknowledgePacket(receivedPacket.messageClass, receivedPacket.messageID, true);
      
    // Poll NAV-PVT (Navigation position velocity time solution - manual polling mode)
    } else if((receivedPacket.messageClass == TGPSE_UBX_CLASS_NAV) &&
              (receivedPacket.messageID == TGPSE_UBX_NAV_PVT) &&
              (receivedPacket.payloadLength == 0)) {
      // Request NAV-PVT packet if not in autoNAVPVT mode
      if(emulatorSettings.autoNAVPVTRate!=0) {
        if(requestNAVPVTPacket) {
          lostNAVPVTRequestCount += (lostNAVPVTRequestCount < 99) ? 1 : 0;
        } else {
          requestNAVPVTPacket = true;
        }
      }
      // *** DON'T ACK UBX-NAV-PVT REQUESTS ***

    // Poll CFG-MSG (autoNAVSTATUSRate)
    } else if((ubxModuleType == TGPSE_UBX_M8_MODULE) &&
              (receivedPacket.messageClass == TGPSE_UBX_CLASS_CFG) &&
              (receivedPacket.messageID == TGPSE_UBX_CFG_MSG) &&
              (receivedPacket.payloadLength == 2) &&
              (receivedPacket.payload[0] == TGPSE_UBX_CLASS_NAV) &&
              (receivedPacket.payload[1] == TGPSE_UBX_NAV_STATUS)) {
      // Return autoNAVSTATUSRate setting
      responsePacket.messageClass = receivedPacket.messageClass;
      responsePacket.messageID = receivedPacket.messageID;
      responsePacket.payloadLength = 3;
      responsePacket.payload[0] = TGPSE_UBX_CLASS_NAV;
      responsePacket.payload[1] = TGPSE_UBX_NAV_STATUS;
      responsePacket.payload[2] = emulatorSettings.autoNAVSTATUSRate;
      calcChecksum(&responsePacket);
      responsePacket.validPacket = true;
      buildAcknowledgePacket(receivedPacket.messageClass, receivedPacket.messageID, true);

    // Set CFG-MSG (autoNAVSTATUSRate)
    } else if((ubxModuleType == TGPSE_UBX_M8_MODULE) &&
              (receivedPacket.messageClass == TGPSE_UBX_CLASS_CFG) &&
              (receivedPacket.messageID == TGPSE_UBX_CFG_MSG) &&
              (receivedPacket.payloadLength == 3) &&
              (receivedPacket.payload[0] == TGPSE_UBX_CLASS_NAV) &&
              (receivedPacket.payload[1] == TGPSE_UBX_NAV_STATUS)) {
      emulatorSettings.autoNAVSTATUSRate = receivedPacket.payload[2];
      buildAcknowledgePacket(receivedPacket.messageClass, receivedPacket.messageID, true);
      
    // Poll NAV-STATUS (Navigation status - manual polling mode)
    } else if((receivedPacket.messageClass == TGPSE_UBX_CLASS_NAV) &&
              (receivedPacket.messageID == TGPSE_UBX_NAV_STATUS) &&
              (receivedPacket.payloadLength == 0)) {
      // Request NAV-STATUS packet if not in autoNAVSTATUS mode
      if(emulatorSettings.autoNAVSTATUSRate!=0) {
        if(requestNAVSTATUSPacket) {
          lostNAVSTATUSRequestCount += (lostNAVSTATUSRequestCount < 99) ? 1 : 0;
        } else {
          requestNAVSTATUSPacket = true;
        }
      }
      // *** DON'T ACK UBX-NAV-STATUS REQUESTS ***

    // Poll CFG-MSG (autoNAVSATRate)
    } else if((ubxModuleType == TGPSE_UBX_M8_MODULE) &&
              (receivedPacket.messageClass == TGPSE_UBX_CLASS_CFG) &&
              (receivedPacket.messageID == TGPSE_UBX_CFG_MSG) &&
              (receivedPacket.payloadLength == 2) &&
              (receivedPacket.payload[0] == TGPSE_UBX_CLASS_NAV) &&
              (receivedPacket.payload[1] == TGPSE_UBX_NAV_SAT)) {
      // Return autoNAVSATRate setting
      responsePacket.messageClass = receivedPacket.messageClass;
      responsePacket.messageID = receivedPacket.messageID;
      responsePacket.payloadLength = 3;
      responsePacket.payload[0] = TGPSE_UBX_CLASS_NAV;
      responsePacket.payload[1] = TGPSE_UBX_NAV_SAT;
      responsePacket.payload[2] = emulatorSettings.autoNAVSATRate;
      calcChecksum(&responsePacket);
      responsePacket.validPacket = true;
      buildAcknowledgePacket(receivedPacket.messageClass, receivedPacket.messageID, true);

    // Set CFG-MSG (autoNAVSATRate)
    } else if((ubxModuleType == TGPSE_UBX_M8_MODULE) &&
              (receivedPacket.messageClass == TGPSE_UBX_CLASS_CFG) &&
              (receivedPacket.messageID == TGPSE_UBX_CFG_MSG) &&
              (receivedPacket.payloadLength == 3) &&
              (receivedPacket.payload[0] == TGPSE_UBX_CLASS_NAV) &&
              (receivedPacket.payload[1] == TGPSE_UBX_NAV_SAT)) {
      emulatorSettings.autoNAVSATRate = receivedPacket.payload[2];
      buildAcknowledgePacket(receivedPacket.messageClass, receivedPacket.messageID, true);

    // Poll NAV-SAT (Satellite tracking info - manual polling mode)
    } else if((receivedPacket.messageClass == TGPSE_UBX_CLASS_NAV) &&
              (receivedPacket.messageID == TGPSE_UBX_NAV_SAT) &&
              (receivedPacket.payloadLength == 0)) {
      // Request NAV-SAT packet if not in autoNAVSAT mode
      if(emulatorSettings.autoNAVSATRate!=0) {
        if(requestNAVSATPacket) {
          lostNAVSATRequestCount += (lostNAVSATRequestCount < 99) ? 1 : 0;
        } else {
          requestNAVSATPacket = true;
        }
      }
      // *** DON'T ACK UBX-NAV-SAT REQUESTS ***

    // Poll MON-VER (receiver and software versions)
    } else if((receivedPacket.messageClass == TGPSE_UBX_CLASS_MON) &&
              (receivedPacket.messageID == TGPSE_UBX_MON_VER) &&
              (receivedPacket.payloadLength == 0)) {
      // Return receiver and software versions
      responsePacket.messageClass = receivedPacket.messageClass;
      responsePacket.messageID = receivedPacket.messageID;
      responsePacket.payloadLength = TGPSE_UBX_MON_VER_PAYLOADLENGTH;
      if(ubxModuleType == TGPSE_UBX_M8_MODULE) {
        memcpy(responsePacket.payload, TGPSE_UBX_M8_MON_VER_PAYLOAD, TGPSE_UBX_MON_VER_PAYLOADLENGTH);
      } else {
        memcpy(responsePacket.payload, TGPSE_UBX_M10_MON_VER_PAYLOAD, TGPSE_UBX_MON_VER_PAYLOADLENGTH);
      }
      calcChecksum(&responsePacket);
      responsePacket.validPacket = true;
      // *** DON'T ACK UBX-MON-VER REQUESTS ***

    // Poll CFG-VALGET (UART1Port UART1 Enabled)
    } else if((ubxModuleType == TGPSE_UBX_M10_MODULE) &&
              (receivedPacket.messageClass == TGPSE_UBX_CLASS_CFG) &&
              (receivedPacket.messageID == TGPSE_UBX_CFG_VALGET) &&
              (receivedPacket.payloadLength == 8) &&
              (receivedPacket.payload[4] == (TGPSE_UBLOX_CFG_UART1_ENABLED & 0xFF)) &&
              (receivedPacket.payload[5] == ((TGPSE_UBLOX_CFG_UART1_ENABLED >> 8) & 0xFF)) &&
              (receivedPacket.payload[6] == ((TGPSE_UBLOX_CFG_UART1_ENABLED >> 16) & 0xFF)) &&
              (receivedPacket.payload[7] == ((TGPSE_UBLOX_CFG_UART1_ENABLED >> 24) & 0xFF))) {
      // Return configuration for COM_PORT_UART1
      responsePacket = receivedPacket;
      responsePacket.payloadLength = 9;
      responsePacket.payload[8] = true;
      calcChecksum(&responsePacket);
      responsePacket.validPacket = true;
      buildAcknowledgePacket(receivedPacket.messageClass, receivedPacket.messageID, true);

    // Set CFG-VALSET (UART1Port UBX Output Enabled)
    } else if((ubxModuleType == TGPSE_UBX_M10_MODULE) &&
              (receivedPacket.messageClass == TGPSE_UBX_CLASS_CFG) &&
              (receivedPacket.messageID == TGPSE_UBX_CFG_VALSET) &&
              (receivedPacket.payloadLength == 9) &&
              (receivedPacket.payload[4] == (TGPSE_UBLOX_CFG_UART1OUTPROT_UBX & 0xFF)) &&
              (receivedPacket.payload[5] == ((TGPSE_UBLOX_CFG_UART1OUTPROT_UBX >> 8) & 0xFF)) &&
              (receivedPacket.payload[6] == ((TGPSE_UBLOX_CFG_UART1OUTPROT_UBX >> 16) & 0xFF)) &&
              (receivedPacket.payload[7] == ((TGPSE_UBLOX_CFG_UART1OUTPROT_UBX >> 24) & 0xFF))) {
      emulatorSettings.outputUBX = (receivedPacket.payload[8] == 0) ? false : true;
      buildAcknowledgePacket(receivedPacket.messageClass, receivedPacket.messageID, true);

    // Set CFG-VALSET (UART1Port NMEA Output Enabled - ignored but still have to ACK)
    } else if((ubxModuleType == TGPSE_UBX_M10_MODULE) &&
              (receivedPacket.messageClass == TGPSE_UBX_CLASS_CFG) &&
              (receivedPacket.messageID == TGPSE_UBX_CFG_VALSET) &&
              (receivedPacket.payloadLength == 9) &&
              (receivedPacket.payload[4] == (TGPSE_UBLOX_CFG_UART1OUTPROT_NMEA & 0xFF)) &&
              (receivedPacket.payload[5] == ((TGPSE_UBLOX_CFG_UART1OUTPROT_NMEA >> 8) & 0xFF)) &&
              (receivedPacket.payload[6] == ((TGPSE_UBLOX_CFG_UART1OUTPROT_NMEA >> 16) & 0xFF)) &&
              (receivedPacket.payload[7] == ((TGPSE_UBLOX_CFG_UART1OUTPROT_NMEA >> 24) & 0xFF))) {
      buildAcknowledgePacket(receivedPacket.messageClass, receivedPacket.messageID, true);

    // Set CFG-VALSET (UART1Port Baudrate)
    } else if((ubxModuleType == TGPSE_UBX_M10_MODULE) &&
              (receivedPacket.messageClass == TGPSE_UBX_CLASS_CFG) &&
              (receivedPacket.messageID == TGPSE_UBX_CFG_VALSET) &&
              (receivedPacket.payloadLength == 12) &&
              (receivedPacket.payload[4] == (TGPSE_UBLOX_CFG_UART1_BAUDRATE & 0xFF)) &&
              (receivedPacket.payload[5] == ((TGPSE_UBLOX_CFG_UART1_BAUDRATE >> 8) & 0xFF)) &&
              (receivedPacket.payload[6] == ((TGPSE_UBLOX_CFG_UART1_BAUDRATE >> 16) & 0xFF)) &&
              (receivedPacket.payload[7] == ((TGPSE_UBLOX_CFG_UART1_BAUDRATE >> 24) & 0xFF))) {
      // Update COM_PORT_UART1 baudrate and ACK if not changing baudRate
      requestedBaudRate = receivedPacket.payload[8];
      requestedBaudRate |= receivedPacket.payload[9] << 8;
      requestedBaudRate |= receivedPacket.payload[10] << 16;
      requestedBaudRate |= receivedPacket.payload[11] << 24;
      if(requestedBaudRate != emulatorSettings.baudRate) {
        if((requestedBaudRate == 9600) ||
           (requestedBaudRate == 38400) ||
           (requestedBaudRate == 115200)) {
          init(*serialPort, requestedBaudRate, ubxModuleType);
          receivedPacket.validPacket = false;
          return;
        } else {
          buildAcknowledgePacket(receivedPacket.messageClass, receivedPacket.messageID, false);
        }
      }

    // Set CFG-VALSET (measurementRate)
    } else if((ubxModuleType == TGPSE_UBX_M10_MODULE) &&
              (receivedPacket.messageClass == TGPSE_UBX_CLASS_CFG) &&
              (receivedPacket.messageID == TGPSE_UBX_CFG_VALSET) &&
              (receivedPacket.payloadLength == 10) &&
              (receivedPacket.payload[4] == (TGPSE_UBLOX_CFG_RATE_MEAS & 0xFF)) &&
              (receivedPacket.payload[5] == ((TGPSE_UBLOX_CFG_RATE_MEAS >> 8) & 0xFF)) &&
              (receivedPacket.payload[6] == ((TGPSE_UBLOX_CFG_RATE_MEAS >> 16) & 0xFF)) &&
              (receivedPacket.payload[7] == ((TGPSE_UBLOX_CFG_RATE_MEAS >> 24) & 0xFF))) {
      // Update measurementRate and navigationRate
      emulatorSettings.measurementRate = receivedPacket.payload[8];
      emulatorSettings.measurementRate |= receivedPacket.payload[9] << 8;
      buildAcknowledgePacket(receivedPacket.messageClass, receivedPacket.messageID, true);

    // Set CFG-VALSET (navigationRate)
    } else if((ubxModuleType == TGPSE_UBX_M10_MODULE) &&
              (receivedPacket.messageClass == TGPSE_UBX_CLASS_CFG) &&
              (receivedPacket.messageID == TGPSE_UBX_CFG_VALSET) &&
              (receivedPacket.payloadLength == 10) &&
              (receivedPacket.payload[4] == (TGPSE_UBLOX_CFG_RATE_NAV & 0xFF)) &&
              (receivedPacket.payload[5] == ((TGPSE_UBLOX_CFG_RATE_NAV >> 8) & 0xFF)) &&
              (receivedPacket.payload[6] == ((TGPSE_UBLOX_CFG_RATE_NAV >> 16) & 0xFF)) &&
              (receivedPacket.payload[7] == ((TGPSE_UBLOX_CFG_RATE_NAV >> 24) & 0xFF))) {
      // Update measurementRate and navigationRate
      emulatorSettings.navigationRate = receivedPacket.payload[8];
      emulatorSettings.navigationRate |= receivedPacket.payload[9] << 8;
      buildAcknowledgePacket(receivedPacket.messageClass, receivedPacket.messageID, true);

    // Set CFG-VALSET (autoNAVPVTRate)
    } else if((ubxModuleType == TGPSE_UBX_M10_MODULE) &&
              (receivedPacket.messageClass == TGPSE_UBX_CLASS_CFG) &&
              (receivedPacket.messageID == TGPSE_UBX_CFG_VALSET) &&
              (receivedPacket.payloadLength == 9) &&
              (receivedPacket.payload[4] == (TGPSE_UBLOX_CFG_MSGOUT_UBX_NAV_PVT_UART1 & 0xFF)) &&
              (receivedPacket.payload[5] == ((TGPSE_UBLOX_CFG_MSGOUT_UBX_NAV_PVT_UART1 >> 8) & 0xFF)) &&
              (receivedPacket.payload[6] == ((TGPSE_UBLOX_CFG_MSGOUT_UBX_NAV_PVT_UART1 >> 16) & 0xFF)) &&
              (receivedPacket.payload[7] == ((TGPSE_UBLOX_CFG_MSGOUT_UBX_NAV_PVT_UART1 >> 24) & 0xFF))) {
      // Update autoNAVPVTRate
      emulatorSettings.autoNAVPVTRate = receivedPacket.payload[8];
      buildAcknowledgePacket(receivedPacket.messageClass, receivedPacket.messageID, true);

    // Set CFG-VALSET (autoNAVSTATUSRate)
    } else if((ubxModuleType == TGPSE_UBX_M10_MODULE) &&
              (receivedPacket.messageClass == TGPSE_UBX_CLASS_CFG) &&
              (receivedPacket.messageID == TGPSE_UBX_CFG_VALSET) &&
              (receivedPacket.payloadLength == 9) &&
              (receivedPacket.payload[4] == (TGPSE_UBLOX_CFG_MSGOUT_UBX_NAV_STATUS_UART1 & 0xFF)) &&
              (receivedPacket.payload[5] == ((TGPSE_UBLOX_CFG_MSGOUT_UBX_NAV_STATUS_UART1 >> 8) & 0xFF)) &&
              (receivedPacket.payload[6] == ((TGPSE_UBLOX_CFG_MSGOUT_UBX_NAV_STATUS_UART1 >> 16) & 0xFF)) &&
              (receivedPacket.payload[7] == ((TGPSE_UBLOX_CFG_MSGOUT_UBX_NAV_STATUS_UART1 >> 24) & 0xFF))) {
      // Update autoNAVSTATUSRate
      emulatorSettings.autoNAVSTATUSRate = receivedPacket.payload[8];
      buildAcknowledgePacket(receivedPacket.messageClass, receivedPacket.messageID, true);

    // Set CFG-VALSET (autoNAVSATRate)
    } else if((ubxModuleType == TGPSE_UBX_M10_MODULE) &&
              (receivedPacket.messageClass == TGPSE_UBX_CLASS_CFG) &&
              (receivedPacket.messageID == TGPSE_UBX_CFG_VALSET) &&
              (receivedPacket.payloadLength == 9) &&
              (receivedPacket.payload[4] == (TGPSE_UBLOX_CFG_MSGOUT_UBX_NAV_SAT_UART1 & 0xFF)) &&
              (receivedPacket.payload[5] == ((TGPSE_UBLOX_CFG_MSGOUT_UBX_NAV_SAT_UART1 >> 8) & 0xFF)) &&
              (receivedPacket.payload[6] == ((TGPSE_UBLOX_CFG_MSGOUT_UBX_NAV_SAT_UART1 >> 16) & 0xFF)) &&
              (receivedPacket.payload[7] == ((TGPSE_UBLOX_CFG_MSGOUT_UBX_NAV_SAT_UART1 >> 24) & 0xFF))) {
      // Update autoNAVSATRate
      emulatorSettings.autoNAVSATRate = receivedPacket.payload[8];
      buildAcknowledgePacket(receivedPacket.messageClass, receivedPacket.messageID, true);

    // NAK anything else (because it is not supported/needed)
    } else {
      unknownPacket = receivedPacket;
      buildAcknowledgePacket(receivedPacket.messageClass, receivedPacket.messageID, false);
    }

    receivedPacket.validPacket = false;
    sendPackets();
  }
}

/********************************************************************/
uint8_t TeenyGPSEmulate::getLostRxPacketCount() {
  return lostRxPacketCount;
}

/********************************************************************/
void TeenyGPSEmulate::buildAcknowledgePacket(uint8_t messageClass, uint8_t messageID, bool ack) {
  acknowledgePacket.messageClass = TGPSE_UBX_CLASS_ACK;
  acknowledgePacket.messageID = ack ? TGPSE_UBX_ACK_ACK : TGPSE_UBX_ACK_NAK;
  acknowledgePacket.payloadLength = 2;
  acknowledgePacket.payload[0] = messageClass;
  acknowledgePacket.payload[1] = messageID;
  calcChecksum(&acknowledgePacket);
  acknowledgePacket.validPacket = true;
}

/********************************************************************/
void TeenyGPSEmulate::sendPackets() {
  if(responsePacket.validPacket) {
    sendPacket(&responsePacket);
    responsePacket.validPacket = false;
  }
  if(acknowledgePacket.validPacket) {
    sendPacket(&acknowledgePacket);
    acknowledgePacket.validPacket = false;
  }
}

/********************************************************************/
void TeenyGPSEmulate::sendPacket(ubxPacket_t *pkt) {
  serialPort->write(pkt->synch1);
  serialPort->write(pkt->synch2);
  serialPort->write(pkt->messageClass);
  serialPort->write(pkt->messageID);
  serialPort->write(pkt->payloadLength & 0xFF);
  serialPort->write(pkt->payloadLength >> 8);
  serialPort->write(pkt->payload, pkt->payloadLength);
  serialPort->write(pkt->checksumA);
  serialPort->write(pkt->checksumB);
}

/********************************************************************/
/********************************************************************/
// Methods for accessing emulator state
/********************************************************************/
/********************************************************************/
uint32_t TeenyGPSEmulate::getBaudRate() {
  return emulatorSettings.baudRate;
}

/********************************************************************/
bool TeenyGPSEmulate::getOutputUBX() {
  return emulatorSettings.outputUBX;
}

/********************************************************************/
uint16_t TeenyGPSEmulate::getMeasurementRate() {
  return emulatorSettings.measurementRate;
}

/********************************************************************/
uint16_t TeenyGPSEmulate::getNavigationRate() {
  return emulatorSettings.navigationRate;
}

/********************************************************************/
uint8_t TeenyGPSEmulate::getAutoNAVPVTRate() {
  return emulatorSettings.autoNAVPVTRate;
}

/********************************************************************/
uint8_t TeenyGPSEmulate::getAutoNAVSTATUSRate() {
  return emulatorSettings.autoNAVSTATUSRate;
}

/********************************************************************/
uint8_t TeenyGPSEmulate::getAutoNAVSATRate() {
  return emulatorSettings.autoNAVSATRate;
}

/********************************************************************/
/********************************************************************/
// Methods for manual and automatic UBX NAVPVT transmission
/********************************************************************/
/********************************************************************/
uint32_t TeenyGPSEmulate::getNAVPVTTransmissionRate() {
  if(emulatorSettings.autoNAVPVTRate == 0) {
    return 0;
  }
  return ((uint32_t)emulatorSettings.measurementRate *
                    emulatorSettings.navigationRate) / emulatorSettings.autoNAVPVTRate;
}

/********************************************************************/
bool TeenyGPSEmulate::isNAVPVTPacketRequested() {
  if(requestNAVPVTPacket) {
    requestNAVPVTPacket = false;
    return true;
  }
  return false;
}

/********************************************************************/
uint8_t TeenyGPSEmulate::getLostNAVPVTRequestCount() {
  return lostNAVPVTRequestCount;
}

/********************************************************************/
void TeenyGPSEmulate::setAutoNAVPVTRate(uint8_t rate) {
  emulatorSettings.autoNAVPVTRate = rate;
}

/********************************************************************/
bool TeenyGPSEmulate::isAutoNAVPVTEnabled() {
  return emulatorSettings.autoNAVPVTRate!=0;
}

/********************************************************************/
bool TeenyGPSEmulate::isNAVPVTPacket() {
  if((ubxLoopPacket.synch1 == TGPSE_UBX_SYNCH_1) &&
     (ubxLoopPacket.synch2 == TGPSE_UBX_SYNCH_2) &&
     (ubxLoopPacket.messageClass == TGPSE_UBX_CLASS_NAV) &&
     (ubxLoopPacket.messageID == TGPSE_UBX_NAV_PVT) &&
     (ubxLoopPacket.payloadLength == TGPSE_UBX_NAV_PVT_PAYLOADLENGTH)) {
    return true;
  }
  return false;
}

/********************************************************************/
bool TeenyGPSEmulate::setNAVPVTPacket() {
  if(isNAVPVTPacket()) {
    memcpy(ubxNAVPVTPacket.payload, ubxLoopPacket.payload, ubxLoopPacket.payloadLength);
    return true;
  }
  return false;
}

/********************************************************************/
void TeenyGPSEmulate::setNAVPVTColdPacket() {
  memcpy(&ubxNAVPVTPacket.payload, TGPSE_UBX_NAV_PVT_COLD_PAYLOAD, TGPSE_UBX_NAV_PVT_PAYLOADLENGTH);
}

/********************************************************************/
ubxNAVPVTInfo_t TeenyGPSEmulate::getNAVPVTPacketInfo() {
  ubxNAVPVTInfo.year          =  ubxNAVPVTPacket.payload[4];
  ubxNAVPVTInfo.year          |= ubxNAVPVTPacket.payload[5] << 8;
  ubxNAVPVTInfo.month         =  ubxNAVPVTPacket.payload[6];
  ubxNAVPVTInfo.day           =  ubxNAVPVTPacket.payload[7];
  ubxNAVPVTInfo.hour          =  ubxNAVPVTPacket.payload[8];
  ubxNAVPVTInfo.minute        =  ubxNAVPVTPacket.payload[9];
  ubxNAVPVTInfo.second        =  ubxNAVPVTPacket.payload[10];
  ubxNAVPVTInfo.dateValid     =  ubxNAVPVTPacket.payload[11] & 0x01;
  ubxNAVPVTInfo.timeValid     =  ubxNAVPVTPacket.payload[11] & 0x02;
  ubxNAVPVTInfo.tAcc          =  ubxNAVPVTPacket.payload[12];
  ubxNAVPVTInfo.tAcc          |= ubxNAVPVTPacket.payload[13] << 8;
  ubxNAVPVTInfo.tAcc          |= ubxNAVPVTPacket.payload[14] << 16;
  ubxNAVPVTInfo.tAcc          |= ubxNAVPVTPacket.payload[15] << 24;
  ubxNAVPVTInfo.fixType       =  ubxNAVPVTPacket.payload[20];
  ubxNAVPVTInfo.locationValid =  ubxNAVPVTPacket.payload[21] & 0x01;
  ubxNAVPVTInfo.numSV         =  ubxNAVPVTPacket.payload[23];
  ubxNAVPVTInfo.longitude     =  ubxNAVPVTPacket.payload[24];
  ubxNAVPVTInfo.longitude     |= ubxNAVPVTPacket.payload[25] << 8;
  ubxNAVPVTInfo.longitude     |= ubxNAVPVTPacket.payload[26] << 16;
  ubxNAVPVTInfo.longitude     |= ubxNAVPVTPacket.payload[27] << 24;
  ubxNAVPVTInfo.latitude      =  ubxNAVPVTPacket.payload[28];
  ubxNAVPVTInfo.latitude      |= ubxNAVPVTPacket.payload[29] << 8;
  ubxNAVPVTInfo.latitude      |= ubxNAVPVTPacket.payload[30] << 16;
  ubxNAVPVTInfo.latitude      |= ubxNAVPVTPacket.payload[31] << 24;
  ubxNAVPVTInfo.altitude      =  ubxNAVPVTPacket.payload[32];
  ubxNAVPVTInfo.altitude      |= ubxNAVPVTPacket.payload[33] << 8;
  ubxNAVPVTInfo.altitude      |= ubxNAVPVTPacket.payload[34] << 16;
  ubxNAVPVTInfo.altitude      |= ubxNAVPVTPacket.payload[35] << 24;
  ubxNAVPVTInfo.altitudeMSL   =  ubxNAVPVTPacket.payload[36];
  ubxNAVPVTInfo.altitudeMSL   |= ubxNAVPVTPacket.payload[37] << 8;
  ubxNAVPVTInfo.altitudeMSL   |= ubxNAVPVTPacket.payload[38] << 16;
  ubxNAVPVTInfo.altitudeMSL   |= ubxNAVPVTPacket.payload[39] << 24;
  ubxNAVPVTInfo.hAcc          =  ubxNAVPVTPacket.payload[40];
  ubxNAVPVTInfo.hAcc          |= ubxNAVPVTPacket.payload[41] << 8;
  ubxNAVPVTInfo.hAcc          |= ubxNAVPVTPacket.payload[42] << 16;
  ubxNAVPVTInfo.hAcc          |= ubxNAVPVTPacket.payload[43] << 24;
  ubxNAVPVTInfo.vAcc          =  ubxNAVPVTPacket.payload[44];
  ubxNAVPVTInfo.vAcc          |= ubxNAVPVTPacket.payload[45] << 8;
  ubxNAVPVTInfo.vAcc          |= ubxNAVPVTPacket.payload[46] << 16;
  ubxNAVPVTInfo.vAcc          |= ubxNAVPVTPacket.payload[47] << 24;
  ubxNAVPVTInfo.headMot       =  ubxNAVPVTPacket.payload[64];
  ubxNAVPVTInfo.headMot       |= ubxNAVPVTPacket.payload[65] << 8;
  ubxNAVPVTInfo.headMot       |= ubxNAVPVTPacket.payload[66] << 16;
  ubxNAVPVTInfo.headMot       |= ubxNAVPVTPacket.payload[67] << 24;
  ubxNAVPVTInfo.pDOP          =  ubxNAVPVTPacket.payload[76];
  ubxNAVPVTInfo.pDOP          |= ubxNAVPVTPacket.payload[77] << 8;
  return ubxNAVPVTInfo;
}

/********************************************************************/
void TeenyGPSEmulate::setNAVPVTPacketDateTime(uint16_t year, uint8_t month, uint8_t day,
                                              uint8_t hour, uint8_t minute, uint8_t second) {
  ubxNAVPVTPacket.payload[4]  = year & 0xFF;
  ubxNAVPVTPacket.payload[5]  = year >> 8;
  ubxNAVPVTPacket.payload[6]  = month;
  ubxNAVPVTPacket.payload[7]  = day;
  ubxNAVPVTPacket.payload[8]  = hour;
  ubxNAVPVTPacket.payload[9]  = minute;
  ubxNAVPVTPacket.payload[10] = second;
}

/********************************************************************/
void TeenyGPSEmulate::unsetNAVPVTPacketDateValidFlag() {
  ubxNAVPVTPacket.payload[11] &= ~0x01;
  // Can't have time and location valid without valid date
  unsetNAVPVTPacketTimeValidFlag();
}
/********************************************************************/
void TeenyGPSEmulate::unsetNAVPVTPacketTimeValidFlag() {
  ubxNAVPVTPacket.payload[11] &= ~0x02;
  // Can't have location valid without valid time
  unsetNAVPVTPacketLocationValidFlag();
}
/********************************************************************/
void TeenyGPSEmulate::unsetNAVPVTPacketLocationValidFlag() {
  ubxNAVPVTPacket.payload[21] &= ~0x01;
}

/********************************************************************/
bool TeenyGPSEmulate::sendNAVPVTPacket() {
  calcChecksum(&ubxNAVPVTPacket);
  serialPort->write((uint8_t*)&ubxNAVPVTPacket, sizeof(ubxNAVPVTPacket));
  return true;
}

/********************************************************************/
/********************************************************************/
// Methods for manual and automatic UBX NAVSTATUS transmission
/********************************************************************/
/********************************************************************/
uint32_t TeenyGPSEmulate::getNAVSTATUSTransmissionRate() {
  if(emulatorSettings.autoNAVSTATUSRate == 0) {
    return 0;
  }
  return ((uint32_t)emulatorSettings.measurementRate *
                    emulatorSettings.navigationRate) / emulatorSettings.autoNAVSTATUSRate;
}

/********************************************************************/
bool TeenyGPSEmulate::isNAVSTATUSPacketRequested() {
  if(requestNAVSTATUSPacket) {
    requestNAVSTATUSPacket = false;
    return true;
  }
  return false;
}

/********************************************************************/
uint8_t TeenyGPSEmulate::getLostNAVSTATUSRequestCount() {
  return lostNAVSTATUSRequestCount;
}

/********************************************************************/
void TeenyGPSEmulate::setAutoNAVSTATUSRate(uint8_t rate) {
  emulatorSettings.autoNAVSTATUSRate = rate;
}

/********************************************************************/
bool TeenyGPSEmulate::isAutoNAVSTATUSEnabled() {
  return emulatorSettings.autoNAVSTATUSRate!=0;
}

/********************************************************************/
bool TeenyGPSEmulate::isNAVSTATUSPacket() {
  if((ubxLoopPacket.synch1 == TGPSE_UBX_SYNCH_1) &&
     (ubxLoopPacket.synch2 == TGPSE_UBX_SYNCH_2) &&
     (ubxLoopPacket.messageClass == TGPSE_UBX_CLASS_NAV) &&
     (ubxLoopPacket.messageID == TGPSE_UBX_NAV_STATUS) &&
     (ubxLoopPacket.payloadLength == TGPSE_UBX_NAV_STATUS_PAYLOADLENGTH)) {
    return true;
  }
  return false;
}

/********************************************************************/
bool TeenyGPSEmulate::setNAVSTATUSPacket() {
  if(isNAVSTATUSPacket()) {
    memcpy(ubxNAVSTATUSPacket.payload, ubxLoopPacket.payload, ubxLoopPacket.payloadLength);
    return true;
  }
  return false;
}

/********************************************************************/
void TeenyGPSEmulate::setNAVSTATUSColdPacket() {
  memcpy(&ubxNAVSTATUSPacket.payload, TGPSE_UBX_NAV_STATUS_COLD_PAYLOAD, TGPSE_UBX_NAV_STATUS_PAYLOADLENGTH);
}

/********************************************************************/
ubxNAVSTATUSInfo_t TeenyGPSEmulate::getNAVSTATUSPacketInfo() {
  ubxNAVSTATUSInfo.gpsFix        = ubxNAVSTATUSPacket.payload[4];
  ubxNAVSTATUSInfo.gpsFixOk      = ubxNAVSTATUSPacket.payload[5] & 0x01;
  ubxNAVSTATUSInfo.psmState      = ubxNAVSTATUSPacket.payload[7] & 0x03;
  ubxNAVSTATUSInfo.spoofDetState = (ubxNAVSTATUSPacket.payload[7] & 0x18) >> 3;
  return ubxNAVSTATUSInfo;
}

/********************************************************************/
bool TeenyGPSEmulate::sendNAVSTATUSPacket() {
  calcChecksum(&ubxNAVSTATUSPacket);
  serialPort->write((uint8_t*)&ubxNAVSTATUSPacket, sizeof(ubxNAVSTATUSPacket));
  return true;
}

/********************************************************************/
/********************************************************************/
// Methods for manual and automatic UBX NAVSAT transmission
/********************************************************************/
/********************************************************************/
uint32_t TeenyGPSEmulate::getNAVSATTransmissionRate() {
  if(emulatorSettings.autoNAVSATRate == 0) {
    return 0;
  }
  return ((uint32_t)emulatorSettings.measurementRate *
                    emulatorSettings.navigationRate) / emulatorSettings.autoNAVSATRate;
}

/********************************************************************/
bool TeenyGPSEmulate::isNAVSATPacketRequested() {
  if(requestNAVSATPacket) {
    requestNAVSATPacket = false;
    return true;
  }
  return false;
}

/********************************************************************/
uint8_t TeenyGPSEmulate::getLostNAVSATRequestCount() {
  return lostNAVSATRequestCount;
}

/********************************************************************/
void TeenyGPSEmulate::setAutoNAVSATRate(uint8_t rate) {
  emulatorSettings.autoNAVSATRate = rate;
}

/********************************************************************/
bool TeenyGPSEmulate::isAutoNAVSATEnabled() {
  return emulatorSettings.autoNAVSATRate!=0;
}

/********************************************************************/
bool TeenyGPSEmulate::isNAVSATPacket() {
  if((ubxLoopPacket.synch1 == TGPSE_UBX_SYNCH_1) &&
     (ubxLoopPacket.synch2 == TGPSE_UBX_SYNCH_2) &&
     (ubxLoopPacket.messageClass == TGPSE_UBX_CLASS_NAV) &&
     (ubxLoopPacket.messageID == TGPSE_UBX_NAV_SAT) &&
     (ubxLoopPacket.payloadLength >= TGPSE_UBX_NAV_SAT_MINPAYLOADLENGTH) &&
     (ubxLoopPacket.payloadLength <= TGPSE_UBX_NAV_SAT_MAXPAYLOADLENGTH)) {
    return true;
  }
  return false;
}

/********************************************************************/
bool TeenyGPSEmulate::setNAVSATPacket() {
  if(isNAVSATPacket()) {
    ubxNAVSATPacket.payloadLength = ubxLoopPacket.payloadLength;
    memcpy(ubxNAVSATPacket.payload, ubxLoopPacket.payload, ubxLoopPacket.payloadLength);
    return true;
  }
  return false;
}

/********************************************************************/
void TeenyGPSEmulate::setNAVSATColdPacket() {
  memcpy(&ubxNAVSATPacket.payload, TGPSE_UBX_NAV_SAT_COLD_PAYLOAD, TGPSE_UBX_NAV_SAT_MINPAYLOADLENGTH);
  ubxNAVSATPacket.payloadLength = TGPSE_UBX_NAV_SAT_MINPAYLOADLENGTH;
}

/********************************************************************/
ubxNAVSATInfo_t TeenyGPSEmulate::getNAVSATPacketInfo() {
  ubxNAVSATInfo.numSvs = ubxNAVSATPacket.payload[5];
  return ubxNAVSATInfo;
}

/********************************************************************/
bool TeenyGPSEmulate::sendNAVSATPacket() {
  calcChecksum(&ubxNAVSATPacket);
  serialPort->write(ubxNAVSATPacket.synch1);
  serialPort->write(ubxNAVSATPacket.synch2);
  serialPort->write(ubxNAVSATPacket.messageClass);
  serialPort->write(ubxNAVSATPacket.messageID);
  serialPort->write(ubxNAVSATPacket.payloadLength & 0xFF);
  serialPort->write(ubxNAVSATPacket.payloadLength >> 8);
  serialPort->write(ubxNAVSATPacket.payload, ubxNAVSATPacket.payloadLength);
  serialPort->write(ubxNAVSATPacket.checksumA);
  serialPort->write(ubxNAVSATPacket.checksumB);
  return true;
}

/********************************************************************/
/********************************************************************/
// Methods for setting cold and emulation loop output packets
/********************************************************************/
/********************************************************************/
void TeenyGPSEmulate::setEmuColdOutputPackets() {
  setNAVPVTColdPacket();
  setNAVSTATUSColdPacket();
  setNAVSATColdPacket();
}

/********************************************************************/
bool TeenyGPSEmulate::setEmuLoopOutputPackets() {
  if(!processUBXLoopPacket()) return false;
  uint32_t _pktTimestamp = getUBXLoopPacketTimeStamp();
  // Check for up to three packets with same timestamp (NAVPVT, NAVSTATUS, NAVSAT)
  if(!assignUBXLoopOutputPacket()) return false;
  if(!processUBXLoopPacket()) return false;
  if((getUBXLoopPacketTimeStamp() == _pktTimestamp) && !assignUBXLoopOutputPacket()) return false;
  if(!processUBXLoopPacket()) return false;
  if((getUBXLoopPacketTimeStamp() == _pktTimestamp) && !assignUBXLoopOutputPacket()) return false;
  return true;
}

/********************************************************************/
bool TeenyGPSEmulate::assignUBXLoopOutputPacket() {
  uint16_t _pktLength = getUBXLoopPacketLength();
  // Assign NAVPVT packet?
  if(setNAVPVTPacket()) {
    ubxLoopPacket.validPacket = false;
    return true;
  }
  // Assign NAVSTATUS packet?
  if(setNAVSTATUSPacket()) {
    ubxLoopPacket.validPacket = false;
    return true;
  }
  // Assign NAVSAT packet?
  if(setNAVSATPacket()) {
    ubxLoopPacket.validPacket = false;
    return true;
  }
  ubxLoopPacket.validPacket = false; // purge unknown ubx packet
  return false;
}

/********************************************************************/
uint16_t TeenyGPSEmulate::getUBXLoopPacketLength() {
  return ubxLoopPacket.payloadLength + 8;
}

/********************************************************************/
uint32_t TeenyGPSEmulate::getUBXLoopPacketTimeStamp() {
  uint32_t _iTOW;
  _iTOW =  ubxLoopPacket.payload[0];
  _iTOW |= ubxLoopPacket.payload[1] << 8;
  _iTOW |= ubxLoopPacket.payload[2] << 16;
  _iTOW |= ubxLoopPacket.payload[3] << 24;
  return _iTOW;
}

/********************************************************************/
bool TeenyGPSEmulate::processUBXLoopPacket() {
  // Only update if packet has been processed
  if(ubxLoopPacket.validPacket) return true;
  // Process packet
  if(readUBXLoopByte() != TGPSE_UBX_SYNCH_1) return false;
  if(readUBXLoopByte() != TGPSE_UBX_SYNCH_2) return false;
  ubxLoopPacket.messageClass  =  readUBXLoopByte();
  ubxLoopPacket.messageID     =  readUBXLoopByte();
  ubxLoopPacket.payloadLength =  readUBXLoopByte();
  ubxLoopPacket.payloadLength |= readUBXLoopByte() << 8;
  if(ubxLoopPacket.payloadLength > TGPSE_UBX_MAXPAYLOADLENGTH) return false;
  for(uint16_t i = 0; i < ubxLoopPacket.payloadLength; i++) {
    ubxLoopPacket.payload[i] = readUBXLoopByte();
  }
  ubxLoopPacket.checksumA     =  readUBXLoopByte();
  ubxLoopPacket.checksumB     =  readUBXLoopByte();
  ubxLoopPacket.validPacket   =  true;
  return true;
}

/********************************************************************/
uint8_t TeenyGPSEmulate::readUBXLoopByte() {
  if(ubxFetch == nullptr) {
    if(ubxLoopPacketIndex >= sizeof(TGPSE_UBX_NAV_PVT_STATUS_SAT_LOOP_PACKETS)) {
      ubxLoopPacketIndex = 0;
    }
    return TGPSE_UBX_NAV_PVT_STATUS_SAT_LOOP_PACKETS[ubxLoopPacketIndex++];
  }
  return (ubxFetch)();
}
  
/********************************************************************/
/********************************************************************/
// checksum
/********************************************************************/
/********************************************************************/
// Given a message, calc and store the two byte "8-Bit Fletcher" checksum over the entirety of the message
// This is called before we send a command message
void TeenyGPSEmulate::calcChecksum(ubxPacket_t *pkt) {
  pkt->checksumA = 0;
  pkt->checksumB = 0;

  pkt->checksumA += pkt->messageClass;
  pkt->checksumB += pkt->checksumA;

  pkt->checksumA += pkt->messageID;
  pkt->checksumB += pkt->checksumA;

  pkt->checksumA += (pkt->payloadLength & 0xFF);
  pkt->checksumB += pkt->checksumA;

  pkt->checksumA += (pkt->payloadLength >> 8);
  pkt->checksumB += pkt->checksumA;

  for(uint16_t i = 0; i < pkt->payloadLength; i++) {
    pkt->checksumA += pkt->payload[i];
    pkt->checksumB += pkt->checksumA;
  }
}
/********************************************************************/
void TeenyGPSEmulate::calcChecksum(ubxNAVPVTPacket_t *pkt) {
  pkt->checksumA = 0;
  pkt->checksumB = 0;

  pkt->checksumA += pkt->messageClass;
  pkt->checksumB += pkt->checksumA;

  pkt->checksumA += pkt->messageID;
  pkt->checksumB += pkt->checksumA;

  pkt->checksumA += (pkt->payloadLength & 0xFF);
  pkt->checksumB += pkt->checksumA;

  pkt->checksumA += (pkt->payloadLength >> 8);
  pkt->checksumB += pkt->checksumA;

  for(uint16_t i = 0; i < pkt->payloadLength; i++) {
    pkt->checksumA += pkt->payload[i];
    pkt->checksumB += pkt->checksumA;
  }
}
/********************************************************************/
void TeenyGPSEmulate::calcChecksum(ubxNAVSTATUSPacket_t *pkt) {
  pkt->checksumA = 0;
  pkt->checksumB = 0;

  pkt->checksumA += pkt->messageClass;
  pkt->checksumB += pkt->checksumA;

  pkt->checksumA += pkt->messageID;
  pkt->checksumB += pkt->checksumA;

  pkt->checksumA += (pkt->payloadLength & 0xFF);
  pkt->checksumB += pkt->checksumA;

  pkt->checksumA += (pkt->payloadLength >> 8);
  pkt->checksumB += pkt->checksumA;

  for(uint16_t i = 0; i < pkt->payloadLength; i++) {
    pkt->checksumA += pkt->payload[i];
    pkt->checksumB += pkt->checksumA;
  }
}
/********************************************************************/
void TeenyGPSEmulate::calcChecksum(ubxNAVSATPacket_t *pkt) {
  pkt->checksumA = 0;
  pkt->checksumB = 0;

  pkt->checksumA += pkt->messageClass;
  pkt->checksumB += pkt->checksumA;

  pkt->checksumA += pkt->messageID;
  pkt->checksumB += pkt->checksumA;

  pkt->checksumA += (pkt->payloadLength & 0xFF);
  pkt->checksumB += pkt->checksumA;

  pkt->checksumA += (pkt->payloadLength >> 8);
  pkt->checksumB += pkt->checksumA;

  for(uint16_t i = 0; i < pkt->payloadLength; i++) {
    pkt->checksumA += pkt->payload[i];
    pkt->checksumB += pkt->checksumA;
  }
}

