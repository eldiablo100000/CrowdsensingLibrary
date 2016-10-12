#include <Fishino.h>
#include <SPI.h>

//DHT11
#include <SimpleDHT.h>

//D18B20TEMP
#include <DallasTemperature.h>
#include <OneWire.h>
#include <DallasTemperature.h>


#ifndef _FISHINO32_
#include <Flash.h>
#endif

#include <ThingSpeak.h>


#define MY_SSID  "MYNETWORK"
#define MY_PASS "MYPASSWORD"
#define IPADDR  192, 168,   1, 251
#define GATEWAY 192, 168,   1, 254
#define MASK    255, 255, 255, 0
#define NFIELDS 3
#define CHANNEL1 "TemperatureDHT11"
#define CHANNEL2 "HumidityDHT11"
#define CHANNEL3 "Temperature18B20"
#define MASTERKEY "3UDH5462XBW85SRN"

#ifdef IPADDR
IPAddress ip(IPADDR);
#endif
#ifdef GATEWAY
IPAddress gw(GATEWAY);
#endif
#ifdef MASK
IPAddress nm(MASK);
#endif

String res;
int pinDHT11 = 2;
int pin18B20 = 5;
SimpleDHT11 dht11;
FishinoClient client;
OneWire oneWire(pin18B20);
DallasTemperature sensors(&oneWire);

unsigned long lastConnectionTime = 0;
const unsigned long postingInterval = 15L * 1000L;//15 sec
  ThingSpeak obj;

void printWifiStatus()
{
  Serial.print("SSID: ");
  Serial.println(Fishino.SSID());

  IPAddress ip = Fishino.localIP();
  Serial << F("IP Address: ");
  Serial.println(ip);
  long rssi = Fishino.RSSI();
  Serial << F("signal strength (RSSI):");
  Serial.print(rssi);
  Serial << F(" dBm\n");
}


void setup()
{

  Serial.begin(115200);
  
  // only for Leonardo needed
  while (!Serial)
    ;

  // initialize SPI
  // inizializza il modulo SPI
  //  SPI.begin();
  //  SPI.setClockDivider(SPI_CLOCK_DIV2);

  while(!Fishino.reset())
    Serial << F("Fishino RESET FAILED, RETRYING...\n");
  Serial << F("Fishino WiFi RESET OK\n");

  Fishino.setMode(STATION_MODE);
  Serial << F("Connecting to AP...");
  while(!Fishino.begin(MY_SSID, MY_PASS))
  {
    Serial << ".";
    delay(2000);
  }
  Serial << "OK\n";
  
  
  #ifdef IPADDR
    Fishino.config(ip,gw,gw,nm);
  #else
    Fishino.staStartDHCP();
  #endif

  Serial << F("Waiting for IP...");
  while(Fishino.status() != STATION_GOT_IP)
  {
    Serial << ".";
    delay(500);
  }
  Serial << "OK\n";
  printWifiStatus();
  //D18B20TEMP
  sensors.begin();
  obj=ThingSpeak();

  for (int i = 0 ; i < EEPROM.length() ; i++) {
    EEPROM.write(i, 0);
  }

  char* vect[NFIELDS]={(char*)CHANNEL1,(char*)CHANNEL2,(char*)CHANNEL3};
  if(obj.KeyExists()==false){
    
   res=obj.CreateChannel(client,MASTERKEY,"ciaociaom",vect,NFIELDS,NULL,-1,-1,-1,true,NULL,NULL);
   Serial.print("ciao");
  lastConnectionTime = millis();
  }
  }

void loop()
{ 
  
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
