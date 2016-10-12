// AstroDuinio Project. Copyright (c) Licensed under GNU General Public License, version 3.0
// Source: https://github.com/tgiphil/AstroDuino

#ifndef _LOGICDISPLAY_h
#define _LOGICDISPLAY_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include <avr/pgmspace.h>
#include <FastLED.h>

#include "Comm.h"
#include "Ticks.h"

#define MAX_COLOR_SEQUENCE 20
#define LED_TYPE WS2812B	// other: SK6812

template <byte LED_COUNT, byte PIN>
class LogicDisplayClass
{
public:
	void Setup()
	{
		Brightness = 100;
		RefreshRate = 20;
		LastTick = 0;
		SequenceLength = MAX_COLOR_SEQUENCE;
		SetEvent(1);
		Refresh();
	}

	void Refresh()
	{
		for (int i = 0; i < LED_COUNT; i++)
		{
			LEDSequence[i] = random(32);
			LEDTimer[i] = 0;

			LEDs[i].setHSV(random(100), random(100), random(100));
		}

		FastLED.addLeds<LED_TYPE, PIN, GRB>(LEDs, LED_COUNT);
		FastLED.show();

		LastTick = Ticks.Now;

		Update();
	}

	void Update()
	{
		if (Event != 0)
			return;

		unsigned long now = Ticks.Now;
		int delta = now - LastTick;

		// don't update too fast
		if (delta < RefreshRate)
			return;

		LastTick = now;

		EventDispatch();
	}

	void Enable()
	{
		SetEvent(1);
	}

	void Disable()
	{
		SetEvent(0);
	}

	void SetRefreshRate(int milli)
	{
		RefreshRate = milli;
	}

	void SetSequenceLength(byte len)
	{
		SequenceLength = len;
	}

	void UpdateColorSequence(byte seq, byte index, byte value)
	{
		Colors[seq][index] = value;
	}

	void UpdateMap(byte index, byte led)
	{
		Map[index] = led;
	}

	void SetEvent(byte event)
	{
		Event = event;
		EventStart = 0;
		EventTimer = 0;
	}

	void SetEvent(byte x, char c, byte y)
	{
		if (c == 'T')
		{
			SetEvent(y);
		}
		else if (c == 'D')
		{
			SetEvent(1);
		}
	}

protected:
	unsigned long LastTick;
	unsigned long EventStart;
	unsigned long EventTimer;
	unsigned int RefreshRate;
	byte SequenceLength;
	byte Brightness;
	byte Event;

	byte Colors[MAX_COLOR_SEQUENCE][5];
	byte Map[LED_COUNT];

	byte LEDSequence[LED_COUNT];
	byte LEDTimer[LED_COUNT];

	CRGB LEDs[LED_COUNT];

	void EventDispatch()
	{
		switch (Event)
		{
		case 0: EventDisabled(); break;
		case 1: EventNormal(); break;
		default: EventNormal(); break;
		}
	}

	void EventDisabled()
	{
		for (int i = 0; i < LED_COUNT; i++)
		{
			LEDs[i].setHSV(0, 0, 0);
		}

		FastLED.show();
	}

	void EventNormal()
	{
		unsigned int delta = (Ticks.Now - LastTick) / 20;

		byte maxsequence = SequenceLength << 1;

		for (int i = 0; i < LED_COUNT; i++)
		{
			byte time = LEDTimer[i];

			if (delta > time)
			{
				byte seq = LEDSequence[i] + 1;

				if (seq > maxsequence) seq = 0;

				byte iseq = (seq < SequenceLength) ? seq : maxsequence - seq;

				byte delayLow = Colors[iseq][4];
				byte delayHigh = Colors[iseq][5];

				byte delay = delayLow;
				byte range = delayHigh - delayLow;

				if (range != 0)
					delay = delay + random(range);

				LEDSequence[i] = seq;
				LEDTimer[i] = delay;

				//byte led = Map[i];
				byte led = i;
				LEDs[led].setHSV(Colors[iseq][0], Colors[iseq][1], Colors[iseq][2]);
			}
			else
			{
				LEDTimer[i] = time - delta;
			}
		}

		FastLED.show();
	}

	void EventImperialMarch()
	{
		fadeToBlackBy(LEDs, LED_COUNT, 15);

		if (Ticks.Now - EventTimer >= 600) {
			fill_solid(LEDs, LED_COUNT, CRGB::Red);
			FastLED.show();
			EventTimer = Ticks.Now;
		}

		if (Ticks.Now - EventStart >= 47000)
		{
			SetEvent(0);
		}
	}
};

#endif
