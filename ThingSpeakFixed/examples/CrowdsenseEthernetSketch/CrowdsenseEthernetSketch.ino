#include <ThingSpeak.h>

#include <ArduinoJson.h>

//versione corretta
#include <Dns.h>
#include <EthernetClient.h>
#include <Dhcp.h>
#include <EthernetServer.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
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

#define NUMERO 3
#define CHANNEL1 "TemperatureDHT11"
#define CHANNEL2 "HumidityDHT11"
#define CHANNEL3 "Temperature18B20"
#define MASTERKEY "3UDH5462XBW85SRN"
String res;
int pinDHT11 = 7;
int pin18B20 = 8;
SimpleDHT11 dht11;
EthernetClient client;
OneWire oneWire(pin18B20);
DallasTemperature sensors(&oneWire);

unsigned long lastConnectionTime = 0;
const unsigned long postingInterval = 15L * 1000L;//15 sec
  ThingSpeak obj;


byte mac[] = { 0x90, 0xA2, 0xDA, 0x03, 0x00, 0xBB };
IPAddress ip(192, 168, 0, 177);
void setup()
{

  Serial.begin(9600);
  
  // only for Leonardo needed
  while (!Serial)
    ;


 if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip);
  }
  //D18B20TEMP
  delay(1000);
  sensors.begin();
  obj=ThingSpeak();
    Serial.print(Ethernet.localIP());
  char* vect[NUMERO]={(char*)CHANNEL1,(char*)CHANNEL2,(char*)CHANNEL3};
  if(obj.KeyExists()==false){
    
   res=obj.CreateChannel(client,MASTERKEY,"canale555",vect,NUMERO,NULL,-1,-1,-1,true,NULL,NULL);
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
  
  float vect[NUMERO]={float(temperature),float(humidity),float(temperature2)};

 if(obj.KeyExists()==false)
  obj.UpdateChannel(client,"I9KH0AQCQFNVX043","",vect,NUMERO,"",-1,-1,-1,true,"","");
 else{
  String key=obj.GetKey();
  delay(1000);
  obj.UpdateChannel(client,key,"",vect,NUMERO,"",-1,-1,-1,true,"","");
  }
  }
}
