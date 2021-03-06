


/******************************************************************************
 Program steps

 Import liberys
 Define pins
 Connect to wifi
 read current
 if current above x then post to thingspeak
 post to thing speak
 There will need to also be a text messenge sent.
 Test change
******************************************************************************/

#include <ThingSpeak.h>
#include <ESP8266WiFi.h> 
#include "EmonLib.h"                   // Include Emon Library
EnergyMonitor emon1;                   // Create an instance

const int CURRENT = A0;                                 // Pin connected to voltage divider
const int LED_PIN = 5;                                  // LED pin - active-high
int val = 0;                                            // Variable for reading the pin status

char ssid[] = "thecottage";                             // Enter your WiFi SSID (router name)
char pass[] = "BadRomance324";                          // Enter your WiFi router password
unsigned long myChannelNumber = 745770;                 // Enter your Thingspeak Channel Number
const char * myWriteAPIKey = "KGVF3I4PG1A37PFH";        // Enter your ThingSpeak API Key

WiFiClient  client;

// the setup routine runs once when you press reset:


void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(115200);
  Serial.println("Setup Starting!!");
  delay(1000);

  emon1.current(1, 111.1);                               // Current: input pin, calibration.

  double Irms = emon1.calcIrms(1480);                    // Calculate Irms only
  
  


  WiFi.begin(ssid, pass);                                 // Start WiFi connection
  ThingSpeak.begin(client);                               // Start ThingSpeak connection
  Serial.begin(115200);                                   // Serial Baud Rate
  pinMode(LED_PIN, OUTPUT);                               // Set the LED pin as an Output
  //pinMode(inputPin, INPUT);                             // Set the Motion Sensor pin as an Input

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
}

// the loop routine runs over and over again forever:
void loop() {

  Serial.print(Irms*230.0);	                             // Apparent power
  Serial.print(" ");
  Serial.println(Irms);		                                // Irms

  Serial.print("ADC Value: ");Serial.println(analogRead(CURRENT));
  delay(1000);
    val = CURRENT;                                                      // assigns val the current value
    Serial.print("val value = ");                                       // prints val value (CURRENT)
    Serial.println(val); 
    ThingSpeak.writeField(myChannelNumber, 1, val, myWriteAPIKey);      // Update ThingSpeak channel with current value  
    Serial.println("Val Data Sent to ThingSpeak!");    
    delay(21000);                                                       // wait 21 seconds as it thingspeak will only accept data every 20 seconds
}
