// AstroDuinio Project. Copyright (c) Licensed under GNU General Public License, version 3.0
// Source: https://github.com/tgiphil/AstroDuino

#include "Command.h"
#include "Global.h"
#include "LogicPanel.h"
#include "Comm.h"

CommandClass Command;

const char OK[] PROGMEM = "OK";
const char ERROR[] PROGMEM = "**Invalid Panel Command";

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
  {
    Comm.OutputLinePGM(OK);
    return;
  }

  ParseOffset = 0;

  char l = GetChar();
  bool ok = false;

  switch (l)
  {
  case ':': break; // todo
  case '*': break; // todo
  case '@': ok = ParseLogicPanel(); break;
  case '$': break; // todo
  case '!': break; // todo
  case '%': break; // todo
  case 'l': ok = true; SetLEDOff(); break;
  case 'L': ok = true; SetLEDOn(); break;
  case 'A': ok = true; Acknowledge(); break;
  case 'e': ok = true; LogicPanel.Disable(); break;
  case 'E': ok = true; LogicPanel.Enable(); break;

  default: return;
  }

  if (ok)
    Comm.OutputLinePGM(OK);
  else
    Comm.OutputLinePGM(ERROR);

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

bool CommandClass::ParseLogicPanel()
{
  // JEDI Display Command format : @xxTyyy\r

  byte x = GetInteger(2);
  char code = GetChar();
  byte y = GetInteger(3);

  LogicPanel.SetEvent(x, code, y);

  return true;
}
