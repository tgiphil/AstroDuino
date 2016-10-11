// AstroDuinio Project. Copyright (c) Licensed under GNU General Public License, version 3.0
// Source: https://github.com/tgiphil/AstroDuino

#include "LogicPanelControl.h"

#include <avr/pgmspace.h>

LogicPanelControlClass LogicPanelControl;

const byte PanelFrontColors[][5] PROGMEM = {
  { 87,  0,  0, 50, 50 },
  { 87, 52, 26, 15, 15 },
  { 87,103, 53, 15, 15 },
  { 87,155, 79, 15, 15 },

  { 87,206,105, 50, 50 },
  { 85,218,125, 15, 15 },
  { 83,231,145, 15, 15 },
  { 81,243,164, 15, 15 },

  { 79,255,184, 50, 50 },
  { 59,255,201, 15, 15 },
  { 40,255,217, 15, 15 },
  { 20,255,234, 15, 15 },

  {  0,255,250, 50, 50 } };

const byte PanelRearColors[][5] PROGMEM = {
  { 170,255,  0, 50, 50 },
  { 170,255, 34, 15, 15 },
  { 170,255, 68, 15, 15 },
  { 170,255,102, 15, 15 },

  { 170,255,136, 50, 50 },
  { 170,255,166, 15, 15 },
  { 170,255,196, 15, 15 },
  { 170,255,225, 15, 15 },

  { 170,255,255, 50, 50 },
  { 170,191,255, 15, 15 },
  { 170,128,255, 15, 15 },
  { 170, 64,255, 15, 15 },

  { 170,  0,255, 50, 50 } };

void LogicPanelControlClass::Setup()
{
	Enabled = true;
	SetRefreshRate(20);

	SetDefaultSequence();

	FrontPanel.Setup();
	RearPanel.Setup();

	FrontPanel.SetRefreshRate(1000 / DEFAULT_REFRESH_RATE);
	RearPanel.SetRefreshRate(1000 / DEFAULT_REFRESH_RATE);
}

void LogicPanelControlClass::SetDefaultSequence()
{
	FrontPanel.SetSequenceLength(13);
	for (int s = 0; s < 13; s++)
	{
		for (int i = 0; i < 6; i++)
		{
			byte front = pgm_read_byte(&PanelFrontColors[s][i]);
			FrontPanel.UpdateColorSequence(s, i, front);
		}
	}

	RearPanel.SetSequenceLength(13);
	for (int s = 0; s < 13; s++)
	{
		for (int i = 0; i < 6; i++)
		{
			byte rear = pgm_read_byte(&PanelRearColors[s][i]);
			RearPanel.UpdateColorSequence(s, i, rear);
		}
	}
}

void LogicPanelControlClass::Update()
{
	if (!Enabled)
		return;

	unsigned long now = Ticks.Now;
	int delta = now - LastTick;

	// don't update too fast
	if (delta < RefreshRate)
		return;

	LastTick = now;

	FrontPanel.Update();
	RearPanel.Update();

	//FastLED.show();
}

void LogicPanelControlClass::Enable()
{
	Enabled = true;
}

void LogicPanelControlClass::Disable()
{
	FastLED.setBrightness(0);
	FrontPanel.Disable();
	RearPanel.Disable();
	Enabled = false;
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

void LogicPanelControlClass::SetEvent(byte x, char c, byte y)
{
	if (c == 'T')
	{
		FrontPanel.SetEvent(x, c, y);
		RearPanel.SetEvent(x, c, y);

		Enable();
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