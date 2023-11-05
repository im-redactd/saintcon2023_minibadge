
#include "Arduino.h"

//animations
#include "Ghost.h"
#include "Pacman.h"

// accellerometer stuff

class Steps {
  public:
    Steps(){};
    bool runPattern();
  private:
	long previousMotionCheck = 0;
	uint32_t step = 0;
	uint32_t last_step = 0;
	float last_xx = 0, last_yy = 0, last_zz = 0;
	float xx = 0, yy = 0, zz = 0;
	int32_t temp = 0;
	int noMotionCount = 0;
	int noMotionCountLimit = 10; // 10 seconds.
	int buttonMode = 0;

	typedef struct {
		uint32_t led_count;
		uint32_t frame_size;
		uint32_t frame_count;
		uint32_t frame_rate;
		uint8_t* frame_data;
		uint16_t* timing_data;
	} FrameSequence;

	FrameSequence frameSequenceGhost = {
		.led_count = Ghost_led_count,
		.frame_size = Ghost_frame_size,
		.frame_count = Ghost_frame_count,
		.frame_rate = Ghost_frame_rate,
		.frame_data = (uint8_t*)&Ghost,
		.timing_data = NULL
	};
	FrameSequence frameSequencePacman = {
		.led_count = Pacman_led_count,
		.frame_size = Pacman_frame_size,
		.frame_count = Pacman_frame_count,
		.frame_rate = Pacman_frame_rate,
		.frame_data = (uint8_t*)&Pacman,
		.timing_data = NULL
	};
	
	FrameSequence *animations[2] = {
		&frameSequenceGhost,
		&frameSequencePacman,
	};

	FrameSequence* currentFrameSequence = animations[currentAnimationIndex];

	uint8_t animationCount = 2;
	uint8_t currentAnimationIndex = 0;

};

bool Steps::runPattern() {
	
	// store the latest step count.
	step = getCurrentSteps();

	tickForPersistenceEvery30Sec();

	if(millis() - previousMotionCheck >= 100) {

		// every x millis update the x,y,z and check if its moved more than ~10. 
		// at idle the accel will vary +- 5 in x or y so we'll subtract the difference. 
		// for now we'll accept under 10 as drift.
		bma456.getAcceleration(&xx, &yy, &zz);
		
		if((xx - last_xx) > 10  || (yy - last_yy) > 10 || (zz - last_zz) > 10 ){
			last_xx = xx;
			last_yy = yy;
			noMotionCount = 0;
			currentFrameSequence = animations[1]; // pacman

		} else {
			last_xx = xx;
			last_yy = yy;
			noMotionCount +=1;
			if(noMotionCount > noMotionCountLimit) currentFrameSequence = animations[0]; // ghost
		}
		previousMotionCheck = millis();
	}

	// mode 0 is the pacman/ghost motion detection
	// mode 1 is the text display of your step count
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
				
				if (modeInterruptRequested) {
					modeInterruptRequested = false;
					Serial.println("FS mode interupt");
					return false;
				}
				if (buttonInterruptRequested) {
					buttonMode = 1;
					buttonInterruptRequested = false;
					break;
				}
				delay(1);
			}
		}
	}
		else {
			
			// static set delay for text
			int16_t frame_delay = 75;

			uint8_t size = max(int(kMatrixWidth/8), 1);
			matrix->setTextSize(size);
    		if (steps<100)
			{
				matrix->setTextColor(LED_RED_MEDIUM);
			}
			else if (steps<500)
			{
				matrix->setTextColor(LED_PURPLE_MEDIUM);
			}
			else if (steps<1000)
			{
				matrix->setTextColor(LED_CYAN_MEDIUM);
			}
			else if (steps>=1000)
			{
				matrix->setTextColor(LED_GREEN_MEDIUM);
			}
			


			matrix_clear();
			matrix->setTextWrap(false);  // we don't wrap text so it scrolls nicely
			matrix->setTextSize(1);
			matrix->setRotation(0);
			
			Serial.println(steps);
			Serial.println(floor(log10(abs(steps))) + 1);
			int step_length = floor(log10(abs(steps))) + 1;

			for (int16_t x=12*size; x>=-(75+(step_length*6)); x--) {
				yield();
				matrix_clear();
				matrix->setCursor(x,kMatrixWidth/2-size*4);
				matrix->print(String(char(3)) + " steps " + step + " " + String(char(3)));
				matrix_show();
				// note that on a big array the refresh rate from show() will be slow enough that
      			// the delay become irrelevant. This is already true on a 32x32 array.
    	    	// loop the frame delay so we can listen for buttons.
				for (size_t tick = 0; tick < frame_delay; tick++)
				{
					modeBtn.read();
					
					if (modeInterruptRequested) {
						modeInterruptRequested = false;
						Serial.println("FS mode interupt");
						return false;
					}
					if (buttonInterruptRequested) {
						buttonMode = 0;
						buttonInterruptRequested = false;
						break;
					}
					
					delay(1);
				}
	    	}

			// loop the frame delay so we can listen for buttons.
			for (size_t tick = 0; tick < frame_delay; tick++)
			{
				modeBtn.read();
				
				if (modeInterruptRequested) {
					modeInterruptRequested = false;
					Serial.println("FS mode interupt");
					return false;
				}
				if (buttonInterruptRequested) {
					buttonMode = 0;
					buttonInterruptRequested = false;
					break;
				}
				
				delay(1);
			}
		}
		

        //step = bma456.getStepCounterOutput();
        // bma456.getAcceleration(&xx, &yy, &zz);
        // temp = bma456.getTemperature();
        // Serial.print("X: ");
        // Serial.print(xx);
        // Serial.print(", Y: ");
        // Serial.print(yy);
        // Serial.print(", Z: ");
        // Serial.print(zz);
        // Serial.print(", T: ");
        // Serial.println(temp);
        //Serial.println("Steps: " + String(step));

	
	return true;
}