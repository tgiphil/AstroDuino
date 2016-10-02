// AstroDuinio Project. Copyright (c) Licensed under the New BSD License.
// Source: https://github.com/tgiphil/AstroDuino

#include "Command.h"
#include "Ticks.h"
#include "Global.h"
//#include <Wire.h>
//#include <Servo.h>

//Servo myservo;

void setup()
{
  SetupSerial();

  Command.Init();

  pinMode(LED_BUILTIN, OUTPUT);

  Ticks.Init();

  //myservo.attach(8);
}

//unsigned long next = 0;

void loop()
{
  Ticks.Update();

  //if (Ticks.Now > next)
  //{
  //  int pos = (Ticks.Now / 50) % 180;

  //  myservo.write(pos);

  //  next = Ticks.Now + 1;
  //}

}

void SetupSerial()
{
  Serial.begin(9600);

  Serial.println("Hello!");
}

void CheckSerial()
{
  if (Serial.available() > 0)
  {
    int byte = Serial.read();

    if (byte > 0)
      Command.Add(byte);
  }
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
