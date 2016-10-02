// AstroDuinio Project. Copyright (c) Licensed under the New BSD License.
// Source: https://github.com/tgiphil/AstroDuino

// More information from: http://rseries.net/logics-engine.html

#include "LogicEngine.h"
#include "Comm.h"

#include <FastLED.h>
#include <avr/pgmspace.h> //to save SRAM, some constants get stored in flash memory
#include <EEPROM.h>       //used to store user settings in EEPROM (settings will persist on reboots)

//////////////////////////////////////////////////////////////////////////////////////////////////////
// FRONT LED MAPPING...
#if (PCBVERSION==0)
//mapping for FLD boards from first run (with 48 LEDs per PCB)
const byte frontLEDmap[]PROGMEM = {
  15,14,13,12,11,10, 9, 8,
  16,17,18,19,20,21,22,23,
  31,30,29,28,27,26,25,24,
  32,33,34,35,36,37,38,39,
  47,46,45,44,43,42,41,40, // 
  88,89,90,91,92,93,94,95, // 
  87,86,85,84,83,82,81,80,
  72,73,74,75,76,77,78,79,
  71,70,69,68,67,66,65,64,
  56,57,58,59,60,61,62,63 };
#else
//mapping for newer FLD PCBs (40 LEDs per PCB, lower FLD upside-down)...
const byte frontLEDmap[]PROGMEM = {
  0, 1, 2, 3, 4, 5, 6, 7,
  15,14,13,12,11,10, 9, 8,
  16,17,18,19,20,21,22,23,
  31,30,29,28,27,26,25,24,
  32,33,34,35,36,37,38,39,
  79,78,77,76,75,74,73,72,
  64,65,66,67,68,69,70,71,
  63,62,61,60,59,58,57,56,
  48,49,50,51,52,53,54,55,
  47,46,45,44,43,42,41,40 };
#endif

//REAR LED MAPPING...
#if (PCBVERSION==0)
//mapping for first RLD (two PCBs soldered together)
const byte rearLEDmap[]PROGMEM = {
  0, 1, 2, 3, 4, 5, 6, 7,48,49,50,51,52,53,54,55,
  15,14,13,12,11,10, 9, 8,63,62,61,60,59,58,57,56,
  16,17,18,19,20,21,22,23,64,65,66,67,68,69,70,71,
  31,30,29,28,27,26,25,24,79,78,77,76,75,74,73,72,
  32,33,34,35,36,37,38,39,80,81,82,83,84,85,86,87,
  47,46,45,44,43,42,41,40,95,94,93,92,91,90,89,88 };
#elif (PCBVERSION==1)
//mapping for single RLD PCB (second parts run on)...
const byte rearLEDmap[]PROGMEM = {
  0, 1, 2,28, 3,27, 4,26, 5,25, 6, 7, 8, 9,22,10,21,11,20,12,19,13,14,15,
  31,30,29,32,33,34,35,36,37,38,39,24,23,40,41,42,43,44,45,46,47,18,17,16,
  64,65,66,63,62,61,60,59,58,57,56,71,72,55,54,53,52,51,50,49,48,77,78,79,
  95,94,93,67,92,68,91,69,90,70,89,88,87,86,73,85,74,84,75,83,76,82,81,80 };
#else  
//mapping for 2016 RLD (Death Star on back, direct mapping)...
const byte rearLEDmap[]PROGMEM = {
  0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,
  47,46,45,44,43,42,41,40,39,38,37,36,35,34,33,32,31,30,29,28,27,26,25,24,
  48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,
  95,94,93,92,91,90,89,88,87,86,85,84,83,82,81,80,79,78,77,76,75,74,73,72 };
#endif

//an array that holds the current color number and direction of each LED
//to save SRAM, we don't store the "direction" anymore, instead we pretend that instead of having 16 colors, we've got 31
struct LEDstat { byte colorNum; byte colorPause; }; //colorDir is its direction (cycling up or down through the colors)

#if (LOGICTYPE==0)||(LOGICTYPE==1)
CRGB frontLEDs[80];
byte frontColors[TOTALCOLORS][3];
const byte frontKeyColors[KEYS][3]PROGMEM = { { 170,255,0 } ,{ 170,255,136 } ,{ 170,255,255 } ,{ 170,0,255 } };   // for help calculating HSV color values see http://joymonkey.com/logic/
LEDstat frontStatus[80];
#endif

#if (LOGICTYPE==0)||(LOGICTYPE==2)
CRGB rearLEDs[96];
byte rearColors[TOTALCOLORS][3];
const byte  rearKeyColors[KEYS][3]PROGMEM = { { 87,0,0 }    ,{ 87,206,105 }  ,{ 79,255,184 }  ,{ 18,255,250 } };
LEDstat rearStatus[96];
#endif

struct userSettings { byte brightness; byte frontFade; byte frontDelay; byte frontHue; byte rearFade; byte rearDelay; byte rearHue; };

#if (CONTROLLER==0)
userSettings settings[] = { 100, 1,40,0, 3,100,0 }; //good settings for 328P
#else
userSettings settings[] = { 100, 1,80,0, 3,200,0 }; //good settings for Teensy 3.1
#endif


#define longPressTime 2000  //how many milliseconds constitutes a long press of the button
#define shortPressTime 50   //and a short press (used for debouncing)


void LogicEngineClass::Setup()
{
  digitalWrite(13, HIGH); //turn on LED on pin 13 during setup()
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  //pinMode(BUTTON_PIN,INPUT);
  delay(10);
  randomSeed(analogRead(0)); //assuming pin 0 is not connected to anything, we use it here to keep things more random

// Check that valid settings are in eeprom, else write them.
// From factory, all 1024 eeprom bytes are set to 255. No sane person would ever set the frontFade setting to something over 16. That's just ridiculous.
// So, we have a look at those two bytes, and if they're 255, we overwite the eeprom with our default settings.
//byte test = EEPROM.read(1);

#if (DEBUG==1)
  Comm.WaitForConnection();
  Comm.DebugEnabled = true;
  Comm.Debug(F("Logic: "));
  Comm.Debug(LOGICTYPE);
  Comm.Debug(F(" , PCB: "));
  Comm.DebugLine(PCBVERSION);
  Comm.Debug(F("frontFade EEPROM ="));
  Comm.DebugLine(EEPROM.read(1));
#endif

  if (EEPROM.read(1) > 100) {
#if (DEBUG==1)
    Comm.DebugLine(F("write EEPROM dflts"));
#endif
    //userSettings { byte brightness; byte frontFade; byte frontDelay; byte frontHue; byte rearFade; byte rearDelay; byte rearHue; };
    EEPROM.write(0, settings[0].brightness);
    EEPROM.write(1, settings[0].frontFade);
    EEPROM.write(2, settings[0].frontDelay);
    EEPROM.write(3, settings[0].frontHue);
    EEPROM.write(4, settings[0].rearFade);
    EEPROM.write(5, settings[0].rearDelay);
    EEPROM.write(6, settings[0].rearHue);
  }
  else {
    //eeprom settings must be okay, read them into our settings structure
#if (DEBUG==1)
    Comm.DebugLine(F("EEPROM all good yo"));
#endif
    settings[0] = { EEPROM.read(0), EEPROM.read(1),EEPROM.read(2), EEPROM.read(3), EEPROM.read(4), EEPROM.read(5), EEPROM.read(6) };
  }

  if (digitalRead(BUTTON_PIN) == LOW) {
    // *** TO DO:
    //button held at startup, we want to do a 'factory reset' write default values to eeprom
    // ***
#if (DEBUG==1)
    Comm.DebugLine(F("Factory reset"));
#endif
  }

  FastLED.setBrightness(settings[0].brightness); //sets the overall brightness to the maximum

#if (LOGICTYPE==0)||(LOGICTYPE==1)
                                                 //calculate front colors...
  for (byte kcol = 0; kcol < (KEYS); kcol++) { //go through each Key color
    byte workColor[3] = { pgm_read_byte(&frontKeyColors[kcol][0]) , pgm_read_byte(&frontKeyColors[kcol][1]) , pgm_read_byte(&frontKeyColors[kcol][2]) };
    //Comm.DebugLine("Key Color #"+String(kcol)+" : "+String(workColor[0])+","+" "+String(workColor[1])+","+" "+String(workColor[2]) );
    //Comm.DebugLine("Key Color #"+String(kcol)+" is actual color #"+String(kcol+(TWEENS*kcol)) );
    frontColors[kcol + (TWEENS*kcol)][0] = workColor[0];
    frontColors[kcol + (TWEENS*kcol)][1] = workColor[1];
    frontColors[kcol + (TWEENS*kcol)][2] = workColor[2];
    if (kcol + (TWEENS*kcol) + 1 != TOTALCOLORS) {
      for (byte el = 0; el < 3; el++) { //loop through H, S and V from this key to the next
        int perStep = int(pgm_read_byte(&frontKeyColors[kcol + 1][el]) - workColor[el]) / (TWEENS + 1);
        if (perStep != 0) {
          //Comm.DebugLine("  Element "+String(el)+" changes by "+String(perStep)+" to reach "+String(pgm_read_byte(&frontKeyColors[kcol+1][el]))+" in "+String(TWEENS)+" steps");
          for (byte tweenCount = 1; tweenCount <= TWEENS; tweenCount++) {
            byte val = pgm_read_byte(&frontKeyColors[kcol][el]) + (tweenCount*perStep);
            //Comm.Debug("  color#"+String(kcol+(TWEENS*kcol)+tweenCount)+"="+String(val));
            frontColors[kcol + (TWEENS*kcol) + tweenCount][el] = val;
          }
          //Comm.DebugLine();
        }
        else {
          //tweens for this element (h,s or v) don't change between this key and the next, fill em up
          for (byte tweenCount = 1; tweenCount <= TWEENS; tweenCount++) {
            frontColors[kcol + (TWEENS*kcol) + tweenCount][el] = workColor[el];
          }
        }
      }
    }
    //Comm.DebugLine();
  }
  FastLED.addLeds<SK6812, FRONT_PIN, GRB>(frontLEDs, 80);
  for (int i = 0; i < 80; i++) frontStatus[i] = { random8(TOTALCOLORSWBIZ),random8() };
#endif
#if (LOGICTYPE==0)||(LOGICTYPE==2)
  //calculate rear colors...
  for (byte kcol = 0; kcol < (KEYS); kcol++) { //go through each Key color
    byte workColor[3] = { pgm_read_byte(&rearKeyColors[kcol][0]) , pgm_read_byte(&rearKeyColors[kcol][1]) , pgm_read_byte(&rearKeyColors[kcol][2]) };
    rearColors[kcol + (TWEENS*kcol)][0] = workColor[0];
    rearColors[kcol + (TWEENS*kcol)][1] = workColor[1];
    rearColors[kcol + (TWEENS*kcol)][2] = workColor[2];
    if (kcol + (TWEENS*kcol) + 1 != TOTALCOLORS) {
      for (byte el = 0; el < 3; el++) { //loop through H, S and V from this key to the next
        int perStep = int(pgm_read_byte(&rearKeyColors[kcol + 1][el]) - workColor[el]) / (TWEENS + 1);
        if (perStep != 0) {
          for (byte tweenCount = 1; tweenCount <= TWEENS; tweenCount++) {
            byte val = pgm_read_byte(&rearKeyColors[kcol][el]) + (tweenCount*perStep);
            rearColors[kcol + (TWEENS*kcol) + tweenCount][el] = val;
          }
        }
        else {
          //tweens for this element (h,s or v) don't change between this key and the next, fill em up
          for (byte tweenCount = 1; tweenCount <= TWEENS; tweenCount++) rearColors[kcol + (TWEENS*kcol) + tweenCount][el] = workColor[el];
        }
      }
    }
  }
  FastLED.addLeds<SK6812, REAR_PIN, GRB>(rearLEDs, 96);
  for (int i = 0; i < 96; i++) rearStatus[i] = { random8(TOTALCOLORSWBIZ),random8() };
#endif

  delay(50);
  digitalWrite(13, LOW); //turn off LED on pin 13
}

//function takes a color number (that may be bizarro) and returns an actual color number
int LogicEngineClass::actualColorNum(int x) {
  if (x >= TOTALCOLORS) x = (TOTALCOLORS - 2) - (x - TOTALCOLORS);
  return(x);
}

void LogicEngineClass::updateLED(byte LEDnum, byte hueVal, bool logicType) {
  //logicType is 0 for front, 1 for rear
  if (logicType == 0) {
    if (frontStatus[LEDnum].colorPause != 0)  frontStatus[LEDnum].colorPause--; //reduce the LEDs pause number and check back next loop
    else {
      frontStatus[LEDnum].colorNum++;
      if (frontStatus[LEDnum].colorNum >= TOTALCOLORSWBIZ) frontStatus[LEDnum].colorNum = 0; //bring it back to color zero
      byte realColor = actualColorNum(frontStatus[LEDnum].colorNum);
      if (frontStatus[LEDnum].colorNum % (KEYS + 1) == 0) frontStatus[LEDnum].colorPause = random8(settings[0].frontDelay); //color is a key, assign random pause
      else frontStatus[LEDnum].colorPause = settings[0].frontFade; //color is a tween, assign a quick pause
      frontLEDs[LEDnum].setHSV((frontColors[realColor][0] + hueVal), frontColors[realColor][1], frontColors[realColor][2]);
      //if (realColor>=TOTALCOLORS) Comm.DebugLine(String(frontStatus[LEDnum].colorNum)+"="+String(realColor));
    }
  }
}

void LogicEngineClass::Update()
{
  //Check the button's state
  buttonStateNow = digitalRead(BUTTON_PIN);
  if (buttonStateNow == 0) {
    if (buttonStatePrev == 1) buttonStart = millis();
  }
  else {
    if (buttonStatePrev == 0) {
      if (millis() - buttonStart > shortPressTime) {
        //button has been released
#if (DEBUG==1)
        Comm.DebugLine("button time " + String(millis() - buttonStart));
#endif
        if (millis() - buttonStart > longPressTime) button = 2; //long press detected
        else button = 1; //short press detected
      }
    }
  }
  buttonStatePrev = buttonStateNow;

  //We use button to cycle through modes brightness/frontFade/frontDelay/frontHue/rearFade/rearDelay/rearHue
  if (adjMode == 0 && button == 2) {
    //enter adjustment display mode
    adjMode = 1;
    button = 0;
#if (DEBUG==1)
    Comm.DebugLine("adjMode " + String(adjMode));
#endif
  }
  if (adjMode > 0) {
    potVal = analogRead(TRIMPOT_PIN) / 4; //read the trimpot      
    if (button == 2) {
      //wite our recent adjustments to eeprom
#if (DEBUG==1)
      Comm.DebugLine("Writing EEPROM");
#endif
      adjMode = 0; //go back to regular display mode
    }
    if (button == 1) {
      adjMode++; //a short press of the botton moves us to the next adjustment mode
      if (adjMode == 8) adjMode = 1;
#if (DEBUG==1)
      Comm.DebugLine("adjMode " + String(adjMode));
#endif        
    }
    if (potVal != potValPrev) {
      if (adjMode == 1) settings[0].brightness = map(potVal, 0, 255, 0, MAX_BRIGHTNESS);
#if (LOGICTYPE==0)||(LOGICTYPE==1)
      else if (adjMode == 2) settings[0].frontFade = map(potVal, 0, 255, 0, 8);
      else if (adjMode == 3) settings[0].frontDelay = potVal;
      else if (adjMode == 4) settings[0].frontHue = potVal;
#endif
#if (LOGICTYPE==0)||(LOGICTYPE==2)
      else if (adjMode == 5) settings[0].rearFade = map(potVal, 0, 255, 0, 8);
      else if (adjMode == 6) settings[0].rearDelay = potVal;
      else if (adjMode == 7) settings[0].rearHue = potVal;
#endif
    }
    potValPrev = potVal;
  }

  //Update all them thar LEDs...  
#if (LOGICTYPE==0)||(LOGICTYPE==1)
  for (byte i = 0; i < 80; i++) updateLED(pgm_read_byte(&frontLEDmap[i]), settings[0].frontHue, 0);
  //if we're in an adjustment mode affecting the front logics, indicate that somehow...
  if (adjMode == 1) for (byte i = 0; i < 4; i++) frontLEDs[pgm_read_byte(&frontLEDmap[i])] = CHSV(0, 0, (i * 50) + 40); //first LEDs display greys, representing brightness adjustment
  else if (adjMode == 2) for (byte i = 0; i < 4; i++) frontLEDs[pgm_read_byte(&frontLEDmap[i])] = CRGB::Red; //frontFade purple
  else if (adjMode == 3) for (byte i = 0; i < 4; i++) frontLEDs[pgm_read_byte(&frontLEDmap[i])] = CRGB::Green;  //frontDelay
  else if (adjMode == 4) for (byte i = 0; i < 4; i++) frontLEDs[pgm_read_byte(&frontLEDmap[i])] = CHSV((i * 63), 255, 150); //frontHue rainbow
  FastLED.show();
#endif
#if (LOGICTYPE==0)||(LOGICTYPE==2)
  for (byte i = 0; i < 96; i++) updateLED(pgm_read_byte(&frontLEDmap[i]), settings[0].rearHue, 1);
  //if we're in an adjustment mode affecting the rear logics, indicate that somehow...
  if (adjMode == 1) for (byte i = 0; i < 3; i++) rearLEDs[pgm_read_byte(&rearLEDmap[i])] = CHSV(0, 0, (i * 50) + 40); //first LEDs display greys, representing brightness adjustment
  else if (adjMode == 5) for (byte i = 0; i < 4; i++) rearLEDs[pgm_read_byte(&frontLEDmap[i])] = CRGB::Red; //rearFade 
  else if (adjMode == 6) for (byte i = 0; i < 4; i++) rearLEDs[pgm_read_byte(&frontLEDmap[i])] = CRGB::Green;  //reartDelay
  else if (adjMode == 7) for (byte i = 0; i < 4; i++) rearLEDs[pgm_read_byte(&frontLEDmap[i])] = CHSV((i * 63), 255, 150); //rearHue rainbow
  FastLED.show();
#endif

  button = 0;

}

LogicEngineClass LogicEngine;

