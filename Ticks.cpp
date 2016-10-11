// AstroDuinio Project. Copyright (c) Licensed under GNU General Public License, version 3.0
// Source: https://github.com/tgiphil/AstroDuino

#include "Ticks.h"

TicksClass Ticks;

void TicksClass::Setup()
{
	start = millis();
}

void TicksClass::Update()
{
	ticks++;

	Now = millis();

	unsigned long diff = Now - start;

	if (diff >= SAMPLE_TICKS_MILLISECONDS)
	{
		ticksPerSecond = ticks;
		start = Now;
		ticks = 0;

		Serial.println(ticksPerSecond);
	}
}
