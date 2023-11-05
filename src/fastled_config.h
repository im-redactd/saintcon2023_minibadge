// NeoMatrix 
#define LAST_VISIBLE_LED  71
#define kMatrixWidth      9          
#define kMatrixHeight     8          
#define NUM_LEDS (kMatrixWidth * kMatrixHeight)                                       
#define MAX_DIMENSION ((kMatrixWidth>kMatrixHeight) ? kMatrixWidth : kMatrixHeight)   
#define COLOR_ORDER     GRB           // If colours look wrong, play with this
#define CHIPSET         WS2812B       // LED strip type
#define MAX_MILLIAMPS   400          // Careful with the amount of power here if running off USB port
const int BRIGHTNESS_SETTINGS[3] = {5, 70, 200};  // 3 Integer array for 3 brightness settings (based on pressing+holding MODE_PIN)
#define LED_VOLTS       3             // Usually 5 or 12

// Helper to map XY coordinates to irregular matrix
// https://macetech.github.io/FastLED-XY-Map-Generator/

uint16_t XY( uint16_t x, uint16_t y)
{
  // any out of bounds address maps to the first hidden pixel
  if ( (x >= kMatrixWidth) || (y >= kMatrixHeight) ) {
    return (LAST_VISIBLE_LED + 1);
  }

  const uint8_t XYTable[] = {
    63,  64,  65,  66,  67,  68,  69,  70,  71,
    62,  61,  60,  59,  58,  57,  56,  55,  54,
    45,  46,  47,  48,  49,  50,  51,  52,  53,
    44,  43,  42,  41,  40,  39,  38,  37,  36,
    27,  28,  29,  30,  31,  32,  33,  34,  35,
    26,  25,  24,  23,  22,  21,  20,  19,  18,
     9,  10,  11,  12,  13,  14,  15,  16,  17,
     8,   7,   6,   5,   4,   3,   2,   1,   0
  };

  uint8_t i = (y * kMatrixWidth) + x;
  uint8_t j = XYTable[i];
  return j;
}

// Like XY, but for a mirror image from the top (used by misconfigured code)
int XY2( int x, int y, bool wrap=false) {
    wrap = wrap; // squelch compiler warning
    return XY(x,kMatrixHeight-1-y);
}

// FastLED stuff
CRGB leds[NUM_LEDS];
CRGB modeLED[4];

// define some shorthands for the Halloween colors
#define PURP 0x6611FF
#define ORAN 0xFF6600
#define GREN 0x00FF11
#define WHIT 0xCCCCCC


DEFINE_GRADIENT_PALETTE( purple_gp ) {
  0,   0, 212, 255,   //blue
255, 179,   0, 255 }; //purple
DEFINE_GRADIENT_PALETTE( outrun_gp ) {
  0, 141,   0, 100,   //purple
127, 255, 192,   0,   //yellow
255,   0,   5, 255 };  //blue
DEFINE_GRADIENT_PALETTE( greenblue_gp ) {
  0,   0, 255,  60,   //green
 64,   0, 236, 255,   //cyan
128,   0,   5, 255,   //blue
192,   0, 236, 255,   //cyan
255,   0, 255,  60 }; //green
DEFINE_GRADIENT_PALETTE( redyellow_gp ) {
  0,   200, 200,  200,   //white
 64,   255, 218,    0,   //yellow
128,   231,   0,    0,   //red
192,   255, 218,    0,   //yellow
255,   200, 200,  200 }; //white


CRGBPalette16 purplePal = purple_gp;
CRGBPalette16 outrunPal = outrun_gp;
CRGBPalette16 greenbluePal = greenblue_gp;
CRGBPalette16 heatPal = redyellow_gp;
//CRGBPalette16 halloweenPal = HalloweenColors_p;

uint8_t colorTimer = 0;


// This could also be defined as matrix->color(255,0,0) but those defines
// are meant to work for adafruit_gfx backends that are lacking color()
#define LED_BLACK		0

#define LED_RED_VERYLOW 	(3 <<  11)
#define LED_RED_LOW 		(7 <<  11)
#define LED_RED_MEDIUM 		(15 << 11)
#define LED_RED_HIGH 		(31 << 11)

#define LED_GREEN_VERYLOW	(1 <<  5)   
#define LED_GREEN_LOW 		(15 << 5)  
#define LED_GREEN_MEDIUM 	(31 << 5)  
#define LED_GREEN_HIGH 		(63 << 5)  

#define LED_BLUE_VERYLOW	3
#define LED_BLUE_LOW 		7
#define LED_BLUE_MEDIUM 	15
#define LED_BLUE_HIGH 		31

#define LED_ORANGE_VERYLOW	(LED_RED_VERYLOW + LED_GREEN_VERYLOW)
#define LED_ORANGE_LOW		(LED_RED_LOW     + LED_GREEN_LOW)
#define LED_ORANGE_MEDIUM	(LED_RED_MEDIUM  + LED_GREEN_MEDIUM)
#define LED_ORANGE_HIGH		(LED_RED_HIGH    + LED_GREEN_HIGH)

#define LED_PURPLE_VERYLOW	(LED_RED_VERYLOW + LED_BLUE_VERYLOW)
#define LED_PURPLE_LOW		(LED_RED_LOW     + LED_BLUE_LOW)
#define LED_PURPLE_MEDIUM	(LED_RED_MEDIUM  + LED_BLUE_MEDIUM)
#define LED_PURPLE_HIGH		(LED_RED_HIGH    + LED_BLUE_HIGH)

#define LED_CYAN_VERYLOW	(LED_GREEN_VERYLOW + LED_BLUE_VERYLOW)
#define LED_CYAN_LOW		(LED_GREEN_LOW     + LED_BLUE_LOW)
#define LED_CYAN_MEDIUM		(LED_GREEN_MEDIUM  + LED_BLUE_MEDIUM)
#define LED_CYAN_HIGH		(LED_GREEN_HIGH    + LED_BLUE_HIGH)

#define LED_WHITE_VERYLOW	(LED_RED_VERYLOW + LED_GREEN_VERYLOW + LED_BLUE_VERYLOW)
#define LED_WHITE_LOW		(LED_RED_LOW     + LED_GREEN_LOW     + LED_BLUE_LOW)
#define LED_WHITE_MEDIUM	(LED_RED_MEDIUM  + LED_GREEN_MEDIUM  + LED_BLUE_MEDIUM)
#define LED_WHITE_HIGH		(LED_RED_HIGH    + LED_GREEN_HIGH    + LED_BLUE_HIGH)

