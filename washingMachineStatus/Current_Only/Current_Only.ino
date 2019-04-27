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

 // int sensor;                 //Define variable to read ADC data
  //sensor = analogRead(A0);    //Read ADC
 // Serial.print("ADC Value:"); //Print Message
 // Serial.println(sensor);     //Print ADC value
 // delay(500);                 //Have some delay to make it visible 
  //float voltage = sensor * (1.0 /1023);
  //Serial.println(voltage);
  double Irms = emon1.calcIrms(1480);  // Calculate Irms only
  Serial.print((Irms*110.0)/100);           // Apparent power
  Serial.print(" ");
  Serial.println(Irms);             // Irms
  delay(500);                       // delay half a second
}
