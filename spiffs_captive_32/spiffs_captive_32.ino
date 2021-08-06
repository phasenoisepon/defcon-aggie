#include <DNSServer.h>
#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include "ESPAsyncWebServer.h"
//#include <SPIFFS.h>
#include "SPIFFS.h"

DNSServer dnsServer;
AsyncWebServer server(80);

class CaptiveRequestHandler : public AsyncWebHandler {
public:
  CaptiveRequestHandler() {}
  virtual ~CaptiveRequestHandler() {}

  bool canHandle(AsyncWebServerRequest *request){
    return true;
  }

  void handleRequest(AsyncWebServerRequest *request) {

    if (SPIFFS.exists("/index.html")) {

      Serial.println("/index exists!");

      AsyncResponseStream *response = request->beginResponseStream("text/html");

      File file = SPIFFS.open("/index.html");

      while(file.available()){

          response->write(file.read());
      }

      request->send(response);
    }

    else{
      Serial.println("/index DNE!");
      request->send(404);
    }

  }
};


void setup(){
  Serial.begin(115200);
  if(!SPIFFS.begin(true))
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
  }

  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
  WiFi.softAP("Aggie Hunt #1");
  dnsServer.start(53, "*", WiFi.softAPIP());
  server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);
  server.begin();
  Serial.println("Server started!");
}

void loop(){
  dnsServer.processNextRequest();
}
