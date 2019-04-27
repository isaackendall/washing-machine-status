// This works with the ESP32, but the calibration is wrong. It should be 
// enough though to use it as checking if something is on or off. 
// link to the circuit is here. https://learn.openenergymonitor.org/electricity-monitoring/ct-sensors/how-to-build-an-arduino-energy-monitor-measuring-current-only
#include "EmonLib.h"
// Include Emon Library
EnergyMonitor emon1;
// Create an instance
void setup()
{
  Serial.begin(115200);
  emon1.current(A0, 111.1);             // Current: input pin, calibration.
}

void loop()
{

  double Irms = emon1.calcIrms(1480);  // Calculate Irms only
  Serial.print(Irms*110.0);           // Apparent power with 110V supply assumed
  Serial.print(" ");
  Serial.println(Irms);             // Irms
  delay(500);                       // delay half a second
}
