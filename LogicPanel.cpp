// AstroDuinio Project. Copyright (c) Licensed under GNU General Public License, version 3.0
// Source: https://github.com/tgiphil/AstroDuino

#include "LogicPanel.h"
#include "Comm.h"
#include "Ticks.h"

#include <avr/pgmspace.h>

// Reference
// https://github.com/joymonkey/logicengine

/*
Events:
* 0 : Normal: 0T1
* 1 : Flash : 0T2
* 2 : Alarm : 0T3
* 3 : ShrtCt: 0T4
* 4 : Scream: 0T5
* 5 : Leia  : 0T6
* 6 : StrWrs: 0T10
* 7 : March : 0T11
* 8 : Cntina: 0T92
* 9 : TEST  : 0T0
*/

LogicPanelClass LogicPanel;

byte FrontColors[][5] = {
  {  87,  0,  0,100,200 },
  {  87, 52, 26, 25, 25 },
  {  87,103, 53, 25, 25 },
  {  87,155, 79, 25, 25 },

  {  87,206,105,100,200 },
  {  85,218,125, 25, 25 },
  {  83,231,145, 25, 25 },
  {  81,243,164, 25, 25 },

  {  79,255,184,100,200 },
  {  64,255,201, 25, 25 },
  {  49,255,217, 25, 25 },
  {  33,255,234, 25, 25 },

  {  18,255,250,100,200 },
  {  14,255,241, 25, 25 },
  {   9,255,232, 25, 25 },
  {   5,255,223, 25, 25 },

  {   0,255,214,200, 200} };

byte RearColors[][5] = {
  { 170,  0,  0,100,200 },
  { 170, 64, 22, 25, 25 },
  { 170,128, 44, 25, 25 },
  { 170,191, 65, 25, 25 },

  { 170,255, 87,100,200 },
  { 169,255,115, 25, 25 },
  { 168,255,144, 25, 25 },
  { 167,255,172, 25, 25 },

  { 166,255,100,200,200 },
  { 163,212,188, 25, 25 },
  { 160,170,175, 25, 25 },
  { 157,127,163, 25, 25 },

  { 154, 84,150,100,200 },
  { 159, 63,163, 25, 25 },
  { 164, 42,175, 25, 25 },
  { 169, 21,188, 25, 25 },

  { 174,  0,100,200,200 } };

void LogicPanelClass::Setup()
{
  Brightness = 100;
  RefreshRate = 1000 / DEFAULT_REFRESH_RATE;
  LastTick = 0;
  Enabled = true;
  Event = 0;

  Refresh();
}

void LogicPanelClass::Refresh()
{
  FastLED.setBrightness(Brightness);

  for (int i = 0; i < FRONT_LED_COUNT; i++)
  {
    FrontLEDSequence[i] = random(32);
    FrontLEDTimer[i] = 0;

    FrontLEDs[i].setHSV(50, 50, 50);
  }

  for (int i = 0; i < REAR_LED_COUNT; i++)
  {
    RearLEDSequence[i] = random(MAX_COLOR_SEQUENCE);
    RearLEDTimer[i] = 0;
  }

  //FastLED.addLeds<SK6812, FRONT_DATA_PIN, GRB>(FrontLEDs, FRONT_LED_COUNT);
  FastLED.addLeds<WS2812B, FRONT_DATA_PIN, GRB>(FrontLEDs, FRONT_LED_COUNT);
  FastLED.show();

  LastTick = Ticks.Now;

  Update();
}

void LogicPanelClass::Update()
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

void LogicPanelClass::EventNormal(int delta)
{
  for (int i = 0; i < FRONT_LED_COUNT; i++)
  {
    byte time = FrontLEDTimer[i];

    if (delta > time)
    {
      byte seq = FrontLEDSequence[i];
      seq++;

      if (seq > 33) seq = 0;

      byte iseq = (seq <= 17) ? seq : 33 - seq;

      byte delayLow = FrontColors[iseq][4];
      byte delayHigh = FrontColors[iseq][5];

      byte delay = delayLow;
      byte range = delayHigh - delayLow;

      if (range != 0)
        delay = delay + random(range);

      FrontLEDSequence[i] = seq;
      FrontLEDTimer[i] = delay;

      FrontLEDs[i].setHSV(FrontColors[iseq][0], FrontColors[iseq][1], FrontColors[iseq][2]);
    }
    else
    {
      FrontLEDTimer[i] = time - (delta / 20);
    }
  }

  FastLED.show();
}

void LogicPanelClass::Enable()
{
  Enabled = true;
}

void LogicPanelClass::Disable()
{
  Enabled = false;
}

void LogicPanelClass::SetRefreshRate(int framespersecond)
{
  RefreshRate = framespersecond;
}

void LogicPanelClass::UpdateMap(byte panel, byte seq, byte index, byte value)
{
  if (panel == 0)
    FrontColors[seq][index] = value;
  else if (panel == 1)
    RearColors[seq][index] = value;
}

void LogicPanelClass::SetEvent(byte event)
{
  Event = event;
}
