#include "TeenyMenuConstants.h"
#include "TeenyMenuPage.h"

#ifndef HEADER_TEENYMENUITEM
#define HEADER_TEENYMENUITEM

// Forward declaration of necessary classes
class TeenyMenuPage;
class TeenyMenuSelect;

// Declaration of TeenyMenuItem class
class TeenyMenuItem {
  template <class T>
  friend class TeenyMenu;
  friend class TeenyMenuPage;
  public:
    /* 
      Constructors for menu item that represents option select, w/ callback
      @param 'title_' - title of the menu item displayed on the screen
      @param 'linkedVariable_' - reference to variable that menu item is associated with (either byte, int, char*)
      @param 'select_' - reference to TeenyMenuSelect option select
      @param 'saveAction_' - pointer to callback function executed when associated variable is successfully saved
    */
    TeenyMenuItem(const char* title_, uint8_t& linkedVariable_, TeenyMenuSelect& select_, void (*saveAction_)(), byte scrollType_=0);
    TeenyMenuItem(const char* title_, int16_t& linkedVariable_, TeenyMenuSelect& select_, void (*saveAction_)(), byte scrollType_=0);
    TeenyMenuItem(const char* title_, int32_t& linkedVariable_, TeenyMenuSelect& select_, void (*saveAction_)(), byte scrollType_=0);
    /* 
      Constructors for menu item that represents option select, w/o callback
      @param 'title_' - title of the menu item displayed on the screen
      @param 'linkedVariable_' - reference to variable that menu item is associated with (either byte, int, char*)
      @param 'select_' - reference to TeenyMenuSelect option select
      @param 'readonly_' (optional) - set readonly mode for variable that option select is associated with
      values TEENYMENU_READONLY (alias for true) default false
    */
    TeenyMenuItem(const char* title_, uint8_t& linkedVariable_, TeenyMenuSelect& select_, boolean readonly_ = false, byte scrollType_=0);
    TeenyMenuItem(const char* title_, int16_t& linkedVariable_, TeenyMenuSelect& select_, boolean readonly_ = false, byte scrollType_=0);
    TeenyMenuItem(const char* title_, int32_t& linkedVariable_, TeenyMenuSelect& select_, boolean readonly_ = false, byte scrollType_=0);
    /* 
      Constructors for menu item that represents variable, w/ callback
      @param 'title_' - title of the menu item displayed on the screen
      @param 'linkedVariable_' - reference to variable that menu item is associated with (either byte, int, char*, boolean)
      @param 'saveAction_' - pointer to callback function executed when associated variable is successfully saved
      @param 'boolFalseStr_' - display string for value=false
      @param 'boolTrueStr_' - display string for value=true
    */
    TeenyMenuItem(const char* title_, uint8_t& linkedVariable_, void (*saveAction_)());
    TeenyMenuItem(const char* title_, int16_t& linkedVariable_, void (*saveAction_)());
    TeenyMenuItem(const char* title_, int32_t& linkedVariable_, void (*saveAction_)());
    TeenyMenuItem(const char* title_, boolean& linkedVariable_, void (*saveAction_)(), const char* boolFalseStr_="FALSE", const char* boolTrueStr_="TRUE");
    /* 
      Constructors for menu item that represents variable, w/ callback, w/ min/max range
      @param 'title_' - title of the menu item displayed on the screen
      @param 'linkedVariable_' - reference to variable that menu item is associated with (either byte, int, char*, boolean)
      @param 'rangeMin_' - range minimum value
      @param 'rangeMax_' - range maximum value
      @param 'saveAction_' - pointer to callback function executed when associated variable is successfully saved
    */
    TeenyMenuItem(const char* title_, uint8_t& linkedVariable_, uint8_t& rangeMin_, uint8_t& rangeMax_, void (*saveAction_)());
    TeenyMenuItem(const char* title_, int16_t& linkedVariable_, int16_t& rangeMin_, int16_t& rangeMax_, void (*saveAction_)());
    TeenyMenuItem(const char* title_, int32_t& linkedVariable_, int32_t& rangeMin_, int32_t& rangeMax_, void (*saveAction_)());
    /* 
      Constructors for menu item that represents variable, w/o callback
      @param 'title_' - title of the menu item displayed on the screen
      @param 'linkedVariable_' - reference to variable that menu item is associated with (either byte, int, char*, boolean)
      @param 'readonly_' (optional) - set readonly mode for variable that menu item is associated with
      values TEENYMENU_READONLY (alias for true) default false
      @param 'boolFalseStr_' - display string for value=false
      @param 'boolTrueStr_' - display string for value=true
    */
    TeenyMenuItem(const char* title_, uint8_t& linkedVariable_, boolean readonly_ = false);
    TeenyMenuItem(const char* title_, int16_t& linkedVariable_, boolean readonly_ = false);
    TeenyMenuItem(const char* title_, int32_t& linkedVariable_, boolean readonly_ = false);
    TeenyMenuItem(const char* title_, boolean& linkedVariable_, boolean readonly_ = false, const char* boolFalseStr_="FALSE", const char* boolTrueStr_ = "TRUE");
    /* 
      Constructors for menu item that represents variable, w/o callback, w/ min/max range
      @param 'title_' - title of the menu item displayed on the screen
      @param 'linkedVariable_' - reference to variable that menu item is associated with (either byte, int, char*, boolean)
      @param 'rangeMin_' - range minimum value
      @param 'rangeMax_' - range maximum value
      @param 'readonly_' (optional) - set readonly mode for variable that menu item is associated with
      values TEENYMENU_READONLY (alias for true) default false
    */
    TeenyMenuItem(const char* title_, uint8_t& linkedVariable_, uint8_t& rangeMin_, uint8_t& rangeMax_, boolean readonly_ = false);
    TeenyMenuItem(const char* title_, int16_t& linkedVariable_, int16_t& rangeMin_, int16_t& rangeMax_, boolean readonly_ = false);
    TeenyMenuItem(const char* title_, int32_t& linkedVariable_, int32_t& rangeMin_, int32_t& rangeMax_, boolean readonly_ = false);
    /* 
      Constructor for menu item that represents link to another menu page (via reference)
      @param 'title_' - title of the menu item displayed on the screen
      @param 'linkedPage_' - reference to TeenyMenuPage menu page that menu item is associated with
      @param 'readonly_' (optional) - set readonly mode for the link (user won't be able to navigate to linked page)
      values TEENYMENU_READONLY (alias for true) default false
    */
    TeenyMenuItem(const char* title_, TeenyMenuPage& linkedPage_, boolean readonly_ = false);
    /* 
      Constructor for menu item that represents link to another menu page (via pointer)
      @param 'title_' - title of the menu item displayed on the screen
      @param 'linkedPage_' - pointer to TeenyMenuPage menu page that menu item is associated with
      @param 'readonly_' (optional) - set readonly mode for the link (user won't be able to navigate to linked page)
      values TEENYMENU_READONLY (alias for true) default false
    */
    TeenyMenuItem(const char* title_, TeenyMenuPage* linkedPage_, boolean readonly_ = false);
    /* 
      Constructor for menu item that represents returning to the previous/parent menu page
      @param 'readonly_' (optional) - set readonly mode for the link (user won't be able to navigate to previous/parent page)
      values TEENYMENU_READONLY (alias for true) default false
    */
    TeenyMenuItem(boolean readonly_ = false);
    /* 
      Constructor for menu item that represents button
      @param 'title_' - title of the menu item displayed on the screen
      @param 'buttonAction_' - pointer to function that will be executed when menu item is activated
      @param 'readonly_' (optional) - set readonly mode for the button (user won't be able to call action associated with it)
      values TEENYMENU_READONLY (alias for true) default false
    */
    TeenyMenuItem(const char* title_, void (*buttonAction_)(), boolean readonly_ = false);
    /* 
      Constructor for menu item that represents a non-functional (readonly) text item
      @param 'title_' - title of the menu item displayed on the screen
    */
    TeenyMenuItem(const char* title_);

    void setTitle(const char* title_);            // Set title of the menu item
    const char* getTitle();                       // Get title of the menu item
    void setReadonly(boolean mode = true);  // Explicitly set or unset readonly mode for variable that menu item is associated with
                                            // (relevant for TEENYMENU_VAL_INT32T, TEENYMENU_VAL_INT16T, TEENYMENU_VAL_UINT8T, TEENYMENU_VAL_BOOLEAN
                                            // variable menu items and TEENYMENU_VAL_SELECT option select), or menu button TEENYMENU_ITEM_BUTTON
                                            // and menu link TEENYMENU_ITEM_LINK/BACK, pressing of which won't result in any action, associated with them
    boolean getReadonly();                  // Get readonly state of the variable that menu item is associated with (as well as menu link or button)
    void hide(boolean hide = true);         // Explicitly hide or show menu item
    void show();                            // Explicitly show menu item
    boolean isHidden();                     // Get hidden state of the menu item
  private:
    const char* title;
    byte type;
    void* linkedVariable;
    byte linkedType;
    byte scrollType;
    const char* boolFalseStr;
    const char* boolTrueStr;
    void* rangeMin;
    void* rangeMax;
    boolean readonly = false;
    boolean hidden = false;
    TeenyMenuSelect* select;
    TeenyMenuPage* parentPage = nullptr;
    TeenyMenuPage* linkedPage;
    TeenyMenuItem* menuItemNext;
    TeenyMenuItem* getMenuItemNext();             // Get next menu item, excluding hidden ones
    void (*buttonAction)();
    void (*saveAction)();
};

#endif

