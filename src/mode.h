
/********************************************************************/
void deviceMode_init() {
  char _dispStr[22];
  switch(deviceState.DEVICE_MODE) {
    case DM_IDLE:
      //statusLED.pulse_repeat(1, 20);
      break;
    case DM_GPSRCVR:
      //statusLED.pulse_repeat(1);
      rtc.setValid(false);
      gpsSerial = &Serial2;
      if(gps.gnss_init(*gpsSerial, GPS_BAUD_RATE, deviceState.GPSRESET, 1, 0, 0)) {
        gpsEnabled = true;
        sprintf(_dispStr, "GPS CONN UBPV=%02d.%02d",
                gps.getProtocolVersionHigh(),
                gps.getProtocolVersionLow());
        msg_update(_dispStr);
        deviceState.GPSRESET = GPS_NORESET;
      } else {
        gpsEnabled = false;
        msg_update("ERROR - GPS Missing");
      }
      break;
    case DM_GPSLOGR:
      //statusLED.pulse_repeat(1);
      rtc.setValid(false);
      gpsSerial = &Serial2;
      if(gps.gnss_init(*gpsSerial, GPS_BAUD_RATE, deviceState.GPSRESET, 1, 1, 10)) {
        gpsEnabled = true;
        sprintf(_dispStr, "GPS CONN UBPV=%02d.%02d",
                gps.getProtocolVersionHigh(),
                gps.getProtocolVersionLow());
        msg_update(_dispStr);
        deviceState.GPSRESET = GPS_NORESET;
      } else {
        gpsEnabled = false;
        msg_update("ERROR - GPS Missing");
      }
      break;
    case DM_GPSSTAT:
      //statusLED.pulse_repeat(1);
      rtc.setValid(false);
      gpsSerial = &Serial2;
      if(gps.gnss_init(*gpsSerial, GPS_BAUD_RATE, deviceState.GPSRESET, 1, 1, 0)) {
        gpsEnabled = true;
        sprintf(_dispStr, "GPS CONN UBPV=%02d.%02d",
                gps.getProtocolVersionHigh(),
                gps.getProtocolVersionLow());
        msg_update(_dispStr);
        deviceState.GPSRESET = GPS_NORESET;
      } else {
        gpsEnabled = false;
        msg_update("ERROR - GPS Missing");
      }
      break;
    case DM_GPSNSAT:
      //statusLED.pulse_repeat(1);
      rtc.setValid(false);
      gpsSerial = &Serial2;
      if(gps.gnss_init(*gpsSerial, GPS_BAUD_RATE, deviceState.GPSRESET, 1, 0, 10)) {
        gpsEnabled = true;
        sprintf(_dispStr, "GPS CONN UBPV=%02d.%02d",
                gps.getProtocolVersionHigh(),
                gps.getProtocolVersionLow());
        msg_update(_dispStr);
        deviceState.GPSRESET = GPS_NORESET;
      } else {
        gpsEnabled = false;
        msg_update("ERROR - GPS Missing");
      }
      break;
    case DM_GPSSCFG:
      //statusLED.pulse_repeat(1);
      rtc.setValid(false);
      gpsSerial = &Serial2;
      if(gps.gnss_init(*gpsSerial, GPS_BAUD_RATE, deviceState.GPSRESET, 0, 0, 0)) {
        gpsEnabled = true;
        sprintf(_dispStr, "GPS CONN UBPV=%02d.%02d",
                gps.getProtocolVersionHigh(),
                gps.getProtocolVersionLow());
        msg_update(_dispStr);
        deviceState.GPSRESET = GPS_NORESET;
      } else {
        gpsEnabled = false;
        msg_update("ERROR - GPS Missing");
      }
      break;
    case DM_GPSSSTP:
      //statusLED.pulse_repeat(1);
      rtc.setValid(false);
      gpsSerial = &Serial2;
      gpsSerial->begin(GPS_BAUD_RATE);
      msg_update("GPS Serial Enabled");
      break;
    case DM_GPSEMU_M8:
      //statusLED.pulse_repeat(1);
      rtc.setValid(false);
      emulatorSerial = &Serial2;
      if(emulator_setup(*emulatorSerial, UBLOX_M8_EMULATOR_BAUD_RATE, TGPSE_UBX_M8_MODULE,
                        deviceState.EMUL_UBXPKTSOURCE)) {
        msg_update("EMU Serial Enabled");
      } else {
        msg_update("ERROR - EMU Setup");
      }
      break;
    case DM_GPSEMU_M10:
      //statusLED.pulse_repeat(1);
      rtc.setValid(false);
      emulatorSerial = &Serial2;
      if(emulator_setup(*emulatorSerial, UBLOX_M10_EMULATOR_BAUD_RATE, TGPSE_UBX_M10_MODULE,
                        deviceState.EMUL_UBXPKTSOURCE)) {
        msg_update("EMU Serial Enabled");
      } else {
        msg_update("ERROR - EMU Setup");
      }
      break;
  }
}

/********************************************************************/
void deviceMode_end() {
  switch(deviceState.DEVICE_MODE) {
    case DM_GPSRCVR:
      if(gpsEnabled) gps.gnss_init(*gpsSerial, GPS_BAUD_RATE, GPS_NORESET, 0, 0, 0);
      gpsEnabled = false;
      msg_update("GPS Receiver Stopped");
      break;
    case DM_GPSLOGR:
      if(gpsEnabled) gps.gnss_init(*gpsSerial, GPS_BAUD_RATE, GPS_NORESET, 0, 0, 0);
      gpsEnabled = false;
      msg_update("GPS Logger Stopped");
      break;
    case DM_GPSSTAT:
      if(gpsEnabled) gps.gnss_init(*gpsSerial, GPS_BAUD_RATE, GPS_NORESET, 0, 0, 0);
      gpsEnabled = false;
      msg_update("GPS NAVSTAT Stopped");
      break;
    case DM_GPSNSAT:
      if(gpsEnabled) gps.gnss_init(*gpsSerial, GPS_BAUD_RATE, GPS_NORESET, 0, 0, 0);
      gpsEnabled = false;
      msg_update("GPS NAVSAT Stopped");
      break;
    case DM_GPSSCFG:
      if(gpsEnabled) gps.gnss_init(*gpsSerial, GPS_BAUD_RATE, GPS_NORESET, 0, 0, 0);
      gpsEnabled = false;
      msg_update("GPS SATCFG Stopped");
      break;
    case DM_GPSSSTP:
      msg_update("Stepper Mode Stopped");
      break;
    case DM_GPSEMU_M8:
      emulator_end();
      msg_update("Emulator Stopped");
      break;
    case DM_GPSEMU_M10:
      emulator_end();
      msg_update("Emulator Stopped");
      break;
  }
  deviceState.DEVICE_MODE = DM_IDLE;
  //statusLED.pulse_repeat(1, 20);
  //rtc.setValid(false);
}

