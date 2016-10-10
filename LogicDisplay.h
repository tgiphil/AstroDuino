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

template <byte LED_COUNT, byte PIN>
class LogicDisplayClass
{
public:
  void Setup()
  {
    Brightness = 100;
    RefreshRate = 50;
    LastTick = 0;
    Enabled = true;
    Event = 0;

    Refresh();
  }

  void Update()
  {
    FastLED.setBrightness(Brightness);

    for (int i = 0; i < LED_COUNT; i++)
    {
      LEDSequence[i] = random(32);
      LEDTimer[i] = 0;

      LEDs[i].setHSV(50, 50, 50);
    }

    //FastLED.addLeds<SK6812, FRONT_DATA_PIN, GRB>(FrontLEDs, LED_COUNT);
    FastLED.addLeds<WS2812B, PIN, GRB>(LEDs, LED_COUNT);
    FastLED.show();

    LastTick = Ticks.Now;

    Update();
  }

  void Refresh()
  {
    if (!Enabled)
      return;

    unsigned long now = Ticks.Now;
    int delta = now - LastTick;

    // don't update too fast
    if (delta < RefreshRate)
      return;

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

  void SetRefreshRate(int fps)
  {
    RefreshRate = fps;
  }

  void UpdateColorSequence(byte seq, byte index, byte value)
  {
    Colors[seq][index] = value;
  }

  void SetEvent(byte x, char c, byte y)
  {
    if (c == 'T')
    {
      Enable();
      Event = y;
    }
    else if (c == 'D')
    {
      Disable();
    }
  }

protected:
  unsigned long LastTick;
  unsigned int RefreshRate;
  bool Enabled;

  byte Colors[LED_COUNT][5];

  byte LEDSequence[LED_COUNT];
  byte LEDTimer[LED_COUNT];

  CRGB LEDs[LED_COUNT];

  byte Brightness;

  byte Event;

  void EventNormal(int delta)
  {
    for (int i = 0; i < LED_COUNT; i++)
    {
      byte time = LEDTimer[i];

      if (delta > time)
      {
        byte seq = LEDSequence[i];
        seq++;

        if (seq > 33) seq = 0;

        byte iseq = (seq <= 17) ? seq : 33 - seq;

        byte delayLow = Colors[iseq][4];
        byte delayHigh = Colors[iseq][5];

        byte delay = delayLow;
        byte range = delayHigh - delayLow;

        if (range != 0)
          delay = delay + random(range);

        LEDSequence[i] = seq;
        LEDTimer[i] = delay;

        LEDs[i].setHSV(Colors[iseq][0], Colors[iseq][1], Colors[iseq][2]);
      }
      else
      {
        LEDTimer[i] = time - (delta / 20);
      }
    }

    FastLED.setBrightness(Brightness);
    FastLED.show();
  }
};

#endif
