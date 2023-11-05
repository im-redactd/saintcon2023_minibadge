#include "Arduino.h"

class Fireworks2 {
  public:
    Fireworks2(){};
    bool runPattern();
  private:
    long previousTime = 0;
};


void screenscale( accum88 hor, accum88 ver, fract8& pixel, fract8& up, fract8& right, fract8& diag) // 3 Pointers to resulting fract8 variables
{
	fract8 x_fract = (hor >> 7) & 15;        // Extracts only the fractional part (in 1/16th) of the pixel position
	fract8 y_fract = (ver >> 7) & 15;

	diag = x_fract * y_fract;                 // Returns a number representing the proportion (in 1/256th) for each of the 4 pixels
	up = (16 - x_fract) * y_fract;  
	right = (16 - y_fract) * x_fract;
	pixel = 256 - (16 * x_fract) - (16 * y_fract) + diag;   
}

bool isSetup = false;

uint8_t gGravity = 20;
uint8_t  gDrag = 1;

accum88 gBurstx;
accum88 gBursty;
saccum78 gBurstxv;
saccum78 gBurstyv;
CRGB gBurstcolor;

#define NONE 0
#define SHELL 1
#define SPARK 2
#define EXPLODING 3
#define BURSTING 4

int wrapX(int x) {
    if (x < 0 ) return 0;
    if (x >= kMatrixWidth) return (kMatrixWidth-1);
    return x;
}

CRGB& setXY( int x, int y) 
{
	// This code is reversed in Y axis, so XY2 fixes this.
	return leds[XY2(wrapX(x),y)];
}


uint16_t speed = 100;

class Dot {
	public:
	int8_t	show;
	int8_t	theType;
	accum88 x;
	accum88 y;
	saccum78 xv;
	saccum78 yv;
	CRGB    color;

	Dot() {						// Constructor for the Dot class
		show = 0;
		theType = 0;
		x =  0;
		y =  0;
		xv = 0;
		yv = 0;
		color.setRGB( 0, 0, 0);
	}


	void Draw() {
		fract8 p00, p01, p10, p11;		// percentage of pixel spread to adjacent pixels
    
		screenscale( x, y, p00, p01, p10, p11);

		uint16_t x_pos = x >> 11;		// Scaling to get x pixel position 0 - 32
		x_pos = map(x_pos, 0, 32, 0, kMatrixWidth-1);	// Scale to Matrix width

		uint16_t y_pos = y >> 10;		// Scaling to get y pixel position 0 - 64
		y_pos = map(y_pos, 0, 64, 0, kMatrixHeight-1);	// Scale to Matrix height

		if (yv > 0){				// In case of equal values, just adding 1 or 2 to any pixel's percentage
			if (p00 == p01) p01++;		// will ensure than only one pixel will have a higher percentage than  
			if (p10 == p11) p11++;		// any of the other 3
		}
		if (yv < 0){
			if (p00 == p01) p00++;
			if (p10 == p11) p10++;
		}
		if (xv > 0){
			if (p00 == p10) p10++;
			if (p01 == p11) p11++;
		}
		if (xv < 0){
			if (p00 == p10) p00++;
			if (p01 == p11) p01++;
		}
   
		if( !show) return;

		if ((p00 > p01) && (p00 > p10) && (p00 > p11)) setXY(x_pos, y_pos) += color;
		if ((p01 > p00) && (p01 > p10) && (p01 > p11)) setXY(x_pos, y_pos + 1) += color;
		if ((p10 > p00) && (p10 > p01) && (p10 > p11)) setXY(x_pos + 1, y_pos) += color;
		if ((p11 > p00) && (p11 > p01) && (p11 > p10)) setXY(x_pos + 1, y_pos + 1) += color;
		if ((p00 > p10) && (p01 > p10) && (p11 > p10)) setXY(x_pos - 1, y_pos) += color;
	    
	}       // End of draw function

	void Move() {
//		if( !show) return;

		if(((xv >  0) && (x+xv < xv)) || ((xv < 0) && (x+xv > xv))) show = 0;	// Prevents pixels wraparounds from side to side
		if(((yv >  0) && (y+yv < yv)))  show = 0;				// Prevents pixels wraparounds from top to bottom
		if( yv < 0 && (y < (-yv)) ) show = 0;					// If velocity is negative AND we are about to hit the ground 

		if (theType == EXPLODING) {						// If the Shell has exploded...
	  
			show = 0;
			theType = BURSTING;						// Prepare for a burst on next frame 
//			gBurstx = x;                           
//			gBursty = y;
//			gBurstxv = xv;
//			gBurstyv = yv;
//			hsv2rgb_rainbow( CHSV( random8(), 240, 200), gBurstcolor);    
		} 
   
		if ((yv < 0) && (theType == SHELL)) {		// If velocity is negative AND it is a shell...
			xv=0;					// Stop further movements
			yv=0;
			color = CRGB(255, 255, 255);		// Set full brightness white color
			theType = EXPLODING;			// Explode the shell !! 
		}

		x += xv;
		y += yv;

		yv -= gGravity;
		if (xv>0) xv -= gDrag;
		if (xv<0) xv += gDrag;
		if (yv>0) yv -= gDrag;
		if (yv<0) yv += gDrag;

	}							// End of the move function 

	void GroundLaunch() {
		gGravity = map8(speed, 0, 6)+3;
		// Vertical velocity = Minimum velocity + Random maximum difference
		if (kMatrixHeight <= 32) yv = ((20*(kMatrixHeight+(3*(gGravity*0.8))))-(kMatrixHeight*5)) + random16(kMatrixHeight*5);	
		else if (kMatrixHeight <= 64)  yv = ((14*(kMatrixHeight+(3*(gGravity*0.8))))-(kMatrixHeight*5)) + random16(kMatrixHeight*3);
		else yv = ((10*(kMatrixHeight+(1.5*(gGravity*0.8))))-(kMatrixHeight*2)) + random16(kMatrixHeight*2);
		xv = random16(350) - 175;			// Generates a signed int value between +/- 175  (Nice width but always inside of frame)      
		y = 0x8000;						// Ground launch
		//x = random16(); 				// Horizontal
		x = random16(0x8000)+0x2000;			// Horizontal middle 7/8 of the matrix
//		x = random16(0x4000)+0x2000;			// Horizontal middle 2/4 of the matrix
//		x = 0x4000;					// Horizontal middle of the matrix
#ifdef NEOMATRIX
		color.setRGB(24,24,24);				// Shells are white color' as a CRGB
#else
		color.setRGB(255,255,255);				// Shells are white color' as a CRGB
#endif
		theType = SHELL;
		show = 1;
	}							// End of Groundlaunch function

	void Skyburst( accum88 basex, accum88 basey, CRGB& basecolor) {
		yv = random16(400) - 100;			// Random number (Y-velocity) between +/- 200
		xv = random16(400) - 100;			// Random number (X-velocity) between +/- 200
		y = basey;
		x = basex;
		color = basecolor;
		color *= 8;
		theType = SPARK;
		show = 1;
	}							// End of Skyburst function
};								// End of Dot class definition

#define MAX_SHELLS 2//(kMatrixHeight/7)
#define MIN_SHELLS 1 //(kMatrixHeight/9)
#define MAX_SPARKS 20
#define MIN_SPARKS 5

Dot *gDot;		// Creates an object named gDot of type Dot class
//Dot gDot[MAX_SHELLS];
Dot *gSparks;		// Creates an array object named gSparks of type Dot class
//Dot gSparks[MAX_SHELLS*MAX_SPARKS];			//Creates an array object named gSparks of type Dot class

void show_free_mem(const char *pre=NULL) {
    Framebuffer_GFX::show_free_mem(pre);
}

void *mallocordie(const char *varname, uint32_t req, bool psram=true) {
    // If varname starts with @, show debug for the allocation
    void *mem;
    #ifndef BOARD_HAS_PSRAM
    psram = false;
#endif
    if (varname[0] == '@') {
        if (psram) Serial.print("PS");
        Serial.print("Malloc ");
        Serial.print(varname);
        Serial.print(" . Requested bytes: ");
        Serial.println(req);
    }
#ifdef ESP32
    if (psram) {
        mem = ps_malloc(req);
    } else {
        mem = calloc(1, req);
    }
#else
    mem = calloc(1, req);
#endif

    if (mem) {
        return mem;
    } else {
        show_free_mem();
        Serial.println("vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv");
        Serial.print("FATAL: ");
        if (psram) Serial.print("ps_");
        Serial.print("malloc failed for ");
        Serial.print(varname);
        Serial.print(" . Requested bytes: ");
        Serial.println(req);
        Serial.println("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^");
        while (1); // delay(1);  Adding this seems to cause an ESP32 bug
    }
    return NULL;
}

void fireworks_setup() {
    gDot    = (Dot *) mallocordie("Fireworks gDot", sizeof(Dot) * MAX_SHELLS);
    gSparks = (Dot *) mallocordie("Fireworks gSparks", sizeof(Dot) * MAX_SHELLS * MAX_SPARKS);
}

void fireworks() 
{
// Background sky color (brightness needs to be low, but not too low, backend dependent)
	//CRGB sky1(0,0,17);
	
	CRGB sky1(0,0,32);				
	CRGB sky2(32,32,64);				

	//CRGB sky1(0,0,48);				// Background sky color
	//CRGB sky2(64,64,128);				// Alternate sky color to create a star twinkle effect 


	for( uint16_t h = 0; h < kMatrixWidth; h++) {	// All leds will be set to 'sky1' (very dark blue) 
		for( int v = 0; v < kMatrixHeight; v++) {
			leds[XY(h,v)] = sky1;
		}
	}

	if( random8() < 48 ) {
		// Around once each 8 frames, a random pixel is set to 'sky2' 
		leds[XY(random16(kMatrixWidth),random16(kMatrixHeight))] = sky2;	
	}

	static uint8_t launchcountdown = 0;
	static uint16_t re_launchcountdown = 0;
     
	if(gDot[MAX_SHELLS-1].show == 0) {		// If the last shell has exploded
		re_launchcountdown--;			// Count down for next re_launch
		if (re_launchcountdown > 50000) {
			re_launchcountdown = 0;
		}
	}

	for( int a = 0; a < MAX_SHELLS; a++) {		// Always moves and draws the MAX number of sparks not the actual number of sparks (Wasteful!?)
		if(re_launchcountdown == 0) {
			if(gDot[a].show == 0) {
				if(launchcountdown == 0) {
					gDot[a].GroundLaunch();
					launchcountdown = random16(50) + 10;
				} else {
					launchcountdown--;
					if (launchcountdown > 254) {
						launchcountdown = 0;
					}
				}
			} 
		}

		if(gDot[MAX_SHELLS-1].show == 1) {
			//re_launchcountdown = random16(400) + 100;	// Last SHELL has launched, restart the relaunch timer
			re_launchcountdown = random16(100);	// Last SHELL has launched, restart the relaunch timer
		}

		if( gDot[a].theType == EXPLODING) {
			gDot[a].color.setRGB(255,255,255);
		}

		if( gDot[a].theType == BURSTING) {
			hsv2rgb_rainbow( CHSV( random8(), 255, random8(64,254)), gBurstcolor);
			gBurstx = gDot[a].x;
			gBursty = gDot[a].y;
			int nsparks = random8(MIN_SPARKS, MAX_SPARKS+1);
			for( int b = 0; b < nsparks; b++) {
				gSparks[a*MAX_SPARKS+b].Skyburst( gBurstx, gBursty, gBurstcolor);
			}
			gDot[a].theType = SPARK;
		}

		gDot[a].Move();					// Calculate the next position of the dots 
		gDot[a].Draw();					// Scale the position of the shell on the LED matrix 

		for( int b = 0; b < MAX_SPARKS; b++) {		// Always moves and draws the MAX number of sparks not the actual number of sparks (Wasteful!?)
			gSparks[a*MAX_SPARKS+b].Move();
			gSparks[a*MAX_SPARKS+b].color.r = gSparks[a*MAX_SPARKS+b].color.r * 255 /256;
			gSparks[a*MAX_SPARKS+b].color.g = gSparks[a*MAX_SPARKS+b].color.g * 255 /256;
			gSparks[a*MAX_SPARKS+b].color.b = gSparks[a*MAX_SPARKS+b].color.b * 255 /256;
			gSparks[a*MAX_SPARKS+b].Draw();
		}
	}
}

bool Fireworks2::runPattern() {

    if (!isSetup) {
        fireworks_setup();
        isSetup = true;
    }

    checkForTouch();
    //tickForPersistenceEvery30Sec();

   if (modeInterruptRequested) {
        modeInterruptRequested = false;
        Serial.println("FS mode interupt");
        return false;
    }
    
    
    fireworks();
    showStrip();
    
    delay(100/FRAMES_PER_SECOND);




  return true;
}