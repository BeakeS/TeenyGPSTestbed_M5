
/********************************************************************/
// Compass/Magnetometer
bool compassEnabled;
#include <DFRobot_QMC5883.h>
DFRobot_QMC5883 compass(&Wire1, QMC5883_ADDRESS);

/********************************************************************/
bool compass_setup() {
  if(compass.begin() && compass.isQMC()) {
    compassEnabled = true;
    // Set declination angle on your location and fix heading
    // You can find your declination on: http://magnetic-declination.com/
    // (+) Positive or (-) for negative
    // For Bytom / Poland declination angle is 4'26E (positive)
    // For Yorktown Heights/ New York / USA Magnetic Declination: -12° 47' NEGATIVE (WEST)
    // Formula: (deg + (min / 60.0)) / (180 / PI);
    float declinationAngle = (-12.0 + (47.0 / 60.0)) / (180 / PI);
    compass.setDeclinationAngle(declinationAngle);
  delay(100);
  } else {
    compassEnabled = false;
  }
  return compassEnabled;
}

