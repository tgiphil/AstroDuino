// AstroDuinio Project. Copyright (c) Licensed under GNU General Public License, version 3.0
// Source: https://github.com/tgiphil/AstroDuino

#ifndef _LOGICPANELCONTROL_h
#define _LOGICPANELCONTROL_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include "LogicDisplay.h"

#define DEFAULT_REFRESH_RATE 20
#define FRONT_LOGIC_PANEL_PIN 6
#define REAR_LOGIC_PANEL_PIN 7

class LogicPanelControlClass
{
public:
	void Setup();
	void Update();

	void Enable();
	void Disable();
	void Enable(byte panel);
	void Disable(byte panel);
	void SetRefreshRate(int milli);
	void SetRefreshRate(byte panel, int milli);
	void SetSequenceLength(byte panel, byte len);
	void UpdateColorSequence(byte panel, byte seq, byte index, byte value);
	void SetEvent(byte x, char c, byte y);
	void SetDefaultSequence();

protected:
	unsigned long LastTick;
	unsigned int RefreshRate;
	bool Enabled;

	LogicDisplayClass<24, FRONT_LOGIC_PANEL_PIN> FrontPanel;
	LogicDisplayClass<90, REAR_LOGIC_PANEL_PIN> RearPanel;
};

extern LogicPanelControlClass LogicPanelControl;

#endif
