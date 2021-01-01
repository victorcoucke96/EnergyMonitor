// //Michael Klements
// //The DIY Life
// //27 October 2014

// #include "EmonLib.h"

// #define ADC_INPUT A0
// #define ADC_BITS    10
// #define ADC_COUNTS  (1<<ADC_BITS)

// auto wifiCreds = WifiCredentials("Mobistar-C9901", "7MrJkczw"); // Your WiFi Network Name and Password

// EnergyMonitor emon1;

// double kilos = 0;
// double peakPower = 0;
// unsigned long startMillis;
// unsigned long endMillis;
// const long period = 1000; // Change this to change how many milliseconds you want between analog port readings

// void setup()
// {
//   Serial.begin(115200); //Start serial communication
//   Serial.println("Running");

//   pinMode(ADC_INPUT, INPUT);
//   emon1.current(ADC_INPUT, 18.5); //23.3639904847
//   startMillis = millis();
// }

// void loop()
// {
//   device.loop(); // Keep AllThingsTalk and WiFi connection alive

//   endMillis = millis();
//   unsigned long time = endMillis - startMillis;
//   if (time >= period)
//   {
//     double RMSCurrent = emon1.calcIrms(1480);
//     if (RMSCurrent <= 0.0025)
//       RMSCurrent = 0;
//     int RMSPower = 230 * RMSCurrent; //Calculates RMS Power Assuming Voltage 230VAC in Belgium, change to 110VAC accordingly
//     if (RMSPower > peakPower)
//     {
//       peakPower = RMSPower;
//     }
//     endMillis = millis();
//     time = endMillis - startMillis;
//     kilos = kilos + ((double)RMSPower * ((double)time / 60 / 60 / 1000000));
//     startMillis = millis();
//     Serial.print(RMSCurrent, 4);
//     Serial.println("A");
//     Serial.print(RMSPower);
//     Serial.println("W");
//     device.send(sensorAsset, RMSPower); // Sends the data to AllThingsTalk. Data is sent to "sensorAsset"
//     Serial.print(kilos,3);
//     Serial.println("kWh");
//     device.send(sensorAsset1, int(kilos*1000)); // Sends the data to AllThingsTalk. Data is sent to "sensorAsset"
//     Serial.print(peakPower);
//     Serial.println("W");

//   }

// }


/* ESP8266 AWS IoT
 *  
 * Simplest possible example (that I could come up with) of using an ESP8266 with AWS IoT.
 * No messing with openssl or spiffs just regular pubsub and certificates in string constants
 * 
 * This is working as at 3rd Aug 2019 with the current ESP8266 Arduino core release:
 * SDK:2.2.1(cfd48f3)/Core:2.5.2-56-g403001e3=20502056/lwIP:STABLE-2_1_2_RELEASE/glue:1.1-7-g82abda3/BearSSL:6b9587f
 * 
 * Author: Anthony Elder 
 * License: Apache License v2
 */
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
extern "C" {
#include "libb64/cdecode.h"
}

const char* ssid = "Mobistar-C9901";
const char* password = "7MrJkczw";

// Find this awsEndpoint in the AWS Console: Manage - Things, choose your thing
// choose Interact, its the HTTPS Rest endpoint 
const char* awsEndpoint = "a292o5in54g9l-ats.iot.eu-west-2.amazonaws.com";
// For the two certificate strings below paste in the text of your AWS 
// device certificate and private key, comment out the BEGIN and END 
// lines, add a quote character at the start of each line and a quote 
// and backslash at the end of each line:

// xxxxxxxxxx-certificate.pem.crt
const String certificatePemCrt = \
// -----BEGIN CERTIFICATE-----
R"EOF(MIIDWjCCAkKgAwIBAgIVAP45jFYfZ5aHOgaY21Jf/mr8aItxMA0GCSqGSIb3DQEB
CwUAME0xSzBJBgNVBAsMQkFtYXpvbiBXZWIgU2VydmljZXMgTz1BbWF6b24uY29t
IEluYy4gTD1TZWF0dGxlIFNUPVdhc2hpbmd0b24gQz1VUzAeFw0yMDEyMzExNDM0
MDZaFw00OTEyMzEyMzU5NTlaMB4xHDAaBgNVBAMME0FXUyBJb1QgQ2VydGlmaWNh
dGUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQC71wlfLZIsWsPti7m9
uxqpt3nkP1MXIh6rDx7HQZnmhMw3nQ1traPn88roNzqEL1V/niLSZvBXt4HNsCYm
dlXTazixeZtI8wKIzyZSNlkuaF1CGMkdKmtqDGZHsyix5AvHGHBwSwnQ3vRjIBjv
AIua0L0TlYrrCbgW7lIcnAx8QRei201/Q9ijiSLc2Vk24F65DizdcUxN+1AfDB/V
DKeXpVLwKqLW46NY8m3/G9x0DKsrBGAsuUwZfc/Ss8YF10bjkxkRkQvO+h4WfvJn
wjrQcwfitOq7jIPxLT0ch1SGrL8ikbJlQlS+pQcQtc4OQMFOAMQIbjh6v+XF/9vg
ix3HAgMBAAGjYDBeMB8GA1UdIwQYMBaAFEOH5Ml55tHokzZmHfbKAh4y92o6MB0G
A1UdDgQWBBSEdTRuRNaVBpaJBBm0HdjRwVbeqTAMBgNVHRMBAf8EAjAAMA4GA1Ud
DwEB/wQEAwIHgDANBgkqhkiG9w0BAQsFAAOCAQEAPQhpvGT6N52oobgKf0+YwEU7
C+AaU01DeVZ3bIfRY/990YsVxoAQPFqSL0vdpdh8vKmABExYzeSZXFygUIdCmJZg
ZUgCjwW7QjzjG2Fs7pys76KzP8/Iroi1TcZYML0tfooeJ6JVmt+BwjPUVLJj6ULb
ztc8JDcfDPibPLVJgxhyPB6ND9gZ8VlybZDWu1Xey9Ya37e6J1ORpNbN0qqjGttO
Bh0mNVTNuAub6LnNOTcAQnCT4b90D5CFhFI/cBF+fooTCLI5xXMdrLjgoHAWdZqc
3R1x+LAGfizxWz1WjI5Od3LvJoWPQ8j/ob8tXJZTwHy8h5LJDFSwcjQ1Go1s+w==)EOF";
// -----END CERTIFICATE----- 

// xxxxxxxxxx-private.pem.key
const String privatePemKey = \
//-----BEGIN RSA PRIVATE KEY-----
R"EOF(MIIEpAIBAAKCAQEAu9cJXy2SLFrD7Yu5vbsaqbd55D9TFyIeqw8ex0GZ5oTMN50N
ba2j5/PK6Dc6hC9Vf54i0mbwV7eBzbAmJnZV02s4sXmbSPMCiM8mUjZZLmhdQhjJ
HSpragxmR7MoseQLxxhwcEsJ0N70YyAY7wCLmtC9E5WK6wm4Fu5SHJwMfEEXottN
f0PYo4ki3NlZNuBeuQ4s3XFMTftQHwwf1Qynl6VS8Cqi1uOjWPJt/xvcdAyrKwRg
LLlMGX3P0rPGBddG45MZEZELzvoeFn7yZ8I60HMH4rTqu4yD8S09HIdUhqy/IpGy
ZUJUvqUHELXODkDBTgDECG44er/lxf/b4IsdxwIDAQABAoIBADGi40EZY7UAzyFo
eM6gMK2EZFtiOGy2gXAmcLWqqiZ2FRjlEXVYGyXiEE5IudfQQkGU4hLDTOOBBHX8
BDjJsGhLvxF21wfx30uToca7wdk3jt02e36PhQ7i8+widbB8WHhmBuBFk/Rz1hST
mMMfW8SvQCd6rtjpGpJEq8IPomT6wgHhSkh4km2sP87mvTIIIO9hvhca/yKice7/
+Ji1r6uYq32VT3a9v6+LtILVHZKrlK134O6TAaPH5IPiwTr3375MEAG08ydO4lNT
hIbTy5OJf1N5uE4NRwE2lk+A4Ik1nrD8Qz0aM4LfzLQcdYPMBLum0MdWrSFsParH
n5KC6kECgYEA8OtF/kI3UxZyjn8CpX0NiGA1swjQiwIcAl0M7MBtJaSvNyUPx2uL
Rr5qwBJ4bZmvD5w9qooM50LbZbkltI3fugSknQjkR7QcE2q2ejZ5L7GbZU/M5XhN
hsX5yO+xjvF0mLAauihvoCyP8GygWLhA+i1T8VDIpRq8Aze3sued49cCgYEAx5kr
8FbvU/9JSmD+dwxCfRG0AD+Tn/P0DLlOaLrzDTWBm6CFIFz9B8N8ahheuAdQ/kIq
g4RIKqxVmei5mbTakQHhPC1/jIyaE/sT5Bkn5UIRRfaNnX6Hfs1mBC7bgbsW3KUY
RVn7aCFVR4rJ9LJOh7dm9KAuN2XoZ0GaDH/pV5ECgYAddridqt13ROSHttLNCGky
R4nptmdCyORCCb096x6uHuDq3pZ6bhWfdkQ570hZJxV41EPaan2/fra0TxtkZIF7
D3xOVDsO5eEVItTkR2s0wQTfNA479iWNGQM+ZsoRUEhRMCBkqfNU5H5KCg7mKsqP
jiGY/qc7QfTCc/r8X0U5CwKBgQDB1tFDIUpfysbEgxndR3yytaSXsSaO6n4SdUve
jeBI7bOnDeb7nRLmnUSQTIaANirEIkHjY24qcx7Wz5qf2XGB7c9tf8olFs7zpJyW
oo+KJ/VaeqzhTOYNkTAH0/G9NZJoSbUedNqhL+4AAptLFncJemV0ILOBfWiMDruq
c/aK4QKBgQDfM9YRp2Ojj1PcCDYGGGB+KEYqTKs8EVnr1sLvfm2BSPoCZFeIPsPB
8AoTZrdVVmsMShu8TleYCsBrR+76xuRTrskkDdyJDWrAHrhdEXFfxei4EaIqKrN+
B9hHVHkowKmcAYUTM5x667PWBWLLR9lAhmLFD1pfezEEM/ASaIrG2g==)EOF";
//-----END RSA PRIVATE KEY-----

// This is the AWS IoT CA Certificate from: 
// https://docs.aws.amazon.com/iot/latest/developerguide/managing-device-certs.html#server-authentication
// This one in here is the 'RSA 2048 bit key: Amazon Root CA 1' which is valid 
// until January 16, 2038 so unless it gets revoked you can leave this as is:
const String caPemCrt = \
//-----BEGIN CERTIFICATE-----
"MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF" \
"ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6" \
"b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL" \
"MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv" \
"b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj" \
"ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM" \
"9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw" \
"IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6" \
"VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L" \
"93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm" \
"jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC" \
"AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA" \
"A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI" \
"U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs" \
"N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv" \
"o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU" \
"5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy" \
"rqXRfboQnoZsG4q5WTP468SQvvG5";
//-----END CERTIFICATE-----

WiFiClientSecure wiFiClient;
void msgReceived(char* topic, byte* payload, unsigned int len);
PubSubClient pubSubClient(awsEndpoint, 8883, msgReceived, wiFiClient); 

void setup() {
  Serial.begin(115200); Serial.println();
  Serial.println("ESP8266 AWS IoT Example");

  Serial.print("Connecting to "); Serial.print(ssid);
  WiFi.begin(ssid, password);
  WiFi.waitForConnectResult();
  Serial.print(", WiFi connected, IP address: "); Serial.println(WiFi.localIP());

  // get current time, otherwise certificates are flagged as expired
  setCurrentTime();

  uint8_t binaryCert[certificatePemCrt.length() * 3 / 4];
  int len = b64decode(certificatePemCrt, binaryCert);
  wiFiClient.setCertificate(binaryCert, len);
  
  uint8_t binaryPrivate[privatePemKey.length() * 3 / 4];
  len = b64decode(privatePemKey, binaryPrivate);
  wiFiClient.setPrivateKey(binaryPrivate, len);

  uint8_t binaryCA[caPemCrt.length() * 3 / 4];
  len = b64decode(caPemCrt, binaryCA);
  wiFiClient.setCACert(binaryCA, len);
}

unsigned long lastPublish;
int msgCount;

void loop() {

  pubSubCheckConnect();

  if (millis() - lastPublish > 10000) {
    String msg = String("Hello from ESP8266: ") + ++msgCount;
    pubSubClient.publish("outTopic", msg.c_str());
    Serial.print("Published: "); Serial.println(msg);
    lastPublish = millis();
  }
}

void msgReceived(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received on "); Serial.print(topic); Serial.print(": ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void pubSubCheckConnect() {
  if ( ! pubSubClient.connected()) {
    Serial.print("PubSubClient connecting to: "); Serial.print(awsEndpoint);
    while ( ! pubSubClient.connected()) {
      Serial.print(".");
      pubSubClient.connect("ESPthing");
    }
    Serial.println(" connected");
    pubSubClient.subscribe("inTopic");
  }
  pubSubClient.loop();
}

int b64decode(String b64Text, uint8_t* output) {
  base64_decodestate s;
  base64_init_decodestate(&s);
  int cnt = base64_decode_block(b64Text.c_str(), b64Text.length(), (char*)output, &s);
  return cnt;
}

void setCurrentTime() {
  configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");

  Serial.print("Waiting for NTP time sync: ");
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print("Current time: "); Serial.print(asctime(&timeinfo));
}