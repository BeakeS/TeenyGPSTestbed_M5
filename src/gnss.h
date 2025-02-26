
/********************************************************************/
// Global Variables
enum gnss_config_mode_t : uint8_t {
  GC_MODE_LOAD    = 0,
  GC_MODE_ENABLE  = 1,
  GC_MODE_DISABLE = 2,
  GC_MODE_TOGGLE  = 3
};
ubloxCFGGNSSState_t gnssState;

/********************************************************************/
// Satellite Configuration

bool gnssConfigurator(uint8_t mode, const char* selectGNSSName) {
  char _msgStr[22];
  // Poll GNSS config state
  bool pollRC = gps.pollGNSSConfig();
  gnssState = gps.getGNSSConfigState();
  if((!pollRC) || mode == GC_MODE_LOAD) {
    sprintf(_msgStr, "Poll CFG-GNSS rc=%d", pollRC);
    msg_update(_msgStr);
    return pollRC;
  }
  int8_t gnssId;
  char gnssIdType;
  char gnssSignalName[7];
  int8_t* gnssEnable;
  if(strcmp(selectGNSSName, "GPS") == 0) {
    gnssId = 0;
    gnssIdType = 'G';
    strcpy(gnssSignalName, "");
    gnssEnable = &gnssState.GPS;
  } else if(strcmp(selectGNSSName, "GPS_L1CA") == 0) {
    gnssId = 0;
    gnssIdType = 'G';
    strcpy(gnssSignalName, "L1CA");
    gnssEnable = &gnssState.GPS_L1CA;
  } else if(strcmp(selectGNSSName, "SBAS") == 0) {
    gnssId = 1;
    gnssIdType = 'S';
    strcpy(gnssSignalName, "");
    gnssEnable = &gnssState.SBAS;
  } else if(strcmp(selectGNSSName, "SBAS_L1CA") == 0) {
    gnssId = 1;
    gnssIdType = 'S';
    strcpy(gnssSignalName, "L1CA");
    gnssEnable = &gnssState.SBAS_L1CA;
  } else if(strcmp(selectGNSSName, "Galileo") == 0) {
    gnssId = 2;
    gnssIdType = 'E';
    strcpy(gnssSignalName, "");
    gnssEnable = &gnssState.Galileo;
  } else if(strcmp(selectGNSSName, "Galileo_E1") == 0) {
    gnssId = 2;
    gnssIdType = 'E';
    strcpy(gnssSignalName, "E1");
    gnssEnable = &gnssState.Galileo_E1;
  } else if(strcmp(selectGNSSName, "BeiDou") == 0) {
    gnssId = 3;
    gnssIdType = 'B';
    strcpy(gnssSignalName, "");
    gnssEnable = &gnssState.BeiDou;
  } else if(strcmp(selectGNSSName, "BeiDou_B1") == 0) {
    gnssId = 3;
    gnssIdType = 'B';
    strcpy(gnssSignalName, "B1");
    gnssEnable = &gnssState.BeiDou_B1;
  } else if(strcmp(selectGNSSName, "BeiDou_B1C") == 0) {
    gnssId = 3;
    gnssIdType = 'B';
    strcpy(gnssSignalName, "_B1C");
    gnssEnable = &gnssState.BeiDou_B1C;
  } else if(strcmp(selectGNSSName, "IMES") == 0) {
    gnssId = 4;
    gnssIdType = 'I';
    strcpy(gnssSignalName, "");
    gnssEnable = &gnssState.IMES;
  } else if(strcmp(selectGNSSName, "QZSS") == 0) {
    gnssId = 5;
    gnssIdType = 'Q';
    strcpy(gnssSignalName, "");
    gnssEnable = &gnssState.QZSS;
  } else if(strcmp(selectGNSSName, "QZSS_L1CA") == 0) {
    gnssId = 5;
    gnssIdType = 'Q';
    strcpy(gnssSignalName, "L1CA");
    gnssEnable = &gnssState.QZSS_L1CA;
  } else if(strcmp(selectGNSSName, "QZSS_L1S") == 0) {
    gnssId = 5;
    gnssIdType = 'Q';
    strcpy(gnssSignalName, "L1S");
    gnssEnable = &gnssState.QZSS_L1S;
  } else if(strcmp(selectGNSSName, "GLONASS") == 0) {
    gnssId = 6;
    gnssIdType = 'R';
    strcpy(gnssSignalName, "");
    gnssEnable = &gnssState.GLONASS;
  } else if(strcmp(selectGNSSName, "GLONASS_L1") == 0) {
    gnssId = 6;
    gnssIdType = 'R';
    strcpy(gnssSignalName, "L1");
    gnssEnable = &gnssState.GLONASS_L1;
  } else {
    gnssIdType = '?';
    strcpy(gnssSignalName, "");
    gnssEnable = &gnssState.UNKNOWN;
  }
  if(gnssEnable <= 0) return false;
  bool setCfgRC = true;
  switch(mode) {
    case  GC_MODE_ENABLE:
      if(!*gnssEnable) {
        if(strlen(gnssSignalName) == 0) {
          setCfgRC = gps.setGNSSConfig(gnssId, true);
        } else {
          setCfgRC = gps.setGNSSSignalConfig(gnssId, gnssSignalName, true);
        }
        if(setCfgRC) *gnssEnable = true;
        sprintf(_msgStr, "Set CFG-GNSS rc=%d", setCfgRC);
        msg_update(_msgStr);
      }
      break;
    case  GC_MODE_DISABLE:
      if(*gnssEnable) {
        if(strlen(gnssSignalName) == 0) {
          setCfgRC = gps.setGNSSConfig(gnssId, false);
        } else {
          setCfgRC = gps.setGNSSSignalConfig(gnssId, gnssSignalName, false);
        }
        if(setCfgRC) *gnssEnable = false;
        sprintf(_msgStr, "Set CFG-GNSS rc=%d", setCfgRC);
        msg_update(_msgStr);
      }
      break;
    case  GC_MODE_TOGGLE:
      if(strlen(gnssSignalName) == 0) {
        setCfgRC = gps.setGNSSConfig(gnssId, !*gnssEnable);
      } else {
        setCfgRC = gps.setGNSSSignalConfig(gnssId, gnssSignalName, !*gnssEnable);
      }
      if(setCfgRC) *gnssEnable = !*gnssEnable;
      sprintf(_msgStr, "Set CFG-GNSS rc=%d", setCfgRC);
      msg_update(_msgStr);
      break;
  }
  return pollRC && setCfgRC;
}

/********************************************************************/
void gnssDisplayConfig() {
  uint8_t ubloxModuleType = gps.getUbloxModuleType();
  menuItemGNSSSatCfgIMESToggle.hide((ubloxModuleType == UBLOX_M8_MODULE) ? false : true);
  menuItemGNSSSatCfgBeiDouB1Toggle.hide((ubloxModuleType == UBLOX_M10_MODULE) ? false : true);
  menuItemGNSSSatCfgBeiDouB1CToggle.hide((ubloxModuleType == UBLOX_M10_MODULE) ? false : true);
  menuItemGNSSSatCfgQZSSL1CAToggle.hide((ubloxModuleType == UBLOX_M10_MODULE) ? false : true);
  menuItemGNSSSatCfgQZSSL1SToggle.hide((ubloxModuleType == UBLOX_M10_MODULE) ? false : true);
  // Update toggle menu item titles
  // Note: setTitle() only accepts const char*
  switch(gnssState.GPS) {
    case -1: menuItemGNSSSatCfgGPSToggle.setTitle("GPS    :Unknown"); break;
    case  0: menuItemGNSSSatCfgGPSToggle.setTitle("GPS    :Disabled"); break;
    case  1: menuItemGNSSSatCfgGPSToggle.setTitle("GPS    :Enabled"); break;
  }
  switch(gnssState.SBAS) {
    case -1: menuItemGNSSSatCfgSBASToggle.setTitle("SBAS   :Unknown"); break;
    case  0: menuItemGNSSSatCfgSBASToggle.setTitle("SBAS   :Disabled"); break;
    case  1: menuItemGNSSSatCfgSBASToggle.setTitle("SBAS   :Enabled"); break;
  }
  switch(gnssState.Galileo) {
    case -1: menuItemGNSSSatCfgGalileoToggle.setTitle("Galileo:Unknown"); break;
    case  0: menuItemGNSSSatCfgGalileoToggle.setTitle("Galileo:Disabled"); break;
    case  1: menuItemGNSSSatCfgGalileoToggle.setTitle("Galileo:Enabled"); break;
  }
  switch(gnssState.BeiDou) {
    case -1: menuItemGNSSSatCfgBeiDouToggle.setTitle("BeiDou :Unknown"); break;
    case  0: menuItemGNSSSatCfgBeiDouToggle.setTitle("BeiDou :Disabled"); break;
    case  1: menuItemGNSSSatCfgBeiDouToggle.setTitle("BeiDou :Enabled"); break;
  }
  switch(gnssState.BeiDou_B1) {
    case -1: menuItemGNSSSatCfgBeiDouB1Toggle.setTitle(" B_B1  :Unknown"); break;
    case  0: menuItemGNSSSatCfgBeiDouB1Toggle.setTitle(" B_B1  :Disabled"); break;
    case  1: menuItemGNSSSatCfgBeiDouB1Toggle.setTitle(" B_B1  :Enabled"); break;
  }
  switch(gnssState.BeiDou_B1C) {
    case -1: menuItemGNSSSatCfgBeiDouB1CToggle.setTitle(" B_B1C :Unknown"); break;
    case  0: menuItemGNSSSatCfgBeiDouB1CToggle.setTitle(" B_B1C :Disabled"); break;
    case  1: menuItemGNSSSatCfgBeiDouB1CToggle.setTitle(" B_B1C :Enabled"); break;
  }
  switch(gnssState.IMES) {
    case -1: menuItemGNSSSatCfgIMESToggle.setTitle("IMES   :Unknown"); break;
    case  0: menuItemGNSSSatCfgIMESToggle.setTitle("IMES   :Disabled"); break;
    case  1: menuItemGNSSSatCfgIMESToggle.setTitle("IMES   :Enabled"); break;
  }
  switch(gnssState.QZSS) {
    case -1: menuItemGNSSSatCfgQZSSToggle.setTitle("QZSS   :Unknown"); break;
    case  0: menuItemGNSSSatCfgQZSSToggle.setTitle("QZSS   :Disabled"); break;
    case  1: menuItemGNSSSatCfgQZSSToggle.setTitle("QZSS   :Enabled"); break;
  }
  switch(gnssState.QZSS_L1CA) {
    case -1: menuItemGNSSSatCfgQZSSL1CAToggle.setTitle(" Q_L1CA:Unknown"); break;
    case  0: menuItemGNSSSatCfgQZSSL1CAToggle.setTitle(" Q_L1CA:Disabled"); break;
    case  1: menuItemGNSSSatCfgQZSSL1CAToggle.setTitle(" Q_L1CA:Enabled"); break;
  }
  switch(gnssState.QZSS_L1S) {
    case -1: menuItemGNSSSatCfgQZSSL1SToggle.setTitle(" Q_L1S :Unknown"); break;
    case  0: menuItemGNSSSatCfgQZSSL1SToggle.setTitle(" Q_L1S :Disabled"); break;
    case  1: menuItemGNSSSatCfgQZSSL1SToggle.setTitle(" Q_L1S :Enabled"); break;
  }
  switch(gnssState.GLONASS) {
    case -1: menuItemGNSSSatCfgGLONASSToggle.setTitle("GLONASS:Unknown"); break;
    case  0: menuItemGNSSSatCfgGLONASSToggle.setTitle("GLONASS:Disabled"); break;
    case  1: menuItemGNSSSatCfgGLONASSToggle.setTitle("GLONASS:Enabled"); break;
  }
  displayRefresh = true;
}

/********************************************************************/
bool gnssReadConfig() {
  bool rcode = gnssConfigurator(GC_MODE_LOAD, "");
  gnssDisplayConfig();
  return rcode;
}

/********************************************************************/
bool gnssEnableConfig(const char* selectGNSSName) {
  bool rcode = gnssConfigurator(GC_MODE_ENABLE, selectGNSSName);
  gnssDisplayConfig();
  return rcode;
}

/********************************************************************/
bool gnssDisableConfig(const char* selectGNSSName) {
  bool rcode = gnssConfigurator(GC_MODE_DISABLE, selectGNSSName);
  gnssDisplayConfig();
  return rcode;
}

/********************************************************************/
bool gnssToggleConfig(const char* selectGNSSName) {
  bool rcode = gnssConfigurator(GC_MODE_TOGGLE, selectGNSSName);
  gnssDisplayConfig();
  return rcode;
}

