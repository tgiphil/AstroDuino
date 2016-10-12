// AstroDuinio Project. Copyright (c) Licensed under GNU General Public License, version 3.0
// Source: https://github.com/tgiphil/AstroDuino

#include "Ticks.h"

TicksClass Ticks;

void TicksClass::Setup()
{
	Start = millis();
}

void TicksClass::Update()
{
	Ticks++;

	Now = millis();

	unsigned long diff = Now - Start;

	if (diff >= SAMPLE_TICKS_MILLISECONDS)
	{
		TicksPerSecond = Ticks;
		Start = Now;
		Ticks = 0;

		Serial.println(TicksPerSecond);
	}
}
