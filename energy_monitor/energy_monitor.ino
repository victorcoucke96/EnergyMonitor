//Michael Klements
//The DIY Life
//27 October 2014
#include "EmonLib.h"

#define ADC_INPUT A0

EnergyMonitor emon1;
double kilos = 0;
double peakPower = 0;
unsigned long startMillis;
unsigned long endMillis;

void setup()
{
  Serial.begin(115200); //Start serial communication
  Serial.println("Running");
  pinMode(ADC_INPUT, INPUT);
  emon1.current(ADC_INPUT, 18.5); //23.3639904847
  startMillis = millis();
}

void loop()
{
  // sensorValue = analogRead(currentPin);

  // Serial.print("sensor = ");
  // Serial.println(sensorValue);
  // delay (500);
  // int current = 0;
  // int maxCurrent = 0;
  // int minCurrent = 1000;
  // for (int i=0 ; i<=200 ; i++)  //Monitors and logs the current input for 200 cycles to determine max and min current
  // {
  //   current = analogRead(currentPin);    //Reads current input and records maximum and minimum current
  //   if(current >= maxCurrent)
  //     maxCurrent = current;
  //   else if(current <= minCurrent)
  //     minCurrent = current;
  // }
  // Serial.print(maxCurrent);
  // Serial.println("A");
  // if (maxCurrent <= 470
  // )
  // {
  //   maxCurrent = 469;
  // }

  double RMSCurrent = emon1.calcIrms(1480);
  if (RMSCurrent <= 0.0025) RMSCurrent = 0;
  double RMSPower = 230 * RMSCurrent;                          //Calculates RMS Power Assuming Voltage 230VAC in Belgium, change to 110VAC accordingly
  if (RMSPower > peakPower)
  {
    peakPower = RMSPower;
  }
  endMillis = millis();
  unsigned long time = endMillis - startMillis;
  kilos = kilos + ((double)RMSPower * ((double)time/60/60/1000000));
  startMillis = millis();
  delay(2000);
  Serial.print(RMSCurrent, 4);
  Serial.println("A");
  Serial.print(RMSPower, 4);
  Serial.println("W");
  Serial.print(kilos);
  Serial.println("kWh");
  Serial.print(peakPower);
  Serial.println("W");
}
