// AstroDuinio Project. Copyright (c) Licensed under GNU General Public License, version 3.0
// Source: https://github.com/tgiphil/AstroDuino

#include "Command.h"
#include "Global.h"
#include "LogicPanel.h"
#include "Comm.h"

CommandClass Command;

void CommandClass::Setup()
{
  position = 0;
  discards = 0;
}

void CommandClass::Update()
{
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
    Parse();

    position = 0;
  }
}

void CommandClass::Parse()
{
  if (position == 0)
    return;

  char l = buffer[0];

  switch (l)
  {
  case ':': return; // todo
  case '*': return; // todo
  case '@': return; // todo
  case '$': return; // todo
  case '!': return; // todo
  case '%': return; // todo
  case 'l': SetLEDOff(); return;
  case 'L': SetLEDOn(); return;
  case 'A': Acknowledge(); return;
  case 'e': LogicPanel.Disable(); return;
  case 'E': LogicPanel.Enable(); return;

  default: return;
  }
}

int CommandClass::Parse(byte& offset, byte length)
{
  int value = 0;
  bool neg = false;
  bool digit = false;

  while (offset < length)
  {
    char c = buffer[offset];
    offset++;

    if (!digit)
    {
      if (c == ' ')
      {
        // eat whitespace
        continue;
      }
      else if (c == '-')
      {
        neg = true;
        continue;
      }
    }

    if (c < '0' || c > '9')
      break;

    digit = true;

    value = (value * 10) + c;
  }

  return neg ? -value : value;
}