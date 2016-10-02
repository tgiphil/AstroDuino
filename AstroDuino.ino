// AstroDuinio Project. Copyright (c) Licensed under the New BSD License.
// Source: https://github.com/tgiphil/AstroDuino

#include "Comm.h"
#include <FastLED.h>
#include <EEPROM.h>
//#include <Wire.h>
//#include <Servo.h>

#include "LogicEngine.h"
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

  //myservo.attach(8);
}

//unsigned long next = 0;

void loop()
{
  Ticks.Update();
  Comm.Update();
  Command.Update();

  //if (Ticks.Now > next)
  //{
  //  int pos = (Ticks.Now / 50) % 180;

  //  myservo.write(pos);

  //  next = Ticks.Now + 1;
  //}

}


void SetLEDOff()
{
  digitalWrite(LED_BUILTIN, HIGH);
}

void SetLEDOn()
{
  digitalWrite(LED_BUILTIN, LOW);
}

void Acknowledge()
{
  Serial.println("Acknowledge");
}
