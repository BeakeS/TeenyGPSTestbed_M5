
/********************************************************************/
// Timer
#include <Ticker.h>
#define HW_TIMER_INTERVAL_MS 10
Ticker ITimer0;
volatile uint8_t newClockSubTickCount = 0;
volatile bool newClockTick = false;

/********************************************************************/
// Timer Handler
void itimer0_handler() {
  newClockSubTickCount++;
  if(newClockSubTickCount>=100) {
    newClockSubTickCount = 0;
    newClockTick = true;
  }
  buttons_tick(); // update every 10ms
  if(gpsEnabled) {
    if(deviceState.DEVICE_MODE==DM_GPSRCVR) gps.gnss_checkUblox(); //update every 10ms
    if(deviceState.DEVICE_MODE==DM_GPSLOGR) gps.gnss_checkUblox(); //update every 10ms
    if(deviceState.DEVICE_MODE==DM_GPSSTAT) gps.gnss_checkUblox(); //update every 10ms
    if(deviceState.DEVICE_MODE==DM_GPSNSAT) gps.gnss_checkUblox(); //update every 10ms
  }
//if(deviceState.STATUSLED) statusLED.tick(); //update every 10ms
  switch(newClockSubTickCount%4) {
    case 0:
      if(emulatorEnabled) {
        if(deviceState.DEVICE_MODE==DM_GPSEMU_M8) emulator.tick(); //update every 40ms
        if(deviceState.DEVICE_MODE==DM_GPSEMU_M10) emulator.tick(); //update every 40ms
      }
      break;
  }
}

/********************************************************************/
void itimer0_setup() {
  ITimer0.attach_ms(HW_TIMER_INTERVAL_MS, itimer0_handler);
}

