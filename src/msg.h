
/********************************************************************/
bool msg_update(const char* msgStr) {
  uint32_t _nowMS = millis();
  static uint32_t _msg_timer = 0;
  static bool _msg_state = false;
  static char _msgStr[22];
  if(msgStr) {
    strcpy(_msgStr, msgStr);
    _msg_state = true;
    _msg_timer = _nowMS;
    displayRefresh = true;
    return true;
  }
  if(_msg_state) {
    if((_nowMS-_msg_timer) > MSG_DISPLAY_TIME) {
      _msg_state = false;
      return true;
    }
    if(displayRefresh) {
      displayPV.prt_str(_msgStr, 20, 0, 304);
    }
  }
  return false;
}

