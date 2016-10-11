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

#define MAX_COLOR_SEQUENCE 24

template <byte LED_COUNT, byte PIN>
class LogicDisplayClass
{
public:
	void Setup()
	{
		Brightness = 100;
		RefreshRate = 20;
		LastTick = 0;
		Enabled = true;
		Event = 0;
		EventSequenceStart = 0;
		SequenceLength = MAX_COLOR_SEQUENCE;
		Refresh();
	}

	void Refresh()
	{
		FastLED.setBrightness(Brightness);

		for (int i = 0; i < LED_COUNT; i++)
		{
			LEDSequence[i] = random(32);
			LEDTimer[i] = 0;

			LEDs[i].setHSV(random(100), random(100), random(100));
		}

		FastLED.addLeds<WS2812B, PIN, GRB>(LEDs, LED_COUNT);
		FastLED.show();

		LastTick = Ticks.Now;

		Update();
	}

	void Update()
	{
		if (!Enabled)
			return;

		unsigned long now = Ticks.Now;
		int delta = now - LastTick;

		// don't update too fast
		if (delta < RefreshRate)
			return;

		if (EventSequenceStart == 0)
			EventSequenceStart = now;

		LastTick = now;

		switch (Event)
		{
		case 0: EventNormal(delta); break;
		default: EventNormal(delta); break;
		}
	}

	void Enable()
	{
		Enabled = true;
	}

	void Disable()
	{
		FastLED.setBrightness(0);
		Enabled = false;
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

	void SetEvent(byte x, char c, byte y)
	{
		if (c == 'T')
		{
			Enable();
			Event = y;
			EventSequenceStart;
		}
		else if (c == 'D')
		{
			Disable();
		}
	}

protected:
	unsigned long LastTick;
	unsigned long EventSequenceStart;
	unsigned int RefreshRate;
	byte SequenceLength;
	bool Enabled;

	byte Colors[MAX_COLOR_SEQUENCE][5];
	byte Map[LED_COUNT];

	byte LEDSequence[LED_COUNT];
	byte LEDTimer[LED_COUNT];

	CRGB LEDs[LED_COUNT];

	byte Brightness;

	byte Event;

	void EventNormal(int delta)
	{
		delta = delta / 20;

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

				byte led = Map[i];
				LEDs[led].setHSV(Colors[iseq][0], Colors[iseq][1], Colors[iseq][2]);
			}
			else
			{
				LEDTimer[i] = time - delta;
			}
		}

		FastLED.setBrightness(Brightness);
		FastLED.show();
	}
};

#endif
