#include "Arduino.h"

class Confetti {
  public:
    Confetti(){};
    bool runPattern();

  private:
        uint8_t  thisfade = 8;                                        // How quickly does it fade? Lower = slower fade rate.
        int       thishue = 50;                                       // Starting hue.
        uint8_t   thisinc = 1;                                        // Incremental value for rotating hues
        uint8_t   thissat = 100;                                      // The saturation, where 255 = brilliant colours.
        uint8_t   thisbri = 255;                                      // Brightness of a sequence. Remember, max_bright is the overall limiter.
        int       huediff = 256;                                      // Range of random #'s to use for hue
        uint8_t thisdelay = 5;                                        // We don't need much delay (if any)
};


bool Confetti::runPattern() {

    modeBtn.read();

   if (modeInterruptRequested) {
        modeInterruptRequested = false;
        Serial.println("FS mode interupt");
        return false;
    }
        fadeToBlackBy(leds, NUM_LEDS, thisfade);                    // Low values = slower fade.
        int pos = random16(NUM_LEDS);                               // Pick an LED at random.
        leds[pos] += CHSV((thishue + random16(huediff))/4 , thissat, thisbri);  // I use 12 bits for hue so that the hue increment isn't too quick.
        thishue = thishue + thisinc;      
    delay(2);
    FastLED.show();  
    
    return true;
}