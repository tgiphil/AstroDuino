// AstroDuinio Project. Copyright (c) Licensed under GNU General Public License, version 3.0
// Source: https://github.com/tgiphil/AstroDuino

#ifndef _COMMAND_h
#define _COMMAND_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

// Size of the command buffer
#define COMMAND_BUFFER_SIZE 64

// Number of discards before buffer is reset
#define COMMAND_DISCARDS_FOR_BUFFER_RESET 100

// Command end of line character
#define COMMAND_EOL '\r'

class CommandClass
{
protected:

  uint8_t buffer[COMMAND_BUFFER_SIZE];
  int position = 0;
  int discards = 0;

public:
  void Setup();
  void Update();
  void Add(char c);

protected:
  void Parse();
};

extern CommandClass Command;

#endif

