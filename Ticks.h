// AstroDuinio Project. Copyright (c) Licensed under GNU General Public License, version 3.0
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
public:
  unsigned long Now = 0;

  void Setup();
  void Update();

protected:
  unsigned long start = 0;
  unsigned int ticks = 0;
  unsigned int ticksPerSecond = 0;
};

extern TicksClass Ticks;

#endif

