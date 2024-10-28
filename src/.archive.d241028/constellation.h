
/********************************************************************/
// Color map from TFT_eSPI library
//#define TFT_BLACK       0x0000 /*   0,   0,   0 */
//#define TFT_NAVY        0x000F /*   0,   0, 128 */
//#define TFT_DARKGREEN   0x03E0 /*   0, 128,   0 */
//#define TFT_DARKCYAN    0x03EF /*   0, 128, 128 */
//#define TFT_MAROON      0x7800 /* 128,   0,   0 */
//#define TFT_PURPLE      0x780F /* 128,   0, 128 */
//#define TFT_OLIVE       0x7BE0 /* 128, 128,   0 */
//#define TFT_LIGHTGREY   0xC618 /* 192, 192, 192 */
//#define TFT_DARKGREY    0x7BEF /* 128, 128, 128 */
//#define TFT_BLUE        0x001F /*   0,   0, 255 */
//#define TFT_GREEN       0x07E0 /*   0, 255,   0 */
//#define TFT_CYAN        0x07FF /*   0, 255, 255 */
//#define TFT_RED         0xF800 /* 255,   0,   0 */
//#define TFT_MAGENTA     0xF81F /* 255,   0, 255 */
//#define TFT_YELLOW      0xFFE0 /* 255, 255,   0 */
//#define TFT_WHITE       0xFFFF /* 255, 255, 255 */
//#define TFT_ORANGE      0xFDA0 /* 255, 180,   0 */
//#define TFT_GREENYELLOW 0xB7E0 /* 180, 255,   0 */
//#define TFT_PINK        0xFC9F /* 255, 192, 255 */

/********************************************************************/
// Color conversion from uint16_t/565 to uint32_t/888
uint32_t color16to24(uint16_t color565) {
  uint8_t r = (color565 >> 8) & 0xF8; r |= (r >> 5);
  uint8_t g = (color565 >> 3) & 0xFC; g |= (g >> 6);
  uint8_t b = (color565 << 3) & 0xF8; b |= (b >> 5);
  return ((uint32_t)r << 16) | ((uint32_t)g << 8) | ((uint32_t)b << 0);
}

/********************************************************************/
// Satellite Constellation Map Generator

uint8_t drawSatConstellation(int16_t compAngle) {

  ubloxNAVSATInfo_t navsatInfo;

  int16_t  mapRadius = 118;
  int16_t  mapSatRadius = 106; // Keep sat graphics inside map
  int16_t  mapCenterX = 120;
  int16_t  mapCenterY = 162;
  uint8_t  numSats = 0;
  int16_t  xCoord;
  int16_t  yCoord;
  int16_t  satRadius = 10;
  uint32_t satColor;
  int16_t  satRingRadius = 12;
  uint32_t satRingColor;
  char _dispStr[4];

  // draw main circles, one at 0deg, and one at 45deg elevation
  display.drawCircle(mapCenterX, mapCenterY, mapRadius, color16to24(TFT_WHITE));
  display.drawCircle(mapCenterX, mapCenterY, (mapRadius>>1)+1, color16to24(TFT_WHITE));

  // draw lines at 0, 45, 90, 135 etc degrees azimuth
  for (int16_t i = 0; i <= 7; i++) {
    xCoord = round(-sin(radians((i * 45) + 180 + compAngle)) * mapRadius);
    yCoord = round(cos(radians((i * 45) + 180 + compAngle)) * mapRadius);
    display.drawLine(mapCenterX, mapCenterY, xCoord + mapCenterX, yCoord + mapCenterY, color16to24(TFT_WHITE));
    if(i % 2) continue;
    xCoord = round(-sin(radians((i * 45) + 180 + compAngle)) * (mapRadius - 8));
    yCoord = round(cos(radians((i * 45) + 180 + compAngle)) * (mapRadius - 8));
    display.fillCircle(xCoord + mapCenterX, yCoord + mapCenterY, 12, color16to24(TFT_BLACK));
    char label;
    switch(i) {
      case 0: label = 'N'; break;
      case 2: label = 'E'; break;
      case 4: label = 'S'; break;
      case 6: label = 'W'; break;
    }
    display.drawChar(xCoord + mapCenterX - 5, yCoord + mapCenterY - 7, label, color16to24(TFT_BLACK), color16to24(TFT_WHITE), 2);
  }

  // Check NAVSAT Packet
  gps.getNAVSATInfo(navsatInfo);
  if(!navsatInfo.validPacket) {
    return numSats;
  }

  // Set text size for satellite IDs
  display.setTextSize(1); // font is 6x8

  // draw the positions of the sats
  for(int16_t i = navsatInfo.numSvsEphValid -1; i >= 0; i--) {

    // Sat position
    numSats ++;
    xCoord = round(-sin(radians(navsatInfo.svSortList[i].azim + 180 + compAngle)) *
                   map(max(navsatInfo.svSortList[i].elev, 0), 0, 90, mapSatRadius, 1));
    yCoord = round(cos(radians(navsatInfo.svSortList[i].azim + 180 + compAngle)) *
                   map(max(navsatInfo.svSortList[i].elev, 0), 0, 90, mapSatRadius, 1));

    // Sat ring color based on SNR
    if(navsatInfo.svSortList[i].cno >= 35) {
      satRingColor = color16to24(TFT_WHITE);
    } else if(navsatInfo.svSortList[i].cno >=20) {
      satRingColor = color16to24(TFT_YELLOW);
    } else {
      satRingColor = color16to24(TFT_ORANGE);
    }
    display.fillCircle(xCoord + mapCenterX, yCoord + mapCenterY, satRingRadius, satRingColor);

    // Sat color based on svUsed
    if(navsatInfo.svSortList[i].svUsed) {
      satColor = color16to24(TFT_BLUE);
    } else if(navsatInfo.svSortList[i].healthy) {
      satColor = color16to24(TFT_RED);
    } else {
      satColor = color16to24(TFT_BLACK);
    }
    display.fillCircle(xCoord + mapCenterX, yCoord + mapCenterY, satRadius, satColor);

    // Sat label
    sprintf(_dispStr, "%c%02d",
            navsatInfo.svSortList[i].gnssIdType,
            navsatInfo.svSortList[i].svId);
    displayPV.prt_str(_dispStr, 3, xCoord + mapCenterX - 8, yCoord + mapCenterY - 3);
  }

  // Restore text size
  display.setTextSize(2); // font is 6x8

  // Return number of sats
  return numSats;

}

