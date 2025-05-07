
/********************************************************************/
// Display
bool displayEnabled;
#include <Wire.h>
#include <TeenyPrtVal.h>
#include <TeenySevenSeg.h>
#include <TeenyMenu.h>

/*
TFT_eSprite display = TFT_eSprite(&M5.Lcd);
TeenyPrtVal<TFT_eSprite> displayPV(display);
TeenySevenSeg<TFT_eSprite> displaySS(display);
//TeenyMenu<TFT_eSprite> menu(display, 12, 16, false, 16, 16, 6, 10, 11, 154, 6, 0, 20);
TeenyMenu<TFT_eSprite> menu(display, 12, 16, false, 24, 20, 14, 10, 11, 154, 6, 0, 20);
*/
M5Canvas display(&M5.Lcd);
TeenyPrtVal<M5Canvas> displayPV(display);
TeenySevenSeg<M5Canvas> displaySS(display);
TeenyMenu<M5Canvas> menu(display, 12, 16, false, 24, 20, 14, 10, 11, 154, 6, 0, 20);

/********************************************************************/
// Display/Message Defines
#define DISPLAY_REFRESH_PERIOD  200
#define MSG_DISPLAY_TIME        5000
int16_t  MENU_MODE = DM_IDLE;
bool displayRefresh;
bool displayUpdate;

/********************************************************************/
// forward declarations
bool msg_update(const char* msgStr);
void display_processingMsg();
void deviceMode_init();
void deviceMode_end();
bool satCalibration_enter();
void satCalibration_exit();
const char* satCalibration_getStatus();
bool gnssReadConfig();
bool gnssToggleConfig(const char* toggleGnssSigName);

/********************************************************************/
// Menus
//
// menu mode item
void menu_menuModeCB(); // forward declaration
SelectOptionUint8t selectMenuModeOptions[] = {
  {"IDLE",   DM_IDLE},
  {"GPSRCV", DM_GPSRCVR},
  {"GPSLOG", DM_GPSLOGR},
  {"NAVSAT", DM_GPSNSAT},
  {"NVSTAT", DM_GPSSTAT},
  {"SATCAL", DM_GPSSCAL},
  {"SATCFG", DM_GPSSCFG},
//{"GPSSST", DM_GPSSSTP},
  {"EMUM8",  DM_GPSEMU_M8},
  {"EMUM10", DM_GPSEMU_M10}};
TeenyMenuSelect selectMenuMode(sizeof(selectMenuModeOptions)/sizeof(SelectOptionUint8t), selectMenuModeOptions);
TeenyMenuItem menuItemMenuMode("Device Mode", MENU_MODE, selectMenuMode, menu_menuModeCB);
//
// gps receiver unit
//
void menu_entrGPSReceiverCB(); // forward declaration
void menu_exitGPSReceiverCB(); // forward declaration
TeenyMenuPage menuPageGPSReceiver("GPS RECEIVER MODE", menu_entrGPSReceiverCB, menu_exitGPSReceiverCB);
TeenyMenuItem menuItemGPSReceiver("*START GPS RCVR*", menuPageGPSReceiver);
TeenyMenuItem menuItemGPSReceiverExit(false); // optional return menu item
TeenyMenuItem menuItemGPSReceiverLabel0("");
TeenyMenuItem menuItemGPSReceiverLabel1("");
//
// gps logging unit
//
void menu_entrGPSLoggerCB(); // forward declaration
void menu_exitGPSLoggerCB(); // forward declaration
TeenyMenuPage menuPageGPSLogger("GPS LOGGING MODE", menu_entrGPSLoggerCB, menu_exitGPSLoggerCB);
TeenyMenuItem menuItemGPSLogger("*START GPS LOGGER*", menuPageGPSLogger);
TeenyMenuItem menuItemGPSLoggerExit(false); // optional return menu item
TeenyMenuItem menuItemGPSLoggerLabel0("");
TeenyMenuItem menuItemGPSLoggerLabel1("");
TeenyMenuItem menuItemGPSLoggerLabel2("");
TeenyMenuItem menuItemGPSLoggerLabel3("");
TeenyMenuItem menuItemGPSLoggerLabel4("");
TeenyMenuItem menuItemGPSLoggerLabel5("");
TeenyMenuItem menuItemGPSLoggerLabel6("");
TeenyMenuItem menuItemGPSLoggerLabel7("");
TeenyMenuItem menuItemGPSLoggerLabel8("");
//
// gps start/stop logging
void menu_startGPSLoggerCB(); // forward declaration
TeenyMenuItem menuItemGPSLoggerStrtLog("Start GPS Logging", menu_startGPSLoggerCB);
void menu_stopGPSLoggerCB(); // forward declaration
TeenyMenuItem menuItemGPSLoggerStopLog("Stop GPS Logging", menu_stopGPSLoggerCB);
//
// gps navsat unit
//
void menu_entrGPSNavSatCB(); // forward declaration
void menu_exitGPSNavSatCB(); // forward declaration
bool menu_GPSNavSatDisplayMap = false;
void menu_GPSNavSatToggleViewCB(); // forward declaration
TeenyMenuPage menuPageGPSNavSat("GPS NAVSAT MODE", menu_entrGPSNavSatCB, menu_exitGPSNavSatCB, nullptr, menu_GPSNavSatToggleViewCB, menu_GPSNavSatToggleViewCB);
TeenyMenuItem menuItemGPSNavSat("*START GPS NAVSAT*", menuPageGPSNavSat);
TeenyMenuItem menuItemGPSNavSatExit(false); // optional return menu item
TeenyMenuItem menuItemGPSNavSatLabel00("");
TeenyMenuItem menuItemGPSNavSatLabel01("");
TeenyMenuItem menuItemGPSNavSatLabel02("");
TeenyMenuItem menuItemGPSNavSatLabel03("");
TeenyMenuItem menuItemGPSNavSatLabel04("");
TeenyMenuItem menuItemGPSNavSatLabel05("");
TeenyMenuItem menuItemGPSNavSatLabel06("");
TeenyMenuItem menuItemGPSNavSatLabel07("");
TeenyMenuItem menuItemGPSNavSatLabel08("");
TeenyMenuItem menuItemGPSNavSatLabel09("");
TeenyMenuItem menuItemGPSNavSatLabel10("");
TeenyMenuItem menuItemGPSNavSatLabel11("");
TeenyMenuItem menuItemGPSNavSatLabel12("");
TeenyMenuItem menuItemGPSNavSatToggleView("", menu_GPSNavSatToggleViewCB);
//
// gps navstat unit
//
void menu_entrGPSNavStatCB(); // forward declaration
void menu_exitGPSNavStatCB(); // forward declaration
TeenyMenuPage menuPageGPSNavStat("GPS NAVSTAT MODE", menu_entrGPSNavStatCB, menu_exitGPSNavStatCB);
TeenyMenuItem menuItemGPSNavStat("*START GPS NVSTAT*", menuPageGPSNavStat);
TeenyMenuItem menuItemGPSNavStatExit(false); // optional return menu item
TeenyMenuItem menuItemGPSNavStatLabel0("");
//
// gps calibration unit
//
void menu_entrGPSSatCalCB(); // forward declaration
void menu_exitGPSSatCalCB(); // forward declaration
TeenyMenuPage menuPageGPSSatCal("GNSS CALIBRATION", menu_entrGPSSatCalCB, menu_exitGPSSatCalCB);
TeenyMenuItem menuItemGPSSatCal("*START GPS SATCAL*", menuPageGPSSatCal);
TeenyMenuItem menuItemGPSSatCalExit(false); // optional return menu item
TeenyMenuItem menuItemGPSSatCalLabel00("");
TeenyMenuItem menuItemGPSSatCalLabel01("");
TeenyMenuItem menuItemGPSSatCalLabel02("");
TeenyMenuItem menuItemGPSSatCalLabel03("");
TeenyMenuItem menuItemGPSSatCalLabel04("");
TeenyMenuItem menuItemGPSSatCalLabel05("");
TeenyMenuItem menuItemGPSSatCalLabel06("");
TeenyMenuItem menuItemGPSSatCalLabel07("");
TeenyMenuItem menuItemGPSSatCalLabel08("");
TeenyMenuItem menuItemGPSSatCalLabel09("");
TeenyMenuItem menuItemGPSSatCalLabel10("");
TeenyMenuItem menuItemGPSSatCalLabel11("");
TeenyMenuItem menuItemGPSSatCalLabel12("");
//
// gps calibration start/stop
void menu_startGPSSatCalCB(); // forward declaration
TeenyMenuItem menuItemGPSSatCalStrt("Start Calibration", menu_startGPSSatCalCB);
void menu_stopGPSSatCalCB(); // forward declaration
TeenyMenuItem menuItemGPSSatCalStop("Stop Calibration", menu_stopGPSSatCalCB);
//
// gps satellite config unit
//
void menu_entrGPSSatCfgCB(); // forward declaration
void menu_exitGPSSatCfgCB(); // forward declaration
TeenyMenuPage menuPageGPSSatCfg("GNSS CONFIGURATOR", menu_entrGPSSatCfgCB, menu_exitGPSSatCfgCB);
TeenyMenuItem menuItemGPSSatCfg("*START GPS SATCFG*", menuPageGPSSatCfg);
TeenyMenuItem menuItemGPSSatCfgExit(false); // optional return menu item
TeenyMenuItem menuItemGPSSatCfgLabel0("");
TeenyMenuItem menuItemGPSSatCfgLabel1("");
TeenyMenuItem menuItemGPSSatCfgLabel2("");
//
// ublox module type
uint8_t menuUbloxModuleType = 0;
uint8_t menuUbloxModuleTypeMin = 0;
uint8_t menuUbloxModuleTypeMax = 99;
TeenyMenuItem menuItemUbloxModuleType("UbloxModule", menuUbloxModuleType, menuUbloxModuleTypeMin, menuUbloxModuleTypeMax, true);
//
// gnss satellite select info
void menu_pollGNSSSelectionCB(); // forward declaration
TeenyMenuPage menuPageGNSSSelInfo("GNSS SELECT INFO", menu_pollGNSSSelectionCB);
TeenyMenuItem menuItemGNSSSelInfo("GNSS Select Info", menuPageGNSSSelInfo);
TeenyMenuItem menuItemGNSSSelInfoExit(false); // optional return menu item
TeenyMenuItem menuItemGNSSSelInfoLabel0("");
TeenyMenuItem menuItemGNSSSelInfoLabel1("");
TeenyMenuItem menuItemGNSSSelInfoLabel2("");
TeenyMenuItem menuItemGNSSSelInfoLabel3("");
TeenyMenuItem menuItemGNSSSelInfoLabel4("");
//
// gnss satellite config info
void menu_pollGNSSSatConfigCB(); // forward declaration
TeenyMenuPage menuPageGNSSSatCfgInfo("GNSS CONFIG INFO", menu_pollGNSSSatConfigCB);
TeenyMenuItem menuItemGNSSSatCfgInfo("GNSS Config Info", menuPageGNSSSatCfgInfo);
TeenyMenuItem menuItemGNSSSatCfgInfoExit(false); // optional return menu item
TeenyMenuItem menuItemGNSSSatCfgInfoLabel0("");
TeenyMenuItem menuItemGNSSSatCfgInfoLabel1("");
TeenyMenuItem menuItemGNSSSatCfgInfoLabel2("");
TeenyMenuItem menuItemGNSSSatCfgInfoLabel3("");
TeenyMenuItem menuItemGNSSSatCfgInfoLabel4("");
TeenyMenuItem menuItemGNSSSatCfgInfoLabel5("");
TeenyMenuItem menuItemGNSSSatCfgInfoLabel6("");
TeenyMenuItem menuItemGNSSSatCfgInfoLabel7("");
TeenyMenuItem menuItemGNSSSatCfgInfoLabel8("");
TeenyMenuItem menuItemGNSSSatCfgInfoLabel9("");
//
// gnss satellite toggle items
void menu_gnssCfgGPSToggleCB(); // forward declaration
void menu_gnssCfgSBASToggleCB(); // forward declaration
void menu_gnssCfgGalileoToggleCB(); // forward declaration
void menu_gnssCfgBeiDouToggleCB(); // forward declaration
void menu_gnssCfgBeiDouB1ToggleCB(); // forward declaration
void menu_gnssCfgBeiDouB1CToggleCB(); // forward declaration
void menu_gnssCfgIMESToggleCB(); // forward declaration
void menu_gnssCfgQZSSToggleCB(); // forward declaration
void menu_gnssCfgQZSSL1CAToggleCB(); // forward declaration
void menu_gnssCfgQZSSL1SToggleCB(); // forward declaration
void menu_gnssCfgGLONASSToggleCB(); // forward declaration
// Note:  Toggle menu item titles set by gnssToggleConfig()
TeenyMenuItem menuItemGNSSSatCfgGPSToggle(      "", menu_gnssCfgGPSToggleCB);
TeenyMenuItem menuItemGNSSSatCfgSBASToggle(     "", menu_gnssCfgSBASToggleCB);
TeenyMenuItem menuItemGNSSSatCfgGalileoToggle(  "", menu_gnssCfgGalileoToggleCB);
TeenyMenuItem menuItemGNSSSatCfgBeiDouToggle(   "", menu_gnssCfgBeiDouToggleCB);
TeenyMenuItem menuItemGNSSSatCfgBeiDouB1Toggle( "", menu_gnssCfgBeiDouB1ToggleCB);
TeenyMenuItem menuItemGNSSSatCfgBeiDouB1CToggle("", menu_gnssCfgBeiDouB1CToggleCB);
TeenyMenuItem menuItemGNSSSatCfgIMESToggle(     "", menu_gnssCfgIMESToggleCB);
TeenyMenuItem menuItemGNSSSatCfgQZSSToggle(     "", menu_gnssCfgQZSSToggleCB);
TeenyMenuItem menuItemGNSSSatCfgQZSSL1CAToggle( "", menu_gnssCfgQZSSL1CAToggleCB);
TeenyMenuItem menuItemGNSSSatCfgQZSSL1SToggle(  "", menu_gnssCfgQZSSL1SToggleCB);
TeenyMenuItem menuItemGNSSSatCfgGLONASSToggle(  "", menu_gnssCfgGLONASSToggleCB);
//
// gps single step unit
//
void menu_entrGPSSingleStepCB(); // forward declaration
void menu_exitGPSSingleStepCB(); // forward declaration
TeenyMenuPage menuPageGPSSingleStep("GPS SINGLESTEP MODE", menu_entrGPSSingleStepCB, menu_exitGPSSingleStepCB);
TeenyMenuItem menuItemGPSSingleStep("*START GPS STEPPER*", menuPageGPSSingleStep);
TeenyMenuItem menuItemGPSSingleStepExit(false); // optional return menu item
void menu_sstBeginCB(); // forward declaration
TeenyMenuItem menuItemGPSSingleStepBegin("gnss.begin", menu_sstBeginCB);
void menu_sstReqMonVerCB(); // forward declaration
TeenyMenuItem menuItemGPSSingleStepReqMonVer("reqMonVer_CMD", menu_sstReqMonVerCB);
//
// gps ublox M8 emulator unit
//
void menu_entrGPSEmulateM8CB(); // forward declaration
void menu_exitGPSEmulateM8CB(); // forward declaration
TeenyMenuPage menuPageGPSEmulateM8("UBLOX M8 EMULATOR", menu_entrGPSEmulateM8CB, menu_exitGPSEmulateM8CB);
TeenyMenuItem menuItemGPSEmulateM8("*START EMULATOR*", menuPageGPSEmulateM8);
TeenyMenuItem menuItemGPSEmulateM8Exit(false); // optional return menu item
TeenyMenuItem menuItemGPSEmulateM8Label0("");
TeenyMenuItem menuItemGPSEmulateM8Label1("");
//
// gps ublox M10 emulator unit
//
void menu_entrGPSEmulateM10CB(); // forward declaration
void menu_exitGPSEmulateM10CB(); // forward declaration
TeenyMenuPage menuPageGPSEmulateM10("UBLOX M10 EMULATOR", menu_entrGPSEmulateM10CB, menu_exitGPSEmulateM10CB);
TeenyMenuItem menuItemGPSEmulateM10("*START EMULATOR*", menuPageGPSEmulateM10);
TeenyMenuItem menuItemGPSEmulateM10Exit(false); // optional return menu item
TeenyMenuItem menuItemGPSEmulateM10Label0("");
TeenyMenuItem menuItemGPSEmulateM10Label1("");
//
// gps reset
//
void menu_GPSResetCB(); // forward declaration
SelectOptionUint8t selectGPSResetOptions[] = {
  {"NO",   GPS_NORESET},
  {"HOT",  GPS_HOTSTART},
  {"WARM", GPS_WARMSTART},
  {"COLD", GPS_COLDSTART},
  {"HWRST", GPS_HARDWARERESET}};
TeenyMenuSelect selectGPSReset(sizeof(selectGPSResetOptions)/sizeof(SelectOptionUint8t), selectGPSResetOptions);
TeenyMenuItem menuItemGPSReset("GPS Reset", deviceState.GPS_RESETMODE, selectGPSReset, menu_GPSResetCB);
//
// TEENYGPSTESTBED SETTINGS
//
// settings menu
TeenyMenuPage menuPageTopLevelSettings("SETTINGS MENU");
TeenyMenuItem menuItemTopLevelSettings("Settings Menu", menuPageTopLevelSettings);
TeenyMenuItem menuItemTopLevelSettingsExit(false); // optional return menu item
//
// TIME ZONE
//
// time zone menu
SelectOptionInt16t selectTimeZoneOptions[] = {
  {"-12:00",-720}, {"-11:00",-660}, {"-10:00",-600}, {"-09:30",-570}, {"-09:00",-540},
  {"-08:00",-480}, {"-07:00",-420}, {"-06:00",-360}, {"-05:00",-300}, {"-04:00",-240},
  {"-03:30",-210}, {"-03:00",-180}, {"-02:30",-150}, {"-02:00",-120}, {"-01:00",-060},
  { "00:00", 000}, { "01:00", 060}, { "02:00", 120}, { "03:00", 180}, { "03:30", 210},
  { "04:00", 240}, { "04:30", 270}, { "05:00", 300}, { "05:30", 330}, { "05:45", 345},
  { "06:00", 360}, { "06:30", 390}, { "07:00", 420}, { "08:00", 480}, { "08:45", 525},
  { "09:00", 540}, { "09:30", 570}, { "10:00", 600}, { "10:30", 630}, { "11:00", 660},
  { "12:00", 720}, { "12:45", 765}, { "13:00", 780}, { "13:45", 825}, { "14:00", 840}};
TeenyMenuSelect selectTimeZone(sizeof(selectTimeZoneOptions)/sizeof(SelectOptionInt16t), selectTimeZoneOptions);
TeenyMenuItem menuItemTimeZone("Time Zone", deviceState.TIMEZONE, selectTimeZone, nullptr, 2);
//
// RTC SETTINGS
//
// rtc settings menu
void menu_getRTC_CB(); // forward declaration
void menu_setRTC_CB(); // forward declaration
TeenyMenuPage menuPageRTCSettings("REAL TIME CLOCK", menu_getRTC_CB);
TeenyMenuItem menuItemRTCSettings("RT Clock Settings", menuPageRTCSettings);
TeenyMenuItem menuItemRTCSettingsExit(false); // optional return menu item
TeenyMenuItem menuItemRTCSettingsLabel0("");
TeenyMenuItem menuItemRTCSettingsLabel1("");
TeenyMenuItem menuItemRTCSettingsLabel2("");
//
// rtc year
int16_t menuRTCYear = 2024;
int16_t menuRTCYearMin = 2024;
int16_t menuRTCYearMax = 2042;
TeenyMenuItem menuItemRTCYear("RTC Year", menuRTCYear, menuRTCYearMin, menuRTCYearMax);
//
// rtc month
uint8_t menuRTCMonth = 1;
uint8_t menuRTCMonthMin = 1;
uint8_t menuRTCMonthMax = 12;
TeenyMenuItem menuItemRTCMonth("RTC Month", menuRTCMonth, menuRTCMonthMin, menuRTCMonthMax);
//
// rtc day
uint8_t menuRTCDay = 1;
uint8_t menuRTCDayMin = 1;
uint8_t menuRTCDayMax = 31;
TeenyMenuItem menuItemRTCDay("RTC Day", menuRTCDay, menuRTCDayMin, menuRTCDayMax);
//
// rtc hour
uint8_t menuRTCHour = 0;
uint8_t menuRTCHourMin = 0;
uint8_t menuRTCHourMax = 23;
TeenyMenuItem menuItemRTCHour("RTC Hour", menuRTCHour, menuRTCHourMin, menuRTCHourMax);
//
// rtc minute
uint8_t menuRTCMinute = 0;
uint8_t menuRTCMinuteMin = 0;
uint8_t menuRTCMinuteMax = 59;
TeenyMenuItem menuItemRTCMinute("RTC Minute", menuRTCMinute, menuRTCMinuteMin, menuRTCMinuteMax);
//
// rtc second
uint8_t menuRTCSecond = 0;
uint8_t menuRTCSecondMin = 0;
uint8_t menuRTCSecondMax = 59;
TeenyMenuItem menuItemRTCSecond("RTC Second", menuRTCSecond, menuRTCSecondMin, menuRTCSecondMax);
//
// rtc set date/time
TeenyMenuItem menuItemRTCSetDateTime("Set RTC Date/Time", menu_setRTC_CB);
//
// GPS SETTINGS
//
// gps settings menu
TeenyMenuPage menuPageGPSSettings("GPS SETTINGS");
TeenyMenuItem menuItemGPSSettings("GPS Settings", menuPageGPSSettings);
TeenyMenuItem menuItemGPSSettingsExit(false); // optional return menu item
//
// gps ubx navsat rate 
uint8_t menuGPSUbloxNAVSATRateMin = 1;
uint8_t menuGPSUbloxNAVSATRateMax = 10;
TeenyMenuItem menuItemGPSUbloxNAVSATRate("NAVSAT Rate", deviceState.GPS_UBXNAVSATRATE, menuGPSUbloxNAVSATRateMin, menuGPSUbloxNAVSATRateMax);
//
// gps ubx packet logging mode
SelectOptionUint8t selectGPSUbloxPktLogModeOptions[] = {
  {"NONE",   GPS_LOGUBX_NONE},
  {"NAVPVT", GPS_LOGUBX_NAVPVT},
  {"NAVSAT", GPS_LOGUBX_NAVSAT},
  {"STATUS", GPS_LOGUBX_NAVSTATUS},
  {"ALL",    GPS_LOGUBX_ALL}};
TeenyMenuSelect selectGPSUbloxPktLogMode(sizeof(selectGPSUbloxPktLogModeOptions)/sizeof(SelectOptionUint8t), selectGPSUbloxPktLogModeOptions);
TeenyMenuItem menuItemGPSUbloxPktLogMode("UBX Pkt Log", deviceState.GPS_LOGUBXMODE, selectGPSUbloxPktLogMode);
//
// gps logging
TeenyMenuItem menuItemGPSLogGPX("GPS Log GPX", deviceState.GPS_LOGGPX, nullptr, "NO", "YES");
TeenyMenuItem menuItemGPSLogKML("GPS Log KML", deviceState.GPS_LOGKML, nullptr, "NO", "YES");
TeenyMenuItem menuItemGPSLogCSV("GPS Log CSV", deviceState.GPS_LOGCSV, nullptr, "NO", "YES");
//
// gps calibration scan period
uint8_t menuGPSCalibrationPeriodMin = 1;
uint8_t menuGPSCalibrationPeriodMax = 60;
TeenyMenuItem menuItemGPSCalibrationPeriod("GNSSCalTime", deviceState.GPS_CALIBRATEPERIOD, menuGPSCalibrationPeriodMin, menuGPSCalibrationPeriodMax);
//
// gps factory reset menu
void menu_cancelDeleteGPSLogFilesCB(); // forward declaration
TeenyMenuPage menuPageDeleteGPSLogFiles("DELETE GPS LOGS", nullptr, menu_cancelDeleteGPSLogFilesCB);
TeenyMenuItem menuItemDeleteGPSLogFiles("Delete GPS Logs", menuPageDeleteGPSLogFiles);
TeenyMenuItem menuItemDeleteGPSLogFilesExit(false); // optional return menu item
//
// gps confirm delete all log files
void menu_confirmDeleteGPSLogFilesCB(); // forward declaration
TeenyMenuItem menuItemConfirmDeleteGPSLogFiles("Confirm Delete?", menu_confirmDeleteGPSLogFilesCB);
//
// gps factory reset menu
void menu_cancelGPSFactoryResetCB(); // forward declaration
TeenyMenuPage menuPageGPSFactoryReset("GPS FACTORY RESET", nullptr, menu_cancelGPSFactoryResetCB);
TeenyMenuItem menuItemGPSFactoryReset("GPS Factory Reset", menuPageGPSFactoryReset);
TeenyMenuItem menuItemGPSFactoryResetExit(false); // optional return menu item
//
// gps confirm reset
void menu_confirmGPSFactoryResetCB(); // forward declaration
TeenyMenuItem menuItemConfirmGPSFactoryReset("Confirm Reset?", menu_confirmGPSFactoryResetCB);
//
// EMULATOR SETTINGS
//
// emulator settings menu
TeenyMenuPage menuPageEMULSettings("EMULATOR SETTINGS");
TeenyMenuItem menuItemEMULSettings("Emulator Settings", menuPageEMULSettings);
TeenyMenuItem menuItemEMULSettingsExit(false); // optional return menu item
//
// emulator ubx packet source
SelectOptionUint8t selectEMUUbloxPktSourceOptions[] = {
  {"PROG",   EMU_PGMINPUT},
  {"SDCARD", EMU_SDCINPUT}};
TeenyMenuSelect selectEMUUbloxPktSource(sizeof(selectEMUUbloxPktSourceOptions)/sizeof(SelectOptionUint8t), selectEMUUbloxPktSourceOptions);
TeenyMenuItem menuItemEMUUbloxPktSource("UBX Pkt Src", deviceState.EMU_UBXPKTSOURCE, selectEMUUbloxPktSource);
//
// emulator cold start packet count
uint8_t menuEMUColdStartPktsMin = 0;
uint8_t menuEMUColdStartPktsMax = 60;
TeenyMenuItem menuItemEMUColdStartPkts("ColdStrtPkts", deviceState.EMU_NUMCOLDSTARTPACKETS, menuEMUColdStartPktsMin, menuEMUColdStartPktsMax);
//
// emulator loop
TeenyMenuItem menuItemEMULoopEnable("EmulateMode", deviceState.EMU_UBXPKTLOOPENABLE, nullptr, "1TIME", "LOOP");
//
// DEVICE SETTINGS
//
// settings menu
TeenyMenuPage menuPageDeviceSettings("DEVICE SETTINGS");
TeenyMenuItem menuItemDeviceSettings("Device Settings", menuPageDeviceSettings);
TeenyMenuItem menuItemDeviceSettingsExit(false); // optional return menu item
//
// display brightness
void menu_displayBrightnessCB(); // forward declaration
SelectOptionUint8t selectDisplayBrightnessOptions[] = {
  {"100", 100},
  {"90",   90},
  {"80",   80},
  {"70",   70},
  {"60",   60},
  {"50",   50}};
TeenyMenuSelect selectDisplayBrightness(sizeof(selectDisplayBrightnessOptions)/sizeof(SelectOptionUint8t), selectDisplayBrightnessOptions);
TeenyMenuItem menuItemDisplayBrightness("Dsp Brghtns", deviceState.DISPLAYBRIGHTNESS, selectDisplayBrightness, menu_displayBrightnessCB);
//
// display timeout
SelectOptionUint8t selectDisplayTimeoutOptions[] = {
  {"NEVER", 99},
  {"30",    30},
  {"20",    20},
  {"10",    10},
  {"5",      5},
  {"1",      1}};
TeenyMenuSelect selectDisplayTimeout(sizeof(selectDisplayTimeoutOptions)/sizeof(SelectOptionUint8t), selectDisplayTimeoutOptions);
TeenyMenuItem menuItemDisplayTimeout("Dsp Timeout", deviceState.DISPLAYTIMEOUT, selectDisplayTimeout);
//
// status led
//TeenyMenuItem menuItemStatusLED("Status LED", deviceState.STATUSLED, nullptr, "OFF", "ON");
//
// debug loop timing
TeenyMenuItem menuItemDebugLoopTiming("DebugTiming", deviceState.DEBUGLOOPTIMING, nullptr, "OFF", "ON");
//
// SAVE SETTINGS
//
// save settings menu
void menu_cancelSaveSettingsCB(); // forward declaration
TeenyMenuPage menuPageSaveSettings("SAVE DEVICE SETTINGS", nullptr, menu_cancelSaveSettingsCB);
TeenyMenuItem menuItemSaveSettings("Save Dev Settings", menuPageSaveSettings);
TeenyMenuItem menuItemSaveSettingsExit(false); // optional return menu item
//
// save state to flash
void menu_confirmSaveSettingsCB(); // forward declaration
TeenyMenuItem menuItemConfirmSaveSettings("Confirm Save?", menu_confirmSaveSettingsCB);
//
// RESTART DEVICE
//
// restart device menu
void menu_cancelRestartDeviceCB(); // forward declaration
TeenyMenuPage menuPageRestartDevice("RESTART DEVICE", nullptr, menu_cancelRestartDeviceCB);
TeenyMenuItem menuItemRestartDevice("Restart Device", menuPageRestartDevice);
TeenyMenuItem menuItemRestartDeviceExit(false); // optional return menu item
//
// confirm restart
void menu_confirmRestartDeviceCB(); // forward declaration
TeenyMenuItem menuItemConfirmRestartDevice("Confirm Restart?", menu_confirmRestartDeviceCB);
//
// RESET DEVICE
//
// reset device menu
void menu_cancelResetDeviceCB(); // forward declaration
TeenyMenuPage menuPageResetDevice("RESET DEVICE", nullptr, menu_cancelResetDeviceCB);
TeenyMenuItem menuItemResetDevice("Reset Device", menuPageResetDevice);
TeenyMenuItem menuItemResetDeviceExit(false); // optional return menu item
//
// confirm reset
void menu_confirmResetDeviceCB(); // forward declaration
TeenyMenuItem menuItemConfirmResetDevice("Confirm Reset?", menu_confirmResetDeviceCB);
//
// MAIN MENU
//
//TeenyMenuPage menuPageMain("TEENY GPS TESTBED", nullptr, menu_exit);
TeenyMenuPage menuPageMain("TEENY GPS TESTBED");
TeenyMenuItem menuItemLabel0("");
TeenyMenuItem menuItemLabel1("");
TeenyMenuItem menuItemLabel2("");
TeenyMenuItem menuItemLabel3("");
TeenyMenuItem menuItemLabel4("");
TeenyMenuItem menuItemLabel5("");
TeenyMenuItem menuItemLabel6("");
//
// menu variables
uint32_t menuInputTime;
bool menuDisplaySleepMode;

/********************************************************************/
void menu_setup() {
  // add menu items
  menuPageMain.addMenuItem(menuItemLabel0);
  menuPageMain.addMenuItem(menuItemGPSReceiver);
  menuPageGPSReceiver.addMenuItem(menuItemGPSReceiverLabel0);
  menuPageGPSReceiver.addMenuItem(menuItemGPSReceiverLabel1);
  //menuPageBaseUnit.addMenuItem(menuItemGPSReceiverExit);
  menuPageMain.addMenuItem(menuItemGPSLogger);
  menuPageGPSLogger.addMenuItem(menuItemGPSLoggerLabel0);
  menuPageGPSLogger.addMenuItem(menuItemGPSLoggerLabel1);
  menuPageGPSLogger.addMenuItem(menuItemGPSLoggerLabel2);
  menuPageGPSLogger.addMenuItem(menuItemGPSLoggerLabel3);
  menuPageGPSLogger.addMenuItem(menuItemGPSLoggerLabel4);
  menuPageGPSLogger.addMenuItem(menuItemGPSLoggerLabel5);
  menuPageGPSLogger.addMenuItem(menuItemGPSLoggerLabel6);
  menuPageGPSLogger.addMenuItem(menuItemGPSLoggerLabel7);
  menuPageGPSLogger.addMenuItem(menuItemGPSLoggerLabel8);
  menuPageGPSLogger.addMenuItem(menuItemGPSLoggerStrtLog);
  menuPageGPSLogger.addMenuItem(menuItemGPSLoggerStopLog);
  //menuPageGPSLogger.addMenuItem(menuItemGPSLoggerExit);
  menuPageMain.addMenuItem(menuItemGPSNavSat);
  menuPageGPSNavSat.addMenuItem(menuItemGPSNavSatLabel00);
  menuPageGPSNavSat.addMenuItem(menuItemGPSNavSatLabel01);
  menuPageGPSNavSat.addMenuItem(menuItemGPSNavSatLabel02);
  menuPageGPSNavSat.addMenuItem(menuItemGPSNavSatLabel03);
  menuPageGPSNavSat.addMenuItem(menuItemGPSNavSatLabel04);
  menuPageGPSNavSat.addMenuItem(menuItemGPSNavSatLabel05);
  menuPageGPSNavSat.addMenuItem(menuItemGPSNavSatLabel06);
  menuPageGPSNavSat.addMenuItem(menuItemGPSNavSatLabel07);
  menuPageGPSNavSat.addMenuItem(menuItemGPSNavSatLabel08);
  menuPageGPSNavSat.addMenuItem(menuItemGPSNavSatLabel09);
  menuPageGPSNavSat.addMenuItem(menuItemGPSNavSatLabel10);
  menuPageGPSNavSat.addMenuItem(menuItemGPSNavSatLabel11);
  menuPageGPSNavSat.addMenuItem(menuItemGPSNavSatLabel12);
  menuPageGPSNavSat.addMenuItem(menuItemGPSNavSatToggleView);
  //menuPageGPSNavSat.addMenuItem(menuItemGPSNavSatExit);
  menuPageMain.addMenuItem(menuItemGPSNavStat);
  menuPageGPSNavStat.addMenuItem(menuItemGPSNavStatLabel0);
  //menuPageGPSNavStat.addMenuItem(menuItemGPSNavStatExit);
  menuPageMain.addMenuItem(menuItemGPSSatCal);
  menuPageGPSSatCal.addMenuItem(menuItemGPSSatCalLabel00);
  menuPageGPSSatCal.addMenuItem(menuItemGPSSatCalLabel01);
  menuPageGPSSatCal.addMenuItem(menuItemGPSSatCalLabel02);
  menuPageGPSSatCal.addMenuItem(menuItemGPSSatCalLabel03);
  menuPageGPSSatCal.addMenuItem(menuItemGPSSatCalLabel04);
  menuPageGPSSatCal.addMenuItem(menuItemGPSSatCalLabel05);
  menuPageGPSSatCal.addMenuItem(menuItemGPSSatCalLabel06);
  menuPageGPSSatCal.addMenuItem(menuItemGPSSatCalLabel07);
  menuPageGPSSatCal.addMenuItem(menuItemGPSSatCalLabel08);
  menuPageGPSSatCal.addMenuItem(menuItemGPSSatCalLabel09);
  menuPageGPSSatCal.addMenuItem(menuItemGPSSatCalLabel10);
  menuPageGPSSatCal.addMenuItem(menuItemGPSSatCalLabel11);
  menuPageGPSSatCal.addMenuItem(menuItemGPSSatCalLabel12);
  menuPageGPSSatCal.addMenuItem(menuItemGPSSatCalStrt);
  menuPageGPSSatCal.addMenuItem(menuItemGPSSatCalStop);
  //menuPageGPSSatCal.addMenuItem(menuItemGPSSatCalExit);
  menuPageMain.addMenuItem(menuItemGPSSatCfg);
  //menuPageGPSSatCfg.addMenuItem(menuItemGPSSatCfgLabel0);
  menuPageGPSSatCfg.addMenuItem(menuItemUbloxModuleType);
  menuPageGPSSatCfg.addMenuItem(menuItemGNSSSelInfo);
  menuPageGNSSSelInfo.addMenuItem(menuItemGNSSSelInfoLabel0);
  menuPageGNSSSelInfo.addMenuItem(menuItemGNSSSelInfoLabel1);
  menuPageGNSSSelInfo.addMenuItem(menuItemGNSSSelInfoLabel2);
  menuPageGNSSSelInfo.addMenuItem(menuItemGNSSSelInfoLabel3);
  menuPageGNSSSelInfo.addMenuItem(menuItemGNSSSelInfoLabel4);
  menuPageGNSSSelInfo.addMenuItem(menuItemGNSSSelInfoExit);
  menuPageGPSSatCfg.addMenuItem(menuItemGNSSSatCfgInfo);
  menuPageGNSSSatCfgInfo.addMenuItem(menuItemGNSSSatCfgInfoLabel0);
  menuPageGNSSSatCfgInfo.addMenuItem(menuItemGNSSSatCfgInfoLabel1);
  menuPageGNSSSatCfgInfo.addMenuItem(menuItemGNSSSatCfgInfoLabel2);
  menuPageGNSSSatCfgInfo.addMenuItem(menuItemGNSSSatCfgInfoLabel3);
  menuPageGNSSSatCfgInfo.addMenuItem(menuItemGNSSSatCfgInfoLabel4);
  menuPageGNSSSatCfgInfo.addMenuItem(menuItemGNSSSatCfgInfoLabel5);
  menuPageGNSSSatCfgInfo.addMenuItem(menuItemGNSSSatCfgInfoLabel6);
  menuPageGNSSSatCfgInfo.addMenuItem(menuItemGNSSSatCfgInfoLabel7);
  menuPageGNSSSatCfgInfo.addMenuItem(menuItemGNSSSatCfgInfoLabel8);
  menuPageGNSSSatCfgInfo.addMenuItem(menuItemGNSSSatCfgInfoLabel9);
  menuPageGNSSSatCfgInfo.addMenuItem(menuItemGNSSSatCfgInfoExit);
  //menuPageGPSSatCfg.addMenuItem(menuItemGPSSatCfgLabel1);
  menuPageGPSSatCfg.addMenuItem(menuItemGNSSSatCfgGPSToggle);
  menuPageGPSSatCfg.addMenuItem(menuItemGNSSSatCfgSBASToggle);
  menuPageGPSSatCfg.addMenuItem(menuItemGNSSSatCfgGalileoToggle);
  menuPageGPSSatCfg.addMenuItem(menuItemGNSSSatCfgBeiDouToggle);
  menuPageGPSSatCfg.addMenuItem(menuItemGNSSSatCfgBeiDouB1Toggle);
  menuPageGPSSatCfg.addMenuItem(menuItemGNSSSatCfgBeiDouB1CToggle);
  menuPageGPSSatCfg.addMenuItem(menuItemGNSSSatCfgIMESToggle);
  menuPageGPSSatCfg.addMenuItem(menuItemGNSSSatCfgQZSSToggle);
  menuPageGPSSatCfg.addMenuItem(menuItemGNSSSatCfgQZSSL1CAToggle);
  menuPageGPSSatCfg.addMenuItem(menuItemGNSSSatCfgQZSSL1SToggle);
  menuPageGPSSatCfg.addMenuItem(menuItemGNSSSatCfgGLONASSToggle);
  //menuPageGPSSatCfg.addMenuItem(menuItemGPSSatCfgLabel2);
  menuPageGPSSatCfg.addMenuItem(menuItemGPSSatCfgExit);
  menuPageMain.addMenuItem(menuItemGPSSingleStep);
  menuPageGPSSingleStep.addMenuItem(menuItemGPSSingleStepBegin);
  menuPageGPSSingleStep.addMenuItem(menuItemGPSSingleStepReqMonVer);
  //menuPageGPSSingleStep.addMenuItem(menuItemGPSSingleStepExit);
  menuPageMain.addMenuItem(menuItemGPSEmulateM8);
  menuPageGPSEmulateM8.addMenuItem(menuItemGPSEmulateM8Label0);
  menuPageGPSEmulateM8.addMenuItem(menuItemGPSEmulateM8Label1);
  //menuPageGPSEmulateM8.addMenuItem(menuItemGPSEmulateM8Exit);
  menuPageMain.addMenuItem(menuItemGPSEmulateM10);
  menuPageGPSEmulateM10.addMenuItem(menuItemGPSEmulateM10Label0);
  menuPageGPSEmulateM10.addMenuItem(menuItemGPSEmulateM10Label1);
  //menuPageGPSEmulateM10.addMenuItem(menuItemGPSEmulateM10Exit);
  menuPageMain.addMenuItem(menuItemGPSReset);
  menuPageMain.addMenuItem(menuItemLabel1);
  menuPageMain.addMenuItem(menuItemLabel2);
  menuPageMain.addMenuItem(menuItemLabel3);
  menuPageMain.addMenuItem(menuItemMenuMode);
  menuPageMain.addMenuItem(menuItemLabel4);
  menuPageMain.addMenuItem(menuItemLabel5);
  menuPageMain.addMenuItem(menuItemLabel6);
  menuPageMain.addMenuItem(menuItemTopLevelSettings);
  menuPageTopLevelSettings.addMenuItem(menuItemTimeZone);
  menuPageTopLevelSettings.addMenuItem(menuItemRTCSettings);
  menuPageRTCSettings.addMenuItem(menuItemRTCYear);
  menuPageRTCSettings.addMenuItem(menuItemRTCMonth);
  menuPageRTCSettings.addMenuItem(menuItemRTCDay);
  menuPageRTCSettings.addMenuItem(menuItemRTCHour);
  menuPageRTCSettings.addMenuItem(menuItemRTCMinute);
  menuPageRTCSettings.addMenuItem(menuItemRTCSecond);
  menuPageRTCSettings.addMenuItem(menuItemRTCSettingsLabel0);
  menuPageRTCSettings.addMenuItem(menuItemRTCSetDateTime);
  menuPageRTCSettings.addMenuItem(menuItemRTCSettingsLabel1);
  menuPageRTCSettings.addMenuItem(menuItemRTCSettingsLabel2);
  menuPageRTCSettings.addMenuItem(menuItemRTCSettingsExit); // optional return menu item
  menuPageTopLevelSettings.addMenuItem(menuItemGPSSettings);
  menuPageGPSSettings.addMenuItem(menuItemGPSUbloxNAVSATRate);
  menuPageGPSSettings.addMenuItem(menuItemGPSUbloxPktLogMode);
  menuPageGPSSettings.addMenuItem(menuItemGPSLogGPX);
  menuPageGPSSettings.addMenuItem(menuItemGPSLogKML);
  menuPageGPSSettings.addMenuItem(menuItemGPSLogCSV);
  menuPageGPSSettings.addMenuItem(menuItemGPSCalibrationPeriod);
  menuPageGPSSettings.addMenuItem(menuItemDeleteGPSLogFiles);
  menuPageDeleteGPSLogFiles.addMenuItem(menuItemConfirmDeleteGPSLogFiles);
  menuPageDeleteGPSLogFiles.addMenuItem(menuItemDeleteGPSLogFilesExit);
  menuPageGPSSettings.addMenuItem(menuItemGPSFactoryReset);
  menuPageGPSFactoryReset.addMenuItem(menuItemConfirmGPSFactoryReset);
  menuPageGPSFactoryReset.addMenuItem(menuItemGPSFactoryResetExit);
  menuPageGPSSettings.addMenuItem(menuItemGPSSettingsExit); // optional return menu item
  menuPageTopLevelSettings.addMenuItem(menuItemEMULSettings);
  menuPageEMULSettings.addMenuItem(menuItemEMUUbloxPktSource);
  menuPageEMULSettings.addMenuItem(menuItemEMUColdStartPkts);
  menuPageEMULSettings.addMenuItem(menuItemEMULoopEnable);
  menuPageEMULSettings.addMenuItem(menuItemEMULSettingsExit); // optional return menu item
  menuPageTopLevelSettings.addMenuItem(menuItemDeviceSettings);
  menuPageDeviceSettings.addMenuItem(menuItemDisplayBrightness);
  menuPageDeviceSettings.addMenuItem(menuItemDisplayTimeout);
  //menuPageDeviceSettings.addMenuItem(menuItemStatusLED);
  menuPageDeviceSettings.addMenuItem(menuItemDebugLoopTiming);
  menuPageDeviceSettings.addMenuItem(menuItemDeviceSettingsExit); // optional return menu item
  menuPageTopLevelSettings.addMenuItem(menuItemSaveSettings);
  menuPageSaveSettings.addMenuItem(menuItemConfirmSaveSettings);
  menuPageSaveSettings.addMenuItem(menuItemSaveSettingsExit);
  menuPageTopLevelSettings.addMenuItem(menuItemRestartDevice);
  menuPageRestartDevice.addMenuItem(menuItemConfirmRestartDevice);
  menuPageRestartDevice.addMenuItem(menuItemRestartDeviceExit);
  menuPageTopLevelSettings.addMenuItem(menuItemResetDevice);
  menuPageResetDevice.addMenuItem(menuItemConfirmResetDevice);
  menuPageResetDevice.addMenuItem(menuItemResetDeviceExit);
  menuPageTopLevelSettings.addMenuItem(menuItemTopLevelSettingsExit);
  // assign the starting menu page
  menu.setMenuPageCurrent(menuPageMain);
  switch(deviceState.DEVICE_MODE) {
    case DM_GPSRCVR:
      menu.linkMenuPage(menuPageGPSReceiver);
      break;
    case DM_GPSLOGR:
      menu.linkMenuPage(menuPageGPSLogger);
      break;
    case DM_GPSNSAT:
      menu.linkMenuPage(menuPageGPSNavSat);
      break;
    case DM_GPSSTAT:
      menu.linkMenuPage(menuPageGPSNavStat);
      break;
    case DM_GPSSCAL:
      menu.linkMenuPage(menuPageGPSSatCal);
      break;
    case DM_GPSSCFG:
      menu.linkMenuPage(menuPageGPSSatCfg);
      break;
    case DM_GPSSSTP:
      menu.linkMenuPage(menuPageGPSSingleStep);
      break;
    case DM_GPSEMU_M8:
      menu.linkMenuPage(menuPageGPSEmulateM8);
      break;
    case DM_GPSEMU_M10:
      menu.linkMenuPage(menuPageGPSEmulateM10);
      break;
  }
  // init and enter menu
  menu.setTextColor(TFT_WHITE, TFT_BLACK);
  menuDisplaySleepMode = false;
  menu_displayBrightnessCB();
}

/********************************************************************/
void menu_init() {
  menu_menuModeCB();
}

/********************************************************************/
void menu_enter() {
  menu.resetMenu();
  displayRefresh = true;
}

/********************************************************************/
void menu_exit() {
  // This is an optional exitAction for menuPageMain (top level menu page)
  // You can execute something here to exit to another/master GUI
}

/********************************************************************/
void menu_idle_timer() {
  if(deviceState.DISPLAYTIMEOUT == 99) {
    menuDisplaySleepMode = false;
    return;
  }
  if((millis()-menuInputTime) > (uint32_t)(deviceState.DISPLAYTIMEOUT*MILLIS_PER_MIN)) {
    if(!menuDisplaySleepMode) {
      menuDisplaySleepMode = true;
      //msg_update("Enter Sleep Mode");
      //M5.Lcd.sleep();
      //M5.Axp.SetDCDC3(false);
      //M5.Axp.ScreenBreath(0);
      M5.Lcd.setBrightness(0);
    }
  } else if(menuDisplaySleepMode) {
    menuDisplaySleepMode = false;
    //M5.Lcd.wakeup();
    menu_displayBrightnessCB();
    msg_update("Exit Sleep Mode");
  }
}

/********************************************************************/
void menu_menuModeCB() {
  // choose submenu item based on MENU_MODE
  switch(MENU_MODE) {
    case DM_IDLE:
      menuItemGPSReceiver.hide();
      menuItemGPSLogger.hide();
      menuItemGPSNavSat.hide();
      menuItemGPSNavStat.hide();
      menuItemGPSSatCal.hide();
      menuItemGPSSatCfg.hide();
      menuItemGPSReset.hide();
      menuItemLabel2.show();
      menuItemGPSSingleStep.hide();
      menuItemGPSEmulateM8.hide();
      menuItemGPSEmulateM10.hide();
      menuItemLabel3.show();
      break;
    case DM_GPSRCVR:
      menuItemGPSReceiver.show();
      menuItemGPSLogger.hide();
      menuItemGPSNavSat.hide();
      menuItemGPSNavStat.hide();
      menuItemGPSSatCal.hide();
      menuItemGPSSatCfg.hide();
      menuItemGPSReset.show();
      menuItemLabel2.hide();
      menuItemGPSSingleStep.hide();
      menuItemGPSEmulateM8.hide();
      menuItemGPSEmulateM10.hide();
      menuItemLabel3.hide();
      break;
    case DM_GPSLOGR:
      menuItemGPSReceiver.hide();
      menuItemGPSLogger.show();
      menuItemGPSNavSat.hide();
      menuItemGPSNavStat.hide();
      menuItemGPSSatCal.hide();
      menuItemGPSSatCfg.hide();
      menuItemGPSReset.show();
      menuItemLabel2.hide();
      menuItemGPSSingleStep.hide();
      menuItemGPSEmulateM8.hide();
      menuItemGPSEmulateM10.hide();
      menuItemLabel3.hide();
      menuItemGPSLoggerStrtLog.hide(gpsLoggingInProgress ? true : false);
      menuItemGPSLoggerStopLog.hide(gpsLoggingInProgress ? false : true);
      break;
    case DM_GPSNSAT:
      menuItemGPSReceiver.hide();
      menuItemGPSLogger.hide();
      menuItemGPSNavSat.show();
      menuItemGPSNavStat.hide();
      menuItemGPSSatCal.hide();
      menuItemGPSSatCfg.hide();
      menuItemGPSReset.show();
      menuItemLabel2.hide();
      menuItemGPSSingleStep.hide();
      menuItemGPSEmulateM8.hide();
      menuItemGPSEmulateM10.hide();
      menuItemLabel3.hide();
      break;
    case DM_GPSSTAT:
      menuItemGPSReceiver.hide();
      menuItemGPSLogger.hide();
      menuItemGPSNavSat.hide();
      menuItemGPSNavStat.show();
      menuItemGPSSatCal.hide();
      menuItemGPSSatCfg.hide();
      menuItemGPSReset.show();
      menuItemLabel2.hide();
      menuItemGPSSingleStep.hide();
      menuItemGPSEmulateM8.hide();
      menuItemGPSEmulateM10.hide();
      menuItemLabel3.hide();
      break;
    case DM_GPSSCAL:
      menuItemGPSReceiver.hide();
      menuItemGPSLogger.hide();
      menuItemGPSNavSat.hide();
      menuItemGPSNavStat.hide();
      menuItemGPSSatCal.show();
      menuItemGPSSatCfg.hide();
      menuItemGPSReset.hide();
      menuItemLabel2.show();
      menuItemGPSSingleStep.hide();
      menuItemGPSEmulateM8.hide();
      menuItemGPSEmulateM10.hide();
      menuItemLabel3.hide();
      menuItemGPSSatCalStrt.hide(satCalibrationInProgress ? true : false);;
      menuItemGPSSatCalStop.hide(satCalibrationInProgress ? false : true);;
      break;
    case DM_GPSSCFG:
      menuItemGPSReceiver.hide();
      menuItemGPSLogger.hide();
      menuItemGPSNavSat.hide();
      menuItemGPSNavStat.hide();
      menuItemGPSSatCal.hide();
      menuItemGPSSatCfg.show();
      menuItemGPSReset.hide();
      menuItemLabel2.show();
      menuItemGPSSingleStep.hide();
      menuItemGPSEmulateM8.hide();
      menuItemGPSEmulateM10.hide();
      menuItemLabel3.hide();
      break;
    case DM_GPSSSTP:
      menuItemGPSReceiver.hide();
      menuItemGPSLogger.hide();
      menuItemGPSNavSat.hide();
      menuItemGPSNavStat.hide();
      menuItemGPSSatCal.hide();
      menuItemGPSSatCfg.hide();
      menuItemGPSReset.hide();
      menuItemLabel2.show();
      menuItemGPSSingleStep.show();
      menuItemGPSEmulateM8.hide();
      menuItemGPSEmulateM10.hide();
      menuItemLabel3.hide();
      break;
    case DM_GPSEMU_M8:
      menuItemGPSReceiver.hide();
      menuItemGPSLogger.hide();
      menuItemGPSNavSat.hide();
      menuItemGPSNavStat.hide();
      menuItemGPSSatCal.hide();
      menuItemGPSSatCfg.hide();
      menuItemGPSReset.hide();
      menuItemLabel2.show();
      menuItemGPSSingleStep.hide();
      menuItemGPSEmulateM8.show();
      menuItemGPSEmulateM10.hide();
      menuItemLabel3.hide();
      break;
    case DM_GPSEMU_M10:
      menuItemGPSReceiver.hide();
      menuItemGPSLogger.hide();
      menuItemGPSNavSat.hide();
      menuItemGPSNavStat.hide();
      menuItemGPSSatCal.hide();
      menuItemGPSSatCfg.hide();
      menuItemGPSReset.hide();
      menuItemLabel2.show();
      menuItemGPSSingleStep.hide();
      menuItemGPSEmulateM8.hide();
      menuItemGPSEmulateM10.show();
      menuItemLabel3.hide();
      break;
  }
  if(MENU_MODE==DM_IDLE) {
    menuItemMenuMode.setTitle("Device Mode");
  } else {
    menuItemMenuMode.setTitle("Change Mode");
    menu.resetMenu();
  }
}

/********************************************************************/
void menu_GPSResetCB() {
  menu.resetMenu();
}

/********************************************************************/
void menu_entrGPSReceiverCB() {
  display_processingMsg();
  deviceState.DEVICE_MODE = DM_GPSRCVR;
  deviceMode_init();
  displayRefresh = true;
}

/********************************************************************/
void menu_exitGPSReceiverCB() {
  display_processingMsg();
  deviceMode_end();
  menu.exitToParentMenuPage();
  displayRefresh = true;
}

/********************************************************************/
void menu_entrGPSLoggerCB() {
  display_processingMsg();
  deviceState.DEVICE_MODE = DM_GPSLOGR;
  deviceMode_init();
  displayRefresh = true;
}

/********************************************************************/
void menu_startGPSLoggerCB() {
  if(gpsLoggingInProgress) return;
  if(!gpsEnabled) {
    msg_update("ERROR - GPS Missing");
    return;
  }
  if((deviceState.GPS_LOGUBXMODE == GPS_LOGUBX_NONE) &&
     !(deviceState.GPS_LOGGPX || deviceState.GPS_LOGKML || deviceState.GPS_LOGCSV)) {
    msg_update("GPS Logging Disabled");
    return;
  }
  if((deviceState.GPS_LOGUBXMODE != GPS_LOGUBX_NONE) && !sdcard_openUBXLoggingFile()) {
    msg_update("SD Card Error");
    return;
  }
  if(deviceState.GPS_LOGGPX && !sdcard_openGPXLoggingFile()) {
    msg_update("SD Card Error");
    return;
  }
  if(deviceState.GPS_LOGKML && !sdcard_openKMLLoggingFile()) {
    msg_update("SD Card Error");
    return;
  }
  if(deviceState.GPS_LOGCSV && !sdcard_openCSVLoggingFile()) {
    msg_update("SD Card Error");
    return;
  }
  gpsLoggingInProgress = true;
  menuItemGPSLoggerStrtLog.hide();
  menuItemGPSLoggerStopLog.show();
  msg_update("GPS Logging Started");
}

/********************************************************************/
void menu_stopGPSLoggerCB() {
  char _msgStr[22];
  menuItemGPSLoggerStrtLog.show();
  menuItemGPSLoggerStopLog.hide();
  if(!gpsLoggingInProgress) return;
  gpsLoggingInProgress = false;
  display_processingMsg();
  if(deviceState.GPS_LOGUBXMODE != GPS_LOGUBX_NONE) {
    sprintf(_msgStr, "F%02d TR=%04d LV=%04d",
            ubxLoggingFileNum,
            min(ubxLoggingFileWriteCount, 9999),
            min(ubxLoggingFileWriteLocValidCount, 9999));
    msg_update(_msgStr);
  } else if(deviceState.GPS_LOGGPX) {
    sprintf(_msgStr, "GPX#%02d Records=%04d",
            gpxLoggingFileNum,
            min(gpxLoggingFileWriteCount, 9999));
    msg_update(_msgStr);
  } else if(deviceState.GPS_LOGKML) {
    sprintf(_msgStr, "KML#%02d Records=%04d",
            kmlLoggingFileNum,
            min(kmlLoggingFileWriteCount, 9999));
    msg_update(_msgStr);
  } else if(deviceState.GPS_LOGCSV) {
    sprintf(_msgStr, "CSV#%02d Records=%04d",
            csvLoggingFileNum,
            min(csvLoggingFileWriteCount, 9999));
    msg_update(_msgStr);
  }
  if(deviceState.GPS_LOGUBXMODE != GPS_LOGUBX_NONE) {
    sdcard_closeUBXLoggingFile();
  }
  if(deviceState.GPS_LOGGPX) {
    sdcard_closeGPXLoggingFile();
  }
  if(deviceState.GPS_LOGKML) {
    sdcard_closeKMLLoggingFile();
  }
  if(deviceState.GPS_LOGCSV) {
    sdcard_closeCSVLoggingFile();
  }
  // capture gnss configuration
  gps_writeGNSSConfigFile();
}

/********************************************************************/
void menu_exitGPSLoggerCB() {
  display_processingMsg();
  menu_stopGPSLoggerCB();
  deviceMode_end();
  menu.exitToParentMenuPage();
  displayRefresh = true;
}

/********************************************************************/
void menu_entrGPSNavSatCB() {
  display_processingMsg();
  deviceState.DEVICE_MODE = DM_GPSNSAT;
  deviceMode_init();
  menu_GPSNavSatDisplayMap = false;
  menuItemGPSNavSatToggleView.setTitle("Map View");
  displayRefresh = true;
}

/********************************************************************/
void menu_GPSNavSatToggleViewCB() {
  menu_GPSNavSatDisplayMap = !menu_GPSNavSatDisplayMap;
  menuItemGPSNavSatToggleView.setTitle(
    menu_GPSNavSatDisplayMap ? "Data View" : "Map View");
  displayRefresh = true;
}

/********************************************************************/
void menu_exitGPSNavSatCB() {
  display_processingMsg();
  deviceMode_end();
  menu.exitToParentMenuPage();
  displayRefresh = true;
}

/********************************************************************/
void menu_entrGPSNavStatCB() {
  display_processingMsg();
  deviceState.DEVICE_MODE = DM_GPSSTAT;
  deviceMode_init();
  displayRefresh = true;
}

/********************************************************************/
void menu_exitGPSNavStatCB() {
  display_processingMsg();
  deviceMode_end();
  menu.exitToParentMenuPage();
  displayRefresh = true;
}

/********************************************************************/
void menu_entrGPSSatCalCB() {
  display_processingMsg();
  deviceState.DEVICE_MODE = DM_GPSSCAL;
  deviceMode_init();
  displayRefresh = true;
}

/********************************************************************/
void menu_startGPSSatCalCB() {
  if(satCalibrationInProgress) return;
  if(!satCalibration_enter()) return;
  display_processingMsg();
  menuItemGPSSatCalStrt.hide();
  menuItemGPSSatCalStop.show();
  msg_update("Calibration Started");
}

/********************************************************************/
void menu_stopGPSSatCalCB() {
  menuItemGPSSatCalStrt.show();
  menuItemGPSSatCalStop.hide();
  if(!satCalibrationInProgress) return;
  display_processingMsg();
  satCalibration_exit();
}

/********************************************************************/
void menu_exitGPSSatCalCB() {
  display_processingMsg();
  menu_stopGPSSatCalCB();
  deviceMode_end();
  menu.exitToParentMenuPage();
  displayRefresh = true;
}

/********************************************************************/
void menu_entrGPSSatCfgCB() {
  display_processingMsg();
  deviceState.DEVICE_MODE = DM_GPSSCFG;
  deviceMode_init();
  gnssReadConfig();
  menuUbloxModuleType = gps.getUbloxModuleType();
  displayRefresh = true;
}

/********************************************************************/
void menu_pollGNSSSelectionCB() {
  char _msgStr[22];
  display_processingMsg();
  bool rcode = gps.pollGNSSSelection();
  sprintf(_msgStr, "Poll MON-GNSS rc=%d", rcode);
  msg_update(_msgStr);
  displayRefresh = true;
}

/********************************************************************/
void menu_pollGNSSSatConfigCB() {
  char _msgStr[22];
  display_processingMsg();
  bool rcode = gps.pollGNSSConfig();
  sprintf(_msgStr, "Poll CFG-GNSS rc=%d", rcode);
  msg_update(_msgStr);
}

/********************************************************************/
void menu_gnssCfgGPSToggleCB() {
  display_processingMsg();
  gnssToggleConfig("GPS");
  displayRefresh = true;
}

/********************************************************************/
void menu_gnssCfgSBASToggleCB() {
  display_processingMsg();
  gnssToggleConfig("SBAS");
  displayRefresh = true;
}

/********************************************************************/
void menu_gnssCfgGalileoToggleCB() {
  display_processingMsg();
  gnssToggleConfig("Galileo");
  displayRefresh = true;
}

/********************************************************************/
void menu_gnssCfgBeiDouToggleCB() {
  display_processingMsg();
  gnssToggleConfig("BeiDou");
  displayRefresh = true;
}

/********************************************************************/
void menu_gnssCfgBeiDouB1ToggleCB() {
  display_processingMsg();
  gnssToggleConfig("BeiDou_B1");
  displayRefresh = true;
}

/********************************************************************/
void menu_gnssCfgBeiDouB1CToggleCB() {
  display_processingMsg();
  gnssToggleConfig("BeiDou_B1C");
  displayRefresh = true;
}

/********************************************************************/
void menu_gnssCfgIMESToggleCB() {
  display_processingMsg();
  gnssToggleConfig("IMES");
  displayRefresh = true;
}

/********************************************************************/
void menu_gnssCfgQZSSToggleCB() {
  display_processingMsg();
  gnssToggleConfig("QZSS");
  displayRefresh = true;
}

/********************************************************************/
void menu_gnssCfgQZSSL1CAToggleCB() {
  display_processingMsg();
  gnssToggleConfig("QZSS_L1CA");
  displayRefresh = true;
}

/********************************************************************/
void menu_gnssCfgQZSSL1SToggleCB() {
  display_processingMsg();
  gnssToggleConfig("QZSS_L1S");
  displayRefresh = true;
}

/********************************************************************/
void menu_gnssCfgGLONASSToggleCB() {
  display_processingMsg();
  gnssToggleConfig("GLONASS");
  displayRefresh = true;
}

/********************************************************************/
void menu_exitGPSSatCfgCB() {
  display_processingMsg();
  deviceMode_end();
  menu.exitToParentMenuPage();
  displayRefresh = true;
}

/********************************************************************/
void menu_entrGPSSingleStepCB() {
  deviceState.DEVICE_MODE = DM_GPSSSTP;
  deviceMode_init();
  displayRefresh = true;
}

/********************************************************************/
void menu_sstBeginCB() {
  char _msgStr[22];
  display_processingMsg();
  //gpsSerial->begin(GPS_BAUD_RATE);
#ifdef CONFIG_IDF_TARGET_ESP32S3 // Core S3SE fix using the correct serial pins
  gpsSerial->begin(GPS_BAUD_RATE, SERIAL_8N1, RXD2, TXD2);
#else
  gpsSerial->begin(GPS_BAUD_RATE);
#endif
  bool rcode = gps.gnss_init(*gpsSerial, GPS_BAUD_RATE);
  sprintf(_msgStr, "gnss.begin rc=%d", rcode);
  msg_update(_msgStr);
}

/********************************************************************/
void menu_sstReqMonVerCB() {
  char _msgStr[22];
  uint8_t host_reqMONVER[8]  = {0xb5,0x62,0x0a,0x04,0x00,0x00,0x0e,0x34};
  while(gpsSerial->available()) gpsSerial->read();
  if(!sdcard_openRxPktFile()) return;
  uint32_t _nowMS = millis();
  gpsSerial->write(host_reqMONVER, sizeof(host_reqMONVER));
  while((millis()-_nowMS)<500) {
    sdcard_writeRxPktFile();
  }
  sdcard_closeRxPktFile();
  sprintf(_msgStr, "%s l=%d",
          rxPktFileName,
          min(rxPktWriteCount, 999));
  msg_update(_msgStr);
}

/********************************************************************/
void menu_exitGPSSingleStepCB() {
  deviceMode_end();
  menu.exitToParentMenuPage();
  displayRefresh = true;
}

/********************************************************************/
void menu_entrGPSEmulateM8CB() {
  deviceState.DEVICE_MODE = DM_GPSEMU_M8;
  deviceMode_init();
  displayRefresh = true;
}

/********************************************************************/
void menu_exitGPSEmulateM8CB() {
  deviceMode_end();
  menu.exitToParentMenuPage();
  displayRefresh = true;
}

/********************************************************************/
void menu_entrGPSEmulateM10CB() {
  deviceState.DEVICE_MODE = DM_GPSEMU_M10;
  deviceMode_init();
  displayRefresh = true;
}

/********************************************************************/
void menu_exitGPSEmulateM10CB() {
  deviceMode_end();
  menu.exitToParentMenuPage();
  displayRefresh = true;
}

/********************************************************************/
void menu_getRTC_CB() {
  if(rtc.isValid()) {
    rtc_datetime_t now = rtc.getRTCTime(deviceState.TIMEZONE);
    menuRTCYear    = now.year;
    menuRTCMonth   = now.month;
    menuRTCDay     = now.day;
    menuRTCHour    = now.hour;
    menuRTCMinute  = now.minute;
    menuRTCSecond  = now.second;
  }
}

/********************************************************************/
void menu_setRTC_CB() {
  rtc.setRTCTime(menuRTCYear, menuRTCMonth, menuRTCDay,
                 menuRTCHour, menuRTCMinute, menuRTCSecond,
                 deviceState.TIMEZONE);
  msg_update("RTC Date/Time Set");
}

/********************************************************************/
void menu_cancelDeleteGPSLogFilesCB() {
  menu.exitToParentMenuPage();
  msg_update("Delete Logs Canceled");
}

/********************************************************************/
void menu_confirmDeleteGPSLogFilesCB() {
  char _msgStr[22];
  display_processingMsg();
  menu.exitToParentMenuPage();
  int16_t deleteCount = sdcard_deleteLogFiles();
  if(deleteCount >= 0) {
    sprintf(_msgStr, "Deleted %d Logs", deleteCount);
    msg_update(_msgStr);
  } else {
    msg_update("Delete Logs Failed");
  }
}

/********************************************************************/
void menu_cancelGPSFactoryResetCB() {
  menu.exitToParentMenuPage();
  msg_update("GPS Reset Canceled");
}

/********************************************************************/
void menu_confirmGPSFactoryResetCB() {
  display_processingMsg();
  menu.exitToParentMenuPage();
  gpsSerial = &Serial2;
  if(gps.gnss_init(*gpsSerial, GPS_BAUD_RATE, GPS_NORESET, 0, 0) &&
    gps.factoryReset()) {
    msg_update("GPS Reset Completed");
  } else {
    msg_update("GPS Reset Failed");
  }
}

/********************************************************************/
void menu_displayBrightnessCB() {
  // Make sure screen is turned on
  //M5.Axp.SetDCDC3(true);
  // Set brightness
  //M5.Axp.ScreenBreath(deviceState.DISPLAYBRIGHTNESS);
  M5.Lcd.setBrightness(deviceState.DISPLAYBRIGHTNESS);
}

/********************************************************************/
void menu_cancelSaveSettingsCB() {
  menu.exitToParentMenuPage();
  msg_update("Save Canceled");
}

/********************************************************************/
void menu_confirmSaveSettingsCB() {
  menu.exitToParentMenuPage();
  if(sdcard_deviceStateSave()) {
    msg_update("Settings Saved");
  } else {
    msg_update("Save Settings Failed");
  }
}

/********************************************************************/
void menu_cancelRestartDeviceCB() {
  menu.exitToParentMenuPage();
  msg_update("Restart Canceled");
}

/********************************************************************/
void menu_confirmRestartDeviceCB() {
  device_reset();
}

/********************************************************************/
void menu_cancelResetDeviceCB() {
  menu.exitToParentMenuPage();
  msg_update("Reset Canceled");
}

/********************************************************************/
void menu_confirmResetDeviceCB() {
  sdcard_deviceStateReset();
  device_reset();
}

/********************************************************************/
void display_clearDisplay() {
  display.fillSprite(TFT_BLACK);
}

/********************************************************************/
void display_display() {
  display.pushSprite(0, 0);
}

/********************************************************************/
bool display_setup() {
  // Setup display
  //M5.Lcd.setRotation(1); // M5Display.cpp default
  M5.Lcd.setRotation(2);
  display.setPsram(true); // Use psram
  //display.setColorDepth(1); // Monochrome
  display.setColorDepth(8);
  display.setTextSize(2); // font is 12x16
  display.setTextColor(TFT_WHITE);
  display.createSprite(240, 320); //320x240 rotated 90 degrees
  display_clearDisplay();
  displayEnabled = true;
  return displayEnabled;
}

