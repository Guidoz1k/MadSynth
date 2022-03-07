#include "main.h"

// main loop delay variables
volatile uint8_t timing = 0;
volatile uint16_t time_counter = 0;

// delay function for the main loop
static void lag(uint16_t time){
    timing = 1;

    while(time_counter != time);

    timing = 0;
    time_counter = 0;
}

// 1ms interrupt function
ISR(TIMER0_COMPA_vect){
    if(timing)
        time_counter++;
}

// asynchronous main loop
void loop(){
    lag(500);
    led_toggle();
}

// main function
int main(){
    display_init();  // initialize the display pins and functions
    onboard_init();  // initialize the onboard components pins and functions
    serial_init();   // initialize the serial pins and functions

    timer_init();    // initialize the timer functions

    while(1){
        loop();
    }
}