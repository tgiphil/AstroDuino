// AstroDuinio Project. Copyright (c) Licensed under the New BSD License.
// Source: https://github.com/tgiphil/AstroDuino

// Global.h

#ifndef _GLOBAL_h
#define _GLOBAL_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

void SetLEDOff();
void SetLEDOn();
void Acknowledge();

#endif

