/*
TeenyPrtVal - Header file for the TeenyPrtVal Arduino Library.
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

#ifndef TEENYPRTVAL_H
#define TEENYPRTVAL_H

#include <Arduino.h>

/********************************************************************/
template <class T>
class TeenyPrtVal {
  public:
    TeenyPrtVal(T& displayObj) : _displayObj(displayObj) {};
    void  prt_int(uint32_t val, int len) {
            char sz[32];
            sprintf(sz, "%ld", val);
            sz[len] = 0;
            for (int i=strlen(sz); i<len; ++i)
                    sz[i] = ' ';
            if (len > 0)
                    sz[len-1] = ' ';
            sz[len] = '\0';
            _displayObj.print(sz);
          }
    void  prt_int(uint32_t val, int len, int col, int row) {
            _displayObj.setCursor(col,row);
            prt_int(val, len);
          }
    void  prt_hex(uint32_t val, int len) {
            char sz[32];
            //sprintf(sz, "%lX", val);
            if (len < 2) sprintf(sz, "%lX", val);
            else if (len < 4) sprintf(sz, "%02lX", val);
            else if (len < 6) sprintf(sz, "%04lX", val);
            else if (len < 8) sprintf(sz, "%06lX", val);
            else sprintf(sz, "%08lX", val);
            sz[len] = 0;
            for (int i=strlen(sz); i<len; ++i)
                    sz[i] = ' ';
            if (len > 0)
                    sz[len-1] = ' ';
            sz[len] = '\0';
            _displayObj.print(sz);
          }
    void  prt_hex(uint32_t val, int len, int col, int row) {
            _displayObj.setCursor(col,row);
            prt_hex(val, len);
          }
    void  prt_float(float val, int len, int prec) {
            String stz = String(val, prec);
            char sz[len+1];
            stz.toCharArray(sz, min(len+1, stz.length()+1));
            for (int i=stz.length(); i<len; ++i) sz[i]=' ';
            sz[len] = '\0';
            _displayObj.print(sz);
          }
    void  prt_float(float val, int len, int prec, int col, int row) {
            _displayObj.setCursor(col,row);
            prt_float(val, len, prec);
          }
    void  prt_char(char pchar, int len) {
            char sz[32];
            sz[0] = pchar;
            for (int i=1; i<len; ++i) sz[i]=' ';
            sz[len] = '\0';
            _displayObj.print(sz);
          }
    void  prt_char(char pchar, int len, int col, int row) {
            _displayObj.setCursor(col,row);
            prt_char(pchar, len);
          }
    void  prt_str(const char* str, int len) {
            char sz[32];
            int slen = strlen(str);
            for (int i=0; i<len; ++i)
              if(i<slen) {sz[i]=str[i];} else {sz[i]=' ';};
            sz[len] = '\0';
            _displayObj.print(sz);
          }
    void  prt_str(const char* str, int len, int col, int row) {
            _displayObj.setCursor(col,row);
            prt_str(str, len);
          }
    void  prt_date(uint16_t year, uint8_t month, uint8_t day) {
            char sz[32];
            sprintf(sz, "%02d/%02d/%02d", month, day, (year % 100));
            _displayObj.print(sz);
          }
    void  prt_date(uint16_t year, uint8_t month, uint8_t day, int col, int row) {
            _displayObj.setCursor(col,row);
            prt_date(year, month, day);
          }
    void  prt_time(uint8_t hour, uint8_t min, uint8_t sec, uint8_t subsec) {
            char sz[32];
            sprintf(sz, "%02d:%02d:%02d:%02d", hour, min, sec, subsec);
            _displayObj.print(sz);
          }
    void  prt_time(uint8_t hour, uint8_t min, uint8_t sec, uint8_t subsec, int col, int row) {
            _displayObj.setCursor(col,row);
            prt_time(hour, min, sec, subsec);
          }
  private:
    T& _displayObj;
};

#endif //TEENYPRTVAL_H

