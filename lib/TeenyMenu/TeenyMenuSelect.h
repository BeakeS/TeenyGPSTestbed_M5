#ifndef HEADER_TEENYMENUSELECT
#define HEADER_TEENYMENUSELECT

// Declaration of SelectOptionUint8t type
struct SelectOptionUint8t {
  const char* name;   // Text label of the option as displayed in select
  uint8_t val_uint8t; // Value of the option that is assigned to linked variable upon option selection
};

// Declaration of SelectOptionInt16t type
struct SelectOptionInt16t {
  const char* name;   // Text label of the option as displayed in select
  int16_t val_int16t; // Value of the option that is assigned to linked variable upon option selection
};

// Declaration of SelectOptionInt32t type
struct SelectOptionInt32t {
  const char* name;   // Text label of the option as displayed in select
  int32_t val_int32t; // Value of the option that is assigned to linked variable upon option selection
};

// Declaration of TeenyMenuSelect class
class TeenyMenuSelect {
  template <class T>
  friend class TeenyMenu;
  public:
    /* 
      @param 'length_' - length of the 'options_' array
      @param 'options_' - array of the available options
    */
    TeenyMenuSelect(byte length_, SelectOptionUint8t* options_);
    TeenyMenuSelect(byte length_, SelectOptionInt16t* options_);
    TeenyMenuSelect(byte length_, SelectOptionInt32t* options_);
  private:
    byte _type;
    byte _length;
    void* _options;
    byte getType();
    byte getLength();
    int getSelectedOptionNum(void* variable);
    char* getSelectedOptionName(void* variable);
    char* getOptionNameByIndex(int index);
    void setValue(void* variable, int index);  // Assign value of the selected option to supplied variable
};
  
#endif

