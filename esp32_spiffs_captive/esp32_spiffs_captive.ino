#include <DNSServer.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include "ESPAsyncWebServer.h"

#include "FS.h"
#include "SPIFFS.h"


void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
   Serial.printf("Listing directory: %s\r\n", dirname);

   File root = fs.open(dirname);
   if(!root){
      Serial.println("− failed to open directory");
      return;
   }
   if(!root.isDirectory()){
      Serial.println(" − not a directory");
      return;
   }

   File file = root.openNextFile();
   while(file){
      if(file.isDirectory()){
         Serial.print("  DIR : ");
         Serial.println(file.name());
         if(levels){
            listDir(fs, file.name(), levels -1);
         }
      } else {
         Serial.print("  FILE: ");
         Serial.print(file.name());
         Serial.print("\tSIZE: ");
         Serial.println(file.size());
      }
      file = root.openNextFile();
   }
}


DNSServer dnsServer;
AsyncWebServer server(80);
unsigned long currentMillis;
const unsigned long period = 1000;
const byte ledPin = 13;
unsigned long startMillis;

class CaptiveRequestHandler : public AsyncWebHandler {
public:
  CaptiveRequestHandler() {}
  virtual ~CaptiveRequestHandler() {}

  bool canHandle(AsyncWebServerRequest *request){
    //request->addInterestingHeader("ANY");
    return true;
  }

  void handleRequest(AsyncWebServerRequest *request) {
    Serial.println("Request for "+request->url());
    if (request->url() == "/cybersec.jpg") {
      request->send(SPIFFS, "/cybersec.jpg","image/jpeg", false);
    } else if (request->url() == "/rev.mp3") {
      request->send(SPIFFS, "/rev.mp3","audio/mpeg", false); 
    } else {
      request->send(SPIFFS, "/challenge1.html","text/html", false); 
    }
  }
};

void setup(){
  //your other setup stuff...
  Serial.begin(115200);
  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  //setup led pin
  pinMode(ledPin, OUTPUT);
  
  //List contents of SPIFFS
  listDir(SPIFFS, "/", 0);

  Serial.println();
  Serial.println("Setting up AP Mode");
  WiFi.mode(WIFI_AP); 
  WiFi.softAP("Aggie Challenge #1");
  Serial.print("AP IP address: ");Serial.println(WiFi.softAPIP());
  Serial.println("Starting DNS Server");
  dnsServer.start(53, "*", WiFi.softAPIP());
  server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);//only when requested from AP
  //more handlers...
  server.begin();
  Serial.println("All Done!");

  startMillis = millis();
}

void loop(){
  dnsServer.processNextRequest();
  currentMillis = millis();
  if(currentMillis - startMillis >= period){
    digitalWrite(ledPin, !digitalRead(ledPin));
    startMillis = currentMillis;
  }
}
