#include "EmonLib.h"
// Include Emon Library
EnergyMonitor emon1;
// Create an instance

int val = 0;
int sensorPin = A0;
int sensorValue = 0;

void setup()
{
  Serial.begin(9600);

  //emon1.current(A0, 111.1);             // Current: input pin, calibration.
}

void loop()
{

  // read the value from the sensor:
  sensorValue = analogRead(sensorPin);
  Serial.println(sensorValue);
  delay(1000);
  /*
double Irms = emon1.calcIrms(1480);  // Calculate Irms only
Serial.print(Irms*110.0);           // Apparent power
  Serial.print(" ");
  Serial.println(Irms);             // Irms
  delay(1000);
  val = (Irms*110);
  Serial.println(val);
  */
}
