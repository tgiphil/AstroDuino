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
#define LED_TYPE WS2812B 	// other: SK6812 & WS2812B

template <byte LED_COUNT, byte PIN>
class LogicDisplayClass
{
public:
	void Setup()
	{
		RefreshRate = 20;
		LastTick = 0;
		SequenceLength = MAX_COLOR_SEQUENCE;
		SetEvent(3);
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

		LastTick = Ticks.Now;

		Update();
	}

	void Update()
	{
		unsigned long now = Ticks.Now;
		int delta = now - LastTick;

		// don't update too fast
		if (delta < RefreshRate)
			return;

		EventDispatch();

		LastTick = now;
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
		if (led > LED_COUNT || led < 0 || index > LED_COUNT || index < 0)
		{
			Comm.Output("## ERROR: ");
			Comm.Output(index);
			Comm.Output(' ');
			Comm.OutputLine(led);
		}

		Map[index] = led;
	}

	void SetMapDimensions(byte h, byte w)
	{
		MapHeight = h;
		MapWidth = w;
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
	byte MapHeight;
	byte MapWidth;
	byte SequenceLength;
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
		case 2: EventImperialMarch(); break;
		case 3: EventFire(); break;
		default: EventNormal(); break;
		}
	}

	void EventDisabled()
	{
		for (int i = 0; i < LED_COUNT; i++)
		{
			LEDs[i].setHSV(0, 0, 0);
		}
	}

	void EventNormal()
	{
		unsigned long delta = Ticks.Now - LastTick;

		byte maxsequence = SequenceLength << 1;

		for (int i = 0; i < LED_COUNT; i++)
		{
			byte time = LEDTimer[i] * 3;

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
				LEDTimer[i] = (time - delta) / 3;
			}
		}
	}

	inline byte GetPositionByXY(byte x, byte y)
	{
		return x * MapHeight + y;
	}

	void EventImperialMarch()
	{
		// constants
		const int FadeTime = 600;
		const int EventEnd = 47000;

		if (EventStart == 0)
		{
			EventStart = Ticks.Now;
			EventTimer = 0;
		}

		int elapsed = Ticks.Now - EventTimer;

		if (elapsed >= FadeTime || EventTimer == 0)
		{
			fill_solid(LEDs, LED_COUNT, CRGB::Red);
			EventTimer = Ticks.Now;
		}
		else
		{
			int fadeby = (elapsed * 255) / FadeTime;
			fadeToBlackBy(LEDs, LED_COUNT, fadeby);
		}

		if (Ticks.Now - EventStart >= EventEnd)
		{
			SetEvent(1);
		}
	}

#define MAXWIDTH 20

	CRGBPalette16 gPal;

	void EventFire()
	{
		// constants
		const int Hot = 180;
		const int MaxHot = Hot*MapHeight;
		const int Cooling = 55;

		//Comm.Output(".");

		static unsigned int spark[MAXWIDTH]; // base heat
		CRGB stack[MapWidth][MapHeight];  // stacks that are cooler

		random16_add_entropy(random16());

		if (EventStart == 0)
		{
			EventStart = Ticks.Now;
			EventTimer = 0;

			gPal = HeatColors_p;

			// clear
			for (int i = 0; i < LED_COUNT; i++)
			{
				LEDs[i].setHSV(i*LED_COUNT / 255, i*LED_COUNT / 255, i*LED_COUNT / 255);
			}
		}

		// 1. Generate sparks to re-heat
		for (int i = 0; i < MapWidth; i++)
		{
			if (spark[i] < Hot)
			{
				int base = Hot * 2;
				uint16_t value = random16(base, MaxHot);
				spark[i] = value;
/*
				Comm.Output("Spark: ");
				Comm.Output(i);
				Comm.Output('=');
				Comm.Output((int)value);
				Comm.OutputLine();*/
			}

		}

		// 2. Cool all the sparks
		for (int i = 0; i < MapWidth; i++)
		{
			spark[i] = qsub8(spark[i], random8(0, Cooling));
		}

		// 3. Build the stack
		/*    This works on the idea that pixels are "cooler" as they get further from the spark at the bottom */

		for (int i = 0; i < MapWidth; i++)
		{
			unsigned int heat = constrain(spark[i], Hot / 2, MaxHot);

			for (int j = MapHeight - 1; j >= 0; j--) {

				/* Calculate the color on the palette from how hot this	pixel is */
				byte index = constrain(heat, 0, Hot);

				stack[i][j] = ColorFromPalette(gPal, index);

				/* The next higher pixel will be "cooler", so calculate	the drop */
				unsigned int drop = random8(0, Hot);

				if (drop > heat)
					heat = 0; // avoid wrap-arounds from going "negative"
				else
					heat -= drop;

				heat = constrain(heat, 0, MaxHot);
			}
		}

		// 4. map stacks to led array
		for (int i = 0; i < MapWidth; i++)
		{
			for (int j = 0; j < MapHeight; j++)
			{
				int led = Map[GetPositionByXY(i, j)];
				LEDs[led] = stack[i][j];
			}
		}

		FastLED.show();
	}
};

#endif
