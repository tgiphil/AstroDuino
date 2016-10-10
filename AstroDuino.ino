// AstroDuinio Project. Copyright (c) Licensed under GNU General Public License, version 3.0
// Source: https://github.com/tgiphil/AstroDuino

#include "LogicPanelControl.h"
#include "LogicDisplay.h"
#include "Comm.h"
#include <FastLED.h>
//#include <EEPROM.h>
//#include <Wire.h>
//#include <Servo.h>

#include "LogicPanel.h"
#include "Command.h"
#include "Ticks.h"
#include "Global.h"

//Servo myservo;

void setup()
{
  Comm.Setup();

  Command.Setup();

  pinMode(LED_BUILTIN, OUTPUT);

  Ticks.Setup();

  //LogicPanel.Setup();
  LogicPanelControl.Setup();

  SetLEDOn();

  //myservo.attach(8);
}

//unsigned long next = 0;

void loop()
{
  Ticks.Update();
  Comm.Update();
  Command.Update();
  //LogicPanel.Update();
  LogicPanelControl.Update();

  //if (Ticks.Now > next)
  //{
  //  int pos = (Ticks.Now / 50) % 180;

  //  myservo.write(pos);

  //  next = Ticks.Now + 1;
  //}
}

void SetLEDOff()
{
  digitalWrite(LED_BUILTIN, LOW);
}

void SetLEDOn()
{
  digitalWrite(LED_BUILTIN, HIGH);
}

void Acknowledge()
{
  Serial.println("Acknowledge");
}
