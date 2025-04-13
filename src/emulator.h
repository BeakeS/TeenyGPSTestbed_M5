

/********************************************************************/
// TO-DO
//*** NEED TO INCORPORATE TRANSMISSION RATE INTO sendNAVPVTPacket()
//*** ALSO NEED TO FACTOR IN LOG RATE VS TRANSMISSION RATE
//uint32_t getNAVPVTTransmissionRate();
//uint32_t getNAVSATTransmissionRate();

/********************************************************************/
// forward declarations
bool sdcard_openUBXInputFile();
bool sdcard_readUBXInputFile(uint8_t* value);
void sdcard_closeUBXInputFile();

/********************************************************************/
// EMULATOR
bool emulatorEnabled;
#include "TeenyGPSEmulate.h"
TeenyGPSEmulate emulator;
uint8_t emuUbxPktSource;
uint8_t emulatorColdStartPacketCount;

/********************************************************************/
bool emulator_setup(HardwareSerial &serialPort, uint32_t baudRate,
                    tgpse_ubx_module_type_t ubxModuleType,
                    bool ubxPktLoopEnable, uint8_t emuUbxPktSource_) {
  emulatorEnabled = false;
  emuUbxPktSource = emuUbxPktSource_;
  emulatorColdStartPacketCount = 0;
  if(emuUbxPktSource == EMU_SDCINPUT) {
    if(sdcard_openUBXInputFile() &&
       emulator.init(serialPort, baudRate, ubxModuleType, ubxPktLoopEnable,
                     [](uint8_t* value) -> bool { return sdcard_readUBXInputFile(value); })) {
      emulatorEnabled = true;
    }
  } else {
    if(emulator.init(serialPort, baudRate, ubxModuleType, ubxPktLoopEnable)) {
      emulatorEnabled = true;
    }
  }
  return emulatorEnabled;
}

/********************************************************************/
bool emulator_update() {
  bool _displayRefresh = false;
  static bool _enqueueSendNAVPVTPkt = false;
  static bool _enqueueSendNAVSATPkt = false;
  static bool _enqueueSendNAVSTATUSPkt = false;

  // Update clock
  rtc_datetime_t _rtcTime = rtc.getRTCTime(); // get the RTC
  uint32_t _clockTime = (uint32_t)(_rtcTime.hour*3600) + (uint32_t)(_rtcTime.minute*60) + _rtcTime.second;
  static uint32_t _prevClockTime = 86400; // This is 24hr rollover seconds so it will never match _clockTime
  bool _clockTick_1sec = false;
  if(_prevClockTime != _clockTime) {
    _prevClockTime = _clockTime;
    _clockTick_1sec = true;
  }

  // Emulation
  if(emulatorEnabled) {
    // Process host commands
    emulator.processIncomingPacket();
    uint8_t _ubxNAVPVTBuf[100];
    ubxNAVPVTInfo_t _ubxNAVPVTInfo;
    // Wait for loop to be enabled by auto* command or packet request
    if(!emulator.isRunning()) return false;
    // Update loop event every second
    if(_clockTick_1sec) {
      if(emulatorColdStartPacketCount < deviceState.EMU_NUMCOLDSTARTPACKETS) {
        // Load optional cold start packets
        emulatorColdStartPacketCount++;
        emulator.setEmuColdOutputPackets(); // Sets cold NAVPVT, NAVSAT, and NAVSTATUS packets
        //statusLED.pulse(1);
      } else {
        // Load emulation packets
        emulator.setEmuLoopOutputPackets(); // Sets NAVPVT/NAVSAT/NAVSTATUS packets
        if(!rtc.isValid()) {
          // Set RTC using first packet date/time
          _ubxNAVPVTInfo = emulator.getNAVPVTPacketInfo();
          if(_ubxNAVPVTInfo.dateValid && _ubxNAVPVTInfo.timeValid) {
            rtc.setRTCTime(_ubxNAVPVTInfo.year, _ubxNAVPVTInfo.month, _ubxNAVPVTInfo.day,
                           _ubxNAVPVTInfo.hour, _ubxNAVPVTInfo.minute, _ubxNAVPVTInfo.second);
            _prevClockTime = (uint32_t)(_ubxNAVPVTInfo.hour*3600) +
                             (uint32_t)(_ubxNAVPVTInfo.minute*60) +
                             _ubxNAVPVTInfo.second;
          }
          //statusLED.pulse(_ubxNAVPVTInfo.locationValid ? 2 : 1);
        } else {
          // Set emulation packet date/time using RTC
          _ubxNAVPVTInfo = emulator.getNAVPVTPacketInfo();
          emulator.setNAVPVTPacketDateTime(_rtcTime.year, _rtcTime.month, _rtcTime.day,
                                           _rtcTime.hour, _rtcTime.minute, _rtcTime.second);
          //statusLED.pulse(_ubxNAVPVTInfo.locationValid ? 2 : 1);
        }
      }
      // Queue packets to be sent
      if((emulator.isAutoNAVPVTEnabled() || emulator.isNAVPVTPacketRequested()) &&
         emulator.isNAVPVTPacketValid()) {
        _enqueueSendNAVPVTPkt = true;
      }
      if((emulator.isAutoNAVSATEnabled() || emulator.isNAVSATPacketRequested()) &&
         emulator.isNAVSATPacketValid()) {
        _enqueueSendNAVSATPkt = true;
      }
      if((emulator.isAutoNAVSTATUSEnabled() || emulator.isNAVSTATUSPacketRequested()) &&
         emulator.isNAVSTATUSPacketValid()) {
        _enqueueSendNAVSTATUSPkt = true;
      }
    }
    // Send packets (one per pass to avoid serial buffer overflow)
    if(_enqueueSendNAVPVTPkt) {
      _enqueueSendNAVPVTPkt = false;
      emulator.sendNAVPVTPacket();
      _displayRefresh = true;
    } else if(_enqueueSendNAVSATPkt) {
      _enqueueSendNAVSATPkt = false;
      emulator.sendNAVSATPacket();
      _displayRefresh = true;
    } else if(_enqueueSendNAVSTATUSPkt) {
      _enqueueSendNAVSTATUSPkt = false;
      emulator.sendNAVSTATUSPacket();
      _displayRefresh = true;
    }
  }
  return _displayRefresh;
}

/********************************************************************/
void emulator_end() {
  if(emuUbxPktSource == EMU_SDCINPUT) {
    sdcard_closeUBXInputFile();
  }
  emulatorEnabled = false;
}

