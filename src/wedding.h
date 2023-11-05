
#include "Arduino.h"

//animations
#include "Heart.h"

class Wedding {
  public:
    Wedding(){};
    bool runPattern();
  private:
	int buttonMode = 0;
	

	typedef struct {
		uint32_t led_count;
		uint32_t frame_size;
		uint32_t frame_count;
		uint32_t frame_rate;
		uint8_t* frame_data;
		uint16_t* timing_data;
	} FrameSequence;

	FrameSequence frameSequenceHeart = {
		.led_count = Heart_led_count,
		.frame_size = Heart_frame_size,
		.frame_count = Heart_frame_count,
		.frame_rate = Heart_frame_rate,
		.frame_data = (uint8_t*)&Heart,
		.timing_data = NULL
	};
	
	FrameSequence *animations[1] = {
		&frameSequenceHeart,
	};

	FrameSequence* currentFrameSequence = animations[currentAnimationIndex];

	uint8_t animationCount = 1;
	uint8_t currentAnimationIndex = 0;

};

bool Wedding::runPattern() {
	
	modeBtn.read();
	spoonBtn.read();
	tickForPersistenceEvery30Sec();
	
	// mode 0 is the heart. 
	// mode 1 is the text display
	// mode 2 is for Ryan and Beth

	if(buttonMode == 0) {
		for(
			uint32_t frameIndex = 0;
			frameIndex < currentFrameSequence->frame_count;
			frameIndex += 1
		) {
			int frameOffset = frameIndex * currentFrameSequence->frame_size;
			for(int y = 0; y < kMatrixHeight; y++) {
				for(int x = 0; x < kMatrixWidth; x++) {
					int colorGridIndex = (y * kMatrixWidth) + x;
					int colorGridOffset = frameOffset + (colorGridIndex * 3);
					int r = currentFrameSequence->frame_data[colorGridOffset + 0];
					int g = currentFrameSequence->frame_data[colorGridOffset + 1];
					int b = currentFrameSequence->frame_data[colorGridOffset + 2];
					leds[XY(x, y)] = CRGB(r,g,b);
				}
			}
			FastLED.show();

			// we can no longer just delay for the whole time chunk and then
			// read button input at the start of next frame, we need to read
			// the button while we count down before displaying the next frame.
			int16_t frame_delay = (currentFrameSequence->timing_data == NULL)
				? 1000/currentFrameSequence->frame_rate
				: currentFrameSequence->timing_data[frameIndex];
			for (size_t tick = 0; tick < frame_delay; tick++)
			{
				modeBtn.read();
				spoonBtn.read();
				
				if (modeInterruptRequested) {
					modeInterruptRequested = false;
					Serial.println("FS mode interupt");
					return false;
				}
				if (buttonInterruptRequested) {
					buttonMode += 1;
					buttonInterruptRequested = false;
					break;
				}
				delay(1);
			}
		}
	}
	else if(buttonMode == 1) {
			
			// static set delay for text
			int16_t frame_delay = 75;

			uint8_t size = max(int(kMatrixWidth/8), 1);
			matrix->setTextSize(size);
    		matrix->setTextColor(LED_PURPLE_MEDIUM);

			matrix_clear();
			matrix->setTextWrap(false);  // we don't wrap text so it scrolls nicely
			matrix->setTextSize(1);
			matrix->setRotation(0);
			for (int16_t x=8*size; x>=-6*14*size; x--) {
				matrix_clear();
				matrix->setCursor(x,kMatrixWidth/2-size*4);
				matrix->print("  beth + ryan");
				matrix_show();
				// note that on a big array the refresh rate from show() will be slow enough that
      			// the delay become irrelevant. This is already true on a 32x32 array.
    	    	// loop the frame delay so we can listen for buttons.
				for (size_t tick = 0; tick < frame_delay; tick++)
				{
					modeBtn.read();
					spoonBtn.read();
					
					if (modeInterruptRequested) {
						modeInterruptRequested = false;
						Serial.println("FS mode interupt");
						return false;
					}
					if (buttonInterruptRequested) {
						if (groom || bride) {
							buttonMode += 1;
						}
						else {
							#ifdef WE_HAVE_A_FRIEND
							modePushCounter = 12;
							return false;

							#else
							buttonMode = 0;
							#endif

						}

						buttonInterruptRequested = false;
						return true; // false;
						break;
					}
					
					delay(1);
				}
	    	}
		}
		
		else if(buttonMode == 2) {
			
			
			// static set delay for text
			int16_t frame_delay = 50;

			uint8_t size = max(int(kMatrixWidth/12), 1);
			matrix->setTextSize(size);
    		matrix->setTextColor(LED_PURPLE_MEDIUM);

			//matrix_clear();
			matrix->setTextWrap(false);  // we don't wrap text so it scrolls nicely
			matrix->setTextSize(1);
			matrix->setRotation(0);
			for (int16_t x=8*size; x>=-6*5*size; x--) {
				matrix_clear();
				Sparkle3(random(255), random(255), random(255), 25);
				matrix->setCursor(x,kMatrixWidth/2-size*4);
				matrix->print("  Mrs.");
				matrix_show();
			// loop the frame delay so we can listen for buttons.
			for (size_t tick = 0; tick < frame_delay; tick++)
			{
				modeBtn.read();
				spoonBtn.read();
				
				if (modeInterruptRequested) {
					modeInterruptRequested = false;
					Serial.println("FS mode interupt");
					return false;
				}
				if (buttonInterruptRequested) {
					buttonMode = 0;
					buttonInterruptRequested = false;
					return true;
					break;
				}
				
				delay(1);
			}
			}
		}
	return true;
}