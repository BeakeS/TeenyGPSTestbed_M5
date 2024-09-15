
/********************************************************************/
// forward declarations
bool sdcard_openUBXInputFile();
uint8_t sdcard_readUBXInputFile();
void sdcard_closeUBXInputFile();

/********************************************************************/
// EMULATOR UBX Packet Source
enum emuUbxPktSource_t : uint8_t {
  EMU_PGMINPUT = 0,
  EMU_SDCINPUT
};

/********************************************************************/
// EMULATOR
bool emulatorEnabled;
#include "TeenyGPSEmulate.h"
TeenyGPSEmulate emulator;
uint8_t emuUbxPktSource;
uint8_t emulatorColdStartPacketCount = 0;
bool emulatorLoopEnabled = false;

/********************************************************************/
bool emulator_setup(HardwareSerial &serialPort, uint32_t baudRate,
                    tgpse_ubx_module_type_t ubxModuleType, uint8_t emuUbxPktSource_) {
  emulatorEnabled = false;
  emuUbxPktSource = emuUbxPktSource_;
  emulatorColdStartPacketCount = 0;
  emulatorLoopEnabled = false;
  if(emuUbxPktSource == EMU_SDCINPUT) {
    if(sdcard_openUBXInputFile() &&
       emulator.init(serialPort, baudRate, ubxModuleType,
                     []() -> uint8_t { return sdcard_readUBXInputFile(); })) {
      emulatorEnabled = true;
    }
  } else {
    if(emulator.init(serialPort, baudRate, ubxModuleType)) {
      emulatorEnabled = true;
    }
  }
  return emulatorEnabled;
}

/********************************************************************/
void emulator_end() {
  if(emuUbxPktSource == EMU_SDCINPUT) {
    sdcard_closeUBXInputFile();
  }
}

