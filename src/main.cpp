#include <Arduino.h>
#include <WiFi.h>

WiFiServer server(80);
void setup() {
  Serial.begin(115200);
  Serial.print("Connecting to wifi");
  WiFi.begin("Dunder MiffLAN","06192012");
  while(WiFi.status()!=WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");

  }
  Serial.println("");
  Serial.println("IP Address: "+WiFi.localIP());
  server.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
}