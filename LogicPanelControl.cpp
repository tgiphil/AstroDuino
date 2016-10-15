// AstroDuinio Project. Copyright (c) Licensed under GNU General Public License, version 3.0
// Source: https://github.com/tgiphil/AstroDuino

#include "LogicPanelControl.h"

#include <avr/pgmspace.h>

LogicPanelControlClass LogicPanelControl;

const byte PanelFrontColors[][5] PROGMEM = {
  { 87,  0,  0, 15, 15 },
  { 87, 52, 26, 15, 15 },
  { 87,103, 53, 15, 15 },
  { 87,155, 79, 15, 15 },

  { 87,206,105, 15, 15 },
  { 85,218,125, 15, 15 },
  { 83,231,145, 15, 15 },
  { 81,243,164, 15, 15 },

  { 79,255,184, 15, 15 },
  { 59,255,201, 15, 15 },
  { 40,255,217, 15, 15 },
  { 20,255,234, 15, 15 },

  {  0,255,250, 15, 15 } };

const byte PanelRearColors[][5] PROGMEM = {
  { 170,255,  0, 15, 15 },
  { 170,255, 34, 15, 15 },
  { 170,255, 68, 15, 15 },
  { 170,255,102, 15, 15 },

  { 170,255,136, 15, 15 },
  { 170,255,166, 15, 15 },
  { 170,255,196, 15, 15 },
  { 170,255,225, 15, 15 },

  { 170,255,255, 15, 15 },
  { 170,191,255, 15, 15 },
  { 170,128,255, 15, 15 },
  { 170, 64,255, 15, 15 },

  { 170,  0,255, 15, 15 } };

const byte FrontLedMapV1[] PROGMEM = {
	0,  1, 2, 3, 4, 5, 6, 7,
	15,14,13,12,11,10, 9, 8,
	16,17,18,19,20,21,22,23,
	31,30,29,28,27,26,25,24,
	32,33,34,35,36,37,38,39,
	79,78,77,76,75,74,73,72,
	64,65,66,67,68,69,70,71,
	63,62,61,60,59,58,57,56,
	48,49,50,51,52,53,54,55,
	47,46,45,44,43,42,41,40 };

const byte RearLedMapV1[] PROGMEM = {
	0,  1,   2, 28,  3, 27,  4, 26,  5, 25,  6,  7,  8, 9,  22, 10, 21, 11, 20, 12, 19, 13, 14, 15,
	31, 30, 29, 32, 33, 34, 35, 36, 37, 38, 39, 24, 23, 40, 41, 42, 43, 44, 45, 46, 47, 18, 17, 16,
	64, 65, 66, 63, 62, 61, 60, 59, 58, 57, 56, 71, 72, 55, 54, 53, 52, 51, 50, 49, 48, 77, 78, 79,
	95, 94, 93, 67, 92, 68, 91, 69, 90, 70, 89, 88, 87, 86, 73, 85, 74, 84, 75, 83, 76, 82, 81, 80 };

void LogicPanelControlClass::Setup()
{
	SetRefreshRate(20);
	SetBrightness(50);
	SetDefaultSequence();

	FrontPanel.Setup();
	RearPanel.Setup();

	SetDefaultMapV1();

	FrontPanel.SetRefreshRate(DEFAULT_REFRESH_RATE);
	RearPanel.SetRefreshRate(DEFAULT_REFRESH_RATE);

	FrontPanel.SetMapDimensions(8, 10);
	RearPanel.SetMapDimensions(4, 26);
}

void LogicPanelControlClass::SetDefaultSequence()
{
	FrontPanel.SetSequenceLength(13);
	for (byte s = 0; s < 13; s++)
	{
		for (byte i = 0; i < 6; i++)
		{
			byte front = pgm_read_byte(&PanelFrontColors[s][i]);
			FrontPanel.UpdateColorSequence(s, i, front);
		}
	}

	RearPanel.SetSequenceLength(13);
	for (byte s = 0; s < 13; s++)
	{
		for (byte i = 0; i < 6; i++)
		{
			byte rear = pgm_read_byte(&PanelRearColors[s][i]);
			RearPanel.UpdateColorSequence(s, i, rear);
		}
	}
}

void LogicPanelControlClass::SetDefaultMapV1()
{
	for (byte i = 0; i < FRONT_LOGIC_PANEL_LEDS_COUNT; i++)
	{
		byte front = pgm_read_byte(&FrontLedMapV1[i]);
		FrontPanel.UpdateMap(i, front);
	}

	for (byte i = 0; i < REAR_LOGIC_PANEL_LEDS_COUNT; i++)
	{
		byte rear = pgm_read_byte(&RearLedMapV1[i]);
		RearPanel.UpdateMap(i, rear);
	}
}

void LogicPanelControlClass::Update()
{
	unsigned long now = Ticks.Now;
	int delta = now - LastTick;

	// don't update too fast
	if (delta < RefreshRate)
		return;

	LastTick = now;

	//FrontPanel.Update();
	RearPanel.Update();

	FastLED.setBrightness(Brightness);
	FastLED.show();
}

void LogicPanelControlClass::SetRefreshRate(int milli)
{
	RefreshRate = milli;
}

void LogicPanelControlClass::UpdateColorSequence(byte panel, byte seq, byte index, byte value)
{
	if (panel == 0)
	{
		FrontPanel.UpdateColorSequence(seq, index, value);
	}
	else if (panel == 1)
	{
		RearPanel.UpdateColorSequence(seq, index, value);
	}
}

void LogicPanelControlClass::SetSequenceLength(byte panel, byte len)
{
	if (panel == 0)
	{
		FrontPanel.SetSequenceLength(len);
	}
	else if (panel == 1)
	{
		RearPanel.SetSequenceLength(len);
	}
}

void LogicPanelControlClass::UpdateMap(byte panel, byte index, byte led)
{
	if (panel == 0)
	{
		FrontPanel.UpdateMap(index, led);
	}
	else if (panel == 1)
	{
		RearPanel.UpdateMap(index, led);
	}
}

void LogicPanelControlClass::SetEvent(byte x, char c, byte y)
{
	if (c == 'T')
	{
		FrontPanel.SetEvent(x, c, y);
		RearPanel.SetEvent(x, c, y);
	}
	else if (c == 'D')
	{
		FrontPanel.Disable();
		RearPanel.Disable();
	}
}

void LogicPanelControlClass::SetRefreshRate(byte panel, int milli)
{
	if (panel == 0)
	{
		FrontPanel.SetRefreshRate(milli);
	}
	else if (panel == 1)
	{
		RearPanel.SetRefreshRate(milli);
	}
}

void LogicPanelControlClass::Enable(byte panel)
{
	if (panel == 0)
	{
		FrontPanel.Enable();
	}
	else if (panel == 1)
	{
		RearPanel.Enable();
	}
}

void LogicPanelControlClass::Disable(byte panel)
{
	if (panel == 0)
	{
		FrontPanel.Disable();
	}
	else if (panel == 1)
	{
		RearPanel.Disable();
	}
}

void LogicPanelControlClass::SetBrightness(byte brightness)
{
	Brightness = brightness;
}