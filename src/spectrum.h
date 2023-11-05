// adapted from Scott Marley's LED Mask project
// https://github.com/s-marley/LEDMask
// https://www.youtube.com/watch?v=zYZkbfmCuEY


// (Heavily) adapted from https://github.com/G6EJD/ESP32-8266-Audio-Spectrum-Display/blob/master/ESP32_Spectrum_Display_02.ino
// Adjusted to allow brightness changes on press+hold, Auto-cycle for 3 button presses within 2 seconds
// Edited to add Neomatrix support for easier compatibility with different layouts.

#include <arduinoFFT.h>

#define SAMPLES         512          // Must be a power of 2
#define SAMPLING_FREQ   20000         // Hz, must be 40000 or less due to ADC conversion time. Determines maximum frequency that can be analysed by the FFT Fmax=sampleF/2.
#define AMPLITUDE       1000          // Depending on your audio source level, you may need to alter this value. Can be used as a 'sensitivity' control.
#define AUDIO_IN_PIN    A0            // Signal in on this pin
#define NUM_BANDS       13           // To change this, you will need to change the bunch of if statements describing the mapping from bins to bands
#define NOISE           500           // Used as a crude noise filter, values below this are ignored
#define BAR_WIDTH      (kMatrixWidth  / (NUM_BANDS - 1))  // If width >= 8 light 1 LED width per bar, >= 16 light 2 LEDs width bar etc
#define TOP            (kMatrixHeight - 0)                // Don't allow the bars to go offscreen
#define SERPENTINE     true                               // Set to false if you're LEDS are connected end to end, true if serpentine

// Sampling and FFT stuff
unsigned int sampling_period_us;
byte peak[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0};              // The length of these arrays must be >= NUM_BANDS
int oldBarHeights[] = {0,0,0,0,0,0,0,0,0,0,0,0,0};
int bandValues[] = {0,0,0,0,0,0,0,0,0,0,0,0,0};
double vReal[SAMPLES];
double vImag[SAMPLES];
unsigned long newTime;
arduinoFFT FFT = arduinoFFT(vReal, vImag, SAMPLES, SAMPLING_FREQ);

bool autoChangePatterns = false;

void startAutoMode() {
  autoChangePatterns = true;
}

// PATTERNS BELOW //

void rainbowBars(int band, int barHeight) {
  int xStart = BAR_WIDTH * band;
  for (int x = xStart; x < xStart + BAR_WIDTH; x++) {
    for (int y = TOP; y >= TOP - barHeight; y--) {
      matrix->drawPixel(x, y, CHSV((x / BAR_WIDTH) * (255 / NUM_BANDS), 255, 255));
    }
  }
}

void purpleBars(int band, int barHeight) {
  int xStart = BAR_WIDTH * band;
  for (int x = xStart; x < xStart + BAR_WIDTH; x++) {
    for (int y = TOP; y >= TOP - barHeight; y--) {
      matrix->drawPixel(x, y, ColorFromPalette(purplePal, y * (255 / (barHeight + 1))));
    }
  }
}

void changingBars(int band, int barHeight) {
  int xStart = BAR_WIDTH * band;
  for (int x = xStart; x < xStart + BAR_WIDTH; x++) {
    for (int y = TOP; y >= TOP - barHeight; y--) {
      matrix->drawPixel(x, y, CHSV(y * (255 / kMatrixHeight) + colorTimer, 255, 255));
    }
  }
}

void centerBars(int band, int barHeight) {
  int xStart = BAR_WIDTH * band;
  for (int x = xStart; x < xStart + BAR_WIDTH; x++) {
    if (barHeight % 2 == 0) barHeight--;
    int yStart = ((kMatrixHeight - barHeight) / 2 );
    for (int y = yStart; y <= (yStart + barHeight); y++) {
      int colorIndex = constrain((y - yStart) * (255 / barHeight), 0, 255);
      matrix->drawPixel(x, y, ColorFromPalette(heatPal, colorIndex));
    }
  }
}

void whitePeak(int band) {
  int xStart = BAR_WIDTH * band;
  int peakHeight = TOP - peak[band] - 1;
  for (int x = xStart; x < xStart + BAR_WIDTH; x++) {
    matrix->drawPixel(x, peakHeight, CHSV(0,0,255));
  }
}

void outrunPeak(int band) {
  int xStart = BAR_WIDTH * band;
  int peakHeight = TOP - peak[band] - 1;
  for (int x = xStart; x < xStart + BAR_WIDTH; x++) {
    matrix->drawPixel(x, peakHeight, ColorFromPalette(outrunPal, peakHeight * (255 / kMatrixHeight)));
  }
}

void waterfall(int band) {
  int xStart = BAR_WIDTH * band;
  double highestBandValue = 60000;        // Set this to calibrate your waterfall

  // Draw bottom line
  for (int x = xStart; x < xStart + BAR_WIDTH; x++) {
    matrix->drawPixel(x, 0, CHSV(constrain(map(bandValues[band],0,highestBandValue,160,0),0,160), 255, 255));
  }

  // Move screen up starting at 2nd row from top
  if (band == NUM_BANDS - 1){
    for (int y = kMatrixHeight - 2; y >= 0; y--) {
      for (int x = 0; x < kMatrixWidth; x++) {
        int pixelIndexY = matrix->XY(x, y + 1);
        int pixelIndex = matrix->XY(x, y);
        leds[pixelIndexY] = leds[pixelIndex];
      }
    }
  }
}

class Spectrum {
  public:
    Spectrum(){};
    bool runPattern();
  private:
    long previousTime = 0;
};

void brightnessButton() {
  if (FastLED.getBrightness() == BRIGHTNESS_SETTINGS[2])  FastLED.setBrightness(BRIGHTNESS_SETTINGS[0]);
  else if (FastLED.getBrightness() == BRIGHTNESS_SETTINGS[0]) FastLED.setBrightness(BRIGHTNESS_SETTINGS[1]);
  else if (FastLED.getBrightness() == BRIGHTNESS_SETTINGS[1]) FastLED.setBrightness(BRIGHTNESS_SETTINGS[2]);
  else if (FastLED.getBrightness() == 0) FastLED.setBrightness(BRIGHTNESS_SETTINGS[0]); //Re-enable if lights are "off"
}

void brightnessOff(){
  FastLED.setBrightness(0);  //Lights out
}


bool Spectrum::runPattern() {
    //if(checkButton()) return false;
    modeBtn.read();

    tickForPersistenceEvery30Sec();

    if (modeInterruptRequested) {
        FastLED.clear();
        SetModeButton();
        FastLED.show();
        yield();
        modeInterruptRequested = false;
        return false;
    }

    if (buttonInterruptRequested) {
      SetModeButton();
      buttonPushCounter = (buttonPushCounter + 1) % 6;
      buttonInterruptRequested = false;
    }

 // Don't clear screen if waterfall pattern, be sure to change this is you change the patterns / order
  if (buttonPushCounter != 5) {
    //FastLED.clear();
    matrix_clear();
  }
  
  // Reset bandValues[]
  for (int i = 0; i<NUM_BANDS; i++){
    bandValues[i] = 0;
  }

  // Sample the audio pin
  for (int i = 0; i < SAMPLES; i++) {
    newTime = micros();
    vReal[i] = analogRead(AUDIO_IN_PIN); // A conversion takes about 9.7uS on an ESP32
    vImag[i] = 0;
    while ((micros() - newTime) < sampling_period_us) { /* chill */ }
  }

  // Compute FFT
  FFT.DCRemoval();
  FFT.Windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.Compute(FFT_FORWARD);
  FFT.ComplexToMagnitude();

  // Analyse FFT results
  for (int i = 2; i < (SAMPLES/2); i++){       // Don't use sample 0 and only first SAMPLES/2 are usable. Each array element represents a frequency bin and its value the amplitude.
    if (vReal[i] > NOISE) {                    // Add a crude noise filter

    // //8 bands, 12kHz top band
    //   if (i<=3 )           bandValues[0]  += (int)vReal[i];
    //   if (i>3   && i<=6  ) bandValues[1]  += (int)vReal[i];
    //   if (i>6   && i<=13 ) bandValues[2]  += (int)vReal[i];
    //   if (i>13  && i<=27 ) bandValues[3]  += (int)vReal[i];
    //   if (i>27  && i<=55 ) bandValues[4]  += (int)vReal[i];
    //   if (i>55  && i<=112) bandValues[5]  += (int)vReal[i];
    //   if (i>112 && i<=229) bandValues[6]  += (int)vReal[i];
    //   if (i>229          ) bandValues[7]  += (int)vReal[i];

      //16 bands, 12kHz top band
      /*
      if (i<=2 )           bandValues[0]  += (int)vReal[i];
      if (i>2   && i<=3  ) bandValues[1]  += (int)vReal[i];
      if (i>3   && i<=5  ) bandValues[2]  += (int)vReal[i];
      if (i>5   && i<=7  ) bandValues[3]  += (int)vReal[i];
      if (i>7   && i<=9  ) bandValues[4]  += (int)vReal[i];
      if (i>9   && i<=13 ) bandValues[5]  += (int)vReal[i];
      if (i>13  && i<=18 ) bandValues[6]  += (int)vReal[i];
      if (i>18  && i<=25 ) bandValues[7]  += (int)vReal[i];
      if (i>25  && i<=36 ) bandValues[8]  += (int)vReal[i];
      if (i>36  && i<=50 ) bandValues[9]  += (int)vReal[i];
      if (i>50  && i<=69 ) bandValues[10] += (int)vReal[i];
      if (i>69  && i<=97 ) bandValues[11] += (int)vReal[i];
      if (i>97  && i<=135) bandValues[12] += (int)vReal[i];
      if (i>135 && i<=189) bandValues[13] += (int)vReal[i];
      if (i>189 && i<=264) bandValues[14] += (int)vReal[i];
      if (i>264          ) bandValues[15] += (int)vReal[i];
      */
      //16 bands, 12kHz top band

      // 13 bands for 13 cols.
      // add a band gain somehow
      // quick hack to divide vreal by some amount in the band that is clipping.
      
      if (i<=2 )            bandValues[0]  += (int)vReal[i];
      if (i>2   && i<=3  )  bandValues[1]  += (int)vReal[i];
      if (i>3   && i<=5  )  bandValues[2]  += (int)vReal[i];
      if (i>5   && i<=7  )  bandValues[3]  += (int)vReal[i];
      if (i>7   && i<=9  )  bandValues[4]  += (int)vReal[i];
      if (i>9   && i<=13 )  bandValues[5]  += (int)vReal[i];
      if (i>13  && i<=18 )  bandValues[6]  += (int)vReal[i];
      if (i>18  && i<=25 )  bandValues[7]  += (int)vReal[i]/3; 
      if (i>25  && i<=36 )  bandValues[8]  += (int)vReal[i];
      if (i>36  && i<=40 )  bandValues[9]  += (int)vReal[i];
      if (i>50  && i<=69 )  bandValues[10] += (int)vReal[i]/2;
      if (i>69  && i<= 89 ) bandValues[11] += (int)vReal[i]/2;
      if (i>97  && i<=137)  bandValues[12] += (int)vReal[i]/1.5;

      
      
    }
  }

  // Process the FFT data into bar heights
  for (byte band = 0; band < NUM_BANDS; band++) {

    // Scale the bars for the display
    int barHeight = bandValues[band] / AMPLITUDE;
    if (barHeight > TOP) barHeight = TOP;

    // Small amount of averaging between frames
    barHeight = ((oldBarHeights[band] * 1) + barHeight) / 2;

    // Move peak up
    if (barHeight > peak[band]) {
      peak[band] = min(TOP, barHeight);
    }

    // LightSalmon=0xFFA07A,           ///< @htmlcolorblock{FFA07A}
    //     LightSeaGreen=0x20B2AA,         ///< @htmlcolorblock{20B2AA}
    //     LightSkyBlue=0x87CEFA,          ///< @htmlcolorblock{87CEFA}
    //     LightSlateGray=0x778899,        ///< @htmlcolorblock{778899}
    //     LightSlateGrey=0x778899,        ///< @htmlcolorblock{778899}
    //     LightSteelBlue=0xB0C4DE,        ///< @htmlcolorblock{B0C4DE}
    //     LightYellow=0xFFFFE0,    

    // Draw bars
    switch (buttonPushCounter) {
      case 0:
        rainbowBars(band, barHeight);
        break;
      case 1:
        // No bars on this one
        break;
      case 2:
        purpleBars(band, barHeight);
        break;
      case 3:
        centerBars(band, barHeight);
        break;
      case 4:
        changingBars(band, barHeight);
        break;
      case 5:
        waterfall(band);
        break;
    }

    // Draw peaks
    switch (buttonPushCounter) {
      case 0:
        whitePeak(band);
        break;
      case 1:
        outrunPeak(band);
        break;
      case 2:
        whitePeak(band);
        break;
      case 3:
        // No peaks
        break;
      case 4:
        // No peaks
        break;
      case 5:
        // No peaks
        break;
    }

    // Save oldBarHeights for averaging later
    oldBarHeights[band] = barHeight;
  }

  // Decay peak
  EVERY_N_MILLISECONDS(60) {
    for (byte band = 0; band < NUM_BANDS; band++)
      if (peak[band] > 0) peak[band] -= 1;
    colorTimer++;
  }

  // Used in some of the patterns
  EVERY_N_MILLISECONDS(10) {
    colorTimer++;
  }

  EVERY_N_SECONDS(10) {
    if (autoChangePatterns) buttonPushCounter = (buttonPushCounter + 1) % 6;
  }

  FastLED.show();

  return true;
}

