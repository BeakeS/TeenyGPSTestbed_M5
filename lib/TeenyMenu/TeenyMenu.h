#ifndef HEADER_TEENYMENU
#define HEADER_TEENYMENU

#include <TeenyPrtVal.h>
#include "TeenyMenuPage.h"
#include "TeenyMenuSelect.h"
#include "TeenyMenuConstants.h"

// Forward declaration of necessary classes
class TeenyMenuItem;

/********************************************************************/
// Declaration of TeenyMenu class
/********************************************************************/
template <class T>
class TeenyMenu {

/********************************************************************/
    /* PUBLIC */
/********************************************************************/
  public:

/********************************************************************/
    /* CONSTRUCTOR */
/********************************************************************/
    /* 
      @param 'display_' - reference to a display object and used for communication with display
      @param 'fontWidth_' (optional) - width of character font
      default 6 (standard font for 128x64 screen)
      @param 'fontHeight_' (optional) - height of character font
      default 8 (standard font for 128x64 screen)
      @param 'cp437_' (optional) - Specify if font supports printable 0x00-0x31 control characters (arrows, etc.)
      default true (font supports code page 437 printable characters)
      @param 'menuFirstItemScreenTopOffset_' (optional) - offset from the top of the screen to accommodate title of the menu page
      default 10 (suitable for 128x64 screen with other variables at their default values)
      @param 'menuItemHeight_' (optional) - height of the menu item
      default 9 (suitable for 128x64 screen with other variables at their default values)
      @param 'menuItemsPerScreen_' (optional) - count of the menu items per screen
      default 6 (suitable for 128x64 screen with other variables at their default values)
      @param 'menuTitleLeftOffset_' (optional) - offset from the left of the screen to the title of the associated with menu item variable
      default 5 (suitable for 128x64 screen with other variables at their default values)
      @param 'menuItemTitleLength_' (optional) - (_menuValuesLeftOffset - 6) / 6;
      default 12 (suitable for 128x64 screen with other variables at their default values)
      @param 'menuItemValueLeftOffset_' (optional) - offset from the left of the screen to the value of the associated with menu item variable (effectively the space left for the title of the menu item to be drawn on screen)
      default 85 (suitable for 128x64 screen with other variables at their default values)
      @param 'menuItemValueLength_' (optional) - (displayWidth - _menuValuesLeftOffset - 6) / 6;
      default 6 (suitable for 128x64 screen with other variables at their default values)
      @param 'menuItemLabelLeftOffset_' (optional) - offset from the left of the screen for label items (may want to adjust label and offset length if you don't want labels to overlap scroll bar)
      default 0 (suitable for 128x64 screen with other variables at their default values)
      @param 'menuItemLabelLength_' (optional) - displayWidth / 6;
      default 21 (suitable for 128x64 screen with other variables at their default values)
    */

    TeenyMenu(T& display_, byte fontWidth_ = 6, byte fontHeight_ = 8, bool cp437_ = true, byte menuFirstItemScreenTopOffset_ = 10, byte menuItemHeight_ = 9, byte menuItemsPerScreen_ = 6, byte menuItemTitleLeftOffset_ = 5, byte menuItemTitleLength_ = 12, byte menuItemValueLeftOffset_ = 85, byte menuItemValueLength_ = 6, byte menuItemLabelLeftOffset_ = 0, byte menuItemLabelLength_ = 21) :
      _display(display_),
      _fontWidth(fontWidth_),
      _fontHeight(fontHeight_),
      _cp437(cp437_),
      _menuFirstItemScreenTopOffset(menuFirstItemScreenTopOffset_),
      _menuItemHeight(menuItemHeight_),
      _menuItemsPerScreen(menuItemsPerScreen_),
      _menuItemTitleLeftOffset(menuItemTitleLeftOffset_),
      _menuItemTitleLength(menuItemTitleLength_),
      _menuItemValueLeftOffset(menuItemValueLeftOffset_),
      _menuItemValueLength(menuItemValueLength_),
      _menuItemLabelLeftOffset(menuItemLabelLeftOffset_),
      _menuItemLabelLength(menuItemLabelLength_),
      _displayPV(display_)
    {
      _white = 1;
      _black = 0;
      _editValueMode = false;
      _editValueSelectNum = -1;
    }

/********************************************************************/
    /* INIT OPERATIONS */
/********************************************************************/
    void setTextColor(uint16_t white, uint16_t black) {
      _white = white;
      _black = black;
    }

    // Set supplied menu page as current
        void setMenuPageCurrent(TeenyMenuPage& menuPageCurrent) {
      _menuPageCurrent = &menuPageCurrent;
    }

    // Get current menu page
    TeenyMenuPage getMenuPageCurrent() {
      return *_menuPageCurrent;
    }

    bool isMenuPageCurrent(TeenyMenuPage& menuPageCurrent) {
      return(_menuPageCurrent==&menuPageCurrent);
    }

    void linkMenuPage(TeenyMenuPage& menuPageLink) {
      TeenyMenuPage* _menuPageLink = &menuPageLink;
      if (_menuPageLink->enterAction != nullptr) {
        _menuPageLink->enterAction();
      }
      _menuPageLink->setParentMenuPage(*_menuPageCurrent);
      _menuPageCurrent = _menuPageLink;
      resetMenu();
    }

    bool exitToParentMenuPage() {
      if (_menuPageCurrent->getParentMenuPage()!=nullptr) {
        resetMenu();
        _menuPageCurrent = _menuPageCurrent->getParentMenuPage();
        return(true);
      }
      return(false);
    }

/********************************************************************/
    /* DRAW OPERATIONS */
/********************************************************************/
    // drawMenu() draws menu page set earlier in TeenyMenu::setMenuPageCurrent()
    void drawMenu() {
      drawTitleBar();
      if(_menuPageCurrent->itemsCountTotal>0) {
        drawMenuPointer();
        if(_menuPageCurrent->itemsCount>0) {
          drawMenuItems();
          drawScrollbar();
        }
      }
    }

    void drawTitleBar() {
      _displayPV.prt_str(_menuPageCurrent->title, strlen(_menuPageCurrent->title),
                         (_display.width()-(strlen(_menuPageCurrent->title)*_fontWidth))/2, 0);
    }

    void drawMenuPointer() {
      if (_menuPageCurrent->getCurrentMenuItem()->readonly) {
        _menuPageCurrent->resetCurrentItemNum();
      }
      if (_menuPageCurrent->itemsCount>0 &&
          !_menuPageCurrent->getCurrentMenuItem()->readonly) {
        uint16_t pointerPosition = getCurrentItemTopOffset();
        _display.drawRect(0, pointerPosition+1, 2, _menuItemHeight-3, _white);
      }
    }

    void drawMenuItems() {
      byte currentPageScreenNum = _menuPageCurrent->currentItemNum / _menuItemsPerScreen;
      TeenyMenuItem* menuItemTmp = (_menuPageCurrent)->getMenuItem(currentPageScreenNum * _menuItemsPerScreen);
      byte i = 0;
      uint16_t yOffset = _menuFirstItemScreenTopOffset;
      while (menuItemTmp != 0 && i < _menuItemsPerScreen) {
        switch (menuItemTmp->type) {
          case TEENYMENU_ITEM_VAL:
            if (menuItemTmp->readonly) {
              _displayPV.prt_str(menuItemTmp->title, _menuItemTitleLength, _menuItemTitleLeftOffset, yOffset);
              _displayPV.prt_char('=', 1);
            } else {
              _displayPV.prt_str(menuItemTmp->title, _menuItemTitleLength, _menuItemTitleLeftOffset, yOffset);
              _displayPV.prt_char(':', 1);
            }
            switch (menuItemTmp->linkedType) {
              case TEENYMENU_VAL_UINT8T:
                if (_editValueMode && menuItemTmp == _menuPageCurrent->getCurrentMenuItem()) {
                  _displayPV.prt_int(_editValue, _menuItemValueLength, _menuItemValueLeftOffset, yOffset);
                  _displayPV.prt_char(_cp437 ? TEENYMENU_CP437CHAR_CODE_SELECTARROWS : TEENYMENU_CHAR_CODE_SELECTARROWS,
                                      1, _display.width()-_fontWidth-1, yOffset);
                } else {
                  _displayPV.prt_int(*(uint8_t*)menuItemTmp->linkedVariable, _menuItemValueLength, _menuItemValueLeftOffset, yOffset);
                }
                break;
              case TEENYMENU_VAL_INT16T:
                if (_editValueMode && menuItemTmp == _menuPageCurrent->getCurrentMenuItem()) {
                  _displayPV.prt_int(_editValue, _menuItemValueLength, _menuItemValueLeftOffset, yOffset);
                  _displayPV.prt_char(_cp437 ? TEENYMENU_CP437CHAR_CODE_SELECTARROWS : TEENYMENU_CHAR_CODE_SELECTARROWS,
                                      1, _display.width()-_fontWidth-1, yOffset);
                } else {
                  _displayPV.prt_int(*(int16_t*)menuItemTmp->linkedVariable, _menuItemValueLength, _menuItemValueLeftOffset, yOffset);
                }
                break;
              case TEENYMENU_VAL_INT32T:
                if (_editValueMode && menuItemTmp == _menuPageCurrent->getCurrentMenuItem()) {
                  _displayPV.prt_int(_editValue, _menuItemValueLength, _menuItemValueLeftOffset, yOffset);
                  _displayPV.prt_char(_cp437 ? TEENYMENU_CP437CHAR_CODE_SELECTARROWS : TEENYMENU_CHAR_CODE_SELECTARROWS,
                                      1, _display.width()-_fontWidth-1, yOffset);
                } else {
                  _displayPV.prt_int(*(int32_t*)menuItemTmp->linkedVariable, _menuItemValueLength, _menuItemValueLeftOffset, yOffset);
                }
                break;
              case TEENYMENU_VAL_BOOLEAN:
                if (*(boolean*)menuItemTmp->linkedVariable) {
                  _displayPV.prt_str(menuItemTmp->boolTrueStr, _menuItemValueLength, _menuItemValueLeftOffset, yOffset);
                } else {
                  _displayPV.prt_str(menuItemTmp->boolFalseStr, _menuItemValueLength, _menuItemValueLeftOffset, yOffset);
                }
                break;
              case TEENYMENU_VAL_SELECT:
                TeenyMenuSelect* select = menuItemTmp->select;
                if (_editValueMode && menuItemTmp == _menuPageCurrent->getCurrentMenuItem()) {
                  _displayPV.prt_str(select->getOptionNameByIndex(_editValueSelectNum), _menuItemValueLength, _menuItemValueLeftOffset, yOffset);
                  _displayPV.prt_char(_cp437 ? TEENYMENU_CP437CHAR_CODE_SELECTARROWS : TEENYMENU_CHAR_CODE_SELECTARROWS,
                                      1, _display.width()-_fontWidth-1, yOffset);
                } else {
                  _displayPV.prt_str(select->getSelectedOptionName(menuItemTmp->linkedVariable), _menuItemValueLength, _menuItemValueLeftOffset, yOffset);
                }
                break;
            }
            break;
          case TEENYMENU_ITEM_LINK:
            if (menuItemTmp->readonly) {
              _displayPV.prt_str(menuItemTmp->title, _menuItemTitleLength+_menuItemValueLength+1, _menuItemTitleLeftOffset, yOffset);
            } else {
              _displayPV.prt_str(menuItemTmp->title, _menuItemTitleLength+_menuItemValueLength+1, _menuItemTitleLeftOffset, yOffset);
            }
            _displayPV.prt_char(_cp437 ? TEENYMENU_CP437CHAR_CODE_ARROWRIGHT : TEENYMENU_CHAR_CODE_ARROWRIGHT,
                                1, _display.width()-_fontWidth-1, yOffset);
            break;
          case TEENYMENU_ITEM_BACK:
            _displayPV.prt_char(_cp437 ? TEENYMENU_CP437CHAR_CODE_ARROWLEFT : TEENYMENU_CHAR_CODE_ARROWLEFT,
                                1, _menuItemTitleLeftOffset, yOffset);
            _displayPV.prt_str("exit", 4, _menuItemTitleLeftOffset+_fontWidth, yOffset);
            break;
          case TEENYMENU_ITEM_BUTTON:
            _displayPV.prt_char(TEENYMENU_CHAR_CODE_BULLET, 1, _menuItemTitleLeftOffset, yOffset);
            if (menuItemTmp->readonly) {
              _displayPV.prt_str(menuItemTmp->title, _menuItemTitleLength+_menuItemValueLength+1, _menuItemTitleLeftOffset+_fontWidth, yOffset);
            } else {
              _displayPV.prt_str(menuItemTmp->title, _menuItemTitleLength+_menuItemValueLength+1, _menuItemTitleLeftOffset+_fontWidth, yOffset);
            }
            break;
          case TEENYMENU_ITEM_LABEL:
            if(strlen(menuItemTmp->title) > 0) {
              _displayPV.prt_str(menuItemTmp->title, _menuItemLabelLength, _menuItemLabelLeftOffset, yOffset);
            }
            break;
        }
        menuItemTmp = menuItemTmp->getMenuItemNext();
        yOffset += _menuItemHeight;
        i++;
      }
    }

    void drawScrollbar() {
      byte screensCount = ((_menuPageCurrent->itemsCount % _menuItemsPerScreen)==0) ?
                              _menuPageCurrent->itemsCount / _menuItemsPerScreen :
                              _menuPageCurrent->itemsCount / _menuItemsPerScreen + 1;
      if (screensCount > 1) {
        byte currentScreenNum = _menuPageCurrent->currentItemNum / _menuItemsPerScreen;
      //byte scrollbarHeight = (_display.height() - _menuFirstItemScreenTopOffset) / screensCount;
        uint16_t scrollbarHeight = (_menuItemHeight * (uint16_t)_menuItemsPerScreen) / screensCount;
        uint16_t scrollbarPosition = (currentScreenNum * scrollbarHeight) + _menuFirstItemScreenTopOffset;
        // draw scrollbar at last pixel column = _display.width()-1
        _display.drawLine(_display.width()-1, scrollbarPosition,
                          _display.width()-1, scrollbarPosition+scrollbarHeight-1, _white);
      }
    }

    // Exit _editValueMode if needed and set first item as current item
    void resetMenu() {
      if (_editValueMode) {
        exitEditValueMode();
      }
      _menuPageCurrent->resetCurrentItemNum();
    }

/********************************************************************/
    /* KEY DETECTION */
/********************************************************************/
    // registerKeyPress() checks that menu is waiting for the key press
    // Register the key press and trigger corresponding action
    // Accepts TEENYMENU_KEY_NONE, TEENYMENU_KEY_UP, TEENYMENU_KEY_RIGHT, TEENYMENU_KEY_DOWN, TEENYMENU_KEY_LEFT values
    void registerKeyPress(byte keyCode) {
      _currentKey = keyCode;
      dispatchKeyPress();
    }

/********************************************************************/
    /* PRIVATE */
/********************************************************************/
  private:

/********************************************************************/
    /* DISPLAY */
/********************************************************************/
    T& _display;
    TeenyPrtVal<T> _displayPV;
    uint16_t _white;
    uint16_t _black;
    byte _fontWidth;
    byte _fontHeight;
    bool _cp437;
    byte _menuFirstItemScreenTopOffset;
    byte _menuItemHeight;
    byte _menuItemsPerScreen;
    byte _menuItemTitleLeftOffset;
    byte _menuItemTitleLength;
    byte _menuItemValueLeftOffset;
    byte _menuItemValueLength;
    byte _menuItemLabelLeftOffset;
    byte _menuItemLabelLength;

    // Private so usr cant infinite loop with page exitAction
    bool exitMenuPage() {
      if (_menuPageCurrent->exitAction != nullptr) {
        resetMenu();
        _menuPageCurrent->exitAction();
        return(true);
      }
      return(exitToParentMenuPage());
    }

/********************************************************************/
    /* DRAW OPERATIONS */
/********************************************************************/
    TeenyMenuPage* _menuPageCurrent;
    TeenyMenuItem* _menuItemCurrent;
    uint16_t getCurrentItemTopOffset() {
      return ((_menuPageCurrent->currentItemNum % _menuItemsPerScreen) * _menuItemHeight) +
              _menuFirstItemScreenTopOffset;
    }

/********************************************************************/
    /* MENU ITEMS NAVIGATION */
/********************************************************************/
    void nextMenuItem() {
      if (_menuPageCurrent->itemsCount) {
        for (byte i=0; i<_menuPageCurrent->itemsCount; i++) {
          if (_menuPageCurrent->currentItemNum == _menuPageCurrent->itemsCount-1) {
            if (_menuPageCurrent->keyDownAction != nullptr) {
              _menuPageCurrent->keyDownAction();
            } else {
              _menuPageCurrent->currentItemNum = 0;
            }
          } else {
            _menuPageCurrent->currentItemNum++;
          }
          if (!_menuPageCurrent->getCurrentMenuItem()->readonly) {
            break;
          }
        }
      } else if (_menuPageCurrent->keyDownAction != nullptr) {
        _menuPageCurrent->keyDownAction();
      }
    }

    void prevMenuItem() {
      if (_menuPageCurrent->itemsCount) {
        for (byte i=0; i<_menuPageCurrent->itemsCount; i++) {
          if (_menuPageCurrent->currentItemNum == 0) {
            if (_menuPageCurrent->keyUpAction != nullptr) {
              _menuPageCurrent->keyUpAction();
            } else {
              _menuPageCurrent->currentItemNum = _menuPageCurrent->itemsCount-1;
            }
          } else {
            _menuPageCurrent->currentItemNum--;
          }
          if (!_menuPageCurrent->getCurrentMenuItem()->readonly) {
            break;
          }
        }
      } else if (_menuPageCurrent->keyUpAction != nullptr) {
        _menuPageCurrent->keyUpAction();
      }
    }

    void menuItemSelect() {
      if (_menuPageCurrent->itemsCount) {
        TeenyMenuItem* menuItemTmp = _menuPageCurrent->getCurrentMenuItem();
        switch (menuItemTmp->type) {
          case TEENYMENU_ITEM_VAL:
            if (!menuItemTmp->readonly) {
              enterEditValueMode();
            }
            break;
          case TEENYMENU_ITEM_BUTTON:
            if (!menuItemTmp->readonly) {
              menuItemTmp->buttonAction();
            }
            break;
          case TEENYMENU_ITEM_LINK:
            if (!menuItemTmp->readonly) {
              linkMenuPage(*menuItemTmp->linkedPage);
            }
            break;
          case TEENYMENU_ITEM_BACK:
            exitMenuPage();
            break;
        }
      } else if (_menuPageCurrent->keySelectAction != nullptr) {
        _menuPageCurrent->keySelectAction();
      }
    }

/********************************************************************/
    /* VALUE EDIT */
/********************************************************************/
    boolean _editValueMode;
    byte _editValueType;
    int _editValue;
    int _editValueSelectNum;

    void enterEditValueMode() {
      _editValueMode = true;
      TeenyMenuItem* menuItemTmp = _menuPageCurrent->getCurrentMenuItem();
      _editValueType = menuItemTmp->linkedType;
      switch (_editValueType) {
        case TEENYMENU_VAL_UINT8T:
          _editValue = *(uint8_t*)menuItemTmp->linkedVariable;
          break;
        case TEENYMENU_VAL_INT16T:
          _editValue = *(int16_t*)menuItemTmp->linkedVariable;
          break;
        case TEENYMENU_VAL_INT32T:
          _editValue = *(int32_t*)menuItemTmp->linkedVariable;
          break;
        case TEENYMENU_VAL_BOOLEAN:
          booleanValueToggle();
          break;
        case TEENYMENU_VAL_SELECT:
          TeenyMenuSelect* select = menuItemTmp->select;
          _editValueSelectNum = select->getSelectedOptionNum(menuItemTmp->linkedVariable);
          break;
      }
    }

    void booleanValueToggle() {
      TeenyMenuItem* menuItemTmp = _menuPageCurrent->getCurrentMenuItem();
      boolean booleanValue = *(boolean*)menuItemTmp->linkedVariable;
      *(boolean*)menuItemTmp->linkedVariable = !booleanValue;
      if (menuItemTmp->saveAction != nullptr) {
        menuItemTmp->saveAction();
        exitEditValueMode();
      } else {
        exitEditValueMode();
      }
    }

    void decrementEditValue() {
      TeenyMenuItem* menuItemTmp = _menuPageCurrent->getCurrentMenuItem();
      switch (menuItemTmp->linkedType) {
        case TEENYMENU_VAL_UINT8T:
          if (menuItemTmp->rangeMin!=nullptr && _editValue<=*(uint8_t*)menuItemTmp->rangeMin) {
            _editValue = *(uint8_t*)menuItemTmp->rangeMin;
          } else if (menuItemTmp->rangeMax!=nullptr && _editValue>*(uint8_t*)menuItemTmp->rangeMax) {
            _editValue = *(uint8_t*)menuItemTmp->rangeMax;
          } else {
            _editValue--;
          }
          break;
        case TEENYMENU_VAL_INT16T:
          if (menuItemTmp->rangeMin!=nullptr && _editValue<=*(int16_t*)menuItemTmp->rangeMin) {
            _editValue = *(int16_t*)menuItemTmp->rangeMin;
          } else if (menuItemTmp->rangeMax!=nullptr && _editValue>*(int16_t*)menuItemTmp->rangeMax) {
            _editValue = *(int16_t*)menuItemTmp->rangeMax;
          } else {
            _editValue--;
          }
          break;
        case TEENYMENU_VAL_INT32T:
          if (menuItemTmp->rangeMin!=nullptr && _editValue<=*(int32_t*)menuItemTmp->rangeMin) {
            _editValue = *(int32_t*)menuItemTmp->rangeMin;
          } else if (menuItemTmp->rangeMax!=nullptr && _editValue>*(int32_t*)menuItemTmp->rangeMax) {
            _editValue = *(int32_t*)menuItemTmp->rangeMax;
          } else {
            _editValue--;
          }
            break;
      }
    }

    void incrementEditValue() {
      TeenyMenuItem* menuItemTmp = _menuPageCurrent->getCurrentMenuItem();
      switch (menuItemTmp->linkedType) {
        case TEENYMENU_VAL_UINT8T:
          if (menuItemTmp->rangeMax!=nullptr && _editValue>=*(uint8_t*)menuItemTmp->rangeMax) {
            _editValue = *(uint8_t*)menuItemTmp->rangeMax;
          } else if (menuItemTmp->rangeMin!=nullptr && _editValue<*(uint8_t*)menuItemTmp->rangeMin) {
            _editValue = *(uint8_t*)menuItemTmp->rangeMin;
          } else {
            _editValue++;
          }
          break;
        case TEENYMENU_VAL_INT16T:
          if (menuItemTmp->rangeMax!=nullptr && _editValue>=*(int16_t*)menuItemTmp->rangeMax) {
            _editValue = *(int16_t*)menuItemTmp->rangeMax;
          } else if (menuItemTmp->rangeMin!=nullptr && _editValue<*(int16_t*)menuItemTmp->rangeMin) {
            _editValue = *(int16_t*)menuItemTmp->rangeMin;
          } else {
            _editValue++;
          }
          break;
        case TEENYMENU_VAL_INT32T:
          if (menuItemTmp->rangeMax!=nullptr && _editValue>=*(int32_t*)menuItemTmp->rangeMax) {
            _editValue = *(int32_t*)menuItemTmp->rangeMax;
          } else if (menuItemTmp->rangeMin!=nullptr && _editValue<*(int32_t*)menuItemTmp->rangeMin) {
            _editValue = *(int32_t*)menuItemTmp->rangeMin;
          } else {
            _editValue++;
          }
          break;
      }
    }

    void nextEditValueSelectNum() {
      setEditValueSelectNum(TEENYMENU_KEY_DOWN);
    }

    void prevEditValueSelectNum() {
      setEditValueSelectNum(TEENYMENU_KEY_UP);
    }

    void setEditValueSelectNum(byte dir) {
      TeenyMenuItem* menuItemTmp = _menuPageCurrent->getCurrentMenuItem();
      TeenyMenuSelect* select = menuItemTmp->select;
      byte scrollType = menuItemTmp->scrollType;
      if (scrollType==TEENYMENU_VAL_SELECT_SCROLL_REV_WRAP ||
          scrollType==TEENYMENU_VAL_SELECT_SCROLL_REV_NOWRAP) {
        dir = (dir==TEENYMENU_KEY_UP) ? TEENYMENU_KEY_DOWN : TEENYMENU_KEY_UP;
      }
      if (dir==TEENYMENU_KEY_UP) {
        if (_editValueSelectNum > 0) {
          _editValueSelectNum--;
        } else {
          if (scrollType==TEENYMENU_VAL_SELECT_SCROLL_FWD_WRAP ||
              scrollType==TEENYMENU_VAL_SELECT_SCROLL_REV_WRAP) {
            _editValueSelectNum = select->getLength()-1;
          } else {
            _editValueSelectNum = 0;
          }
        }
      } else {
        if (_editValueSelectNum < (select->getLength()-1)) {
          _editValueSelectNum++;
        } else {
          if (scrollType==TEENYMENU_VAL_SELECT_SCROLL_FWD_WRAP ||
              scrollType==TEENYMENU_VAL_SELECT_SCROLL_REV_WRAP) {
            _editValueSelectNum = 0;
          } else {
            _editValueSelectNum = select->getLength()-1;
          }
        }
      }
    }

    void saveEditValue() {
      TeenyMenuItem* menuItemTmp = _menuPageCurrent->getCurrentMenuItem();
      switch (menuItemTmp->linkedType) {
        case TEENYMENU_VAL_UINT8T:
          *(uint8_t*)menuItemTmp->linkedVariable = _editValue;
          break;
        case TEENYMENU_VAL_INT16T:
          *(int16_t*)menuItemTmp->linkedVariable = _editValue;
          break;
        case TEENYMENU_VAL_INT32T:
          *(int32_t*)menuItemTmp->linkedVariable = _editValue;
          break;
        case TEENYMENU_VAL_SELECT:
          TeenyMenuSelect* select = menuItemTmp->select;
          select->setValue(menuItemTmp->linkedVariable, _editValueSelectNum);
          break;
      }
      if (menuItemTmp->saveAction != nullptr) {
        menuItemTmp->saveAction();
      }
      exitEditValueMode();
    }

    void exitEditValueMode() {
      _editValueSelectNum = -1;
      _editValueMode = false;
    }

/********************************************************************/
    /* KEY DETECTION */
/********************************************************************/
    byte _currentKey;

    void dispatchKeyPress() {
      if (_editValueMode) {
        switch (_currentKey) {
          case TEENYMENU_KEY_UP:
            if (_editValueType == TEENYMENU_VAL_SELECT) {
              prevEditValueSelectNum();
            } else {
              incrementEditValue();
            }
            break;
          case TEENYMENU_KEY_RIGHT:
            if (_editValueType == TEENYMENU_VAL_SELECT) {
              saveEditValue();
            } else {
              saveEditValue();
            }
            break;
          case TEENYMENU_KEY_DOWN:
            if (_editValueType == TEENYMENU_VAL_SELECT) {
              nextEditValueSelectNum();
            } else {
              decrementEditValue();
            }
            break;
          case TEENYMENU_KEY_LEFT:
            if (_editValueType == TEENYMENU_VAL_SELECT) {
              exitEditValueMode();
            } else {
              exitEditValueMode();
            }
              break;
        }
      } else {
        switch (_currentKey) {
          case TEENYMENU_KEY_UP:
            prevMenuItem();
            break;
          case TEENYMENU_KEY_RIGHT:
            menuItemSelect();
            break;
          case TEENYMENU_KEY_DOWN:
            nextMenuItem();
            break;
          case TEENYMENU_KEY_LEFT:
            exitMenuPage();
            break;
        }
      }
    }

};

#endif

