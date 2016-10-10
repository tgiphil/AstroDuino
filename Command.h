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
#define COMMAND_DISCARDS_FOR_BUFFER_RESET 32

class CommandClass
{
public:
  void Setup();
  void Update();
  void Add(char c);

protected:
  uint8_t buffer[COMMAND_BUFFER_SIZE];
  int Length = 0;
  int Discards = 0;
  int ParseOffset = 0;

  void Parse();
  char GetChar();
  char PeekChar();
  void EatWhiteSpace();
  int GetInteger(byte maxlen = 0);

  bool ParseCustom();
  bool ParseLogicPanel();
};

extern CommandClass Command;

#endif

