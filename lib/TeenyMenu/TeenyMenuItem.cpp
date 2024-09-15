#include <Arduino.h>
#include "TeenyMenuItem.h"
#include "TeenyMenuConstants.h"

TeenyMenuItem::TeenyMenuItem(const char* title_, uint8_t& linkedVariable_, TeenyMenuSelect& select_, void (*saveAction_)(), byte scrollType_)
  : title(title_)
  , linkedVariable(&linkedVariable_)
  , linkedType(TEENYMENU_VAL_SELECT)
  , select(&select_)
  , saveAction(saveAction_)
  , scrollType(scrollType_)
  , type(TEENYMENU_ITEM_VAL)
{ }

TeenyMenuItem::TeenyMenuItem(const char* title_, int16_t& linkedVariable_, TeenyMenuSelect& select_, void (*saveAction_)(), byte scrollType_)
  : title(title_)
  , linkedVariable(&linkedVariable_)
  , linkedType(TEENYMENU_VAL_SELECT)
  , select(&select_)
  , saveAction(saveAction_)
  , scrollType(scrollType_)
  , type(TEENYMENU_ITEM_VAL)
{ }

TeenyMenuItem::TeenyMenuItem(const char* title_, int32_t& linkedVariable_, TeenyMenuSelect& select_, void (*saveAction_)(), byte scrollType_)
  : title(title_)
  , linkedVariable(&linkedVariable_)
  , linkedType(TEENYMENU_VAL_SELECT)
  , select(&select_)
  , saveAction(saveAction_)
  , scrollType(scrollType_)
  , type(TEENYMENU_ITEM_VAL)
{ }

//---

TeenyMenuItem::TeenyMenuItem(const char* title_, uint8_t& linkedVariable_, TeenyMenuSelect& select_, boolean readonly_, byte scrollType_)
  : title(title_)
  , linkedVariable(&linkedVariable_)
  , linkedType(TEENYMENU_VAL_SELECT)
  , select(&select_)
  , readonly(readonly_)
  , scrollType(scrollType_)
  , type(TEENYMENU_ITEM_VAL)
{ }

TeenyMenuItem::TeenyMenuItem(const char* title_, int16_t& linkedVariable_, TeenyMenuSelect& select_, boolean readonly_, byte scrollType_)
  : title(title_)
  , linkedVariable(&linkedVariable_)
  , linkedType(TEENYMENU_VAL_SELECT)
  , select(&select_)
  , readonly(readonly_)
  , scrollType(scrollType_)
  , type(TEENYMENU_ITEM_VAL)
{ }

TeenyMenuItem::TeenyMenuItem(const char* title_, int32_t& linkedVariable_, TeenyMenuSelect& select_, boolean readonly_, byte scrollType_)
  : title(title_)
  , linkedVariable(&linkedVariable_)
  , linkedType(TEENYMENU_VAL_SELECT)
  , select(&select_)
  , readonly(readonly_)
  , scrollType(scrollType_)
  , type(TEENYMENU_ITEM_VAL)
{ }

//---

TeenyMenuItem::TeenyMenuItem(const char* title_, uint8_t& linkedVariable_, void (*saveAction_)())
  : title(title_)
  , linkedVariable(&linkedVariable_)
  , linkedType(TEENYMENU_VAL_UINT8T)
  , saveAction(saveAction_)
  , type(TEENYMENU_ITEM_VAL)
{ }

TeenyMenuItem::TeenyMenuItem(const char* title_, int16_t& linkedVariable_, void (*saveAction_)())
  : title(title_)
  , linkedVariable(&linkedVariable_)
  , linkedType(TEENYMENU_VAL_INT16T)
  , saveAction(saveAction_)
  , type(TEENYMENU_ITEM_VAL)
{ }

TeenyMenuItem::TeenyMenuItem(const char* title_, int32_t& linkedVariable_, void (*saveAction_)())
  : title(title_)
  , linkedVariable(&linkedVariable_)
  , linkedType(TEENYMENU_VAL_INT32T)
  , saveAction(saveAction_)
  , type(TEENYMENU_ITEM_VAL)
{ }

TeenyMenuItem::TeenyMenuItem(const char* title_, boolean& linkedVariable_, void (*saveAction_)(), const char* boolFalseStr_, const char* boolTrueStr_)
  : title(title_)
  , linkedVariable(&linkedVariable_)
  , linkedType(TEENYMENU_VAL_BOOLEAN)
  , saveAction(saveAction_)
  , boolFalseStr(boolFalseStr_)
  , boolTrueStr(boolTrueStr_)
  , type(TEENYMENU_ITEM_VAL)
{ }

//---

TeenyMenuItem::TeenyMenuItem(const char* title_, uint8_t& linkedVariable_, uint8_t& rangeMin_, uint8_t& rangeMax_, void (*saveAction_)())
  : title(title_)
  , linkedVariable(&linkedVariable_)
  , linkedType(TEENYMENU_VAL_UINT8T)
  , rangeMin(&rangeMin_)
  , rangeMax(&rangeMax_)
  , saveAction(saveAction_)
  , type(TEENYMENU_ITEM_VAL)
{ }

TeenyMenuItem::TeenyMenuItem(const char* title_, int16_t& linkedVariable_, int16_t& rangeMin_, int16_t& rangeMax_, void (*saveAction_)())
  : title(title_)
  , linkedVariable(&linkedVariable_)
  , linkedType(TEENYMENU_VAL_INT16T)
  , rangeMin(&rangeMin_)
  , rangeMax(&rangeMax_)
  , saveAction(saveAction_)
  , type(TEENYMENU_ITEM_VAL)
{ }

TeenyMenuItem::TeenyMenuItem(const char* title_, int32_t& linkedVariable_, int32_t& rangeMin_, int32_t& rangeMax_, void (*saveAction_)())
  : title(title_)
  , linkedVariable(&linkedVariable_)
  , linkedType(TEENYMENU_VAL_INT32T)
  , rangeMin(&rangeMin_)
  , rangeMax(&rangeMax_)
  , saveAction(saveAction_)
  , type(TEENYMENU_ITEM_VAL)
{ }

//---

TeenyMenuItem::TeenyMenuItem(const char* title_, uint8_t& linkedVariable_, boolean readonly_)
  : title(title_)
  , linkedVariable(&linkedVariable_)
  , linkedType(TEENYMENU_VAL_UINT8T)
  , readonly(readonly_)
  , type(TEENYMENU_ITEM_VAL)
{ }

TeenyMenuItem::TeenyMenuItem(const char* title_, int16_t& linkedVariable_, boolean readonly_)
  : title(title_)
  , linkedVariable(&linkedVariable_)
  , linkedType(TEENYMENU_VAL_INT16T)
  , readonly(readonly_)
  , type(TEENYMENU_ITEM_VAL)
{ }

TeenyMenuItem::TeenyMenuItem(const char* title_, int32_t& linkedVariable_, boolean readonly_)
  : title(title_)
  , linkedVariable(&linkedVariable_)
  , linkedType(TEENYMENU_VAL_INT32T)
  , readonly(readonly_)
  , type(TEENYMENU_ITEM_VAL)
{ }

TeenyMenuItem::TeenyMenuItem(const char* title_, boolean& linkedVariable_, boolean readonly_, const char* boolFalseStr_, const char* boolTrueStr_)
  : title(title_)
  , linkedVariable(&linkedVariable_)
  , linkedType(TEENYMENU_VAL_BOOLEAN)
  , readonly(readonly_)
  , boolFalseStr(boolFalseStr_)
  , boolTrueStr(boolTrueStr_)
  , type(TEENYMENU_ITEM_VAL)
{ }

//---

TeenyMenuItem::TeenyMenuItem(const char* title_, uint8_t& linkedVariable_, uint8_t& rangeMin_, uint8_t& rangeMax_, boolean readonly_)
  : title(title_)
  , linkedVariable(&linkedVariable_)
  , linkedType(TEENYMENU_VAL_UINT8T)
  , rangeMin(&rangeMin_)
  , rangeMax(&rangeMax_)
  , readonly(readonly_)
  , type(TEENYMENU_ITEM_VAL)
{ }

TeenyMenuItem::TeenyMenuItem(const char* title_, int16_t& linkedVariable_, int16_t& rangeMin_, int16_t& rangeMax_, boolean readonly_)
  : title(title_)
  , linkedVariable(&linkedVariable_)
  , linkedType(TEENYMENU_VAL_INT16T)
  , rangeMin(&rangeMin_)
  , rangeMax(&rangeMax_)
  , readonly(readonly_)
  , type(TEENYMENU_ITEM_VAL)
{ }

TeenyMenuItem::TeenyMenuItem(const char* title_, int32_t& linkedVariable_, int32_t& rangeMin_, int32_t& rangeMax_, boolean readonly_)
  : title(title_)
  , linkedVariable(&linkedVariable_)
  , linkedType(TEENYMENU_VAL_INT32T)
  , rangeMin(&rangeMin_)
  , rangeMax(&rangeMax_)
  , readonly(readonly_)
  , type(TEENYMENU_ITEM_VAL)
{ }

//---

TeenyMenuItem::TeenyMenuItem(const char* title_, TeenyMenuPage& linkedPage_, boolean readonly_)
  : title(title_)
  , linkedPage(&linkedPage_)
  , readonly(readonly_)
  , type(TEENYMENU_ITEM_LINK)
{ }

TeenyMenuItem::TeenyMenuItem(const char* title_, TeenyMenuPage* linkedPage_, boolean readonly_)
  : title(title_)
  , linkedPage(linkedPage_)
  , readonly(readonly_)
  , type(TEENYMENU_ITEM_LINK)
{ }

//---

TeenyMenuItem::TeenyMenuItem(boolean readonly_)
  : title("")
  , linkedPage(nullptr)
  , readonly(readonly_)
  , type(TEENYMENU_ITEM_BACK)
{ }

//---

TeenyMenuItem::TeenyMenuItem(const char* title_, void (*buttonAction_)(), boolean readonly_)
  : title(title_)
  , buttonAction(buttonAction_)
  , readonly(readonly_)
  , type(TEENYMENU_ITEM_BUTTON)
{ }

//---

TeenyMenuItem::TeenyMenuItem(const char* title_)
  : title(title_)
  , readonly(true)
  , type(TEENYMENU_ITEM_LABEL)
{ }

//---

void TeenyMenuItem::setTitle(const char* title_) {
  title = title_;
}

const char* TeenyMenuItem::getTitle() {
  return title;
}

void TeenyMenuItem::setReadonly(boolean mode) {
  readonly = mode;
}

boolean TeenyMenuItem::getReadonly() {
  return readonly;
}

void TeenyMenuItem::hide(boolean hide) {
  if (hide) {
    if (!hidden) {
      if (parentPage != nullptr) {
        parentPage->hideMenuItem(*this);
      } else {
        hidden = true;
      }
    }
  } else {
    show();
  }
}

void TeenyMenuItem::show() {
  if (hidden) {
    if (parentPage != nullptr) {
      parentPage->showMenuItem(*this);
    } else {
      hidden = false;
    }
  }
}

boolean TeenyMenuItem::isHidden() {
  return hidden;
}

TeenyMenuItem* TeenyMenuItem::getMenuItemNext() {
  TeenyMenuItem* menuItemTmp = menuItemNext;
  while (menuItemTmp != 0 && menuItemTmp->hidden) {
    menuItemTmp = menuItemTmp->menuItemNext;
  }
  return menuItemTmp;
}

