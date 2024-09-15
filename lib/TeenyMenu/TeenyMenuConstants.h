#ifndef HEADER_TEENYMENUCONSTANTS
#define HEADER_TEENYMENUCONSTANTS

// Macro constant (alias) for supported length of the string (character sequence) variable of type char[TEENYMENU_STR_LEN]
#define TEENYMENU_STR_LEN 17

// Macro constants (aliases) for the keys (buttons) used to navigate and interact with menu
enum teenyMenu_key_t : byte {
  TEENYMENU_KEY_NONE  = 0, // No key presses are detected
  TEENYMENU_KEY_UP    = 1, // Up key is pressed (navigate up through the menu items list, increment variable, or previous option in select)
  TEENYMENU_KEY_RIGHT = 2, // Right key is pressed (navigate through the link to another (child) menu page, toggle boolean menu item, enter edit mode of the associated non-boolean variable, save variable, execute code associated with button)
  TEENYMENU_KEY_DOWN  = 3, // Down key is pressed (navigate down through the menu items list, decrement variable, or next option in select)
  TEENYMENU_KEY_LEFT  = 4  // Left key is pressed (navigate to the previous (parent) menu page, exit without saving the variable or select option)
};

// Macro constants (aliases) for some of the ASCII character codes
#define TEENYMENU_CHAR_CODE_BULLET 0xAF
#define TEENYMENU_CHAR_CODE_ARROWRIGHT 0x3E
#define TEENYMENU_CHAR_CODE_ARROWLEFT 0x3C
#define TEENYMENU_CHAR_CODE_SELECTARROWS 0xF0
#define TEENYMENU_CP437CHAR_CODE_ARROWRIGHT 0x10
#define TEENYMENU_CP437CHAR_CODE_ARROWLEFT 0x11
#define TEENYMENU_CP437CHAR_CODE_SELECTARROWS 0x12

// Macro constants (aliases) for menu item types
#define TEENYMENU_ITEM_VAL    0  // Menu item represents associated variable
#define TEENYMENU_ITEM_LINK   1  // Menu item represents link to another menu page
#define TEENYMENU_ITEM_BACK   2  // Menu item represents Back button
#define TEENYMENU_ITEM_BUTTON 3  // Menu item represents button for calling of user-defined function
#define TEENYMENU_ITEM_LABEL  4  // Non-functional (readonly) Menu item

// Macro constant (alias) for readonly modifier of associated with menu item variable
#define TEENYMENU_READONLY true

// Macro constant (alias) for hidden state of the menu item
#define TEENYMENU_HIDDEN true

// Macro constants (aliases) for supported types of associated with menu item variable
#define TEENYMENU_VAL_INT32T  0  // Associated variable is of type int32_t
#define TEENYMENU_VAL_INT16T  1  // Associated variable is of type int16_t
#define TEENYMENU_VAL_UINT8T  2  // Associated variable is of type uint8_t
#define TEENYMENU_VAL_BOOLEAN 3  // Associated variable is of type boolean
#define TEENYMENU_VAL_SELECT  4  // Associated variable is either of type int, byte or char[] with option select used to pick a predefined value from the list
                                 // (note that char[] array should be big enough to hold select option with the longest value)

// Macro constants (aliases) for supported scrolling of TEENYMENU_VAL_SELECT menu item values
#define TEENYMENU_VAL_SELECT_SCROLL_FWD_WRAP   0  // Associated variable is of type int
#define TEENYMENU_VAL_SELECT_SCROLL_FWD_NOWRAP 1  // Associated variable is of type int
#define TEENYMENU_VAL_SELECT_SCROLL_REV_WRAP   2  // Associated variable is of type int
#define TEENYMENU_VAL_SELECT_SCROLL_REV_NOWRAP 3  // Associated variable is of type int

#endif

