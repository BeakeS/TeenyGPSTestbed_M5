/*
TeenySevenSeg - Header file for the TeenySevenSeg Arduino Library.
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

#ifndef _TEENYSEVENSEG_H
#define _TEENYSEVENSEG_H

#include <Arduino.h>

/********************************************************************/
template <class T>
class TeenySevenSeg {

  public:

    TeenySevenSeg(T& displayObj) : _displayObj(displayObj) {};

    void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
      _displayObj.drawFastHLine(x, y, w, color);
    }

    void drawFastVLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
      _displayObj.drawFastVLine(x, y, w, color);
    }

    void drawSSDigit(int16_t value, int16_t x, int16_t y, int16_t l, int16_t w, uint16_t color) {
      switch(value) {
        case -1:
          drawSSDigitSegs(_digitSegV_blank, x, y, l, w, color);
          break;
        case 0:
          drawSSDigitSegs(_digitSegV_0, x, y, l, w, color);
          break;
        case 1:
          drawSSDigitSegs(_digitSegV_1, x, y, l, w, color);
          break;
        case 2:
          drawSSDigitSegs(_digitSegV_2, x, y, l, w, color);
          break;
        case 3:
          drawSSDigitSegs(_digitSegV_3, x, y, l, w, color);
          break;
        case 4:
          drawSSDigitSegs(_digitSegV_4, x, y, l, w, color);
          break;
        case 5:
          drawSSDigitSegs(_digitSegV_5, x, y, l, w, color);
          break;
        case 6:
          drawSSDigitSegs(_digitSegV_6, x, y, l, w, color);
          break;
        case 7:
          drawSSDigitSegs(_digitSegV_7, x, y, l, w, color);
          break;
        case 8:
          drawSSDigitSegs(_digitSegV_8, x, y, l, w, color);
          break;
        case 9:
          drawSSDigitSegs(_digitSegV_9, x, y, l, w, color);
          break;
        default:
          drawSSDigitSegs(_digitSegV_error, x, y, l, w, color);
          break;
      }
    }

    void drawSSTwoDigit(int16_t value, int16_t x, int16_t y, int16_t l, int16_t w, int16_t s, uint16_t color) {
      int16_t _value = min(value, 99);
      uint16_t digitOffset = 0;
      drawSSDigit((_value/10), x+digitOffset, y, l, w, color);
      digitOffset+=l+s;
      drawSSDigit((_value%10), x+digitOffset, y, l, w, color);
    }

    // drawSSClock is depricated - use drawSSFourDigitTime instead
    void drawSSClock(uint32_t time, int16_t x, int16_t y, int16_t l, int16_t w, uint16_t color) {
      drawSSClock(time, x, y, l, w, w, w, w, color);
    }

    // drawSSClock is depricated - use drawSSFourDigitTime instead
    void drawSSClock(uint32_t time, int16_t x, int16_t y, int16_t l, int16_t w, int16_t s, int16_t cw, int16_t cs, uint16_t color) {
      uint32_t _time = min(time, (uint32_t) 5999);
      uint32_t mins = _time / 60;
      uint32_t secs = _time % 60;
      uint16_t digitOffset = 0;
      drawSSDigit((mins/10), x+digitOffset, y, l, w, color);
      digitOffset+=l+s;
      drawSSDigit((mins%10), x+digitOffset, y, l, w, color);
      digitOffset+=l+cs;
      drawSSColon(x+digitOffset, y, l, w, cw, color);
      digitOffset+=cw+cs;
      drawSSDigit((secs/10), x+digitOffset, y, l, w, color);
      digitOffset+=l+s;
      drawSSDigit((secs%10), x+digitOffset, y, l, w, color);
    }

    void drawSSFourDigitTime(uint32_t time, uint32_t modulus, int16_t x, int16_t y, int16_t l, int16_t w, uint16_t color) {
      drawSSFourDigitTime(time, modulus, x, y, l, w, w, w, w, color);
    }

    void drawSSFourDigitTime(uint32_t time, uint32_t modulus, int16_t x, int16_t y, int16_t l, int16_t w, int16_t s, int16_t cw, int16_t cs, uint16_t color) {
      uint32_t _time = min(time, (100 * modulus)-1);
      uint32_t _major = _time / modulus;
      uint32_t _minor = _time % modulus;
      uint16_t digitOffset = 0;
      drawSSDigit((_major / 10), x + digitOffset, y, l, w, color);
      digitOffset += l + s;
      drawSSDigit((_major % 10), x + digitOffset, y, l, w, color);
      digitOffset += l + cs;
      drawSSColon(x + digitOffset, y, l, w, cw, color);
      digitOffset += cw + cs;
      drawSSDigit((_minor / 10), x + digitOffset, y, l, w, color);
      digitOffset += l + s;
      drawSSDigit((_minor % 10), x + digitOffset, y, l, w, color);
    }

    void drawSSSixDigitTime(uint32_t time, uint32_t modulus1, uint32_t modulus2, int16_t x, int16_t y, int16_t l, int16_t w, uint16_t color) {
      drawSSSixDigitTime(time, modulus1, modulus2, x, y, l, w, w, w, w, color);
    }

    void drawSSSixDigitTime(uint32_t time, uint32_t modulus1, uint32_t modulus2, int16_t x, int16_t y, int16_t l, int16_t w, int16_t s, int16_t cw, int16_t cs, uint16_t color) {
      uint32_t _time = min(time, ((100 * modulus1) * modulus2)-1);
      uint32_t _submajor = _time / modulus1;
      uint32_t _subminor = _time % modulus1;
      uint32_t _major    = _submajor / modulus2;
      uint32_t _minor    = _submajor % modulus2;
      uint16_t digitOffset = 0;
      drawSSDigit((_major / 10), x + digitOffset, y, l, w, color);
      digitOffset += l + s;
      drawSSDigit((_major % 10), x + digitOffset, y, l, w, color);
      digitOffset += l + cs;
      drawSSColon(x + digitOffset, y, l, w, cw, color);
      digitOffset += cw + cs;
      drawSSDigit((_minor / 10), x + digitOffset, y, l, w, color);
      digitOffset += l + s;
      drawSSDigit((_minor % 10), x + digitOffset, y, l, w, color);
      digitOffset += l + cs;
      drawSSColon(x + digitOffset, y, l, w, cw, color);
      digitOffset += cw + cs;
      drawSSDigit((_subminor / 10), x + digitOffset, y, l, w, color);
      digitOffset += l + s;
      drawSSDigit((_subminor % 10), x + digitOffset, y, l, w, color);
    }

  protected:

  private:

    T& _displayObj;

    typedef enum{ SegA, SegB, SegC, SegD, SegE, SegF, SegG } segName_t;
    typedef enum{ SegH, SegV } segDir_t;

    // Segment Map
    //
    //     A
    //   F   B
    //     G
    //   E   C
    //     D
    //

    uint8_t _digitSegV_blank[7] = {0, 0, 0, 0, 0, 0, 0};
    uint8_t _digitSegV_0[7]     = {1, 1, 1, 1, 1, 1, 0};
    uint8_t _digitSegV_1[7]     = {0, 1, 1, 0, 0, 0, 0};
    uint8_t _digitSegV_2[7]     = {1, 1, 0, 1, 1, 0, 1};
    uint8_t _digitSegV_3[7]     = {1, 1, 1, 1, 0, 0, 1};
    uint8_t _digitSegV_4[7]     = {0, 1, 1, 0, 0, 1, 1};
    uint8_t _digitSegV_5[7]     = {1, 0, 1, 1, 0, 1, 1};
    uint8_t _digitSegV_6[7]     = {1, 0, 1, 1, 1, 1, 1};
    uint8_t _digitSegV_7[7]     = {1, 1, 1, 0, 0, 0, 0};
    uint8_t _digitSegV_8[7]     = {1, 1, 1, 1, 1, 1, 1};
    uint8_t _digitSegV_9[7]     = {1, 1, 1, 0, 0, 1, 1};
    uint8_t _digitSegV_error[7] = {1, 0, 0, 1, 1, 1, 1};

    void drawSSSegStripe(segName_t segName, int16_t n, int16_t x, int16_t y, int16_t l, int16_t w, uint16_t color) {
      struct segInfo_t { segDir_t segDir; int16_t xOffset, yOffset; } segInfo;
      // Get raw segment x0,y0
      switch(segName) {
        //case SegA : segInfo = {SegH, 0,     n           }; break;
        //case SegB : segInfo = {SegV, l-n-1, 0           }; break;
        //case SegC : segInfo = {SegV, l-n-1, l-w+(w<3 ? 0 : (w%2==0 ? 0 : 1))       }; break;
        //case SegD : segInfo = {SegH, 0,     (2*l)-w-n-1 }; break;
        //case SegE : segInfo = {SegV, n,     l-w+(w<3 ? 0 : (w%2==0 ? 0 : 1))       }; break;
        //case SegF : segInfo = {SegV, n,     0           }; break;
        //case SegG : segInfo = {SegH, 0,     l-n-1       }; break;
        //default   : segInfo = {SegH, 0,     n           }; break;
        case SegA : segInfo.segDir=SegH; segInfo.xOffset=0;     segInfo.yOffset=n;                                break;
        case SegB : segInfo.segDir=SegV; segInfo.xOffset=l-n-1; segInfo.yOffset=0;                                break;
        case SegC : segInfo.segDir=SegV; segInfo.xOffset=l-n-1; segInfo.yOffset=l-w+(w<3 ? 0 : (w%2==0 ? 0 : 1)); break;
        case SegD : segInfo.segDir=SegH; segInfo.xOffset=0;     segInfo.yOffset=(2*l)-w-n-1;                      break;
        case SegE : segInfo.segDir=SegV; segInfo.xOffset=n;     segInfo.yOffset=l-w+(w<3 ? 0 : (w%2==0 ? 0 : 1)); break;
        case SegF : segInfo.segDir=SegV; segInfo.xOffset=n;     segInfo.yOffset=0;                                break;
        case SegG : segInfo.segDir=SegH; segInfo.xOffset=0;     segInfo.yOffset=l-n-1;                            break;
        default   : segInfo.segDir=SegH; segInfo.xOffset=0;     segInfo.yOffset=n;                                break;
      }
      // Clip segment ends (this needs aesthetic tuning)
      int16_t segClip = 0;
      if(w < 2) {
        segClip = 1;
      } else {
        if(segName==SegG) {
          if(n > (w/2)) segClip=n-(w/2);
          if(w%2==0) {
            if(n < ((w/2)-1)) segClip=((w/2)-1)-n;
          } else {
            if(n < ((w/2)-0)) segClip=((w/2)-0)-n;
          }
          segClip++;
        } else {
          if(n < (w/2)) segClip=(w/2)-n;
        }
      }
      if(segInfo.segDir==SegH) {
        drawFastHLine(x+segInfo.xOffset+segClip, y+segInfo.yOffset, l-(segClip*2), color);
      } else {
        drawFastVLine(x+segInfo.xOffset, y+segInfo.yOffset+segClip, l-(segClip*2)-(w<3 ? 0 : (w%2==0 ? 0 : 1)), color);
      }
    }

    void drawSSSeg(segName_t segName, int16_t x, int16_t y, int16_t l, int16_t w, uint16_t color) {
      for(int16_t i=0; i<w; i++) {
        drawSSSegStripe(segName, i, x, y, l, w, color);
      }
    }

    void drawSSDigitSegs(const uint8_t segMask[7], int16_t x, int16_t y, int16_t l, int16_t w, uint16_t color) {
      segName_t segNameList[7] = {SegA, SegB, SegC, SegD, SegE, SegF, SegG};
      for(uint8_t i=0; i<7; i++) {
        if(segMask[i]==1) drawSSSeg(segNameList[i], x, y, l, w, color);
      }
    }

    void drawSSColon(int16_t x, int16_t y, int16_t l, int16_t cl, int16_t cw, uint16_t color) {
      struct SegInfo { segDir_t segDir; int16_t xOffset, yOffset; } segInfo;
      int16_t colonOffset = (l/2)-1;
      for(int16_t i=0; i<2; i++) {     //i=segment count
        for(int16_t j=0; j<cw; j++) {   //j=stripe count
          //segInfo = {SegV, j, i*(l-cw)};
          segInfo.segDir=SegV; segInfo.xOffset=j; segInfo.yOffset=i*(l-cw);
          drawFastVLine(x+segInfo.xOffset, y+segInfo.yOffset+colonOffset, cl, color);
        }
      }
    }

};

#endif // _TEENYSEVENSEG_H

