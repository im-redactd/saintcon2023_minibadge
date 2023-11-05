#include "timber_wheel.h"
#include "timber_radial_rainbow.h"
#include "SlowRainbowSpiral.h"
#include "IntersectingCircles.h"




#define LED_COUNT timber_wheel_led_count
#define NUMBER_OF_FRAMES timber_wheel_frame_count
#define FRAMES_PER_SECOND timber_wheel_frame_rate
#define FRAME_DATA_SIZE timber_wheel_frame_size

uint8_t animationCount = 4;
uint8_t currentAnimationIndex = 0;


class PlayFrameSequence {
  public:
    PlayFrameSequence(){};
    bool runPattern();
  private:
    
    typedef struct {
    uint32_t led_count;
    uint32_t frame_size;
    uint32_t frame_count;
    uint32_t frame_rate;
    uint8_t* frame_data;
    } FrameSequence;

    FrameSequence frameSequence1 = {
    .led_count = timber_wheel_led_count,
    .frame_size = timber_wheel_frame_size,
    .frame_count = timber_wheel_frame_count,
    .frame_rate = timber_wheel_frame_rate,
    .frame_data = (uint8_t*)&timber_wheel
    };

    FrameSequence frameSequence2 = {
    .led_count = timber_radial_rainbows_led_count,
    .frame_size = timber_radial_rainbows_frame_size,
    .frame_count = timber_radial_rainbows_frame_count,
    .frame_rate = timber_radial_rainbows_frame_rate,
    .frame_data = (uint8_t*)&timber_radial_rainbows
    };

 
    FrameSequence frameSequence4 = {
    .led_count = SlowRainbowSpiral_led_count,
    .frame_size = SlowRainbowSpiral_frame_size,
    .frame_count = SlowRainbowSpiral_frame_count,
    .frame_rate = SlowRainbowSpiral_frame_rate,
    .frame_data = (uint8_t*)&SlowRainbowSpiral
    };

   FrameSequence frameSequence5 = {
    .led_count = IntersectingCircles_led_count,
    .frame_size = IntersectingCircles_frame_size,
    .frame_count = IntersectingCircles_frame_count,
    .frame_rate = IntersectingCircles_frame_rate,
    .frame_data = (uint8_t*)&IntersectingCircles
    };

    FrameSequence *animations[4] = {
    &frameSequence1,
    &frameSequence2,
    &frameSequence4,
    &frameSequence5
    };

    FrameSequence* currentFrameSequence = animations[currentAnimationIndex];
};


bool PlayFrameSequence::runPattern() {
  
  for(
    uint32_t frameIndex = 0;
    frameIndex < currentFrameSequence->frame_count;
    frameIndex += 1
    ) {
      
    checkForTouch();
    
    //tickForPersistenceEvery30Sec();

    if (modeInterruptRequested) {
        modeInterruptRequested = false;
        if(currentAnimationIndex == (animationCount-1)) {
          currentAnimationIndex = 0;
          return false;
        }
        else {
          buttonInterruptRequested = true;
        }
    }

    if (buttonInterruptRequested) {
      currentAnimationIndex += 1;
      currentAnimationIndex %= animationCount;
      currentFrameSequence = animations[currentAnimationIndex];
      buttonInterruptRequested = false;
      break;
    }

    int frameOffset = currentFrameSequence->frame_size * frameIndex;
    
    for(int ledIndex = 0; ledIndex < currentFrameSequence->led_count; ledIndex++) {
      int ledOffset = frameOffset + (ledIndex * 3);
      int r = currentFrameSequence->frame_data[ledOffset + 0];
      int g = currentFrameSequence->frame_data[ledOffset + 1];
      int b = currentFrameSequence->frame_data[ledOffset + 2];
      
      //matrix.setPixelColor(ledIndex, matrix.gamma32(strip.Color(r,g,b)));
      setPixel(ledIndex,dim8_raw(r),dim8_raw(g),dim8_raw(b));
    }
    showStrip();
    delay(1000/currentFrameSequence->frame_rate);
    }
  return true;
}