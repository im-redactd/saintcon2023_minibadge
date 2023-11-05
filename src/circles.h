// adapted from Scott Marley's LED Mask project
// https://github.com/s-marley/LEDMask
// https://www.youtube.com/watch?v=zYZkbfmCuEY

// Pulsing circles from centre of matrix.

#include "Arduino.h"

class Circles {
  public:
    Circles(){};
    bool runPattern();
  private:
    void drawCircle(int xc, int yc, int r, CRGB color);
    void drawPattern(uint8_t pattern);
    uint8_t _pattern = 0;
    uint8_t hue = 0;
};

bool Circles::runPattern() {
    
    modeBtn.read();

   if (modeInterruptRequested) {
        modeInterruptRequested = false;
        Serial.println("FS mode interupt");
        return false;
    }

    fadeToBlackBy( leds, NUM_LEDS, 20);
    
    EVERY_N_MILLISECONDS(100) {
        if(_pattern == 8) hue += 32;
        _pattern = (_pattern + 1) %9;
        drawPattern(_pattern);
        Serial.println("im here 1/");
    }

    FastLED.show();
    delay(1);
    //delayMicroseconds(75);
    return true;
}

void Circles::drawPattern(uint8_t pattern) {
  drawCircle(kMatrixWidth / 2, kMatrixHeight / 2, pattern, CHSV(hue,255,255));
}

void Circles::drawCircle(int xc, int yc, int r, CRGB color)
{
  int x = -r;
  int y = 0;
  int e = 2 - (2 * r);
  do
  {
    leds[XY(xc + x, yc - y)] = color;
    leds[XY(xc - x, yc + y)] = color;
    leds[XY(xc + y, yc + x)] = color;
    leds[XY(xc - y, yc - x)] = color;
    int _e = e;
    if (_e <= y)
      e += (++y * 2) + 1;
    if ((_e > x) || (e > y))
      e += (++x * 2) + 1;
  }
while (x < 0);
    delay(1);
}