#include <Arduino.h>
#include <RTTStream.h>
RTTStream rtt;

// cap touch

#define N_SAMPLES 10
#define TIMEOUT_TICKS 10000
#define TOUCH_DETECT 100
#define MODE_PIN   11

long lastTime = 0;


static uint16_t get_raw_reading() {

    uint16_t ticks = 0;
   
    for (uint16_t i = 0; i < N_SAMPLES; i++) {

        // set pad to digital output high for 10us to charge it
        pinMode(MODE_PIN,OUTPUT);
        digitalWrite(MODE_PIN, HIGH);
        sleep_us(10);

        // set pad back to an input and take some samples

        pinMode(MODE_PIN,INPUT);

        while (digitalRead(MODE_PIN)) {
            if (ticks >= TIMEOUT_TICKS) {
                return TIMEOUT_TICKS;
            }
            ticks++;
        }
    }
    return ticks;
}


void checkForTouch() {

    u_int16_t check_touch = get_raw_reading();
    if(check_touch > 120) {
        
        if (millis() - lastTime > 50) {

            lastTime = millis();
            rtt.println("touched.");
            rtt.print(check_touch);
            rtt.println("");
            
            modeInterruptRequested = true;
            
        }
        else {
            lastTime = millis();
            //rtt.println("debounce");
        }
    }
    else if(check_touch > 200) {
        rtt.println("touch detected.");
    }

}
