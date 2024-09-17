
/********************************************************************/
// Battery

#ifndef BatteryStatus_h
#define BatteryStatus_h

/********************************************************************/
class BatteryStatus {

  public:

    // Constructor / destructor / disallow copy and move
    BatteryStatus() {}
    virtual ~BatteryStatus() {}
    BatteryStatus(const BatteryStatus&);
    BatteryStatus& operator=(const BatteryStatus&);

    void readStatus() {
      uint32_t _nowMS = millis();
      if((_nowMS - lastSampleTime) < samplePeriod) return;
      lastSampleTime = _nowMS;
    //voltage = M5.Axp.GetBatVoltage();
    //charging = M5.Axp.isCharging();
      voltage = M5.Power.getBatteryVoltage();
      percentage = static_cast<uint8_t>(M5.Power.getBatteryLevel());
      charging = M5.Power.isCharging();
    }

    int16_t getVoltage() {
      readStatus();
      return voltage;
    }

    uint8_t getPercentage() {
      readStatus();
      return percentage;
    }

    bool isCharging() {
      readStatus();
      return charging;
    }

  private:

    uint32_t samplePeriod = 1000;   // sample once a second
    uint32_t lastSampleTime = millis() - 1000;
    int16_t  voltage;
    uint8_t  percentage;
    bool     charging;

};

#endif //BatteryStatus_h

/********************************************************************/
// Battery instance
BatteryStatus battery;

