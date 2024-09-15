#include <Arduino.h>
#include "TeenyMenuPage.h"

TeenyMenuPage::TeenyMenuPage(const char* title_, void (*enterAction_)(), void (*exitAction_)(), void (*keySelectAction_)(), void (*keyUpAction_)(), void (*keyDownAction_)()) :
  title(title_),
  enterAction(enterAction_),
  exitAction(exitAction_),
  keySelectAction(keySelectAction_),
  keyUpAction(keyUpAction_),
  keyDownAction(keyDownAction_)
{ }

void TeenyMenuPage::setParentMenuPage(TeenyMenuPage& parentMenuPage) {
  _parentMenuPage = &parentMenuPage;
}

TeenyMenuPage* TeenyMenuPage::getParentMenuPage() {
  return _parentMenuPage;
}

void TeenyMenuPage::setTitle(const char* title_) {
  title = title_;
}

const char* TeenyMenuPage::getTitle() {
  return title;
}

void TeenyMenuPage::addMenuItem(TeenyMenuItem& menuItem) {
  // Prevent adding menu item that was already added to another (or the same) page
  if (menuItem.parentPage == nullptr) {
    if (itemsCountTotal == 0) {
      // If menu page is empty, link supplied menu item from within page directly (this will be the first menu item in a page)
      _menuItem = &menuItem;
    } else {
      // If menu page is not empty, link supplied menu item from within the last menu item of the page
      getMenuItem(itemsCountTotal-1, true)->menuItemNext = &menuItem;
    }
    menuItem.parentPage = this;
    if (!menuItem.hidden) {
      itemsCount++;
    }
    itemsCountTotal++;
    resetCurrentItemNum();
  }
}

byte TeenyMenuPage::getCurrentItemNum() {
  return currentItemNum;
}

void TeenyMenuPage::resetCurrentItemNum() {
  currentItemNum = 0;
  if(itemsCount>0) {
    currentItemNum = itemsCount-1;
    if(itemsCount>1) {
      for (byte i=itemsCount-1; i>0; i--) {
        if ((getMenuItem(i-1)->type!=TEENYMENU_ITEM_BACK && !getMenuItem(i-1)->readonly) ||
            (getMenuItem(currentItemNum)->readonly)) {
          currentItemNum = i-1;
        }
      }
    }
  }
}

TeenyMenuItem* TeenyMenuPage::getMenuItem(byte index, boolean total) {
  TeenyMenuItem* menuItemTmp = (_menuItem->hidden) ? _menuItem->getMenuItemNext() : _menuItem;
  for (byte i=0; i<index; i++) {
    menuItemTmp = (total) ? menuItemTmp->menuItemNext : menuItemTmp->getMenuItemNext();
  }
  return menuItemTmp;
}

TeenyMenuItem* TeenyMenuPage::getCurrentMenuItem() {
  return getMenuItem(currentItemNum);
}

int TeenyMenuPage::getMenuItemNum(TeenyMenuItem& menuItem) {
  TeenyMenuItem* menuItemTmp = (_menuItem->hidden) ? _menuItem->getMenuItemNext() : _menuItem;
  for (byte i=0; i<itemsCount; i++) {
    if (menuItemTmp == &menuItem) {
      return i;
    }
    menuItemTmp = menuItemTmp->getMenuItemNext();
  }
  return -1;
}

void TeenyMenuPage::hideMenuItem(TeenyMenuItem& menuItem) {
  int menuItemNum = getMenuItemNum(menuItem);
  menuItem.hidden = true;
  itemsCount--;
  if (menuItemNum <= currentItemNum) {
    if (currentItemNum > 0) {
      currentItemNum--;
    }
  }
  // if (itemsCount==1 && _menuItemBack.linkedPage!=nullptr) {
  //   currentItemNum = 0;
  // }
  if (itemsCount==1 && getMenuItem(0)->type==TEENYMENU_ITEM_BACK) {
    currentItemNum = 0;
  }
}

void TeenyMenuPage::showMenuItem(TeenyMenuItem& menuItem) {
  menuItem.hidden = false;
  itemsCount++;
  int menuItemNum = getMenuItemNum(menuItem);
  if (menuItemNum < currentItemNum) {
    if (currentItemNum < itemsCount-1) {
      currentItemNum++;
    }
  }
  // if (itemsCount>1 && _menuItemBack.linkedPage!=nullptr) {
  //   currentItemNum = 1;
  // }
  if (itemsCount>1 && currentItemNum==0 &&
      getMenuItem(0)->type==TEENYMENU_ITEM_BACK) {
    currentItemNum = 1;
  }
}

