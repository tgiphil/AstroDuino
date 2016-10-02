// AstroDuinio Project. Copyright (c) Licensed under the New BSD License.
// Source: https://github.com/tgiphil/AstroDuino

#ifndef _TICKS_h
#define _TICKS_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

// Generate ticks per second (1000 milliseconds)
#define SAMPLE_TICKS_MILLISECONDS 1000

class TicksClass
{
protected:

  unsigned long start = 0;
  unsigned int ticks = 0;
  unsigned int ticksPerSecond = 0;

public:
  unsigned long Now = 0;

  void Init();
  void Update();

protected:
};

extern TicksClass Ticks;

#endif

