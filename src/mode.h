
/********************************************************************/
void deviceMode_init() {
  char _dispStr[22];
  switch(deviceState.DEVICE_MODE) {
    case DM_IDLE:
      //statusLED.pulse_repeat(1, 20);
      break;
    case DM_GPSRCVR:
      //statusLED.pulse_repeat(1);
      rtc.resetRTCTime();
      gpsSerial = &Serial2;
      if(gps.gnss_init(*gpsSerial, GPS_BAUD_RATE, deviceState.GPS_RESETMODE, 1, 0, 0, true)) {
        gpsEnabled = true;
        sprintf(_dispStr, "GPS CONN UBPV=%02d.%02d",
                gps.getProtocolVersionHigh(),
                gps.getProtocolVersionLow());
        msg_update(_dispStr);
        deviceState.GPS_RESETMODE = GPS_NORESET;
      } else {
        gpsEnabled = false;
        msg_update("ERROR - GPS Missing");
      }
      break;
    case DM_GPSLOGR:
      //statusLED.pulse_repeat(1);
      rtc.resetRTCTime();
      gpsSerial = &Serial2;
      if(gps.gnss_init(*gpsSerial, GPS_BAUD_RATE, deviceState.GPS_RESETMODE, 1, deviceState.GPS_UBXNAVSATRATE, 1, true)) {
        gpsEnabled = true;
        sprintf(_dispStr, "GPS CONN UBPV=%02d.%02d",
                gps.getProtocolVersionHigh(),
                gps.getProtocolVersionLow());
        msg_update(_dispStr);
        deviceState.GPS_RESETMODE = GPS_NORESET;
      } else {
        gpsEnabled = false;
        msg_update("ERROR - GPS Missing");
      }
      break;
    case DM_GPSNSAT:
      //statusLED.pulse_repeat(1);
      rtc.resetRTCTime();
      gpsSerial = &Serial2;
      if(gps.gnss_init(*gpsSerial, GPS_BAUD_RATE, deviceState.GPS_RESETMODE, 1, deviceState.GPS_UBXNAVSATRATE, 0, true)) {
        gpsEnabled = true;
        sprintf(_dispStr, "GPS CONN UBPV=%02d.%02d",
                gps.getProtocolVersionHigh(),
                gps.getProtocolVersionLow());
        msg_update(_dispStr);
        deviceState.GPS_RESETMODE = GPS_NORESET;
      } else {
        gpsEnabled = false;
        msg_update("ERROR - GPS Missing");
      }
      break;
    case DM_GPSSTAT:
      //statusLED.pulse_repeat(1);
      rtc.resetRTCTime();
      gpsSerial = &Serial2;
      if(gps.gnss_init(*gpsSerial, GPS_BAUD_RATE, deviceState.GPS_RESETMODE, 1, 0, 1, true)) {
        gpsEnabled = true;
        sprintf(_dispStr, "GPS CONN UBPV=%02d.%02d",
                gps.getProtocolVersionHigh(),
                gps.getProtocolVersionLow());
        msg_update(_dispStr);
        deviceState.GPS_RESETMODE = GPS_NORESET;
      } else {
        gpsEnabled = false;
        msg_update("ERROR - GPS Missing");
      }
      break;
    case DM_GPSSCAL:
      //statusLED.pulse_repeat(1);
      rtc.resetRTCTime();
      gpsSerial = &Serial2;
      if(gps.gnss_init(*gpsSerial, GPS_BAUD_RATE, GPS_NORESET, 0, 0, 0)) {
        gpsEnabled = true;
        sprintf(_dispStr, "GPS CONN UBPV=%02d.%02d",
                gps.getProtocolVersionHigh(),
                gps.getProtocolVersionLow());
        msg_update(_dispStr);
        deviceState.GPS_RESETMODE = GPS_NORESET;
      } else {
        gpsEnabled = false;
        msg_update("ERROR - GPS Missing");
      }
      break;
    case DM_GPSSCFG:
      //statusLED.pulse_repeat(1);
      rtc.resetRTCTime();
      gpsSerial = &Serial2;
      if(gps.gnss_init(*gpsSerial, GPS_BAUD_RATE, GPS_NORESET, 0, 0, 0)) {
        gpsEnabled = true;
        sprintf(_dispStr, "GPS CONN UBPV=%02d.%02d",
                gps.getProtocolVersionHigh(),
                gps.getProtocolVersionLow());
        msg_update(_dispStr);
        deviceState.GPS_RESETMODE = GPS_NORESET;
      } else {
        gpsEnabled = false;
        msg_update("ERROR - GPS Missing");
      }
      break;
    case DM_GPSSSTP:
      //statusLED.pulse_repeat(1);
      rtc.resetRTCTime();
      gpsSerial = &Serial2;
#ifdef CONFIG_IDF_TARGET_ESP32S3 // Core S3SE fix using the correct serial pins
      gpsSerial->begin(GPS_BAUD_RATE, SERIAL_8N1, RXD2, TXD2);
#else
      gpsSerial->begin(GPS_BAUD_RATE);
#endif
      msg_update("GPS Serial Enabled");
      break;
    case DM_GPSEMU_M8:
      //statusLED.pulse_repeat(1);
      rtc.resetRTCTime();
      emulatorSerial = &Serial2;
      if(emulator_setup(*emulatorSerial, UBLOX_M8_EMULATOR_BAUD_RATE, TGPSE_UBX_M8_MODULE,
                        deviceState.EMU_UBXPKTLOOPENABLE, deviceState.EMU_UBXPKTSOURCE)) {
        msg_update("EMU Serial Enabled");
      } else {
        msg_update("ERROR - EMU Setup");
      }
      break;
    case DM_GPSEMU_M10:
      //statusLED.pulse_repeat(1);
      rtc.resetRTCTime();
      emulatorSerial = &Serial2;
      if(emulator_setup(*emulatorSerial, UBLOX_M10_EMULATOR_BAUD_RATE, TGPSE_UBX_M10_MODULE,
                        deviceState.EMU_UBXPKTLOOPENABLE, deviceState.EMU_UBXPKTSOURCE)) {
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
      if(gpsEnabled) {
        if(gps.gnss_init(*gpsSerial, GPS_BAUD_RATE, GPS_NORESET, 0, 0, 0)) {
          msg_update("GPS Receiver Stopped");
        } else {
          msg_update("ERROR - GPS Reset");
        }
      }
      gpsEnabled = false;
      break;
    case DM_GPSLOGR:
      if(gpsEnabled) {
        if(gps.gnss_init(*gpsSerial, GPS_BAUD_RATE, GPS_NORESET, 0, 0, 0)) {
          msg_update("GPS Logger Stopped");
        } else {
          msg_update("ERROR - GPS Reset");
        }
      }
      gpsEnabled = false;
      break;
    case DM_GPSNSAT:
      if(gpsEnabled) {
        if(gps.gnss_init(*gpsSerial, GPS_BAUD_RATE, GPS_NORESET, 0, 0, 0)) {
          msg_update("GPS NAVSAT Stopped");
        } else {
          msg_update("ERROR - GPS Reset");
        }
      }
      gpsEnabled = false;
      break;
    case DM_GPSSTAT:
      if(gpsEnabled) {
        if(gps.gnss_init(*gpsSerial, GPS_BAUD_RATE, GPS_NORESET, 0, 0, 0)) {
          msg_update("GPS NAVSTAT Stopped");
        } else {
          msg_update("ERROR - GPS Reset");
        }
      }
      gpsEnabled = false;
      break;
    case DM_GPSSCAL:
      if(gpsEnabled) {
        if(gps.gnss_init(*gpsSerial, GPS_BAUD_RATE, GPS_NORESET, 0, 0, 0)) {
          msg_update("GPS SATCAL Stopped");
        } else {
          msg_update("ERROR - GPS Reset");
        }
      }
      gpsEnabled = false;
      break;
    case DM_GPSSCFG:
      if(gpsEnabled) {
        if(gps.gnss_init(*gpsSerial, GPS_BAUD_RATE, GPS_NORESET, 0, 0, 0)) {
          msg_update("GPS SATCFG Stopped");
        } else {
          msg_update("ERROR - GPS Reset");
        }
      }
      gpsEnabled = false;
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
  //rtc.resetRTCTime();
}

