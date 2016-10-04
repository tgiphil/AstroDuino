// AstroDuinio Project. Copyright (c) Licensed under the New BSD License.
// Source: https://github.com/tgiphil/AstroDuino

#include "Command.h"
#include "Global.h"
#include "LogicPanel.h"

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
