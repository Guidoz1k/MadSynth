#include "onboard.h"
/*
    PORTB 7 --> onboard LED
*/

static uint8_t led_state = 0;

void led_toggle(void){
    if(led_state)
        led_off();
    else
        led_on();

}

void led_output(uint8_t bit){
    if(bit)
        led_on();
    else
        led_off();
}

void led_off(void){
    led_state = 0;
    PORTB &= 0b01111111;    // resets PORTB bit 7
}

void led_on(void){
    led_state = 1;
    PORTB |= 0b10000000;    // sets PORTB bit 7
}

void onboard_init(){
	DDRB |= 0b10000000;	// PORTB 7 OUTPUT
    led_off();
}
