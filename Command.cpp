// AstroDuinio Project. Copyright (c) Licensed under GNU General Public License, version 3.0
// Source: https://github.com/tgiphil/AstroDuino

#include "Command.h"
#include "Global.h"
#include "LogicPanel.h"
#include "Comm.h"

CommandClass Command;

void CommandClass::Setup()
{
  Length = 0;
  Discards = 0;
}

void CommandClass::Update()
{
}

void CommandClass::Add(char c)
{
  if (Length >= COMMAND_BUFFER_SIZE)
  {
    // buffer full, discard character
    Discards++;

    if (Discards >= COMMAND_DISCARDS_FOR_BUFFER_RESET)
    {
      // too many discarded characters, reset buffer
      Length = 0;
      Discards = 0;
    }

    return;
  }

  buffer[Length++] = c;

  if (c == COMMAND_EOL)
  {
    Parse();

    Length = 0;
  }
}

void CommandClass::Parse()
{
  if (Length == 0)
    return;

  ParseOffset = 0;

  char l = GetChar();

  switch (l)
  {
  case ':': return; // todo
  case '*': return; // todo
  case '@': ParseLogicPanel(); return;
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

char CommandClass::PeekChar()
{
  if (ParseOffset < Length)
    return '\0';

  return buffer[ParseOffset];
}

char CommandClass::GetChar()
{
  if (ParseOffset < Length)
    return '\0';

  char c = buffer[ParseOffset];
  ParseOffset++;
  return c;
}

int CommandClass::GetInteger(byte maxlen)
{
  int value = 0;
  bool neg = false;
  bool digit = false;
  int len = 0;

  while (ParseOffset < Length && (len < maxlen || maxlen == 0))
  {
    char c = GetChar();
    len++;

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
    {
      ParseOffset--; // back up on character
      break;
    }

    digit = true;

    value = (value * 10) + c;
  }

  return neg ? -value : value;
}

void CommandClass::ParseLogicPanel()
{
  // JEDI Display Command format : @xxTyyy\r

  byte x = GetInteger(2);
  char code = GetChar();
  byte y = GetInteger(3);

  if (code == 'T')
  {
    LogicPanel.SetEvent(x, y);
  }
}
