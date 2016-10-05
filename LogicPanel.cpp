// AstroDuinio Project. Copyright (c) Licensed under GNU General Public License, version 3.0
// Source: https://github.com/tgiphil/AstroDuino

#include "LogicPanel.h"
#include "Comm.h"
#include "Ticks.h"

#include <avr/pgmspace.h>

LogicPanelClass LogicPanel;

const byte FrontColors[][4] = {
  {  87,  0,  0,  0 },
  {  87, 52, 26,255 },
  {  87,103, 53,255 },
  {  87,155, 79,255 },
  {  87,206,105,  0 },
  {  85,218,125,255 },
  {  83,231,145,255 },
  {  81,243,164,255 },
  {  79,255,184,  0 },
  {  64,255,201,255 },
  {  49,255,217,255 },
  {  33,255,234,255 },
  {  18,255,250,  0 },
  {  14,255,241,255 },
  {   9,255,232,255 },
  {   5,255,223,255 },
  {   0,255,214,  0 } };

const byte RearColors[][4] = {
  { 170,  0,  0,  0 },
  { 170, 64, 22,255 },
  { 170,128, 44,255 },
  { 170,191, 65,255 },
  { 170,255, 87,  0 },
  { 169,255,115,255 },
  { 168,255,144,255 },
  { 167,255,172,255 },
  { 166,255,200,  0 },
  { 163,212,188,255 },
  { 160,170,175,255 },
  { 157,127,163,255 },
  { 154, 84,150,  0 },
  { 159, 63,163,255 },
  { 164, 42,175,255 },
  { 169, 21,188,255 },
  { 174,  0,200,  0 } };

void LogicPanelClass::Setup()
{
  Brightness = 100;
  RefreshRate = 1000 / DEFAULT_REFRESH_RATE;
  LastTick = 0;
  Enabled = true;

  Refresh();
}

void LogicPanelClass::Refresh()
{
  FastLED.setBrightness(Brightness);

  for (int i = 0; i < FRONT_LED_COUNT; i++)
  {
    FrontLEDSequence[i] = random(MAX_COLOR_SEQUENCE);
    FrontLEDTimer[i] = random(50);

    FrontLEDs[i].setHSV(50, 50, 50);
  }

  for (int i = 0; i < REAR_LED_COUNT; i++)
  {
    RearLEDSequence[i] = random(MAX_COLOR_SEQUENCE);
    RearLEDTimer[i] = random(50);
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

  // don't update too last
  if (delta < RefreshRate)
    return;

  LastTick = now;

  for (int i = 0; i < FRONT_LED_COUNT; i++)
  {
    byte time = FrontLEDTimer[i];

    if (delta > time)
    {
      int seq = FrontLEDSequence[i];
      seq++;

      if (seq >= 32) seq = 0;

      byte iseq = (seq < 16) ? seq : 32 - seq;

      int delay = FrontColors[iseq][4];

      if (delay == 0)
        delay = random(50);
      else if (delay == 255)
        delay = 5;

      FrontLEDSequence[i] = seq;
      FrontLEDTimer[i] = delay;

      FrontLEDs[i].setHSV(FrontColors[iseq][0], FrontColors[iseq][1], FrontColors[iseq][2]);
    }
    else
    {
      FrontLEDTimer[i] = time - delta;
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

