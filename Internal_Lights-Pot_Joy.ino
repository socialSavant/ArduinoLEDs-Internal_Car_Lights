#include <FastLED.h>

// FastLED constants.
#define LED_TYPE      WS2812B
#define COLOR_ORDER   GRB
const int DATA_PIN = 2;
const int NUM_LEDS = 150;
const int FRAMES_PER_SECOND = 120;

// FastLED variables.
CRGB leds[NUM_LEDS];
int gHue = 0;  // rotating "base color" used by many of the patterns

// Pin assignments.
const int POT_PIN = A6;
const int JOY_X_PIN = A4;
const int JOY_Y_PIN = A2;

// Variable init.
int pot = 0;
int joyZone = 0;
int brightness = 0;
int baseColor = 100;
boolean rainbowColor = false;

// Set calibration values.
int potLowCutoff = 0;
int potHighCutoff = 975;
int xLowCutoff = 20;
int xHighCutoff = 1024;  // Must be higher than LowCutoff
int yLowCutoff = 0;
int yHighCutoff = 995;  // Must be higher than LowCutoff
int xDeadZone = 175; // Between 0-512
int yDeadZone = 175; // Between 0-512
int brightnessMin = 5;
int brightnessMax = 20;

//  ------------------------------------
void setup() {
  // Sanity delay for bootup and recovery.
  delay(3000);
  
  // Setup I/O
  pinMode(POT_PIN, INPUT);
  pinMode(JOY_X_PIN, INPUT);
  pinMode(JOY_Y_PIN, INPUT);
  
  // Init FastLED library with LED Strip configuration.
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
}

void loop() {
  // Read from Potentiometers and remap output to useful calibrated values.
  potRead();
  joyRead();

  // Turn on LEDs as appropriate.
  if (joyZone == 7) {
    setBaseColor();
  }
  else if (pot < potHighCutoff) {
    brightness = map(pot, potHighCutoff, potLowCutoff, brightnessMin, brightnessMax);
    lightsOn();
  }
  else {
    allBlack();
  }

  // Set brightness, then send data to LEDs.
  pushFastLED();
  EVERY_N_MILLISECONDS(40) {gHue++;} // slowly cycle the "rotating base color".
}

//  ------------------------------------
void potRead() {
  pot = analogRead(POT_PIN);
}

void joyRead() {
  // Read Joystick Pots and assign to Adj Variables
  int xAdj = constrain(map(analogRead(JOY_X_PIN),xLowCutoff,xHighCutoff,0,1024),0,1024);
  int yAdj = constrain(map(analogRead(JOY_Y_PIN),yLowCutoff,yHighCutoff,0,1024),0,1024);
  int xTopDeadCenter = (xHighCutoff-xLowCutoff)/2;
  int yTopDeadCenter = (yHighCutoff-yLowCutoff)/2;
  int xValue = 2;
  int yValue = 2;
  // Adj X-Axis to useful Zones
  if (xAdj < (xTopDeadCenter-xDeadZone)) {xValue = 1;}
  else if (xAdj > (xTopDeadCenter+xDeadZone)) {xValue = 3;}
  else {xValue = 2;}
  // Adj Y-Axis to useful Zones
  if (yAdj < (yTopDeadCenter-yDeadZone)) {yValue = 1;}
  else if (yAdj > (yTopDeadCenter+yDeadZone)) {yValue = 3;}
  else {yValue = 2;}
  // Assign 8-Way Zones (and Dead Zone) using Combined X & Y Values
  if (xValue == 2 && yValue == 3) {joyZone = 1;}
  else if (xValue == 3 && yValue == 3) {joyZone = 2;}
  else if (xValue == 3 && yValue == 2) {joyZone = 3;}
  else if (xValue == 3 && yValue == 1) {joyZone = 4;}
  else if (xValue == 2 && yValue == 1) {joyZone = 5;}
  else if (xValue == 1 && yValue == 1) {joyZone = 6;}
  else if (xValue == 1 && yValue == 2) {joyZone = 7;}
  else if (xValue == 1 && yValue == 3) {joyZone = 8;}
  // else {joyZone = 0;} // Comment out to preserve last chosen Zone
}

void setBaseColor() {
  brightness = brightnessMax;
  if (analogRead(POT_PIN) < 50) {
    baseColor = gHue;
    rainbowColor = true;
  }
  else {
    baseColor = map(analogRead(POT_PIN),1024,50,0,255);
    rainbowColor = false;
  }
  fill_solid(leds, NUM_LEDS, CHSV(baseColor, 255, 255));
}

void lightsOn() {
  if (rainbowColor) {
    baseColor = gHue;
  }
  switch (joyZone) {
    case 1: rearDomeLight(); break;
    case 2: baseColor_Sinelon(); break;
    case 3: baseColor_Breath(); break;
    case 4: baseColor_ConfettiWithGlitter(); break;
    case 5: allWhite(); break;
    case 7: setBaseColor(); break;
  }
}

void pushFastLED() {
  FastLED.setBrightness(brightness);
  FastLED.show();
  FastLED.delay(1000/FRAMES_PER_SECOND);
}

void allBlack() {
  // Fade off all LED's
  fadeToBlackBy( leds, NUM_LEDS, 50);
}

void allWhite() {
  // All LED's to WhiteSmoke
  brightness = brightnessMax;
  fill_solid(leds, NUM_LEDS, CRGB::WhiteSmoke);
}

void driverMapLight() {
  // Turn on Driver Side Map Light
  brightness = brightnessMax;
  int D_MAP_START = 0;
  int D_MAP_STOP = 27;
  fadeToBlackBy(leds, NUM_LEDS, 50);
  for(int i = D_MAP_START; i < D_MAP_STOP; i++)
  {
    leds[i] = CRGB::WhiteSmoke;
  }
}

void passengerMapLight() {
  // Turn on Driver Side Map Light
  brightness = brightnessMax;
  int P_MAP_START = 102;
  int P_MAP_STOP = 134;
  fadeToBlackBy(leds, NUM_LEDS, 50);
  for(int i = P_MAP_START; i < P_MAP_STOP; i++)
  {
    leds[i] = CRGB::WhiteSmoke;
  }
}

void rearDomeLight() {
  // Turn on Rear Dome Light
  brightness = brightnessMax;
  int R_DOME_START = 28;
  int R_DOME_STOP = 101;
  fadeToBlackBy(leds, NUM_LEDS, 50);
  for(int i = R_DOME_START; i < R_DOME_STOP; i++)
  {
    leds[i] = CRGB::WhiteSmoke;
  }
}

void baseColor_Sinelon() {
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy(leds, NUM_LEDS, 5);
  int pos = beatsin16(8,0,NUM_LEDS);
  leds[pos] += CHSV(baseColor, 255, 255);
}

void baseColor_Breath() {
  fadeToBlackBy(leds, NUM_LEDS, 20);
  int v = beatsin16(10,150,255);
  fill_solid(leds, NUM_LEDS, CHSV(baseColor, 255, v));  
}

void baseColor_Confetti() {
  fadeToBlackBy(leds, NUM_LEDS, 5);
  int pos = random16(NUM_LEDS);
  delay(10);
  leds[pos] += CHSV(baseColor, 255, 255);
}

void baseColor_ConfettiWithGlitter() {
  baseColor_Confetti();
  addGlitter(30);
}
// ------------------------------------------
void rainbow() 
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
}

void rainbowWithGlitter() 
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(120);
}

void addGlitter( fract8 chanceOfGlitter) 
{
  if( random8() < chanceOfGlitter) 
  {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

void confetti() 
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
}

void confettiWithGlitter()
{
  confetti();
  addGlitter(120);
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16(13,0,NUM_LEDS);
  leds[pos] += CHSV( gHue, 255, 192);
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 56;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) 
  { //9948
    leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}

void juggle() 
{
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) 
  {
    leds[beatsin16(i+7,0,NUM_LEDS)] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}
