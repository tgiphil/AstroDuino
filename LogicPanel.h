#ifndef _LOGICPANEL_h
#define _LOGICPANEL_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include <FastLED.h>

#define FRONT_DATA_PIN 6
#define REAR_DATA_PIN 7

#define MAX_COLOR_SEQUENCE 32
#define FRONT_LED_COUNT 24 // 80
#define REAR_LED_COUNT 96
#define DEFAULT_REFRESH_RATE 8

class LogicPanelClass
{
public:
  void Setup();
  void Update();
  void Refresh();

  void Enable();
  void Disable();
  void SetRefreshRate(int framespersecond);
  void UpdateMap(byte panel, byte seq, byte index, byte value);

protected:
  unsigned long LastTick;
  unsigned int RefreshRate;
  bool Enabled;

  int FrontLEDCount = FRONT_LED_COUNT;
  int RearLEDCount = REAR_LED_COUNT;

  byte FrontLEDSequence[FRONT_LED_COUNT];
  byte RearLEDSequence[REAR_LED_COUNT];
  byte FrontLEDTimer[FRONT_LED_COUNT];
  byte RearLEDTimer[REAR_LED_COUNT];

  CRGB FrontLEDs[FRONT_LED_COUNT];
  CRGB RearLEDs[REAR_LED_COUNT];

  byte Brightness;
};

extern LogicPanelClass LogicPanel;

#endif

