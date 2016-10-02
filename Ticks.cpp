// AstroDuinio Project. Copyright (c) Licensed under the New BSD License.
// Source: https://github.com/tgiphil/AstroDuino

#include "Ticks.h"

TicksClass Ticks;

void TicksClass::Init()
{
  start = millis();
}

void TicksClass::Update()
{
  ticks++;

  Now = millis();
  unsigned long diff = Now - start;

  if (diff >= SAMPLE_TICKS_MILLISECONDS)
  {
    ticksPerSecond = ticks;
    start = Now;
    ticks = 0;

    Serial.println(ticksPerSecond);
  }
}
