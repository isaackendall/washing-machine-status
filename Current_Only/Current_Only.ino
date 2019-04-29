// This works with the ESP32, but the calibration is wrong. It should be 
// enough though to use it as checking if something is on or off. 
// link to the circuit is here. https://learn.openenergymonitor.org/electricity-monitoring/ct-sensors/how-to-build-an-arduino-energy-monitor-measuring-current-only
#include "EmonLib.h"                                    // Include Emon Library
#include "ThingSpeak.h"                                 // Include ThingSpeak Library
// #include "ESP8266WiFi.h">                                // Include ESP8266wifi Library
#include <WiFi.h>

EnergyMonitor emon1;
// Create an instance

int val = 0;
//const int LED_PIN = 5;                                  // LED pin - active-high

char ssid[] = "thecottage";                             // Enter your WiFi SSID (router name)
char pass[] = "BadRomance324";                          // Enter your WiFi router password
unsigned long myChannelNumber = 745770;                 // Enter your Thingspeak Channel Number
const char * myWriteAPIKey = "KGVF3I4PG1A37PFH";        // Enter your ThingSpeak API Key

unsigned long currentMillis = millis();
unsigned long previousMillis = 0;
unsigned long resetMillis = 0;
const long interval = 90000;                            // Time delay interval (90 seconds) to prevent false triggers when setting up device 
                                                        
int status = WL_IDLE_STATUS;
WiFiClient  client;



void setup()
{
  
 
  Serial.begin(115200);
  
  emon1.current(A2, 111.1);             // Current: input pin, calibration.
  
  WiFi.begin(ssid, pass);                               // Start WiFi connection
  ThingSpeak.begin(client);                             // Start ThingSpeak connection
 // pinMode(LED_PIN, OUTPUT);                              // Set the LED pin as an Output

  Serial.println();
  Serial.print("Connecting to ");
  Serial.print(ssid); 
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected");  
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP()); 

 // Serial.print("value of val is ");
 // Serial.println(val); 
  
  
 ThingSpeak.writeField(myChannelNumber, 1, val, myWriteAPIKey);           // Update ThingSpeak channel with the value of val


}

void loop()
{
val = 0;                                //set val to = 0
for (int i = 0; i < 21; i++) {
  
  double Irms = emon1.calcIrms(1480);   // Calculate Irms only
  Serial.print(Irms*110);               // Apparent power with 110V supply assumed
  Serial.print(" ");
  Serial.println(Irms);                 // Irms
  delay(500);                           // delay half a second
  val = val + (Irms*110);               // add the new value of Irms to val
  Serial.print("IRMS " );
  Serial.println(val);
  delay(500);
  }
  val = val/21;
Serial.print(val);
Serial.println(" final Irms!!!!!");
delay(2000);


ThingSpeak.writeField(myChannelNumber, 1, val, myWriteAPIKey);      // Update ThingSpeak channel the PIR sensor value of val/21  
Serial.print(val);
Serial.println(" Sent to ThingSpeak!");  
delay(500);


}
