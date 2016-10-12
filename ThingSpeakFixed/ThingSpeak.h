#ifndef ThingSpeak_h
#define ThingSpeak_h

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#ifndef _FISHINO32_
#include <Flash.h>
#endif
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <WiFiUdp.h>

#include <Dns.h>
#include <EthernetClient.h>
#include <Dhcp.h>
#include <EthernetServer.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <Fishino.h>
#include <SPI.h>
#include <EEPROM.h>
#define LENGTHKEY 16
#define SERVER "184.106.153.149"
class ThingSpeak{
	public:
		ThingSpeak();
		void UpdateChannel(FishinoClient client, String api_key,String name, float val[],int n_fields, String desc, int elevation,float lat, float lon, bool public_f, String tags, String url);
		String CreateChannel(FishinoClient client,char* api_key,char* name,char** val, int n_fields, char* desc, int elevation, float lat, float lon, bool public_f, char* tags, char* url);
		void UpdateChannel(EthernetClient client, String api_key,String name, float val[],int n_fields, String desc, int elevation,float lat, float lon, bool public_f, String tags, String url);
		String CreateChannel(EthernetClient client,char* api_key,char* name,char** val, int n_fields, char* desc, int elevation, float lat, float lon, bool public_f, char* tags, char* url);
		void UpdateChannel(WiFiClient client, String api_key,String name, float val[],int n_fields, String desc, int elevation,float lat, float lon, bool public_f, String tags, String url);
		String CreateChannel(WiFiClient client,char* api_key,char* name,char** val, int n_fields, char* desc, int elevation, float lat, float lon, bool public_f, char* tags, char* url);
		String GetKey();
		bool KeyExists();

	private:
		byte value;
		int address;
		void WriteKey(char* api_key,int start);
		String ParseKey();
		void PrintKey(int add,int key_length);
};
#endif
