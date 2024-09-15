#ifndef HEADER_TEENYMENUPAGE
#define HEADER_TEENYMENUPAGE

#include <Arduino.h>
#include "TeenyMenuItem.h"

// Declaration of TeenyMenuPage class
class TeenyMenuPage {
  template <class T>
  friend class TeenyMenu;
  friend class TeenyMenuItem;
  public:
    /* 
      @param 'title_' - title of the menu page displayed at top of the screen
      @param 'enterAction_' - optional pointer to callback function executed when entering page
      @param 'exitAction_' - optional pointer to callback function executed when TEENYMENU_KEY_LEFT is pressed or TEENYMENU_ITEM_BACK is selected
                           - If exitAction_ is not specified, return to parent menu page or nothing if at top level page
                           - ***NOTE*** exitAction_ REPLACES return to parent menu page ***BUT*** you can add
                             exitToParentMenuPage() to exitAction_ if you want to do something AND return to the parent menu page
      @param 'keySelectAction_' - optional pointer to callback function executed for TEENYMENU_KEY_RIGHT (Only valid for itemless pages)
      @param 'keyUpAction_' - optional pointer to callback function executed for TEENYMENU_KEY_UP (Only valid for itemless pages)
      @param 'keyDownAction_' - optional pointer to callback function executed for TEENYMENU_KEY_DOWN (Only valid for itemless pages)
    */
    TeenyMenuPage(const char* title_ = "", void (*enterAction_)() = nullptr, void (*exitAction_)() = nullptr,
                  void (*keySelectUpAction_)() = nullptr, void (*keyUpAction_)() = nullptr, void (*keyDownAction_)() = nullptr);
    void setParentMenuPage(TeenyMenuPage& parentMenuPage);  // Specify parent level menu page (to know where to go back to when pressing Back button)
    TeenyMenuPage* getParentMenuPage();                     // Get parent level menu page (to know where to go back to when pressing Back button)
    void setTitle(const char* title_);                      // Set title of the menu page
    const char* getTitle();                                 // Get title of the menu page
    void addMenuItem(TeenyMenuItem& menuItem);        // Add menu item to menu page
    byte getCurrentItemNum();                         // Get currently selected (focused) menu item of the page
    void resetCurrentItemNum();                       // Find first item that is not readonly or type TEENYMENU_ITEM_BACK
    TeenyMenuItem* getMenuItem(byte index, boolean total = false);
  private:
    TeenyMenuPage* _parentMenuPage = nullptr;
    const char* title;
    byte currentItemNum = 0;                          // Currently selected (focused) menu item of the page
    byte itemsCount = 0;                              // Items count excluding hidden ones
    byte itemsCountTotal = 0;                         // Items count incuding hidden ones
    TeenyMenuItem* _menuItem;                         // First menu item of the page (the following ones are linked from within one another)
    TeenyMenuItem* getCurrentMenuItem();
    int getMenuItemNum(TeenyMenuItem& menuItem);      // Find index of the supplied menu item
    void hideMenuItem(TeenyMenuItem& menuItem);
    void showMenuItem(TeenyMenuItem& menuItem);
    void (*enterAction)();
    void (*exitAction)();
    void (*keySelectAction)();
    void (*keyUpAction)();
    void (*keyDownAction)();
};
  
#endif

