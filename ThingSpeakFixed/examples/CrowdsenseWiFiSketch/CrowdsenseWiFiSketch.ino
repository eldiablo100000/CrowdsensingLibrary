
#include <SPI.h>
#include <WiFi.h>

//DHT11
#include <SimpleDHT.h>

//D18B20TEMP
#include <DallasTemperature.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ThingSpeak.h>

#ifndef _FISHINO32_
#include <Flash.h>
#endif
#include 
#define NFIELDS 3
#define CHANNEL1 "TemperatureDHT11"
#define CHANNEL2 "HumidityDHT11"
#define CHANNEL3 "Temperature18B20"
#define MASTERKEY "3UDH5462XBW85SRN"

#define MY_SSID  "MYNETWORK"
#define MY_PASS "MYPASSWORD"
String res;
int pinDHT11 = 2;
int pin18B20 = 5;
SimpleDHT11 dht11;
WiFiClient client;
OneWire oneWire(pin18B20);
DallasTemperature sensors(&oneWire);

unsigned long lastConnectionTime = 0;
const unsigned long postingInterval = 15L * 1000L;//15 sec
  ThingSpeak obj;

int keyIndex = 0;            // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;

WiFiClient client;
void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}


void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv != "1.1.0") {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(MY_SSID, MY_PASS);

    // wait 10 seconds for connection:
    delay(10000);
  }
  Serial.println("Connected to wifi");
  printWifiStatus();
//D18B20TEMP
  sensors.begin();
  obj=ThingSpeak();

  for (int i = 0 ; i < EEPROM.length() ; i++) {
    EEPROM.write(i, 0);
  }

  char* vect[NFIELDS]={(char*)CHANNEL1,(char*)CHANNEL2,(char*)CHANNEL3};
  if(obj.KeyExists()==false){
    
   res=obj.CreateChannel(client,MASTERKEY,"ciaociaoWifi",vect,NFIELDS,NULL,-1,-1,-1,true,NULL,NULL);
   Serial.print("ciao");
  lastConnectionTime = millis();
  }
}

void loop() {
 //DHT11 PART
  byte temperature = 0;
  byte humidity = 0;
  byte data[40] = {0};
  if (dht11.read(pinDHT11, &temperature, &humidity, data)) {
    return;
  }

  //D18B20TEMP
  sensors.requestTemperatures();
  float temperature2=    sensors.getTempCByIndex(0);
   
  if (millis() - lastConnectionTime > postingInterval)
  {
  //REQUEST PART
  
  float vect[NFIELDS]={float(temperature),float(humidity),float(temperature2)};
 
  if(obj.KeyExists()==false)
  obj.UpdateChannel(client,"I9KH0AQCQFNVX043","",vect,NFIELDS,"",-1,-1,-1,true,"","");
  else{
  String key=obj.GetKey();
  delay(1000);
  obj.UpdateChannel(client,key,"",vect,NFIELDS,"",-1,-1,-1,true,"","");
  }
  }
}



