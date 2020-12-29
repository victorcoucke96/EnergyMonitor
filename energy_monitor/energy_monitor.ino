//Michael Klements
//The DIY Life
//27 October 2014
#include <AllThingsTalk_WiFi.h>
#include "EmonLib.h"

#define ADC_INPUT A0

auto wifiCreds = WifiCredentials("Mobistar-C9901", "7MrJkczw");                                                // Your WiFi Network Name and Password
auto deviceCreds = DeviceConfig("vZKvbLklpyKzl4rLDiCfMDDF", "maker:4Mjs1naycgnaG0lqFuHNSY76EVxCGyIqqmFwzYY0"); // Go to AllThingsTalk Maker > Devices > Your Device > Settings > Authentication to get your Device ID and Token
auto device = Device(wifiCreds, deviceCreds);                                                                  // Create "device" object
char *sensorAsset = "analog-example";                                                                          // Name of asset on AllThingsTalk to which you'll receive the value (automatically created below)

EnergyMonitor emon1;
double kilos = 0;
double peakPower = 0;
unsigned long startMillis;
unsigned long endMillis;
const long period = 1000; // Change this to change how many milliseconds you want between analog port readings

void setup()
{
  Serial.begin(115200); //Start serial communication
  Serial.println("Running");

  device.debugPort(Serial);                                             // Enable debug output from AllThingsTalk SDK.
  device.wifiSignalReporting(true);                                     // Enable AllThingsTalk WiFi SDK's feature that sends NodeMCU's WiFi Signal Strength to your AllThingsTalk Maker
  device.createAsset(sensorAsset, "Analog Value", "sensor", "integer"); // Create asset on AllThingsTalk to send analog value to
  device.init();                                                        // Initialize WiFi and AllThingsTalk

  pinMode(ADC_INPUT, INPUT);
  emon1.current(ADC_INPUT, 18.5); //23.3639904847
  startMillis = millis();
}

void loop()
{
  device.loop(); // Keep AllThingsTalk and WiFi connection alive

  endMillis = millis();
  unsigned long time = endMillis - startMillis;
  if (time >= period)
  {
    double RMSCurrent = emon1.calcIrms(1480);
    if (RMSCurrent <= 0.0025)
      RMSCurrent = 0;
    int RMSPower = 230 * RMSCurrent; //Calculates RMS Power Assuming Voltage 230VAC in Belgium, change to 110VAC accordingly
    if (RMSPower > peakPower)
    {
      peakPower = RMSPower;
    }
    endMillis = millis();
    time = endMillis - startMillis;
    kilos = kilos + ((double)RMSPower * ((double)time / 60 / 60 / 1000000));
    startMillis = millis();
    Serial.print(RMSCurrent, 4);
    Serial.println("A");
    Serial.print(RMSPower);
    device.send(sensorAsset, RMSPower); // Sends the data to AllThingsTalk. Data is sent to "sensorAsset"
    Serial.println("W");
    Serial.print(kilos);
    Serial.println("kWh");
    Serial.print(peakPower);
    Serial.println("W");

  }

}
