/*
  Example from WiFi > WiFiScan
  Complete details at https://RandomNerdTutorials.com/esp32-useful-wi-fi-functions-arduino/
*/

#include "WiFi.h"
#include <WebServer.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <Bounce2.h>
#include <elapsedMillis.h>
#include "TMC2208_AccelStepper.h"
#include <MultiStepper.h>
#include <SoftwareSerial.h>

AsyncWebServer server(80);

const int STEPS_PER_MM = 25;

bool InitSpiffs();
bool InitWifi();
bool InitServer();
bool InitButtons();
bool InitSteppers();
bool Home();
Bounce m_eStop;
Bounce m_endStop;
Bounce m_homeLimit;


TMC2208_AccelStepper* m_stepper1;
TMC2208_AccelStepper* m_stepper2;


elapsedMillis printTime;

int m_target = 0;

enum class ProgramStatus
{
  Stop,
  Run,
  Home,
  FindLimits
};

ProgramStatus m_status;

void setup()
{

  Serial.begin(115200);

  if (!InitButtons())
    return;
  if (!InitSteppers())
    return;
  if (!InitSpiffs())
    return;
  if (!InitWifi())
    return;
  if (!InitServer())
    return;
}

bool InitButtons()
{

  m_eStop.attach(GPIO_NUM_21, INPUT_PULLUP);
  m_eStop.interval(50);
  m_eStop.SetInvertState(true);

  m_homeLimit.attach(GPIO_NUM_22, INPUT_PULLUP);
  m_homeLimit.interval(50);
  m_homeLimit.SetInvertState(true);

  m_endStop.attach(GPIO_NUM_23, INPUT_PULLUP);
  m_endStop.interval(50);
  m_endStop.SetInvertState(true);

  return true;
}

bool InitSteppers()
{
  Serial.println("Initializing Steppers...");
  m_stepper1 = new TMC2208_AccelStepper(AccelStepper::DRIVER, GPIO_NUM_2, GPIO_NUM_15,GPIO_NUM_0,GPIO_NUM_4,GPIO_NUM_16,.11f);
  m_stepper1->setPinsInverted(true, true, true);
  m_stepper1->setSpeedMM(50);
  m_stepper1->setAcclerationMMPSS(50);
  m_stepper1->enableOutputs();

  return true;
}

bool InitSpiffs()
{
  if (!SPIFFS.begin())
  {
    Serial.println("Unable to mount SPIFFS.");
    return false;
  }
  return true;
}

bool InitWifi()
{

  // Set WiFi to station mode and disconnect from an AP if it was previously connected
  WiFi.mode(WIFI_STA);
  Serial.println("Connecting to wifi");
  WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
  WiFi.setHostname("BensPlaneSmoother");

  WiFi.begin("Dunder MiffLAN", "06192012");

  int count = 0;
  int delayMillis = 500;
  int timeout = 30 * 1000;
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(delayMillis);
    count += delayMillis;
    if (count > timeout)
    {
      Serial.println("Unable to connect to wifi");
      return false;
    }
  }
  Serial.println();
  Serial.print("Local IP Address: ");
  Serial.println(WiFi.localIP());
  return true;
}

bool InitServer()
{

  Serial.println("Starting Server...");
  server.begin();

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/ws.html", "text/html"); });

  server.on("/jquery-3.6.1.min.js", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/jquery-3.6.1.min.js", "text/javascript"); });

  server.on("/EStop", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, String(), m_eStop.read() ? "Triggered" : "Open"); });
  server.on("/HomeLimit", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, String(), m_homeLimit.read() ? "Triggered" : "Open"); });
  server.on("/EndStop", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, String(), m_endStop.read() ? "Triggered" : "Open"); });
  server.on("/Home", HTTP_POST, [](AsyncWebServerRequest *request)
            { m_status= ProgramStatus::Home;request->send(200, String(),"OK");});

  Serial.println("Server Started.");
  return true;
}

void StopAll()
{
  m_status = ProgramStatus::Stop;
  m_stepper1->stop();
}

bool Home()
{
  Serial.println("Homing...");
  elapsedMillis elapsed;
  m_stepper1->setSpeed(-10000);
  while (!m_homeLimit.read() && !m_eStop.read() && elapsed < 60000)
  {
    Serial.println("Homing...");
    m_homeLimit.update();
    m_eStop.update();
    m_stepper1->runSpeed();
  }
   m_stepper1->stop();
  if (m_eStop.read())
    return false;
  if (elapsed >= 60000)
    return false;

    Serial.println("bumping...");
  m_stepper1->setCurrentPosition(0);
  m_stepper1->setSpeed(5000);
  m_stepper1->moveTo(5*STEPS_PER_MM);
  while(!m_eStop.read() && m_stepper1->run())
  {
     m_eStop.update();
     m_homeLimit.update();
    Serial.println("bumping...");

  }
  Serial.println("done bumping...");
  if(m_eStop.read()) return false;
  m_stepper1->setSpeed(-1000);
  
  Serial.println("starting to home...");

  while (!m_homeLimit.read() && !m_eStop.read())
  {
    Serial.println("Homing...");
    m_homeLimit.update();
    m_eStop.update();
    m_stepper1->runSpeed();
  }

  return true;
}

void loop()
{
  m_eStop.update();
  m_endStop.update();
  m_homeLimit.update();
  if(m_status == ProgramStatus::Home)
  {
    Home();
    m_status=ProgramStatus::Stop;
  }
}
