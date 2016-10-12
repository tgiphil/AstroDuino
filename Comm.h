// AstroDuinio Project. Copyright (c) Licensed under GNU General Public License, version 3.0
// Source: https://github.com/tgiphil/AstroDuino

#ifndef _COMM_h
#define _COMM_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

class CommClass
{
public:
	void Setup();
	void Update();
	void WaitForConnection();

	void OutputLine(const String &s);
	void OutputLine(const char* s);
	void OutputLinePGM(const char* s);
	void OutputLine(int i);
	void OutputLine();
	void Output(const String &s);
	void Output(const char* s);
	void OutputPGM(const char* s);
	void Output(int i);
	void Output(char c);
	void DebugLine(const String &s);
	void DebugLine(const char* s);
	void DebugLine();
	void Debug(String &s);
	void Debug(char* s);
	void Debug(const char* s);
	void Debug(char c);

	bool DebugEnabled = false;

protected:

	void CheckSerial();
};

extern CommClass Comm;

#endif
