/*
  Example from WiFi > WiFiScan
  Complete details at https://RandomNerdTutorials.com/esp32-useful-wi-fi-functions-arduino/
*/

#include "WiFi.h"
#include <WebServer.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>

AsyncWebServer server(80);

bool InitSpiffs();
bool InitWifi();
bool InitServer();


bool InitSpiffs()
{
  if(!SPIFFS.begin()){
    Serial.println("Unable to mount SPIFFS.");
    return false;
  }
  return true;
}

bool InitWifi()
{
  // Set WiFi to station mode and disconnect from an AP if it was previously connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  Serial.println("Connecting to wifi");
  WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
  WiFi.setHostname("BensPlaneSmoother");

  WiFi.begin("Dunder MiffLAN","06192012");

  int count = 0;
  int delayMillis = 500;
  int timeout = 30*1000;
  while (WiFi.status()!=WL_CONNECTED)
  {
    Serial.print(".");
    delay(delayMillis);
    count+=delayMillis;
    if(count > timeout)
    {
      Serial.println("Unable to connect to wifi");
      return false;
    }
  }
  Serial.println();
  Serial.print("Local IP Address: ");
  Serial.println( WiFi.localIP());
  return true;
}

bool InitServer()
{
  server.begin();

   server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/ws.html", "text/html");
  });
}




void setup() 
{
  Serial.begin(115200);

  if(!InitSpiffs());
  if(!InitWifi());
  
}



void loop() {

  


  /*Serial.println("scan start");

  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0) {
      Serial.println("no networks found");
  } else {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i) {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
      delay(10);
    }
  }
  Serial.println("");

  // Wait a bit before scanning again
  delay(5000);*/
}