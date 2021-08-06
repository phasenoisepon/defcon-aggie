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

class CaptiveRequestHandler : public AsyncWebHandler {
public:
  CaptiveRequestHandler() {}
  virtual ~CaptiveRequestHandler() {}

  bool canHandle(AsyncWebServerRequest *request){
    //request->addInterestingHeader("ANY");
    return true;
  }

  void handleRequest(AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/challenge1.html","text/html", false); 
    Serial.println("other dec /challenge1.html");
  }
};

void setupServer(){

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(SPIFFS, "/challenge1.html","text/html", false); 
      Serial.println("/challenge1.html");
  });

  server.on("/cybersec.jpg", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(SPIFFS, "/cybersec.jpg","image/jpeg", false); 
      Serial.println("/cybersec.jpg requested");
  });

  server.on("/rev.mp3", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(SPIFFS, "/rev.mp3","audio/mpeg", false); 
      Serial.println("/rev.mp3 requested");
  });

}


void setup(){
  //your other setup stuff...
  Serial.begin(115200);
  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  
  //List contents of SPIFFS
  listDir(SPIFFS, "/", 0);

  Serial.println();
  Serial.println("Setting up AP Mode");
  WiFi.mode(WIFI_AP); 
  WiFi.softAP("Aggie Challenge #1");
  Serial.print("AP IP address: ");Serial.println(WiFi.softAPIP());
  Serial.println("Setting up Async WebServer");
  setupServer();
  Serial.println("Starting DNS Server");
  dnsServer.start(53, "*", WiFi.softAPIP());
  server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);//only when requested from AP
  //more handlers...
  server.begin();
  Serial.println("All Done!");
}

void loop(){
  dnsServer.processNextRequest();
}
