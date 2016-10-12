
#include "ThingSpeak.h"

//Costructor 
ThingSpeak::ThingSpeak(){
  address=0;
}

//Function to write buffer on ROM 
void ThingSpeak::WriteKey(char* api_key,int start){
  //	Serial.println(api_key);
  for(int j=0; j< strlen(api_key); j++){
    EEPROM.write(j+start,api_key[j]);
  }
}

//Function to parse HTTP response stored, clear ROM and replace with new api key
String ThingSpeak::ParseKey(){
  // char key[512];
  String key;
  for (int i=0;i<EEPROM.length();i++){
    byte val= EEPROM.read(i);
    //Serial.println((char)val);
    if(i>180) //per non bufferizzare tutto l output di thingspeak
      key+=(char)val;
  } 
  char api[LENGTHKEY+1];
  int init;
  init=key.indexOf("api_keys");
  int el;
  api[0]='#';
  for(el=0;el<LENGTHKEY;el++){
    api[el+1]=key[init+el+1+22];
  }
  for (int i = 0 ; i < EEPROM.length() ; i++){
    EEPROM.write(i, 0);
  }
  WriteKey(api,0);
  return api;
}

//Function to retrieve api key on ROM
String ThingSpeak::GetKey(){
  String api;
  if((char)EEPROM.read(0)=='#'){

    for(int i=0;i<LENGTHKEY;i++){
      api+=(char)EEPROM.read(i+1);
	
	if(EEPROM.read(i+1)==0){
		Serial.println("error in GetKey(): key null");
		api="";		
		return api;		
	}
    }
    return api;
    }
  else{
    Serial.println("error in GetKey()");
    api="";
    return api;
  }
}

//Function to print api key on Serial 
void ThingSpeak::PrintKey(int add,int key_length){
  for (int i=0;i<key_length;i++){
    value = EEPROM.read(add);
    Serial.print(add);
    Serial.print("\t");
    Serial.print((char)value);
    Serial.println();
    add = add + 1;
    if (add == EEPROM.length()){
      add = 0;
    }
  }
}

//Function to check if api key exists
bool ThingSpeak::KeyExists(){
  if(GetKey()==""){
    //Serial.println("ApiKey doesn't exist! Let's create...");
    return false;
  }
  else{
    //PrintKey(1,16);
    return true;
  }
}

void ThingSpeak::UpdateChannel(FishinoClient client, String api_key,String name, float val[],int n_fields, String desc, int elevation,float lat, float lon, bool public_f, String tags, String url){
  client.stop();
  if (client.connect(SERVER,80)){
    String postStr = api_key;
    if(name!=NULL){
      postStr+="&name=";
      postStr+=name;
    }
    if(desc!=NULL){
      postStr+="&description=";
      postStr+=desc;
    }
    if(elevation>=0){
      postStr+="&elevation=";
      postStr+=elevation;
    }

    int i;
    for(i=0;i<n_fields;i++)
    {
      postStr +="&field";
      postStr +=String(i+1);
      postStr +="=";
      postStr += String(float(val[i])); 
    }
    if(lat>=0){
      postStr+="&latitude=";
      postStr+=lat;
    }
    if(lon>=0){
      postStr+="&longitude=";
      postStr+=lon;
    }
    if(public_f){
      postStr+="&public_flag=true";
    }
    if(tags!=NULL){
      postStr+="&tags=";
      postStr+=tags;
    }
    if(url!=NULL){
      postStr+="&url=";
      postStr+=url;
    }
    postStr += "\r\n\r\n";
    Serial.print(postStr);
    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: "+api_key+"\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);

    //PrintKey(0,8);

    /*while(!client.available()){
    Serial.write(".")
    }
    while (client.available())
    {
    char c = client.read();
    //Serial.write("client...");
    Serial.write(c);
    }*/    
    }
  else{
    Serial.println(SERVER);
    Serial << F("connection failed\n");

  }


}

String ThingSpeak::CreateChannel(FishinoClient client,char* api_key,char* name,char** val, int n_fields, char* desc, int elevation, float lat, float lon, bool public_f, char* tags, char* url){ 

  for(int i=0;i<EEPROM.length();i++){
    EEPROM.write(i,0);
  }
  if(api_key==NULL){
    Serial.println("api_key null");
 	String a="";
    return a ;
  }
  client.stop();
  if (client.connect(SERVER,80)){ // "184.106.153.149" or api.thingspeak.com
    Serial.println("Create");
    String postStr = api_key;
    if(name!=NULL){
      postStr+="&name=";
      postStr+=name;
    }
    if(desc!=NULL){
      postStr+="&description=";
      postStr+=desc;
    }
    if(elevation>=0){
      postStr+="&elevation=";
      postStr+=elevation;
    }

    int i;
    for(i=0;i<n_fields;i++)
    {
      postStr +="&field";
      postStr +=String(i+1);
      postStr +="=";
      postStr += String(val[i]); 
    }
    if(lat>=0){
      postStr+="&latitude=";
      postStr+=lat;
    }
    if(lon>=0){
      postStr+="&longitude=";
      postStr+=lon;
    }
    if(public_f){
      postStr+="&public_flag=true";
    }
    if(tags!=NULL){
      postStr+="&tags=";
      postStr+=tags;
    }
    if(url!=NULL){
      postStr+="&url=";
      postStr+=url;
    }
    postStr += "\r\n\r\n";
    Serial.print(postStr);
    client.print("POST /channels.json HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: ");
    client.print(api_key);
    client.print("\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);
    //WriteKey("creating");
    int a=-1;
    String res;
    bool store=false;
    int count=0;
    while(client.connected()){
      /*while(!client.available()){
        Serial.write(".");
      }
      Serial.println();*/
      while (client.available()){
        char c = client.read();
        if(c=='{')
          store=true;
        if(store){
          Serial.print(c);   
          char api[1]={c};
          WriteKey(api,count);
          count+=1;
        }
      }
    }

    res=ParseKey();
    //PrintKey(0,20);
    if(res.length()>0){
      Serial.print("Creato Canale con successo! Api Key :  ");
      Serial.println(res);
    }
    res.replace("#","");
    return res;
  }
  else{
    Serial.println(SERVER);
    Serial << F("connection failed\n");
  }
}



void ThingSpeak::UpdateChannel(EthernetClient client, String api_key,String name, float val[],int n_fields, String desc, int elevation,float lat, float lon, bool public_f, String tags, String url){
  client.stop();
  if (client.connect(SERVER,80)){
    String postStr = api_key;
    if(name!=NULL){
      postStr+="&name=";
      postStr+=name;
    }
    if(desc!=NULL){
      postStr+="&description=";
      postStr+=desc;
    }
    if(elevation>=0){
      postStr+="&elevation=";
      postStr+=elevation;
    }

    int i;
    for(i=0;i<n_fields;i++)
    {
      postStr +="&field";
      postStr +=String(i+1);
      postStr +="=";
      postStr += String(float(val[i])); 
    }
    if(lat>=0){
      postStr+="&latitude=";
      postStr+=lat;
    }
    if(lon>=0){
      postStr+="&longitude=";
      postStr+=lon;
    }
    if(public_f){
      postStr+="&public_flag=true";
    }
    if(tags!=NULL){
      postStr+="&tags=";
      postStr+=tags;
    }
    if(url!=NULL){
      postStr+="&url=";
      postStr+=url;
    }
    postStr += "\r\n\r\n";
    Serial.print(postStr);
    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: "+api_key+"\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);

    //PrintKey(0,8);

    /*while(!client.available()){
    Serial.write(".")
    }
    while (client.available())
    {
    char c = client.read();
    //Serial.write("client...");
    Serial.write(c);
    }*/    
    }
  else{
    Serial.println(SERVER);
    Serial << F("connection failed\n");

  }


}

String ThingSpeak::CreateChannel(EthernetClient client,char* api_key,char* name,char** val, int n_fields, char* desc, int elevation, float lat, float lon, bool public_f, char* tags, char* url){ 

  for(int i=0;i<EEPROM.length();i++){
    EEPROM.write(i,0);
  }
  if(api_key==NULL){
    Serial.println("api_key null");
    String a="";
    return a ;
  }
  client.stop();
  if (client.connect(SERVER,80)){ // "184.106.153.149" or api.thingspeak.com
    Serial.println("Create");
    String postStr = api_key;
    if(name!=NULL){
      postStr+="&name=";
      postStr+=name;
    }
    if(desc!=NULL){
      postStr+="&description=";
      postStr+=desc;
    }
    if(elevation>=0){
      postStr+="&elevation=";
      postStr+=elevation;
    }

    int i;
    for(i=0;i<n_fields;i++)
    {
      postStr +="&field";
      postStr +=String(i+1);
      postStr +="=";
      postStr += String(val[i]); 
    }
    if(lat>=0){
      postStr+="&latitude=";
      postStr+=lat;
    }
    if(lon>=0){
      postStr+="&longitude=";
      postStr+=lon;
    }
    if(public_f){
      postStr+="&public_flag=true";
    }
    if(tags!=NULL){
      postStr+="&tags=";
      postStr+=tags;
    }
    if(url!=NULL){
      postStr+="&url=";
      postStr+=url;
    }
    postStr += "\r\n\r\n";
    Serial.print(postStr);
    client.print("POST /channels.json HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: ");
    client.print(api_key);
    client.print("\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);
    //WriteKey("creating");
    int a=-1;
    String res;
    bool store=false;
    int count=0;
    while(client.connected()){
      /*while(!client.available()){
        Serial.write(".");
      }
      Serial.println();*/
      while (client.available()){
        char c = client.read();
        if(c=='{')
          store=true;
        if(store){
          Serial.print(c);   
          char api[1]={c};
          WriteKey(api,count);
          count+=1;
        }
      }
    }

    res=ParseKey();
    //PrintKey(0,20);
    if(res.length()>0){
      Serial.print("Creato Canale con successo! Api Key :  ");
      Serial.println(res);
    }
    res.replace("#","");
    return res;
  }
  else{
    Serial.println(SERVER);
    Serial << F("connection failed\n");
  }
}



void ThingSpeak::UpdateChannel(WiFiClient client, String api_key,String name, float val[],int n_fields, String desc, int elevation,float lat, float lon, bool public_f, String tags, String url){
  client.stop();
  if (client.connect(SERVER,80)){
    String postStr = api_key;
    if(name!=NULL){
      postStr+="&name=";
      postStr+=name;
    }
    if(desc!=NULL){
      postStr+="&description=";
      postStr+=desc;
    }
    if(elevation>=0){
      postStr+="&elevation=";
      postStr+=elevation;
    }

    int i;
    for(i=0;i<n_fields;i++)
    {
      postStr +="&field";
      postStr +=String(i+1);
      postStr +="=";
      postStr += String(float(val[i])); 
    }
    if(lat>=0){
      postStr+="&latitude=";
      postStr+=lat;
    }
    if(lon>=0){
      postStr+="&longitude=";
      postStr+=lon;
    }
    if(public_f){
      postStr+="&public_flag=true";
    }
    if(tags!=NULL){
      postStr+="&tags=";
      postStr+=tags;
    }
    if(url!=NULL){
      postStr+="&url=";
      postStr+=url;
    }
    postStr += "\r\n\r\n";
    Serial.print(postStr);
    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: "+api_key+"\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);

    //PrintKey(0,8);

    /*while(!client.available()){
    Serial.write(".")
    }
    while (client.available())
    {
    char c = client.read();
    //Serial.write("client...");
    Serial.write(c);
    }*/    
    }
  else{
    Serial.println(SERVER);
    Serial << F("connection failed\n");

  }


}

String ThingSpeak::CreateChannel(WiFiClient client,char* api_key,char* name,char** val, int n_fields, char* desc, int elevation, float lat, float lon, bool public_f, char* tags, char* url){ 

  for(int i=0;i<EEPROM.length();i++){
    EEPROM.write(i,0);
  }
  if(api_key==NULL){
    Serial.println("api_key null");
    String a="";
    return a ;
  }
  client.stop();
  if (client.connect(SERVER,80)){ // "184.106.153.149" or api.thingspeak.com
    Serial.println("Create");
    String postStr = api_key;
    if(name!=NULL){
      postStr+="&name=";
      postStr+=name;
    }
    if(desc!=NULL){
      postStr+="&description=";
      postStr+=desc;
    }
    if(elevation>=0){
      postStr+="&elevation=";
      postStr+=elevation;
    }

    int i;
    for(i=0;i<n_fields;i++)
    {
      postStr +="&field";
      postStr +=String(i+1);
      postStr +="=";
      postStr += String(val[i]); 
    }
    if(lat>=0){
      postStr+="&latitude=";
      postStr+=lat;
    }
    if(lon>=0){
      postStr+="&longitude=";
      postStr+=lon;
    }
    if(public_f){
      postStr+="&public_flag=true";
    }
    if(tags!=NULL){
      postStr+="&tags=";
      postStr+=tags;
    }
    if(url!=NULL){
      postStr+="&url=";
      postStr+=url;
    }
    postStr += "\r\n\r\n";
    Serial.print(postStr);
    client.print("POST /channels.json HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: ");
    client.print(api_key);
    client.print("\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);
    //WriteKey("creating");
    int a=-1;
    String res;
    bool store=false;
    int count=0;
    while(client.connected()){
      /*while(!client.available()){
        Serial.write(".");
      }
      Serial.println();*/
      while (client.available()){
        char c = client.read();
        if(c=='{')
          store=true;
        if(store){
          Serial.print(c);   
          char api[1]={c};
          WriteKey(api,count);
          count+=1;
        }
      }
    }

    res=ParseKey();
    //PrintKey(0,20);
    if(res.length()>0){
      Serial.print("Creato Canale con successo! Api Key :  ");
      Serial.println(res);
    }
    res.replace("#","");
    return res;
  }
  else{
    Serial.println(SERVER);
    Serial << F("connection failed\n");
  }
}
