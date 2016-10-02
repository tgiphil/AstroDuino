#ifndef _LOGICENGINE_h
#define _LOGICENGINE_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

/*
Logic Engine 2016 base Sketch
=============================

This is designed to run on Teensy 3.1 or Arduino UNO (or any 328P board, like the Logic Engine AVR board).
Settings at the start of the sketch may need to be adjusted depending on your setup.

How Key, Tween and Bizarro Colors work:
Say we have 4 KEY colors, with 4 tweens between each one. So 16 color total (48 bytes)
K              K              K              K
00,01,02,03,04,05,06,07,08,09,10,11,12,13,14,15
In the standard logic patterns, each pixel cycles up through all these colors, pausing a little longer on the Key colors.
When the pixel reaches the top, it reverses back through the same colors.
To prevent us from having to store the colors twice or storing the "direction" of each pixel within the color palette (both wasting precious SRAM), we just pretend that we have 31 colors.
The colors from 16 to 31 are a mirror image of the colors from 01 to 15, so we refer to them as "Bizarro".
We use a small helper function to check if a color is bizarro, and then treat it like its real color equivalent.
For more key/tween/bizarro info see http://rseries.net/logic2/color/?keys=4&tweens=4

The Key colors are defined in advance. The tweens are calculated at setup() and all colors are then stored in SRAM.
The LED status structs are also sored in SRAM.
LED-to-pixel mapping is stored in flash.

*/
//////////////////////////////////////////////////////////////////////////////////////////////////////


#define CONTROLLER 1 // 0=UNO or 1=TEENSY

#define LOGICTYPE 0  // 0=BOTH , 1=FRONT , 2=REAR . BOTH may run slowly on an UNO, and consume too much SRAM to run stabily.

#define PCBVERSION 2 //what kind of LED PCBs are they? 0 = Originals (with Naboo logo on backs of Front and Rear Logic)
//                                1 = 2014 Version (with Kenny & McQuarry art on Rear, C3PO on Fronts)
//                                2 = 2016 Version (with Deathstar plans on back of Rear Logic)

#define MAX_BRIGHTNESS 200  //can go up to 255, but why? this limit keeps current and heat down, and not noticeably dimmer than 255

#define DEBUG 1 //set to 1 to print some stuff to serial, do not leave this on for general use (will affect speed and consume SRAM)


//////////////////////////////////////////////////////////////////////////////////////////////////////
// The average user shouldn't need to adjust anything below here

#if (CONTROLLER==0)
#define KEYS 4
#if (LOGICTYPE==0)
#define TWEENS 6 //if running both boards on an UNO, we halve the number of Tweens to maintain speed
#else
#define TWEENS 12
#endif  
#define TRIMPOT_PIN A0
#define FRONT_PIN 6
#define REAR_PIN 7
#else
#define KEYS 4
#define TWEENS 12
#define TRIMPOT_PIN A8
#define FRONT_PIN 6
#define REAR_PIN 7
#endif

#define BUTTON_PIN 2

#define TOTALCOLORS (KEYS+(TWEENS*(KEYS-1)))
#define TOTALCOLORSWBIZ ((TOTALCOLORS*2)-2)

class LogicEngineClass
{
public:

  void Setup();
  void Update();

protected:
  byte adjMode = 0; // 0 for standard logics, 1 or higher for adjustments
  bool buttonStateNow = 1; //using internal pullup, button state is normally high
  bool buttonStatePrev = 1;
  unsigned long buttonStart;
  byte button = 0; //0 for inactive, 1 for short press, 2 for long press
  int potVal = 0;
  int potValPrev = 0;

  int actualColorNum(int x);
  void updateLED(byte LEDnum, byte hueVal, bool logicType);

};

extern LogicEngineClass LogicEngine;

#endif

