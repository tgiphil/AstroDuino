// AstroDuinio Project. Copyright (c) Licensed under the New BSD License.
// Source: https://github.com/tgiphil/AstroDuino

#include "Comm.h"
#include "Command.h"

void CommClass::Setup()
{
  Serial.begin(9600);

  OutputLine("AstroDuino Ready!");
}

void CommClass::Update()
{
  CheckSerial();
}

void CommClass::WaitForConnection()
{
  // wait for serial port to connect. 
  while (!Serial) {
    ;
  }
}

void CommClass::CheckSerial()
{
  if (Serial.available() > 0)
  {
    int byte = Serial.read();

    if (byte > 0)
      Command.Add(byte);
  }
}

void CommClass::OutputLine(const String &s)
{
  Serial.println(s);
}

void CommClass::OutputLine(const char* s)
{
  Serial.println(s);
}

void CommClass::Output(const String &s)
{
  Serial.print(s);
}

void CommClass::Output(const char* s)
{
  Serial.print(s);
}

void CommClass::DebugLine(const String &s)
{
  if (DebugEnabled)
    Serial.println(s);
}

void CommClass::DebugLine(const char* s)
{
  if (DebugEnabled)
    Serial.println(s);
}

void CommClass::Debug(const String &s)
{
  if (DebugEnabled)
    Serial.print(s);
}

void CommClass::Debug(const char* s)
{
  if (DebugEnabled)
    Serial.print(s);
}

CommClass Comm;

