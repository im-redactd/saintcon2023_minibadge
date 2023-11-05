
#include "Arduino.h"

//animations
#include "Fireworks.h"
#include "MagicSpellOrb.h"
#include "ElectricBranches.h"
#include "RainbowSlinky.h"
#include "HexTubeAlt.h"
#include "Honk.h"
//#include "Hamster.h"
#include "trianglerainbow.h"
#include "heartthrob.h"
#include "copperdoo.h"
#include "radial_convergence.h"
#include "Goots.h"
#include "spinny.h"
#include "insomnia_strikes.h"
#include "candy_corn_rain.h"
#include "blood_helix.h"
#include "hex_doctor.h"
#include "snowflake_lattice.h"
#include "hypermographnetworking.h"
#include "marching_ants.h"
#include "RainbowTorusSpirals.h"

class Motion {
  public:
    Motion(){};
    bool runPattern();
  private:
	int test;
	typedef struct {
		uint32_t led_count;
		uint32_t frame_size;
		uint32_t frame_count;
		uint32_t frame_rate;
		uint8_t* frame_data;
		uint16_t* timing_data;
	} FrameSequence;

	FrameSequence frameSequenceRainbowTorus = {
		.led_count = RainbowTorusSpirals_led_count,
		.frame_size = RainbowTorusSpirals_frame_size,
		.frame_count = RainbowTorusSpirals_frame_count,
		.frame_rate = RainbowTorusSpirals_frame_rate,
		.frame_data = (uint8_t*)&RainbowTorusSpirals,
		.timing_data = NULL
	};

	FrameSequence frameSequenceHypermoGraphine = {
		.led_count = hypermographnetworking_led_count,
		.frame_size = hypermographnetworking_frame_size,
		.frame_count = hypermographnetworking_frame_count,
		.frame_rate = hypermographnetworking_frame_rate,
		.frame_data = (uint8_t*)&hypermographnetworking,
		.timing_data = NULL
	};

	FrameSequence frameSequenceMarchingAnts = {
		.led_count = marching_ants_led_count,
		.frame_size = marching_ants_frame_size,
		.frame_count = marching_ants_frame_count,
		.frame_rate = marching_ants_frame_rate,
		.frame_data = (uint8_t*)&marching_ants,
		.timing_data = NULL
	};

	FrameSequence frameSequenceHexDoctor = {
		.led_count = hex_doctor_led_count,
		.frame_size = hex_doctor_frame_size,
		.frame_count = hex_doctor_frame_count,
		.frame_rate = hex_doctor_frame_rate,
		.frame_data = (uint8_t*)&hex_doctor,
		.timing_data = NULL
	};

	FrameSequence frameSequenceSnowflakeLattice = {
		.led_count = snowflake_lattice_led_count,
		.frame_size = snowflake_lattice_frame_size,
		.frame_count = snowflake_lattice_frame_count,
		.frame_rate = snowflake_lattice_frame_rate,
		.frame_data = (uint8_t*)&snowflake_lattice,
		.timing_data = NULL
	};

	FrameSequence frameSequenceBloodHelix = {
		.led_count = blood_helix_led_count,
		.frame_size = blood_helix_frame_size,
		.frame_count = blood_helix_frame_count,
		.frame_rate = blood_helix_frame_rate,
		.frame_data = (uint8_t*)&blood_helix,
		.timing_data = NULL
	};

	FrameSequence frameSequenceCandyCorn = {
		.led_count = candy_corn_rain_led_count,
		.frame_size = candy_corn_rain_frame_size,
		.frame_count = candy_corn_rain_frame_count,
		.frame_rate = candy_corn_rain_frame_rate,
		.frame_data = (uint8_t*)&candy_corn_rain,
		.timing_data = NULL
	};

	FrameSequence frameSequenceInsomnia = {
		.led_count = insomnia_strikes_led_count,
		.frame_size = insomnia_strikes_frame_size,
		.frame_count = insomnia_strikes_frame_count,
		.frame_rate = insomnia_strikes_frame_rate,
		.frame_data = (uint8_t*)&insomnia_strikes,
		.timing_data = NULL
	};

	FrameSequence frameSequenceSpinny = {
		.led_count = spinny_led_count,
		.frame_size = spinny_frame_size,
		.frame_count = spinny_frame_count,
		.frame_rate = spinny_frame_rate,
		.frame_data = (uint8_t*)&spinny,
		.timing_data = NULL
	};
	FrameSequence frameSequenceGoots = {
		.led_count = Goots_led_count,
		.frame_size = Goots_frame_size,
		.frame_count = Goots_frame_count,
		.frame_rate = Goots_frame_rate,
		.frame_data = (uint8_t*)&Goots,
		.timing_data = (uint16_t*)&Goots_frame_timing
	};

	FrameSequence frameSequenceHonk = {
		.led_count = Honk_led_count,
		.frame_size = Honk_frame_size,
		.frame_count = Honk_frame_count,
		.frame_rate = Honk_frame_rate,
		.frame_data = (uint8_t*)&Honk,
		.timing_data = (uint16_t*)&Honk_frame_timing
	};

	// FrameSequence frameSequenceHamster = {
	// 	.led_count = Hamster_led_count,
	// 	.frame_size = Hamster_frame_size,
	// 	.frame_count = Hamster_frame_count,
	// 	.frame_rate = Hamster_frame_rate,
	// 	.frame_data = (uint8_t*)&Hamster,
	// 	.timing_data = NULL
	// };
	
	
	FrameSequence frameSequencRadialConvergence = {
		.led_count = radial_convergence_led_count,
		.frame_size = radial_convergence_frame_size,
		.frame_count = radial_convergence_frame_count,
		.frame_rate = radial_convergence_frame_rate,
		.frame_data = (uint8_t*)&radial_convergence,
		.timing_data = NULL
	};
	
	FrameSequence frameSequenceTriangleRainbow = {
		.led_count = triangle_rainbow_led_count,
		.frame_size = triangle_rainbow_frame_size,
		.frame_count = triangle_rainbow_frame_count,
		.frame_rate = triangle_rainbow_frame_rate,
		.frame_data = (uint8_t*)&triangle_rainbow,
		.timing_data = NULL
	};
	FrameSequence frameSequencHeart = {
		.led_count = heart_led_count,
		.frame_size = heart_frame_size,
		.frame_count = heart_frame_count,
		.frame_rate = heart_frame_rate,
		.frame_data = (uint8_t*)&heart,
		.timing_data = NULL
	};
	FrameSequence frameSequenceBlueWaves = {
		.led_count = RainbowSlinky_led_count,
		.frame_size = RainbowSlinky_frame_size,
		.frame_count = RainbowSlinky_frame_count,
		.frame_rate = RainbowSlinky_frame_rate,
		.frame_data = (uint8_t*)&RainbowSlinky,
		.timing_data = NULL
	};
	FrameSequence frameSequenceElectricBranches = {
		.led_count = ElectricBranches_led_count,
		.frame_size = ElectricBranches_frame_size,
		.frame_count = ElectricBranches_frame_count,
		.frame_rate = ElectricBranches_frame_rate,
		.frame_data = (uint8_t*)&ElectricBranches,
		.timing_data = NULL
	};
	FrameSequence frameSequenceMagic = {
		.led_count = MagicSpellOrb_led_count,
		.frame_size = MagicSpellOrb_frame_size,
		.frame_count = MagicSpellOrb_frame_count,
		.frame_rate = MagicSpellOrb_frame_rate,
		.frame_data = (uint8_t*)&MagicSpellOrb,
		.timing_data = NULL
	};
	FrameSequence frameSequenceFireworks = {
		.led_count = Fireworks_led_count,
		.frame_size = Fireworks_frame_size,
		.frame_count = Fireworks_frame_count,
		.frame_rate = Fireworks_frame_rate,
		.frame_data = (uint8_t*)&Fireworks,
		.timing_data = NULL
	};

	FrameSequence frameSequenceCopperdoo = {
		.led_count = cooperdoo_led_count,
		.frame_size = cooperdoo_frame_size,
		.frame_count = cooperdoo_frame_count,
		.frame_rate = cooperdoo_frame_rate,
		.frame_data = (uint8_t*)&cooperdoo,
		.timing_data = NULL
	};
	FrameSequence *animations[15] = {
		&frameSequenceTriangleRainbow,
		&frameSequencHeart,
		&frameSequenceSpinny,
		&frameSequenceGoots,
		&frameSequencRadialConvergence,
		&frameSequenceCandyCorn,
		&frameSequenceBloodHelix,
		&frameSequenceInsomnia,
		&frameSequenceMagic,
		&frameSequenceCopperdoo,
		&frameSequenceHexDoctor,
		&frameSequenceSnowflakeLattice,
		&frameSequenceHypermoGraphine,
		&frameSequenceMarchingAnts,
		&frameSequenceRainbowTorus
	};

	FrameSequence* currentFrameSequence = animations[currentAnimationIndex];

	uint8_t animationCount = 15;
	uint8_t currentAnimationIndex = 0;

};

bool Motion::runPattern() {
	

	for(
		uint32_t frameIndex = 0;
		frameIndex < currentFrameSequence->frame_count;
		frameIndex += 1
	) {
		//int frameOffset = currentFrameSequence->frame_size * frameIndex;
		int frameOffset = frameIndex * currentFrameSequence->frame_size;
		//RainbowSlinky_frame_size;

		for(int y = 0; y < kMatrixHeight; y++) {
			for(int x = 0; x < kMatrixWidth; x++) {
				int colorGridIndex = (y * kMatrixWidth) + x;
				int colorGridOffset = frameOffset + (colorGridIndex * 3);
				int r = currentFrameSequence->frame_data[colorGridOffset + 0];
				int g = currentFrameSequence->frame_data[colorGridOffset + 1];
				int b = currentFrameSequence->frame_data[colorGridOffset + 2];
				leds[XY(x, y)] = CRGB(dim8_raw(r),dim8_raw(g),dim8_raw(b));
				
			}
		}
		FastLED.show();
		//yield();

		// we can no longer just delay for the whole time chunk and then
		// read button input at the start of next frame, we need to read
		// the button while we count down before displaying the next frame.
		int16_t frame_delay = (currentFrameSequence->timing_data == NULL)
			? 1000/currentFrameSequence->frame_rate
			: currentFrameSequence->timing_data[frameIndex];
		for (size_t tick = 0; tick < frame_delay; tick++)
		{
			checkForTouch();
			//tickForPersistenceEvery30Sec();

			if (modeInterruptRequested) {
				modeInterruptRequested = false;
				if(currentAnimationIndex == (animationCount-1)) {
					currentAnimationIndex = 0;
					return false;
				}
				else if (on_badge && !on_badge_motion) {
					on_badge_motion = true;
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
			delay(1);
		}
    }
	return true;
}