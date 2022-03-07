#include "onboard.h"

/*
PORTB 7     ==> onboard LED
*/

static uint8_t led_state = 0;

void ledTOGGLE(void){
    if(led_state)
        ledOFF();
    else
        ledON();

}

void ledOUTPUT(uint8_t bit){
    if(bit)
        ledON();
    else
        ledOFF();
}

void ledOFF(void){
    led_state = 0;
    PORTB &= 0b01111111;    // resets PORTB bit 7
}

void ledON(void){
    led_state = 1;
    PORTB |= 0b10000000;    // sets PORTB bit 7
}

void onboardInit(){
	DDRB |= 0b10000000;	// PORTB 7 OUTPUT
    ledOFF();
}
