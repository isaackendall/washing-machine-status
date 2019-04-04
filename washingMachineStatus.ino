


/******************************************************************************
 Program steps

 Import liberys
 Define pins
 Connect to wifi
 read current
 if current above x then post to thingspeak
 post to thing speak

******************************************************************************/

#include <ThingSpeak.h>
#include <ESP8266WiFi.h> 


// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(115200);
  Serial.println("test console print");
  delay(1000);
}

// the loop routine runs over and over again forever:
void loop() {
  Serial.print("ADC Value: ");Serial.println(analogRead(A0));
  delay(1000);
}
