#include <Arduino.h>
#include <AccelStepper.h>
#include <MultiStepper.h>
#include <EEPROM.h>
#include <LiquidCrystal.h>
#include <yxml.h>
#include <XMLWriter.h>
typedef struct Settings
{
  int numSteppers = 5;
  int stepPins[10];
  int dirPins[10];
  int enablePins[10];
  float speed = 250;
  float acceleration = 250;
  float endToEndDist = 0;
  bool isInitialized = false;
};

const int rs=12,en=11,d4=5,d5=4,d6=3,d7=2;



LiquidCrystal lcd(rs,en,d4,d5,d6,d7);
AccelStepper* m_steppers;
MultiStepper m_multiStepper;
Settings m_settings;

void InitializeSteppers();
void ReportSettings();
void SetSettings(String serialInput);
void WriteStatus(String status);
void WriteStatusToLCD(String status);
void ClearEEPROM();
void ReadEEPROM();

void setup() 
{

  lcd.begin(16,2);
  Serial.begin(256000);
  delay(500);
  WriteStatus("Controller Started!");
  WriteStatus("Initializing...");



  char buff[10];
  Serial.println(itoa((int)m_settings.isInitialized,buff,10));

  if(!m_settings.isInitialized){
    Serial.print("Please initialize the settings\r\n");
        //lcd.clear();
    //lcd.write("Please initialize the settings\r\n");
    
    return;
  }

  InitializeSteppers();

}

void WriteStatus(String status)
{
  Serial.print(status);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.write(status.c_str());
}

void ClearEEPROM(){
  for(int i = 0;i<EEPROM.end();i++)
    EEPROM.put(i,0);
  
}

void ReadEEPROM()
{
  WriteStatus("Reading Settings from EEPROM");
  EEPROM.get(0,m_settings);
}


void InitializeSteppers()
{

  delete[] m_steppers;
  m_steppers = new AccelStepper[m_settings.numSteppers];

  for(int i = 0;i<m_settings.numSteppers;i++)
  {

    m_steppers[i] = AccelStepper(AccelStepper::DRIVER,m_settings.stepPins[i],m_settings.dirPins[i]);
    m_steppers[i].setEnablePin(m_settings.enablePins[i]);
    m_multiStepper.addStepper(m_steppers[i]);
  }
}



void loop() 
{
  const String GET_SETTINGS_TAG = "GET SETTINGS",
               SET_SETTINGS_TAG = "SET SETTINGS";

  while(Serial.available()==0){} // wait for input
  String serialInput = Serial.readString();
  serialInput.trim();
  serialInput.toUpperCase();

  if(serialInput==GET_SETTINGS_TAG) ReportSettings();
  if(serialInput.startsWith(SET_SETTINGS_TAG)) SetSettings(serialInput);
}

void ReportSettings()
{
  XMLWriter xml;
  xml.header();
  xml.tagOpen("settings");
  {
  char buf[10];
  
  xml.writeNode("NumSteppers",itoa(m_settings.numSteppers,buf,10));
  xml.writeNode("Speed",dtostrf(m_settings.speed,10,2,buf));
  xml.writeNode("Acceleration",dtostrf(m_settings.acceleration,10,2,buf));
  xml.writeNode("EndToEndDist",dtostrf(m_settings.endToEndDist,10,2,buf));
  xml.tagOpen("Steppers");
  {
    for(int i = 0;i<m_settings.numSteppers;i++)
      {
        xml.tagOpen("Stepper");
        xml.writeNode("StepperNumber",itoa(i,buf,10));
        xml.writeNode("StepPin",itoa(m_settings.stepPins[i],buf,10));
        xml.writeNode("DirectionPin",itoa(m_settings.dirPins[i],buf,10));
        xml.writeNode("EnablePin",itoa(m_settings.enablePins[i],buf,10));
        xml.tagClose();
      }
    
  }
  xml.tagClose();
 }
  xml.tagClose();
  xml.flush();
}

void SetSettings(String serialInput)
{

}
