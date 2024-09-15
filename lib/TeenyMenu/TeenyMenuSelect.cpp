#include <Arduino.h>
#include "TeenyMenuSelect.h"
#include "TeenyMenuConstants.h"

TeenyMenuSelect::TeenyMenuSelect(byte length_, SelectOptionUint8t* options_)
  : _type(TEENYMENU_VAL_UINT8T)
  , _length(length_)
  , _options(options_)
{ }

TeenyMenuSelect::TeenyMenuSelect(byte length_, SelectOptionInt16t* options_)
  : _type(TEENYMENU_VAL_INT16T)
  , _length(length_)
  , _options(options_)
{ }

TeenyMenuSelect::TeenyMenuSelect(byte length_, SelectOptionInt32t* options_)
  : _type(TEENYMENU_VAL_INT32T)
  , _length(length_)
  , _options(options_)
{ }

byte TeenyMenuSelect::getType() {
  return _type;
}

byte TeenyMenuSelect::getLength() {
  return _length;
}

int TeenyMenuSelect::getSelectedOptionNum(void* variable) {
  SelectOptionUint8t* optsUint8t = (SelectOptionUint8t*)_options;
  SelectOptionInt16t* optsInt16t = (SelectOptionInt16t*)_options;
  SelectOptionInt32t* optsInt32t = (SelectOptionInt32t*)_options;
  boolean found = false;
  for (byte i=0; i<_length; i++) {
    switch (_type) {
      case TEENYMENU_VAL_UINT8T:
        if (optsUint8t[i].val_uint8t == *(uint8_t*)variable) { found = true; }
        break;
      case TEENYMENU_VAL_INT16T:
        if (optsInt16t[i].val_int16t == *(int16_t*)variable) { found = true; }
        break;
      case TEENYMENU_VAL_INT32T:
        if (optsInt32t[i].val_int32t == *(int32_t*)variable) { found = true; }
        break;
    }
    if (found) { return i; }
  }
  return -1;
}

char* TeenyMenuSelect::getSelectedOptionName(void* variable) {
  int selectedOptionNum = getSelectedOptionNum(variable);
  return getOptionNameByIndex(selectedOptionNum);
}

char* TeenyMenuSelect::getOptionNameByIndex(int index) {
  const char* name;
  SelectOptionUint8t* optsUint8t = (SelectOptionUint8t*)_options;
  SelectOptionInt16t* optsInt16t = (SelectOptionInt16t*)_options;
  SelectOptionInt32t* optsInt32t = (SelectOptionInt32t*)_options;
  switch (_type) {
    case TEENYMENU_VAL_UINT8T:
      name = (index > -1 && index < _length) ? optsUint8t[index].name : "";
      break;
    case TEENYMENU_VAL_INT16T:
      name = (index > -1 && index < _length) ? optsInt16t[index].name : "";
      break;
    case TEENYMENU_VAL_INT32T:
      name = (index > -1 && index < _length) ? optsInt32t[index].name : "";
      break;
  }
  return const_cast<char*>(name);
}

void TeenyMenuSelect::setValue(void* variable, int index) {
  SelectOptionUint8t* optsUint8t = (SelectOptionUint8t*)_options;
  SelectOptionInt16t* optsInt16t = (SelectOptionInt16t*)_options;
  SelectOptionInt32t* optsInt32t = (SelectOptionInt32t*)_options;
  if (index > -1 && index < _length) {
    switch (_type) {
      case TEENYMENU_VAL_UINT8T:
        *(uint8_t*)variable = optsUint8t[index].val_uint8t;
        break;
      case TEENYMENU_VAL_INT16T:
        *(int16_t*)variable = optsInt16t[index].val_int16t;
        break;
      case TEENYMENU_VAL_INT32T:
        *(int32_t*)variable = optsInt32t[index].val_int32t;
        break;
    }
  }
}

