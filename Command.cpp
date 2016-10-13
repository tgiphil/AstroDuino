// AstroDuinio Project. Copyright (c) Licensed under GNU General Public License, version 3.0
// Source: https://github.com/tgiphil/AstroDuino

#include "Command.h"
#include "Global.h"
#include "LogicPanelControl.h"
#include "Comm.h"

CommandClass Command;

const char OK[] PROGMEM = "OK";
const char ERROR[] PROGMEM = "**Invalid Panel Command";

void CommandClass::Setup()
{
	Length = 0;
	Discards = 0;
	DefaultCommandType = 0;
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

	if (c == '\r')
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
	case ':':ok = ParseControllerCommand(); break;
	case '*': break;
	case '@': ok = ParseLogicPanelCommand(); break;
	case '$': break;
	case '!': break;
	case '%': break;
	case 'l': ok = true; SetLEDOff(); break;
	case 'L': ok = true; SetLEDOn(); break;
	case 'A': ok = true; Acknowledge(); break;

	case '&': ok = ParseCustom(); break;
	case '#': ok = true; break; // ignore

	default:
	{
		switch (DefaultCommandType)
		{
		case 0: ok = ParseLogicPanelCommand(); break;
		default: break;
		}

		break;
	}
	}

	if (ok)
		Comm.OutputLinePGM(OK);
	else
		Comm.OutputLinePGM(ERROR);
}

char CommandClass::PeekChar()
{
	if (ParseOffset >= Length)
		return '\0';

	return buffer[ParseOffset];
}

char CommandClass::GetChar()
{
	if (ParseOffset >= Length)
		return '\0';

	char c = buffer[ParseOffset];
	ParseOffset++;
	return c;
}

void CommandClass::EatWhiteSpace()
{
	while (ParseOffset < Length)
	{
		char c = PeekChar();

		if (c == ' ' || c == '\t')
		{
			GetChar();
			continue;
		}

		return;
	}
}

int CommandClass::GetInteger(byte maxlen)
{
	int value = 0;
	bool neg = false;
	bool digit = false;
	byte len = 0;

	while (ParseOffset < Length && (len < maxlen || maxlen == 0))
	{
		char c = PeekChar();
		len++;

		if (!digit && c == '-')
		{
			GetChar();
			neg = true;
			continue;
		}

		if (c < '0' || c > '9')
			break;

		GetChar();
		digit = true;

		value = (value * 10) + (c - '0');
	}

	return neg ? -value : value;
}

bool CommandClass::ParseLogicPanelCommand()
{
	// JEDI Display Command format : @xxTyyy\r

	byte x = GetInteger(2);
	char code = GetChar();
	byte y = GetInteger(3);

	Comm.Output(x);
	Comm.Output('-');
	Comm.Output(code);
	Comm.Output('-');
	Comm.Output(y);
	Comm.OutputLine();

	LogicPanelControl.SetEvent(x, code, y);

	return true;
}

bool CommandClass::ParseControllerCommand()
{
	// :CCxx

	char code1 = GetChar();
	char code2 = GetChar();
	byte y = GetInteger(3);

	Comm.Debug(code1);
	Comm.Debug(' ');
	Comm.Debug(code2);
	Comm.Debug(' ');
	Comm.Debug(y);
	Comm.DebugLine();

	return true;
}

bool CommandClass::ParseCustom()
{
	// Format &Caaa xxx yyy zzz

	char c = GetChar();
	EatWhiteSpace();
	byte a = GetInteger();
	EatWhiteSpace();
	byte x = GetInteger();
	EatWhiteSpace();
	byte y = GetInteger();
	EatWhiteSpace();
	byte z = GetInteger();

	if (c == '\0')
		return true;

	if (c == 'L')
	{
		switch (a)
		{
		case 11: LogicPanelControl.SetDefaultSequence(); return true;
		case 12: LogicPanelControl.SetRefreshRate(x); return true;
		case 101: LogicPanelControl.UpdateColorSequence(0, x, y, z); return true;
		case 102: LogicPanelControl.UpdateColorSequence(1, x, y, z); return true;
		case 111: LogicPanelControl.SetSequenceLength(0, x); return true;
		case 112: LogicPanelControl.SetSequenceLength(1, x); return true;
		case 121: LogicPanelControl.Enable(1); return true;
		case 122: LogicPanelControl.Enable(1); return true;
		case 131: LogicPanelControl.Disable(1); return true;
		case 132: LogicPanelControl.Disable(1); return true;
		default: return false;
		}
	}

	return false;
}
