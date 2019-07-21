#include <Arduino.h>



/*

ADC NOTES: OVERVIEW - ONLY USE 32-39 if you want Wifi. (37 & 38 are also used, leaving 7 pins, Ref, AC, 5 x CTs)
The ADC driver API supports ADC1 (8 channels, attached to GPIOs 32 - 39), and ADC2 (10 channels, attached to GPIOs 0, 2, 4, 12 - 15 and 25 - 27). However, the usage of ADC2 has some restrictions for the application:
ADC2 is used by the Wi-Fi driver. Therefore the application can only use ADC2 when the Wi-Fi driver has not started.
Some of the ADC2 pins are used as strapping pins (GPIO 0, 2, 15) thus cannot be used freely. Such is the case in the following official Development Kits:

Top left and anti clockwise CT's
GPIO
 0 - DON'T USE ,25,33,32,35,36,39,34

27  2.5v Reference
26  AC Voltage

17  Button
23  Reed switch
5   WS2812

*NOTES*
A LM358 Comparator supplies a buffered stable 1.64v BIAS for the AC and all the CT's

AC Voltage Sensing
A Voltage divider of 12k and 1k protect the pin from overvoltage
example:
A 9V (RMS) power adapter, the positive voltage peak be 12.7V, the negative peak -12.7V
CALC:
peak_voltage_output = R1 / (R1 + R2) x peak_voltage_input =
1k / (1k + 12k) x 12.7v = 0.977v
Applying the offset:
1.64v + 0.977v = 2.62v for the positive peak
1.64v - 0.977v = 0.66v for the negative peak
*/

// #define ICAL 0.0245  //111.1
#define VCAL 0.218
#define PHASECAL 1.9
const byte CT_PINS[7] = {34,39,36,35,32,33,25};
/*ICAL is calibration for each type of CT clamp
SCT013 30A/1v = 0.0245  <= Remove Burden resistor as it's a voltage output.
SCT013 100A/50mA =
*/             //           TR                   BR     BL                   TL-Dont use
const float CT_ICAL[7] = {0.0245,0.0245,0.0245, 0.0679,0.0245,0.0245,0.0245};
#define REF_2V5_PIN 27
#define AC_VOLTAGE_PIN 26
// unsigned long summedVoltageSquared[7] = {0};

double ReadVoltage(double reading){ //byte pin){
  // double reading = analogRead(pin); // Reference voltage is 3v3 so maximum reading is 3v3 = 4095 in range 0 to 4095
  if(reading < 1 || reading > 4095) return 0;
  // return -0.000000000009824 * pow(reading,3) + 0.000000016557283 * pow(reading,2) + 0.000854596860691 * reading + 0.065440348345433;
  return -0.000000000000016 * pow(reading,4) + 0.000000000118171 * pow(reading,3)- 0.000000301211691 * pow(reading,2)+ 0.001109019271794 * reading + 0.034143524634089;
} // Added an improved polynomial, use either, comment out as required

// void adjustedAnalogRead(){
//   double reference = ReadVoltage(REF_2V5_PIN);
//   Serial.print(reference);
//   Serial.print("\t");
//   Serial.print(reference * (3.3/4096));
//   Serial.print("\t adjusted:");
//   Serial.print(reference*(2.5/reference));
//   Serial.print("\t VCC:");
//   Serial.println(4096*(2.5/reference));
// }

void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);
    // Serial.println("Power Monitor");
}

// int SAMPLES = 1;
// #define ADC_BITS    12
// #define ADC_COUNTS (1<<ADC_BITS)
#define AC_BIAS 1855 //on ADC 1 GPIO32+  //1877 on ADC1 GPIO25 (which can't be used with wifi)   //1.63v

void loop() {
    // Serial.println(ReadVoltage(REF_2V5_PIN));

    unsigned int crossCount = 0;                             //Used to measure number of times threshold is crossed.
    unsigned int numberOfSamples = 0;                        //This is now incremented

    //-------------------------------------------------------------------------------------------------------------------------
    // 1) Waits for the waveform to cross 'zero' (mid-scale adc) part in sin curve.
    //-------------------------------------------------------------------------------------------------------------------------

    unsigned long startTime = millis();    //millis()-start makes sure it doesnt get stuck in the loop if there is an error.
    boolean zeroCross=false;                                  //an indicator to exit the while loop
    unsigned int lastACRead = analogRead(AC_VOLTAGE_PIN);
    while (zeroCross==false) {                                   //the while loop...
      unsigned int ACRead = analogRead(AC_VOLTAGE_PIN);                    //using the voltage waveform
      if (ACRead >= AC_BIAS && lastACRead < AC_BIAS) {           //   <= 163 = voltage BIAS that is added
        zeroCross = true;
      }
      if ((millis()-startTime)>2000) {
        zeroCross = true;
      }
      lastACRead = ACRead;
    }

    unsigned long summedVoltageSquared = 0;
    unsigned long summedCurrentSquared[7] = {0};
//*******************
    double lastFilteredV;
    double filteredV;          //Filtered_ is the raw analog value minus the DC offset
    double filteredI[7];
    double offsetV = AC_BIAS;                          //Low-pass filter output
    double offsetI[7] = {AC_BIAS,AC_BIAS,AC_BIAS,AC_BIAS,AC_BIAS,AC_BIAS,AC_BIAS};                          //Low-pass filter output
    double sumP[7]; //instP[7],
    double phaseShiftedV; //Holds the calibrated phase shifted voltage.
//*******************

    startTime = millis();
    lastACRead = analogRead(AC_VOLTAGE_PIN);
    while (crossCount < 5) {                                   //the while loop...

      lastFilteredV = filteredV; //Used for delay/phase compensation  ***********************

      //-----------------------------------------------------------------------------
      // A) Read in raw voltage samples
      //-----------------------------------------------------------------------------
      unsigned int ACRead = analogRead(AC_VOLTAGE_PIN);                    //using the voltage waveform
      // int noBiasVoltage = ACRead - AC_BIAS;
      // summedVoltageSquared += noBiasVoltage * noBiasVoltage;

      //-----------------------------------------------------------------------------
      // B) Apply digital low pass filters to extract the 1.64 V dc offset,
      //     then subtract this - signal is now centred on 0 counts.
      //-----------------------------------------------------------------------------
      offsetV = offsetV + ((ACRead-offsetV)/4095);  // ***********************
      filteredV = ACRead - offsetV;                // ***********************

      //-----------------------------------------------------------------------------
      // C) Root-mean-square method voltage
      //-----------------------------------------------------------------------------
      summedVoltageSquared += filteredV * filteredV;

      //-----------------------------------------------------------------------------
      // E) Phase calibration
      //-----------------------------------------------------------------------------
      phaseShiftedV = lastFilteredV + PHASECAL * (filteredV - lastFilteredV);


      for (byte i = 0; i < 6; i++) {
        //-----------------------------------------------------------------------------
        // A) Read in raw current samples
        //-----------------------------------------------------------------------------
        unsigned int CTRead = analogRead(CT_PINS[i]);                    //using the voltage waveform
        // int noBiasCurrent = CTRead - AC_BIAS;
        // summedCurrentSquared[i] += noBiasCurrent * noBiasCurrent;

        //-----------------------------------------------------------------------------
        // B) Apply digital low pass filters to extract the 2.5 V or 1.65 V dc offset,
        //     then subtract this - signal is now centred on 0 counts.
        //-----------------------------------------------------------------------------

        offsetI[i] = offsetI[i] + ((CTRead-offsetI[i])/4096);  // ***********************
        filteredI[i] = CTRead - offsetI[i];                  // ***********************

        //-----------------------------------------------------------------------------
        // D) Root-mean-square method current
        //-----------------------------------------------------------------------------
        summedCurrentSquared[i] += filteredI[i] * filteredI[i];

        //-----------------------------------------------------------------------------
        // F) Instantaneous power calc
        //-----------------------------------------------------------------------------
        // instP[i] = phaseShiftedV * filteredI[i];          //Instantaneous Power
        sumP[i] += phaseShiftedV * filteredI[i]; //instP[i]; //Sum
      }




      //-----------------------------------------------------------------------------
      // G) Find the number of times the voltage has crossed Zero voltage (1 Wavelength)
      //-----------------------------------------------------------------------------

      //Count Positive ZeroCrossing's
      if (ACRead >= AC_BIAS && lastACRead < AC_BIAS) {           //   <= 163 = voltage BIAS that is added
        crossCount++;
      }
      if ((millis()-startTime)>7000) {
        crossCount = 99999;
      }
      lastACRead = ACRead;
      numberOfSamples++;
    }

    double Vrms = sqrt(summedVoltageSquared/numberOfSamples)*VCAL;
    Serial.print("Vrms: ");
    Serial.print(Vrms);
    Serial.print("\t Samples: ");
    Serial.print(numberOfSamples);
    Serial.print("\t WaveCount: ");
    Serial.print(crossCount);
    Serial.print("\t samples/cross: ");
    Serial.print(numberOfSamples/crossCount);
    unsigned int timeTaken = millis()-startTime;
    Serial.print("\t Freq: ");
    Serial.print(1000/((timeTaken*1.0)/(crossCount*1.0)));
    Serial.print("\t Time(mS): ");
    Serial.println(timeTaken);

    for (byte i = 0; i < 6; i++) {
      double Irms = sqrt(summedCurrentSquared[i]/numberOfSamples)*CT_ICAL[i];
      if (Irms > 0.3) {
        Serial.print(" Irms[");
        Serial.print(i);
        Serial.print("]:");
        Serial.print(Irms);

        //Calculation power values
        double realPower = VCAL * CT_ICAL[i] * sumP[i] / numberOfSamples;
        double apparentPower = Vrms * Irms;
        double powerFactor = realPower / apparentPower;

        Serial.print("\t realPower:");
        Serial.print(realPower);
        Serial.print("\t\t apparentPower:");
        Serial.print(apparentPower);
        Serial.print("\t powerFactor:");
        Serial.println(powerFactor);
      }
      summedCurrentSquared[i] = 0;
      sumP[i] = 0;
    }

    summedVoltageSquared = 0;


    delay(1000);

}
