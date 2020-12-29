//Michael Klements
//The DIY Life
//27 October 2014

const int currentPin = A0;              //Assign CT input to pin 1
double kilos = 0;
double peakPower = 0;
int sensorValue = 0;
void setup() 
{ 
  Serial.begin(115200);            //Start serial communication
  Serial.println("Running");
}

void loop() 
{ 
  sensorValue = analogRead(currentPin);
  Serial.print("sensor = ");
  Serial.println(sensorValue);
  delay (500);
  int current = 0;
  int maxCurrent = 0;
  int minCurrent = 1000;
  for (int i=0 ; i<=200 ; i++)  //Monitors and logs the current input for 200 cycles to determine max and min current
  {
    current = analogRead(currentPin);    //Reads current input and records maximum and minimum current
    if(current >= maxCurrent)
      maxCurrent = current;
    else if(current <= minCurrent)
      minCurrent = current;
  }
  Serial.print(maxCurrent);
  Serial.println("A");
  if (maxCurrent <= 470
  )
  {
    maxCurrent = 469;
  }

  double RMSCurrent = ((maxCurrent - 469)*0.707)/44.98585;    //Calculates RMS current based on maximum value  12.27
  double RMSPower = 230*RMSCurrent;    //Calculates RMS Power Assuming Voltage 230VAC in Belgium, change to 110VAC accordingly
  if (RMSPower > peakPower)
  {
    peakPower = RMSPower;
  }
  kilos = kilos + (RMSPower * (2.05/60/60/1000));    //Calculate kilowatt hours used
  delay (2000);
  Serial.print(RMSCurrent,4);
  Serial.println("A");
  Serial.print(RMSPower,4);
  Serial.println("W");
  Serial.print(kilos);
  Serial.println("kWh");
  Serial.print(peakPower);
  Serial.println("W");
}
