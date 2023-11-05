// #include <Arduino.h>
// #include <RTTStream.h>
// RTTStream rtt;
//#include "elapsed.h"
#include "pico/stdlib.h"

// cap touch

#define N_SAMPLES 10
#define TIMEOUT_TICKS 10000
#define TOUCH_DETECT 100
#define MODE_PIN   11

long lastTime = 0;
int start = 0;


static uint16_t get_raw_reading() {

    uint16_t ticks = 0;
   
    for (uint16_t i = 0; i < N_SAMPLES; i++) {

        // set pad to digital output high for 10us to charge it
        pinMode(MODE_PIN,OUTPUT);
        // gpio_set_dir(MODE_PIN,OUTPUT);
        // gpio_put(MODE_PIN,true);
        digitalWrite(MODE_PIN, HIGH);
        sleep_us(20);

        // set pad back to an input and take some samples

        pinMode(MODE_PIN,INPUT);
        //gpio_set_dir(MODE_PIN,INPUT);

        while (digitalRead(MODE_PIN)) {
        //while (gpio_get(MODE_PIN)) {
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
    
    if(check_touch > 820) {
        
        //modeInterruptRequested = true;

        //if (millis() - lastTime > 50) {
        if (to_ms_since_boot(get_absolute_time()) - start > 50 ) {
            //lastTime = millis();
            start = to_ms_since_boot(get_absolute_time());
            //rtt.println("touched.");
            //rtt.print(check_touch);
            //rtt.println("");
            
            modeInterruptRequested = true;
            
        }
        else {
            //lastTime = millis();
            start = to_ms_since_boot(get_absolute_time());
            //rtt.println("debounce");
        }
    }
    else if(check_touch > 200) {
        //rtt.println("touch detected.");

    }

}
