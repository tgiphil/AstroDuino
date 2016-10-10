// AstroDuinio Project. Copyright (c) Licensed under GNU General Public License, version 3.0
// Source: https://github.com/tgiphil/AstroDuino

#include "LogicPanelControl.h"

#include <avr/pgmspace.h>

LogicPanelControlClass LogicPanelControl;

const byte PanelFrontColors[][5] PROGMEM = {
  { 87,  0,  0,100,200 },
  { 87, 52, 26, 25, 25 },
  { 87,103, 53, 25, 25 },
  { 87,155, 79, 25, 25 },

  { 87,206,105,100,200 },
  { 85,218,125, 25, 25 },
  { 83,231,145, 25, 25 },
  { 81,243,164, 25, 25 },

  { 79,255,184,100,200 },
  { 64,255,201, 25, 25 },
  { 49,255,217, 25, 25 },
  { 33,255,234, 25, 25 },

  { 18,255,250,100,200 },
  { 14,255,241, 25, 25 },
  { 9,255,232, 25, 25 },
  { 5,255,223, 25, 25 },

  { 0,255,214,200, 200 } };

const byte PanelRearColors[][5] PROGMEM = {
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

void LogicPanelControlClass::Setup()
{
  SetRefreshRate(100);

  FrontPanel.Setup();
  RearPanel.Setup();

  FrontPanel.SetRefreshRate(1000 / DEFAULT_REFRESH_RATE);
  RearPanel.SetRefreshRate(1000 / DEFAULT_REFRESH_RATE);

  // todo --- send default colors to each panel
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

  FrontPanel.Refresh();
  RearPanel.Refresh();

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

void LogicPanelControlClass::SetRefreshRate(int fps)
{
  RefreshRate = fps;
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

void LogicPanelControlClass::SetRefreshRate(byte panel, int fps)
{
  if (panel == 0)
  {
    FrontPanel.SetRefreshRate(fps);
  }
  else if (panel == 1)
  {
    RearPanel.SetRefreshRate(fps);
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