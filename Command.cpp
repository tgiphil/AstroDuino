// AstroDuinio Project. Copyright (c) Licensed under the New BSD License.
// Source: https://github.com/tgiphil/AstroDuino

#include "Command.h"
#include "Global.h"

CommandClass Command;

void CommandClass::Init()
{
  position = 0;
  discards = 0;
}

void CommandClass::Add(char c)
{
  if (position >= COMMAND_BUFFER_SIZE)
  {
    // buffer full, discard character
    discards++;

    if (discards >= COMMAND_DISCARDS_FOR_BUFFER_RESET)
    {
      // too many discarded characters, reset buffer
      position = 0;
      discards = 0;
    }

    return;
  }

  buffer[position++] = c;

  if (c == COMMAND_EOL)
  {
    // todo: parse command buffer


    position = 0;
  }
}

bool CommandClass::Parse()
{
  if (position == 0)
    return false;

  char l = buffer[0];

  switch (l)
  {
  case ':':  return true; // todo
  case '*': return true; // todo
  case '@': return true; // todo
  case '$': return true; // todo
  case '!': return true; // todo
  case '%': return true; // todo
  case 'l':  SetLEDOff(); return true;
  case 'L':  SetLEDOn(); return true;
  case 'A':  Acknowledge(); return true;

  default: return false;
  }
}
