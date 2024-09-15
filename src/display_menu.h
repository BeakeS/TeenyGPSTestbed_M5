
/********************************************************************/
// Display
bool displayEnabled;
#include <Wire.h>
#include <TeenyPrtVal.h>
#include <TeenyMenu.h>

/*
TFT_eSprite display = TFT_eSprite(&M5.Lcd);
TeenyPrtVal<TFT_eSprite> displayPV(display);
//TeenyMenu<TFT_eSprite> menu(display, 12, 16, false, 16, 16, 6, 10, 11, 154, 6, 0, 20);
TeenyMenu<TFT_eSprite> menu(display, 12, 16, false, 24, 20, 14, 10, 11, 154, 6, 0, 20);
*/
M5Canvas display(&M5.Lcd);
TeenyPrtVal<M5Canvas> displayPV(display);
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
void deviceMode_init();
void deviceMode_end();

/********************************************************************/
// Menus
//
// menu mode item
void menu_menuModeCB(); // forward declaration
SelectOptionUint8t selectMenuModeOptions[] = {
  {"IDLE",   DM_IDLE},
  {"GPSRCV", DM_GPSRCVR},
  {"GPSLOG", DM_GPSLOGR},
  {"NVSTAT", DM_GPSSTAT},
  {"NAVSAT", DM_GPSNSAT},
  {"SATCFG", DM_GPSSCFG},
//{"GPSSST", DM_GPSSSTP},
  {"EMUM8",  DM_GPSEMU_M8},
  {"EMUM10", DM_GPSEMU_M10}};
TeenyMenuSelect selectMenuMode(sizeof(selectMenuModeOptions)/sizeof(SelectOptionUint8t), selectMenuModeOptions);
TeenyMenuItem menuItemMenuMode("Device Mode", MENU_MODE, selectMenuMode, menu_menuModeCB);
//
// gps receiver unit
//
void menu_entrGPSRcvrCB(); // forward declaration
void menu_exitGPSRcvrCB(); // forward declaration
TeenyMenuPage menuPageGPSRcvr("GPS RECEIVER MODE", menu_entrGPSRcvrCB, menu_exitGPSRcvrCB);
TeenyMenuItem menuItemGPSRcvr("*START GPS RCVR*", menuPageGPSRcvr);
TeenyMenuItem menuItemGPSRcvrExit(false); // optional return menu item
TeenyMenuItem menuItemGPSRcvrLabel0("");
TeenyMenuItem menuItemGPSRcvrLabel1("");
//
// gps logging unit
//
void menu_entrGPSLogrCB(); // forward declaration
void menu_exitGPSLogrCB(); // forward declaration
TeenyMenuPage menuPageGPSLogr("GPS LOGGING MODE", menu_entrGPSLogrCB, menu_exitGPSLogrCB);
TeenyMenuItem menuItemGPSLogr("*START GPS LOGGER*", menuPageGPSLogr);
TeenyMenuItem menuItemGPSLogrExit(false); // optional return menu item
TeenyMenuItem menuItemGPSLogrLabel0("");
TeenyMenuItem menuItemGPSLogrLabel1("");
TeenyMenuItem menuItemGPSLogrLabel2("");
TeenyMenuItem menuItemGPSLogrLabel3("");
TeenyMenuItem menuItemGPSLogrLabel4("");
TeenyMenuItem menuItemGPSLogrLabel5("");
TeenyMenuItem menuItemGPSLogrLabel6("");
TeenyMenuItem menuItemGPSLogrLabel7("");
//
// gps start/stop logging
void menu_startGPSLogrCB(); // forward declaration
TeenyMenuItem menuItemGPSLogrStrtLog("Start GPS Logging", menu_startGPSLogrCB);
void menu_stopGPSLogrCB(); // forward declaration
TeenyMenuItem menuItemGPSLogrStopLog("Stop GPS Logging", menu_stopGPSLogrCB);
//
// gps navstat unit
//
void menu_entrGPSStatCB(); // forward declaration
void menu_exitGPSStatCB(); // forward declaration
TeenyMenuPage menuPageGPSStat("GPS NAVSTAT MODE", menu_entrGPSStatCB, menu_exitGPSStatCB);
TeenyMenuItem menuItemGPSStat("*START GPS NVSTAT*", menuPageGPSStat);
TeenyMenuItem menuItemGPSStatExit(false); // optional return menu item
TeenyMenuItem menuItemGPSStatLabel0("");
//
// gps navsat unit
//
void menu_entrGPSNsatCB(); // forward declaration
void menu_exitGPSNsatCB(); // forward declaration
bool menu_GPSNsatDisplayMap = false;
void menu_GPSNsatToggleViewCB(); // forward declaration
TeenyMenuPage menuPageGPSNsat("GPS NAVSAT MODE", menu_entrGPSNsatCB, menu_exitGPSNsatCB, nullptr, menu_GPSNsatToggleViewCB, menu_GPSNsatToggleViewCB);
TeenyMenuItem menuItemGPSNsat("*START GPS NAVSAT*", menuPageGPSNsat);
TeenyMenuItem menuItemGPSNsatExit(false); // optional return menu item
TeenyMenuItem menuItemGPSNsatLabel0("");
TeenyMenuItem menuItemGPSNsatLabel1("");
TeenyMenuItem menuItemGPSNsatLabel2("");
TeenyMenuItem menuItemGPSNsatLabel3("");
TeenyMenuItem menuItemGPSNsatLabel4("");
TeenyMenuItem menuItemGPSNsatLabel5("");
TeenyMenuItem menuItemGPSNsatLabel6("");
TeenyMenuItem menuItemGPSNsatLabel7("");
TeenyMenuItem menuItemGPSNsatLabel8("");
TeenyMenuItem menuItemGPSNsatLabel9("");
TeenyMenuItem menuItemGPSNsatLabel10("");
TeenyMenuItem menuItemGPSNsatLabel11("");
TeenyMenuItem menuItemGPSNsatLabel12("");
TeenyMenuItem menuItemGPSNsatToggleView("", menu_GPSNsatToggleViewCB);
//
// gps satellite config unit
//
void menu_entrGPSScfgCB(); // forward declaration
void menu_exitGPSScfgCB(); // forward declaration
TeenyMenuPage menuPageGPSScfg("GNSS CONFIG TOOLS", menu_entrGPSScfgCB, menu_exitGPSScfgCB);
TeenyMenuItem menuItemGPSScfg("*START GPS SATCFG*", menuPageGPSScfg);
TeenyMenuItem menuItemGPSScfgExit(false); // optional return menu item
TeenyMenuItem menuItemGPSScfgLabel0("");
TeenyMenuItem menuItemGPSScfgLabel1("");
TeenyMenuItem menuItemGPSScfgLabel2("");
//
// gnss satellite select info
void menu_pollGNSSSelInfoCB(); // forward declaration
TeenyMenuPage menuPageGNSSSelInfo("GNSS SELECT INFO", menu_pollGNSSSelInfoCB);
TeenyMenuItem menuItemGNSSSelInfo("GNSS Select Info", menuPageGNSSSelInfo);
TeenyMenuItem menuItemGNSSSelInfoExit(false); // optional return menu item
TeenyMenuItem menuItemGNSSSelInfoLabel0("");
TeenyMenuItem menuItemGNSSSelInfoLabel1("");
TeenyMenuItem menuItemGNSSSelInfoLabel2("");
TeenyMenuItem menuItemGNSSSelInfoLabel3("");
TeenyMenuItem menuItemGNSSSelInfoLabel4("");
//
// gnss satellite config info
void menu_pollGNSSCfgInfoCB(); // forward declaration
TeenyMenuPage menuPageGNSSCfgInfo("GNSS CONFIG INFO", menu_pollGNSSCfgInfoCB);
TeenyMenuItem menuItemGNSSCfgInfo("GNSS Config Info", menuPageGNSSCfgInfo);
TeenyMenuItem menuItemGNSSCfgInfoExit(false); // optional return menu item
TeenyMenuItem menuItemGNSSCfgInfoLabel0("");
TeenyMenuItem menuItemGNSSCfgInfoLabel1("");
TeenyMenuItem menuItemGNSSCfgInfoLabel2("");
TeenyMenuItem menuItemGNSSCfgInfoLabel3("");
TeenyMenuItem menuItemGNSSCfgInfoLabel4("");
TeenyMenuItem menuItemGNSSCfgInfoLabel5("");
TeenyMenuItem menuItemGNSSCfgInfoLabel6("");
TeenyMenuItem menuItemGNSSCfgInfoLabel7("");
TeenyMenuItem menuItemGNSSCfgInfoLabel8("");
TeenyMenuItem menuItemGNSSCfgInfoLabel9("");
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
// Note:  Toggle menu item titles set by menu_gnssConfigurator()
TeenyMenuItem menuItemGNSSCfgGPSToggle(      "", menu_gnssCfgGPSToggleCB);
TeenyMenuItem menuItemGNSSCfgSBASToggle(     "", menu_gnssCfgSBASToggleCB);
TeenyMenuItem menuItemGNSSCfgGalileoToggle(  "", menu_gnssCfgGalileoToggleCB);
TeenyMenuItem menuItemGNSSCfgBeiDouToggle(   "", menu_gnssCfgBeiDouToggleCB);
TeenyMenuItem menuItemGNSSCfgBeiDouB1Toggle( "", menu_gnssCfgBeiDouB1ToggleCB);
TeenyMenuItem menuItemGNSSCfgBeiDouB1CToggle("", menu_gnssCfgBeiDouB1CToggleCB);
TeenyMenuItem menuItemGNSSCfgIMESToggle(     "", menu_gnssCfgIMESToggleCB);
TeenyMenuItem menuItemGNSSCfgQZSSToggle(     "", menu_gnssCfgQZSSToggleCB);
TeenyMenuItem menuItemGNSSCfgQZSSL1CAToggle( "", menu_gnssCfgQZSSL1CAToggleCB);
TeenyMenuItem menuItemGNSSCfgQZSSL1SToggle(  "", menu_gnssCfgQZSSL1SToggleCB);
TeenyMenuItem menuItemGNSSCfgGLONASSToggle(  "", menu_gnssCfgGLONASSToggleCB);
//
// gps single step unit
//
void menu_entrGPSStepCB(); // forward declaration
void menu_exitGPSStepCB(); // forward declaration
TeenyMenuPage menuPageGPSStep("GPS SINGLESTEP MODE", menu_entrGPSStepCB, menu_exitGPSStepCB);
TeenyMenuItem menuItemGPSStep("*START GPS STEPPER*", menuPageGPSStep);
TeenyMenuItem menuItemGPSStepExit(false); // optional return menu item
void menu_sstBeginCB(); // forward declaration
TeenyMenuItem menuItemGPSStepBegin("gnss.begin", menu_sstBeginCB);
void menu_sstReqMonVerCB(); // forward declaration
TeenyMenuItem menuItemGPSStepReqMonVer("reqMonVer_CMD", menu_sstReqMonVerCB);
//
// gps ublox M8 emulator unit
//
void menu_entrGPSEmuM8CB(); // forward declaration
void menu_exitGPSEmuM8CB(); // forward declaration
TeenyMenuPage menuPageGPSEmuM8("UBLOX M8 EMULATOR", menu_entrGPSEmuM8CB, menu_exitGPSEmuM8CB);
TeenyMenuItem menuItemGPSEmuM8("*START EMULATOR*", menuPageGPSEmuM8);
TeenyMenuItem menuItemGPSEmuM8Exit(false); // optional return menu item
TeenyMenuItem menuItemGPSEmuM8Label0("");
TeenyMenuItem menuItemGPSEmuM8Label1("");
//
// gps ublox M10 emulator unit
//
void menu_entrGPSEmuM10CB(); // forward declaration
void menu_exitGPSEmuM10CB(); // forward declaration
TeenyMenuPage menuPageGPSEmuM10("UBLOX M10 EMULATOR", menu_entrGPSEmuM10CB, menu_exitGPSEmuM10CB);
TeenyMenuItem menuItemGPSEmuM10("*START EMULATOR*", menuPageGPSEmuM10);
TeenyMenuItem menuItemGPSEmuM10Exit(false); // optional return menu item
TeenyMenuItem menuItemGPSEmuM10Label0("");
TeenyMenuItem menuItemGPSEmuM10Label1("");
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
TeenyMenuItem menuItemGPSReset("GPS Reset", deviceState.GPSRESET, selectGPSReset, menu_GPSResetCB);
//
// TEENYGPSTESTBED SETTINGS
//
// settings menu
TeenyMenuPage menuPageTopLevelSettings("SETTINGS MENU");
TeenyMenuItem menuItemTopLevelSettings("Device Settings", menuPageTopLevelSettings);
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
// ubx logging mode
SelectOptionUint8t selectUBXPktLogModeOptions[] = {
  {"NAVPVT", UBXPKTLOG_NAVPVT},
  {"STATUS", UBXPKTLOG_NAVSTATUS},
  {"NAVSAT", UBXPKTLOG_NAVSAT},
  {"ALL",    UBXPKTLOG_ALL}};
TeenyMenuSelect selectUBXPktLogMode(sizeof(selectUBXPktLogModeOptions)/sizeof(SelectOptionUint8t), selectUBXPktLogModeOptions);
TeenyMenuItem menuItemUBXPktLogMode("UBX Pkt Log", deviceState.UBXPKTLOGMODE, selectUBXPktLogMode);
//
// gps logging mode
SelectOptionUint8t selectGPSLogModeOptions[] = {
  {"NONE", GPSLOG_NONE},
  {"GPX",  GPSLOG_GPX},
  {"KML",  GPSLOG_KML}};
TeenyMenuSelect selectGPSLogMode(sizeof(selectGPSLogModeOptions)/sizeof(SelectOptionUint8t), selectGPSLogModeOptions);
TeenyMenuItem menuItemGPSLogMode("GPS Logging", deviceState.GPSLOGMODE, selectGPSLogMode);
//
// gps factory reset menu
void menu_cancelGPSFactoryResetCB(); // forward declaration
TeenyMenuPage menuPageGPSFactoryReset("GPS FACTORY RESET", nullptr, menu_cancelGPSFactoryResetCB);
TeenyMenuItem menuItemGPSFactoryReset("GPS Factory Reset", menuPageGPSFactoryReset);
TeenyMenuItem menuItemGPSFactoryResetExit(false); // optional return menu item
//
// confirm gps reset
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
SelectOptionUint8t selectEMUUBXPktSourceOptions[] = {
  {"PROG",   EMU_PGMINPUT},
  {"SDCARD", EMU_SDCINPUT}};
TeenyMenuSelect selectEMUUBXPktSource(sizeof(selectEMUUBXPktSourceOptions)/sizeof(SelectOptionUint8t), selectEMUUBXPktSourceOptions);
TeenyMenuItem menuItemEMUUBXPktSource("UBX Pkt Src", deviceState.EMUL_UBXPKTSOURCE, selectEMUUBXPktSource);
//
// emulator cold start packet count
uint8_t menuColdStartPktsMin = 0;
uint8_t menuColdStartPktsMax = 60;
TeenyMenuItem menuItemColdStartPkts("ColdStrtPkts", deviceState.EMUL_NUMCOLDSTARTPACKETS, menuColdStartPktsMin, menuColdStartPktsMax);
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
  menuPageMain.addMenuItem(menuItemGPSRcvr);
  menuPageGPSRcvr.addMenuItem(menuItemGPSRcvrLabel0);
  menuPageGPSRcvr.addMenuItem(menuItemGPSRcvrLabel1);
  //menuPageBaseUnit.addMenuItem(menuItemGPSRcvrExit);
  menuPageMain.addMenuItem(menuItemGPSLogr);
  menuPageGPSLogr.addMenuItem(menuItemGPSLogrLabel0);
  menuPageGPSLogr.addMenuItem(menuItemGPSLogrLabel1);
  menuPageGPSLogr.addMenuItem(menuItemGPSLogrLabel2);
  menuPageGPSLogr.addMenuItem(menuItemGPSLogrLabel3);
  menuPageGPSLogr.addMenuItem(menuItemGPSLogrLabel4);
  menuPageGPSLogr.addMenuItem(menuItemGPSLogrLabel5);
  menuPageGPSLogr.addMenuItem(menuItemGPSLogrLabel6);
  menuPageGPSLogr.addMenuItem(menuItemGPSLogrLabel7);
  menuPageGPSLogr.addMenuItem(menuItemGPSLogrStrtLog);
  menuPageGPSLogr.addMenuItem(menuItemGPSLogrStopLog);
  //menuPageGPSLogr.addMenuItem(menuItemGPSLogrExit);
  menuPageMain.addMenuItem(menuItemGPSStat);
  menuPageGPSStat.addMenuItem(menuItemGPSStatLabel0);
  //menuPageGPSStat.addMenuItem(menuItemGPSStatExit);
  menuPageMain.addMenuItem(menuItemGPSNsat);
  menuPageGPSNsat.addMenuItem(menuItemGPSNsatLabel0);
  menuPageGPSNsat.addMenuItem(menuItemGPSNsatLabel1);
  menuPageGPSNsat.addMenuItem(menuItemGPSNsatLabel2);
  menuPageGPSNsat.addMenuItem(menuItemGPSNsatLabel3);
  menuPageGPSNsat.addMenuItem(menuItemGPSNsatLabel4);
  menuPageGPSNsat.addMenuItem(menuItemGPSNsatLabel5);
  menuPageGPSNsat.addMenuItem(menuItemGPSNsatLabel6);
  menuPageGPSNsat.addMenuItem(menuItemGPSNsatLabel7);
  menuPageGPSNsat.addMenuItem(menuItemGPSNsatLabel8);
  menuPageGPSNsat.addMenuItem(menuItemGPSNsatLabel9);
  menuPageGPSNsat.addMenuItem(menuItemGPSNsatLabel10);
  menuPageGPSNsat.addMenuItem(menuItemGPSNsatLabel11);
  menuPageGPSNsat.addMenuItem(menuItemGPSNsatLabel12);
  menuPageGPSNsat.addMenuItem(menuItemGPSNsatToggleView);
  //menuPageGPSNsat.addMenuItem(menuItemGPSNsatExit);
  menuPageMain.addMenuItem(menuItemGPSScfg);
  //menuPageGPSScfg.addMenuItem(menuItemGPSScfgLabel0);
  menuPageGPSScfg.addMenuItem(menuItemGNSSSelInfo);
  menuPageGNSSSelInfo.addMenuItem(menuItemGNSSSelInfoLabel0);
  menuPageGNSSSelInfo.addMenuItem(menuItemGNSSSelInfoLabel1);
  menuPageGNSSSelInfo.addMenuItem(menuItemGNSSSelInfoLabel2);
  menuPageGNSSSelInfo.addMenuItem(menuItemGNSSSelInfoLabel3);
  menuPageGNSSSelInfo.addMenuItem(menuItemGNSSSelInfoLabel4);
  menuPageGNSSSelInfo.addMenuItem(menuItemGNSSSelInfoExit);
  menuPageGPSScfg.addMenuItem(menuItemGNSSCfgInfo);
  menuPageGNSSCfgInfo.addMenuItem(menuItemGNSSCfgInfoLabel0);
  menuPageGNSSCfgInfo.addMenuItem(menuItemGNSSCfgInfoLabel1);
  menuPageGNSSCfgInfo.addMenuItem(menuItemGNSSCfgInfoLabel2);
  menuPageGNSSCfgInfo.addMenuItem(menuItemGNSSCfgInfoLabel3);
  menuPageGNSSCfgInfo.addMenuItem(menuItemGNSSCfgInfoLabel4);
  menuPageGNSSCfgInfo.addMenuItem(menuItemGNSSCfgInfoLabel5);
  menuPageGNSSCfgInfo.addMenuItem(menuItemGNSSCfgInfoLabel6);
  menuPageGNSSCfgInfo.addMenuItem(menuItemGNSSCfgInfoLabel7);
  menuPageGNSSCfgInfo.addMenuItem(menuItemGNSSCfgInfoLabel8);
  menuPageGNSSCfgInfo.addMenuItem(menuItemGNSSCfgInfoLabel9);
  menuPageGNSSCfgInfo.addMenuItem(menuItemGNSSCfgInfoExit);
  menuPageGPSScfg.addMenuItem(menuItemGPSScfgLabel1);
  menuPageGPSScfg.addMenuItem(menuItemGNSSCfgGPSToggle);
  menuPageGPSScfg.addMenuItem(menuItemGNSSCfgSBASToggle);
  menuPageGPSScfg.addMenuItem(menuItemGNSSCfgGalileoToggle);
  menuPageGPSScfg.addMenuItem(menuItemGNSSCfgBeiDouToggle);
  menuPageGPSScfg.addMenuItem(menuItemGNSSCfgBeiDouB1Toggle);
  menuPageGPSScfg.addMenuItem(menuItemGNSSCfgBeiDouB1CToggle);
  menuPageGPSScfg.addMenuItem(menuItemGNSSCfgIMESToggle);
  menuPageGPSScfg.addMenuItem(menuItemGNSSCfgQZSSToggle);
  menuPageGPSScfg.addMenuItem(menuItemGNSSCfgQZSSL1CAToggle);
  menuPageGPSScfg.addMenuItem(menuItemGNSSCfgQZSSL1SToggle);
  menuPageGPSScfg.addMenuItem(menuItemGNSSCfgGLONASSToggle);
  //menuPageGPSScfg.addMenuItem(menuItemGPSScfgLabel2);
  menuPageGPSScfg.addMenuItem(menuItemGPSScfgExit);
  menuPageMain.addMenuItem(menuItemGPSStep);
  menuPageGPSStep.addMenuItem(menuItemGPSStepBegin);
  menuPageGPSStep.addMenuItem(menuItemGPSStepReqMonVer);
  //menuPageGPSStep.addMenuItem(menuItemGPSStepExit);
  menuPageMain.addMenuItem(menuItemGPSEmuM8);
  menuPageGPSEmuM8.addMenuItem(menuItemGPSEmuM8Label0);
  menuPageGPSEmuM8.addMenuItem(menuItemGPSEmuM8Label1);
  //menuPageGPSEmuM8.addMenuItem(menuItemGPSEmuM8Exit);
  menuPageMain.addMenuItem(menuItemGPSEmuM10);
  menuPageGPSEmuM10.addMenuItem(menuItemGPSEmuM10Label0);
  menuPageGPSEmuM10.addMenuItem(menuItemGPSEmuM10Label1);
  //menuPageGPSEmuM10.addMenuItem(menuItemGPSEmuM10Exit);
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
  menuPageGPSSettings.addMenuItem(menuItemUBXPktLogMode);
  menuPageGPSSettings.addMenuItem(menuItemGPSLogMode);
  menuPageGPSSettings.addMenuItem(menuItemGPSFactoryReset);
  menuPageGPSFactoryReset.addMenuItem(menuItemConfirmGPSFactoryReset);
  menuPageGPSFactoryReset.addMenuItem(menuItemGPSFactoryResetExit);
  menuPageGPSSettings.addMenuItem(menuItemGPSSettingsExit); // optional return menu item
  menuPageTopLevelSettings.addMenuItem(menuItemEMULSettings);
  menuPageEMULSettings.addMenuItem(menuItemEMUUBXPktSource);
  menuPageEMULSettings.addMenuItem(menuItemColdStartPkts);
  menuPageEMULSettings.addMenuItem(menuItemEMULSettingsExit); // optional return menu item
  menuPageTopLevelSettings.addMenuItem(menuItemDisplayBrightness);
  menuPageTopLevelSettings.addMenuItem(menuItemDisplayTimeout);
  //menuPageTopLevelSettings.addMenuItem(menuItemStatusLED);
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
      menu.linkMenuPage(menuPageGPSRcvr);
      break;
    case DM_GPSLOGR:
      menu.linkMenuPage(menuPageGPSLogr);
      break;
    case DM_GPSSTAT:
      menu.linkMenuPage(menuPageGPSStat);
      break;
    case DM_GPSNSAT:
      menu.linkMenuPage(menuPageGPSNsat);
      break;
    case DM_GPSSCFG:
      menu.linkMenuPage(menuPageGPSScfg);
      break;
    case DM_GPSSSTP:
      menu.linkMenuPage(menuPageGPSStep);
      break;
    case DM_GPSEMU_M8:
      menu.linkMenuPage(menuPageGPSEmuM8);
      break;
    case DM_GPSEMU_M10:
      menu.linkMenuPage(menuPageGPSEmuM10);
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
      menuItemGPSRcvr.hide();
      menuItemGPSLogr.hide();
      menuItemGPSStat.hide();
      menuItemGPSNsat.hide();
      menuItemGPSScfg.hide();
      menuItemGPSReset.hide();
      menuItemLabel2.show();
      menuItemGPSStep.hide();
      menuItemGPSEmuM8.hide();
      menuItemGPSEmuM10.hide();
      menuItemLabel3.show();
      break;
    case DM_GPSRCVR:
      menuItemGPSRcvr.show();
      menuItemGPSLogr.hide();
      menuItemGPSStat.hide();
      menuItemGPSNsat.hide();
      menuItemGPSScfg.hide();
      menuItemGPSReset.show();
      menuItemLabel2.hide();
      menuItemGPSStep.hide();
      menuItemGPSEmuM8.hide();
      menuItemGPSEmuM10.hide();
      menuItemLabel3.hide();
      break;
    case DM_GPSLOGR:
      menuItemGPSRcvr.hide();
      menuItemGPSLogr.show();
      menuItemGPSStat.hide();
      menuItemGPSNsat.hide();
      menuItemGPSScfg.hide();
      menuItemGPSReset.show();
      menuItemLabel2.hide();
      menuItemGPSStep.hide();
      menuItemGPSEmuM8.hide();
      menuItemGPSEmuM10.hide();
      menuItemLabel3.hide();
      menuItemGPSLogrStrtLog.hide(ubxLoggingInProgress ? true : false);
      menuItemGPSLogrStopLog.hide(ubxLoggingInProgress ? false : true);
      break;
    case DM_GPSSTAT:
      menuItemGPSRcvr.hide();
      menuItemGPSLogr.hide();
      menuItemGPSStat.show();
      menuItemGPSNsat.hide();
      menuItemGPSScfg.hide();
      menuItemGPSReset.show();
      menuItemLabel2.hide();
      menuItemGPSStep.hide();
      menuItemGPSEmuM8.hide();
      menuItemGPSEmuM10.hide();
      menuItemLabel3.hide();
      break;
    case DM_GPSNSAT:
      menuItemGPSRcvr.hide();
      menuItemGPSLogr.hide();
      menuItemGPSStat.hide();
      menuItemGPSNsat.show();
      menuItemGPSScfg.hide();
      menuItemGPSReset.show();
      menuItemLabel2.hide();
      menuItemGPSStep.hide();
      menuItemGPSEmuM8.hide();
      menuItemGPSEmuM10.hide();
      menuItemLabel3.hide();
      break;
    case DM_GPSSCFG:
      menuItemGPSRcvr.hide();
      menuItemGPSLogr.hide();
      menuItemGPSStat.hide();
      menuItemGPSNsat.hide();
      menuItemGPSScfg.show();
      menuItemGPSReset.show();
      menuItemLabel2.hide();
      menuItemGPSStep.hide();
      menuItemGPSEmuM8.hide();
      menuItemGPSEmuM10.hide();
      menuItemLabel3.hide();
      break;
    case DM_GPSSSTP:
      menuItemGPSRcvr.hide();
      menuItemGPSLogr.hide();
      menuItemGPSStat.hide();
      menuItemGPSNsat.hide();
      menuItemGPSScfg.hide();
      menuItemGPSReset.hide();
      menuItemLabel2.show();
      menuItemGPSStep.show();
      menuItemGPSEmuM8.hide();
      menuItemGPSEmuM10.hide();
      menuItemLabel3.hide();
      break;
    case DM_GPSEMU_M8:
      menuItemGPSRcvr.hide();
      menuItemGPSLogr.hide();
      menuItemGPSStat.hide();
      menuItemGPSNsat.hide();
      menuItemGPSScfg.hide();
      menuItemGPSReset.hide();
      menuItemLabel2.show();
      menuItemGPSStep.hide();
      menuItemGPSEmuM8.show();
      menuItemGPSEmuM10.hide();
      menuItemLabel3.hide();
      break;
    case DM_GPSEMU_M10:
      menuItemGPSRcvr.hide();
      menuItemGPSLogr.hide();
      menuItemGPSStat.hide();
      menuItemGPSNsat.hide();
      menuItemGPSScfg.hide();
      menuItemGPSReset.hide();
      menuItemLabel2.show();
      menuItemGPSStep.hide();
      menuItemGPSEmuM8.hide();
      menuItemGPSEmuM10.show();
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
void menu_entrGPSRcvrCB() {
  deviceState.DEVICE_MODE = DM_GPSRCVR;
  deviceMode_init();
  displayRefresh = true;
}

/********************************************************************/
void menu_exitGPSRcvrCB() {
  deviceMode_end();
  menu.exitToParentMenuPage();
  displayRefresh = true;
}

/********************************************************************/
void menu_entrGPSLogrCB() {
  deviceState.DEVICE_MODE = DM_GPSLOGR;
  deviceMode_init();
  displayRefresh = true;
}

/********************************************************************/
void menu_startGPSLogrCB() {
  if(ubxLoggingInProgress) return;
  if(sdcard_openUBXLoggingFile()) {
    ubxLoggingInProgress = true;
    menuItemGPSLogrStrtLog.hide();
    menuItemGPSLogrStopLog.show();
    msg_update("GPS Logging Started");
  } else {
    msg_update("SD Card Error");
  }
  if((deviceState.GPSLOGMODE == GPSLOG_GPX) && !sdcard_openGPXLoggingFile()) {
    msg_update("SD Card Error");
  }
  if((deviceState.GPSLOGMODE == GPSLOG_KML) && !sdcard_openKMLLoggingFile()) {
    msg_update("SD Card Error");
  }
  displayRefresh = true;
}

/********************************************************************/
void menu_stopGPSLogrCB() {
  char _msgStr[22];
  if(!ubxLoggingInProgress) return;
  sdcard_closeUBXLoggingFile();
  ubxLoggingInProgress = false;
  menuItemGPSLogrStrtLog.show();
  menuItemGPSLogrStopLog.hide();
  if(deviceState.GPSLOGMODE == GPSLOG_GPX) {
    sdcard_closeGPXLoggingFile();
  }
  if(deviceState.GPSLOGMODE == GPSLOG_KML) {
    sdcard_closeKMLLoggingFile();
  }
  sprintf(_msgStr, "F%02d TP=%04d VP=%04d",
          ubxLoggingFileNum,
          min(ubxLoggingFileWriteCount, 9999),
          min(ubxLoggingFileWriteValidCount, 9999));
  msg_update(_msgStr);
  displayRefresh = true;
}

/********************************************************************/
void menu_exitGPSLogrCB() {
  menu_stopGPSLogrCB();
  deviceMode_end();
  menu.exitToParentMenuPage();
  displayRefresh = true;
}

/********************************************************************/
void menu_entrGPSStatCB() {
  deviceState.DEVICE_MODE = DM_GPSSTAT;
  deviceMode_init();
  displayRefresh = true;
}

/********************************************************************/
void menu_exitGPSStatCB() {
  deviceMode_end();
  menu.exitToParentMenuPage();
  displayRefresh = true;
}

/********************************************************************/
void menu_entrGPSNsatCB() {
  deviceState.DEVICE_MODE = DM_GPSNSAT;
  deviceMode_init();
  menu_GPSNsatDisplayMap = false;
  menuItemGPSNsatToggleView.setTitle("Map View");
  displayRefresh = true;
}

/********************************************************************/
void menu_GPSNsatToggleViewCB() {
  menu_GPSNsatDisplayMap = !menu_GPSNsatDisplayMap;
  menuItemGPSNsatToggleView.setTitle(
    menu_GPSNsatDisplayMap ? "Data View" : "Map View");
  displayRefresh = true;
}

/********************************************************************/
void menu_exitGPSNsatCB() {
  deviceMode_end();
  menu.exitToParentMenuPage();
  displayRefresh = true;
}

/********************************************************************/
void menu_gnssConfigurator(const char toggleGnssIdType, const char* toggleGnssSigName) {
  char _msgStr[22];
  // GNSS State - Unknown=-1, Disabled=0, Enabled=1
  typedef struct {
    int8_t GPS = -1;
    int8_t SBAS = -1;
    int8_t Galileo = -1;
    int8_t BeiDou = -1;
    int8_t BeiDouB1 = -1;
    int8_t BeiDouB1C = -1;
    int8_t IMES = -1;
    int8_t QZSS = -1;
    int8_t QZSSL1CA = -1;
    int8_t QZSSL1S = -1;
    int8_t GLONASS = -1;
    int8_t spare00;
  } gnss_state_t;
  gnss_state_t gnssState;
  // Reset State
  // Poll GNSS config state
  bool pollRC = gps.pollGNSSConfigInfo();
  uint8_t ubloxModuleType = gps.getUbloxModuleType();
  ubloxCFGGNSSInfo_t ubloxCFGGNSSInfo = gps.getGNSSConfigInfo();
  menuItemGNSSCfgIMESToggle.hide((ubloxModuleType == UBLOX_M8_MODULE) ? false : true);
  menuItemGNSSCfgBeiDouB1Toggle.hide((ubloxModuleType == UBLOX_M10_MODULE) ? false : true);
  menuItemGNSSCfgBeiDouB1CToggle.hide((ubloxModuleType == UBLOX_M10_MODULE) ? false : true);
  menuItemGNSSCfgQZSSL1CAToggle.hide((ubloxModuleType == UBLOX_M10_MODULE) ? false : true);
  menuItemGNSSCfgQZSSL1SToggle.hide((ubloxModuleType == UBLOX_M10_MODULE) ? false : true);
  uint8_t gnssId;
  char gnssIdType;
  uint8_t gnssEnable;
  uint8_t gnssSignalEnable;
  bool toggleRC;
  // Toggle selected GNSS
  if(pollRC) {
    if(ubloxModuleType == UBLOX_M8_MODULE) {
      for(uint8_t i=0; i<min(ubloxCFGGNSSInfo.M8.numConfigBlocks, 7); i++) {
        gnssId     = ubloxCFGGNSSInfo.M8.configBlockList[i].gnssId;
        gnssIdType = ubloxCFGGNSSInfo.M8.configBlockList[i].gnssIdType;
        gnssEnable = ubloxCFGGNSSInfo.M8.configBlockList[i].enable;
        // Toggle selected GNSS enable
        if(toggleGnssIdType == gnssIdType) {
          toggleRC = gps.setGNSSConfig(gnssId, !gnssEnable);
          if(toggleRC) gnssEnable = !gnssEnable;
          sprintf(_msgStr, "Set CFG-GNSS rc=%d", toggleRC);
          msg_update(_msgStr);
        }
        switch(gnssIdType) {
          case  'G':
            gnssState.GPS = gnssEnable;
            break;
          case  'S':
            gnssState.SBAS = gnssEnable;
            break;
          case  'E':
            gnssState.Galileo = gnssEnable;
            break;
          case  'B':
            gnssState.BeiDou = gnssEnable;
            break;
          case  'I':
            gnssState.IMES = gnssEnable;
            break;
          case  'Q':
            gnssState.QZSS = gnssEnable;
            break;
          case  'R':
            gnssState.GLONASS = gnssEnable;
            break;
        }
      }
    } else if((ubloxModuleType == UBLOX_M9_MODULE) || (ubloxModuleType == UBLOX_M10_MODULE)) {
      for(uint8_t i=0; i<min(ubloxCFGGNSSInfo.M10.numConfigBlocks, 6); i++) {
        gnssId     = ubloxCFGGNSSInfo.M10.configBlockList[i].gnssId;
        gnssIdType = ubloxCFGGNSSInfo.M10.configBlockList[i].gnssIdType;
        gnssEnable = ubloxCFGGNSSInfo.M10.configBlockList[i].enable;
        // Toggle selected GNSS enable
        if(toggleGnssIdType == gnssIdType) {
          if(toggleGnssSigName == "") {
            toggleRC = gps.setGNSSConfig(gnssId, !gnssEnable);
            if(toggleRC) gnssEnable = !gnssEnable;
            sprintf(_msgStr, "Set CFG-GNSS rc=%d", toggleRC);
            msg_update(_msgStr);
          } else {
            for(uint8_t j=0; j<ubloxCFGGNSSInfo.M10.configBlockList[i].numSigs; j++) {
              if(strcmp(ubloxCFGGNSSInfo.M10.configBlockList[i].signalList[j].name, toggleGnssSigName)==0) {
                gnssSignalEnable = ubloxCFGGNSSInfo.M10.configBlockList[i].signalList[j].enable;
                toggleRC = gps.setGNSSSignalConfig(gnssId, toggleGnssSigName, !gnssSignalEnable);
                if(toggleRC) {
                  ubloxCFGGNSSInfo.M10.configBlockList[i].signalList[j].enable = !gnssSignalEnable;
                }
                sprintf(_msgStr, "Set CFG-GNSS rc=%d", toggleRC);
                msg_update(_msgStr);
              }
            }
          }
        }
        switch(gnssIdType) {
          case  'G':
            gnssState.GPS = gnssEnable;
            break;
          case  'S':
            gnssState.SBAS = gnssEnable;
            break;
          case  'E':
            gnssState.Galileo = gnssEnable;
            break;
          case  'B':
            gnssState.BeiDou = gnssEnable;
            for(uint8_t j=0; j<ubloxCFGGNSSInfo.M10.configBlockList[i].numSigs; j++) {
              if(strcmp(ubloxCFGGNSSInfo.M10.configBlockList[i].signalList[j].name, "B1")==0) {
                gnssState.BeiDouB1 = ubloxCFGGNSSInfo.M10.configBlockList[i].signalList[j].enable;
              } else if(strcmp(ubloxCFGGNSSInfo.M10.configBlockList[i].signalList[j].name, "B1C")==0) {
                gnssState.BeiDouB1C = ubloxCFGGNSSInfo.M10.configBlockList[i].signalList[j].enable;
              }
            }
            //gnssState.BeiDouB1 = ubloxCFGGNSSInfo.M10.configBlockList[i].signalList[0].enable;
            //gnssState.BeiDouB1C = ubloxCFGGNSSInfo.M10.configBlockList[i].signalList[1].enable;
            break;
          case  'Q':
            gnssState.QZSS = gnssEnable;
            for(uint8_t j=0; j<ubloxCFGGNSSInfo.M10.configBlockList[i].numSigs; j++) {
              if(strcmp(ubloxCFGGNSSInfo.M10.configBlockList[i].signalList[j].name, "L1CA")==0) {
                gnssState.QZSSL1CA = ubloxCFGGNSSInfo.M10.configBlockList[i].signalList[j].enable;
              } else if(strcmp(ubloxCFGGNSSInfo.M10.configBlockList[i].signalList[j].name, "L1S")==0) {
                gnssState.QZSSL1S = ubloxCFGGNSSInfo.M10.configBlockList[i].signalList[j].enable;
              }
            }
            break;
          case  'R':
            gnssState.GLONASS = gnssEnable;
            break;
        }
      }
    }
  }
  if((!pollRC) || toggleGnssIdType == '\0') {
    sprintf(_msgStr, "Poll CFG-GNSS rc=%d", pollRC);
    msg_update(_msgStr);
  }
  // Update toggle menu item titles
  // Note: setTitle() only accepts const char*
  switch(gnssState.GPS) {
    case -1: menuItemGNSSCfgGPSToggle.setTitle("GPS    :Unknown"); break;
    case  0: menuItemGNSSCfgGPSToggle.setTitle("GPS    :Disabled"); break;
    case  1: menuItemGNSSCfgGPSToggle.setTitle("GPS    :Enabled"); break;
  }
  switch(gnssState.SBAS) {
    case -1: menuItemGNSSCfgSBASToggle.setTitle("SBAS   :Unknown"); break;
    case  0: menuItemGNSSCfgSBASToggle.setTitle("SBAS   :Disabled"); break;
    case  1: menuItemGNSSCfgSBASToggle.setTitle("SBAS   :Enabled"); break;
  }
  switch(gnssState.Galileo) {
    case -1: menuItemGNSSCfgGalileoToggle.setTitle("Galileo:Unknown"); break;
    case  0: menuItemGNSSCfgGalileoToggle.setTitle("Galileo:Disabled"); break;
    case  1: menuItemGNSSCfgGalileoToggle.setTitle("Galileo:Enabled"); break;
  }
  switch(gnssState.BeiDou) {
    case -1: menuItemGNSSCfgBeiDouToggle.setTitle("BeiDou :Unknown"); break;
    case  0: menuItemGNSSCfgBeiDouToggle.setTitle("BeiDou :Disabled"); break;
    case  1: menuItemGNSSCfgBeiDouToggle.setTitle("BeiDou :Enabled"); break;
  }
  switch(gnssState.BeiDouB1) {
    case -1: menuItemGNSSCfgBeiDouB1Toggle.setTitle(" B_B1  :Unknown"); break;
    case  0: menuItemGNSSCfgBeiDouB1Toggle.setTitle(" B_B1  :Disabled"); break;
    case  1: menuItemGNSSCfgBeiDouB1Toggle.setTitle(" B_B1  :Enabled"); break;
  }
  switch(gnssState.BeiDouB1C) {
    case -1: menuItemGNSSCfgBeiDouB1CToggle.setTitle(" B_B1C :Unknown"); break;
    case  0: menuItemGNSSCfgBeiDouB1CToggle.setTitle(" B_B1C :Disabled"); break;
    case  1: menuItemGNSSCfgBeiDouB1CToggle.setTitle(" B_B1C :Enabled"); break;
  }
  switch(gnssState.IMES) {
    case -1: menuItemGNSSCfgIMESToggle.setTitle("IMES   :Unknown"); break;
    case  0: menuItemGNSSCfgIMESToggle.setTitle("IMES   :Disabled"); break;
    case  1: menuItemGNSSCfgIMESToggle.setTitle("IMES   :Enabled"); break;
  }
  switch(gnssState.QZSS) {
    case -1: menuItemGNSSCfgQZSSToggle.setTitle("QZSS   :Unknown"); break;
    case  0: menuItemGNSSCfgQZSSToggle.setTitle("QZSS   :Disabled"); break;
    case  1: menuItemGNSSCfgQZSSToggle.setTitle("QZSS   :Enabled"); break;
  }
  switch(gnssState.QZSSL1CA) {
    case -1: menuItemGNSSCfgQZSSL1CAToggle.setTitle(" Q_L1CA:Unknown"); break;
    case  0: menuItemGNSSCfgQZSSL1CAToggle.setTitle(" Q_L1CA:Disabled"); break;
    case  1: menuItemGNSSCfgQZSSL1CAToggle.setTitle(" Q_L1CA:Enabled"); break;
  }
  switch(gnssState.QZSSL1S) {
    case -1: menuItemGNSSCfgQZSSL1SToggle.setTitle(" Q_L1S :Unknown"); break;
    case  0: menuItemGNSSCfgQZSSL1SToggle.setTitle(" Q_L1S :Disabled"); break;
    case  1: menuItemGNSSCfgQZSSL1SToggle.setTitle(" Q_L1S :Enabled"); break;
  }
  switch(gnssState.GLONASS) {
    case -1: menuItemGNSSCfgGLONASSToggle.setTitle("GLONASS:Unknown"); break;
    case  0: menuItemGNSSCfgGLONASSToggle.setTitle("GLONASS:Disabled"); break;
    case  1: menuItemGNSSCfgGLONASSToggle.setTitle("GLONASS:Enabled"); break;
  }
  displayRefresh = true;
}

/********************************************************************/
void menu_entrGPSScfgCB() {
  deviceState.DEVICE_MODE = DM_GPSSCFG;
  deviceMode_init();
  menu_gnssConfigurator('\0', "");
  displayRefresh = true;
}

/********************************************************************/
void menu_pollGNSSSelInfoCB() {
  char _msgStr[22];
  bool rcode = gps.pollGNSSSelectionInfo();
  sprintf(_msgStr, "Poll MON-GNSS rc=%d", rcode);
  msg_update(_msgStr);
  displayRefresh = true;
}

/********************************************************************/
void menu_pollGNSSCfgInfoCB() {
  char _msgStr[22];
  bool rcode = gps.pollGNSSConfigInfo();
  sprintf(_msgStr, "Poll CFG-GNSS rc=%d", rcode);
  msg_update(_msgStr);
  displayRefresh = true;
}

/********************************************************************/
void menu_gnssCfgGPSToggleCB() {
  menu_gnssConfigurator('G', "");
  displayRefresh = true;
}

/********************************************************************/
void menu_gnssCfgSBASToggleCB() {
  menu_gnssConfigurator('S', "");
  displayRefresh = true;
}

/********************************************************************/
void menu_gnssCfgGalileoToggleCB() {
  menu_gnssConfigurator('E', "");
  displayRefresh = true;
}

/********************************************************************/
void menu_gnssCfgBeiDouToggleCB() {
  menu_gnssConfigurator('B', "");
  displayRefresh = true;
}

/********************************************************************/
void menu_gnssCfgBeiDouB1ToggleCB() {
  menu_gnssConfigurator('B', "B1");
  displayRefresh = true;
}

/********************************************************************/
void menu_gnssCfgBeiDouB1CToggleCB() {
  menu_gnssConfigurator('B', "B1C");
  displayRefresh = true;
}

/********************************************************************/
void menu_gnssCfgIMESToggleCB() {
  menu_gnssConfigurator('I', "");
  displayRefresh = true;
}

/********************************************************************/
void menu_gnssCfgQZSSToggleCB() {
  menu_gnssConfigurator('Q', "");
  displayRefresh = true;
}

/********************************************************************/
void menu_gnssCfgQZSSL1CAToggleCB() {
  menu_gnssConfigurator('Q', "L1CA");
  displayRefresh = true;
}

/********************************************************************/
void menu_gnssCfgQZSSL1SToggleCB() {
  menu_gnssConfigurator('Q', "L1S");
  displayRefresh = true;
}

/********************************************************************/
void menu_gnssCfgGLONASSToggleCB() {
  menu_gnssConfigurator('R', "");
  displayRefresh = true;
}

/********************************************************************/
void menu_exitGPSScfgCB() {
  deviceMode_end();
  menu.exitToParentMenuPage();
  displayRefresh = true;
}

/********************************************************************/
void menu_entrGPSStepCB() {
  deviceState.DEVICE_MODE = DM_GPSSSTP;
  deviceMode_init();
  displayRefresh = true;
}

/********************************************************************/
void menu_sstBeginCB() {
  char _msgStr[22];
  gpsSerial->begin(GPS_BAUD_RATE);
  bool rcode = gps.gnss_init(*gpsSerial, GPS_BAUD_RATE);
  sprintf(_msgStr, "gnss.begin rc=%d", rcode);
  msg_update(_msgStr);
  displayRefresh = true;
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
  displayRefresh = true;
}

/********************************************************************/
void menu_exitGPSStepCB() {
  deviceMode_end();
  menu.exitToParentMenuPage();
  displayRefresh = true;
}

/********************************************************************/
void menu_entrGPSEmuM8CB() {
  deviceState.DEVICE_MODE = DM_GPSEMU_M8;
  deviceMode_init();
  displayRefresh = true;
}

/********************************************************************/
void menu_exitGPSEmuM8CB() {
  deviceMode_end();
  menu.exitToParentMenuPage();
  displayRefresh = true;
}

/********************************************************************/
void menu_entrGPSEmuM10CB() {
  deviceState.DEVICE_MODE = DM_GPSEMU_M10;
  deviceMode_init();
  displayRefresh = true;
}

/********************************************************************/
void menu_exitGPSEmuM10CB() {
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
void menu_cancelGPSFactoryResetCB() {
  menu.exitToParentMenuPage();
  msg_update("GPS Reset Canceled");
  displayRefresh = true;
}

/********************************************************************/
void menu_confirmGPSFactoryResetCB() {
  menu.exitToParentMenuPage();
  gpsSerial = &Serial2;
  if(gps.gnss_init(*gpsSerial, GPS_BAUD_RATE, GPS_NORESET, 0, 0) &&
     gps.factoryReset()) {
    msg_update("GPS Reset Completed");
  } else {
    msg_update("GPS Reset Failed");
  }
  displayRefresh = true;
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
  displayRefresh = true;
}

/********************************************************************/
void menu_confirmSaveSettingsCB() {
  menu.exitToParentMenuPage();
  if(sdcard_deviceStateSave()) {
    msg_update("Settings Saved");
  } else {
    msg_update("Save Settings Failed");
  }
  displayRefresh = true;
}

/********************************************************************/
void menu_cancelRestartDeviceCB() {
  menu.exitToParentMenuPage();
  msg_update("Restart Canceled");
  displayRefresh = true;
}

/********************************************************************/
void menu_confirmRestartDeviceCB() {
  device_reset();
}

/********************************************************************/
void menu_cancelResetDeviceCB() {
  menu.exitToParentMenuPage();
  msg_update("Reset Canceled");
  displayRefresh = true;
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

