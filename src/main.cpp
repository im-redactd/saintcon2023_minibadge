// #include "hardware/xosc.h"
// #define PICO_XOSC_STARTUP_DELAY_MULTIPLIER 64
#include "pico.h"
#include "hardware/structs/xosc.h"


// Allow lengthening startup delay to accommodate slow-starting oscillators

// PICO_CONFIG: PICO_XOSC_STARTUP_DELAY_MULTIPLIER, Multiplier to lengthen xosc startup delay to accommodate slow-starting oscillators, type=int, min=1, default=1, group=hardware_xosc
// #ifndef PICO_XOSC_STARTUP_DELAY_MULTIPLIER
// #define PICO_BOOT_STAGE2_CHOOSE_GENERIC_03H 1
// #define PICO_XOSC_STARTUP_DELAY_MULTIPLIER 64
// #endif


#include <FastLED_NeoMatrix.h>
#include "fastled_config.h"
#include <Wire.h>


extern "C" void flash_get_unique_id(uint8_t *p);

#define LED_PIN    12
#define MODE_PIN   11

#define LONG_PRESS_MS   200           // Number of ms to count as a long press

// Button stuff
bool buttonInterruptRequested = false;
bool modeInterruptRequested = false;
bool dropsModeInterruptRequested = false;

#include "touch.h"


uint8_t buttonPushCounter = 1;
// initial mode on boot.
uint8_t modePushCounter = 0;

// Marquee.
#define WE_HAVE_A_FRIEND
#define FRIEND_NAME " saintcon " // add some padding on either side of the name to make it flow better.
#define FRIEND_COLOR LED_ORANGE_MEDIUM  //LED_CYAN_MEDIUM
#define FRIEND_SPARKLE true

uint8_t total_modes = 10;
uint8_t modes = total_modes;  

static const uint I2C_SLAVE_ADDRESS = 0x13;

// unique id for i2c serial
uint8_t UniqueID[8];
uint8_t fakeID[8];
bool on_badge = false;
bool on_badge_motion = false;


void handleI2CReceive(int numBytes)
{
  byte command = Wire.read(); // pretty much just ignore the command
  if(command==0x80) {
    if (!on_badge){
    on_badge = true;
    modes = 11;
    modePushCounter = 9;
    modeInterruptRequested = true;
    
    }
    }
  return;
}

void handleI2CRequest()
{
  if(on_badge){
    Wire.write(UniqueID, 8);
  }
  else {
    Wire.write(fakeID, 8);
  }
  
  return;
}

bool checkButton() {
  if (buttonInterruptRequested) return true;
  return false;
}

FastLED_NeoMatrix *matrix = new FastLED_NeoMatrix(leds, kMatrixWidth, kMatrixHeight,
  NEO_MATRIX_TOP        + NEO_MATRIX_LEFT +
  NEO_MATRIX_ROWS       + NEO_MATRIX_ZIGZAG +
  NEO_TILE_TOP + NEO_TILE_LEFT + NEO_TILE_ROWS);

void matrix_show() {
    matrix->show();
}

void matrix_clear() {
    memset(leds, 0, sizeof(leds));
}

void showStrip() {
 #ifdef ADAFRUIT_NEOPIXEL_H
   // NeoPixel
   matrix.show();
 #endif
 #ifndef ADAFRUIT_NEOPIXEL_H
   // FastLED
   FastLED.show();
 #endif
}

void setPixel(int Pixel, byte red, byte green, byte blue) {
 #ifdef ADAFRUIT_NEOPIXEL_H
   // NeoPixel
   matrix.setPixelColor(Pixel, strip.gamma32(strip.Color(red, green, blue)));
 #endif
 #ifndef ADAFRUIT_NEOPIXEL_H
   // FastLED
   leds[Pixel].r = red;
   leds[Pixel].g = green;
   leds[Pixel].b = blue;
 #endif
}

void setAll(byte red, byte green, byte blue) {
  for(int i = 0; i < NUM_LEDS; i++ ) {
    setPixel(i, red, green, blue);
  }
  showStrip();
}

void Sparkle(byte red, byte green, byte blue, int SpeedDelay) {
  int Pixel = random(NUM_LEDS);
  setPixel(Pixel,red,green,blue);
  showStrip();
  delay(SpeedDelay);
  setPixel(Pixel,0,0,0);
}

void Sparkle3(byte red, byte green, byte blue, int SpeedDelay) {
  int Pixel1 = random(NUM_LEDS);
  setPixel(Pixel1,red,green,blue);
  int Pixel2 = random(NUM_LEDS);
  setPixel(Pixel2,red,green,blue);
  int Pixel3 = random(NUM_LEDS);
  setPixel(Pixel3,red,green,blue);
  delay(SpeedDelay);
}

void changeMode() {
  //rtt.println("[*] long press");
  modeInterruptRequested = true;
  
  modePushCounter += 1;
  modePushCounter %= modes;
}


#include "frame_sequence.h"
//#include "spectrum.h"
#include "matrix.h"
#include "motion.h"
#include "fire.h"
#include "plasma.h"
#include "drops.h"
#include "noise.h"
#include "snake.h"
#include "fireworks2.h"
//#include "steps.h"
#include "friends.h"
#include "crosshatch.h"
//#include "testing.h"
//#include "discostrobe.h"
//#include "twinkle.h"

void runFriends() {
  bool isRunning = true;
  Friends friends = Friends();
  while (isRunning) isRunning = friends.runPattern();
}

void runMotion() {
  bool isRunning = true;
  Motion motion = Motion();
  while (isRunning) isRunning = motion.runPattern();
}

void runFS() {
  bool isRunning = true;
  PlayFrameSequence playframesequence = PlayFrameSequence();
  while (isRunning) isRunning = playframesequence.runPattern();
}

void runFire() {
  bool isRunning = true;
  Fire fire = Fire();
  while (isRunning) isRunning = fire.runPattern();
}

void runTheMatrix() {
  bool isRunning = true;
  TheMatrix thematrix = TheMatrix();
  while (isRunning) isRunning = thematrix.runPattern();
}

void runPlasma() {
  bool isRunning = true;
  Plasma plasma = Plasma();
  while (isRunning) isRunning = plasma.runPattern();
}

void runDrops() {
  bool isRunning = true;
  Drops drops = Drops();
  while (isRunning) isRunning = drops.runPattern();
}

void runNoise() {
  bool isRunning = true;
  Noise noise = Noise();
  while (isRunning) isRunning = noise.runPattern();
}

void runSnake() {
  bool isRunning = true;
  Snake snake = Snake();
  while (isRunning) isRunning = snake.runPattern();
}

void runFireworks2() {
  bool isRunning = true;
  Fireworks2 fireworks2 = Fireworks2();
  while (isRunning) isRunning = fireworks2.runPattern();
}

void runCrosshatch() {
  bool isRunning = true;
  Crosshatch crosshatch = Crosshatch();
  while (isRunning) isRunning = crosshatch.runPattern();
}

// void runTwinkle() {
//   bool isRunning = true;
//   Twinkle twinkle = Twinkle();
//   while (isRunning) isRunning = twinkle.runPattern();
// }



void setup() {


  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalSMD5050);
  
  FastLED.setMaxPowerInVoltsAndMilliamps(LED_VOLTS, MAX_MILLIAMPS);
  FastLED.setBrightness(BRIGHTNESS_SETTINGS[1]);
  FastLED.clear();
  matrix->setRemapFunction(XY);


  // boot up animation
  //for(int bootSparkleIndex=0; bootSparkleIndex<250; bootSparkleIndex ++) {
  //  Sparkle(random(255), random(255), random(255), 15);
  // }
  
  flash_get_unique_id(UniqueID);

  Wire.begin(I2C_SLAVE_ADDRESS);
  Wire.onRequest(handleI2CRequest);
  Wire.onReceive(handleI2CReceive);
  
  //rtt.println("booted!");

}

void loop() {

    //buttonInterruptRequested = false;
    modeInterruptRequested = false;

    checkForTouch();

    switch (modePushCounter) {
      case 0:
        //rtt.println("[*] mode 1");
        runFriends();
        changeMode();
        break;
      case 1:
        //rtt.println("[*] mode 2");
        runMotion();
        changeMode();
        break;
      case 2:
         //rtt.println("[*] mode: fireworks2.");
        runFireworks2();
        changeMode();
        break;
      case 3:
       //rtt.println("[*] mode fire.");
        runFire();
        changeMode();
        break;
      case 4:
        //rtt.println("[*] mode drops.");
        
        changeMode();
        break;
      case 5:
        //rtt.println("[*] mode: noise.");
        runNoise();
        changeMode();
        break;
      case 6:
        //rtt.println("[*] mode plasma.");
        runPlasma();
        changeMode();
        break;
      case 7:
        //rtt.println("[*] mode: snake.");
        runSnake();
        changeMode();
        break;
      
      case 8:
       //rtt.println("[*] mode ch");
        
        changeMode();
        break; 

      case 9:
       //rtt.println("[*] mode fs");
        runFS();
        changeMode();
        break;  

      case 10:
       //rtt.println("[*] mode matrix");
        runTheMatrix();
        changeMode();
        break;
        
    }
}
