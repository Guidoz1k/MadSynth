#include "ui.h"
/*
    PORTB:
        0   encoder pin A
        1   encoder pin B
    	2   encoder button
    	3   UP button
    	4   DOWN button
    	5   RIGHT button
    	6   LEFT button
*/

static volatile uint8_t pin_a;
static volatile uint8_t pin_b;
static int8_t rotation = 0;

// any of the two encoder pins switched
ISR(PCINT0_vect){
    uint8_t new_a = 0;
    uint8_t new_b = 0;

    if(PINB & 0b00000001)
        new_a = 1;
    if(PINB & 0b00000010)
        new_b = 1;

    if(new_a != pin_a){     // pin a switched
        if(new_a != pin_b)  // pins are at diff states
            rotation++;
        else
            rotation--;
        pin_a = new_a;
    }
    else
        if(new_b != pin_b){     // pin b switched
            if(new_b != pin_a)  // pins are at diff states
                rotation--;
            else
                rotation++;
            pin_b = new_b;
        }
    

}

uint8_t inputs_read(uint8_t input){
/*  input   button
    0       encoder
    1       up
    2       down
    3       right
    4       left
*/
    uint8_t data = 0;

    if(PINB & (0b00000100 << input))
        data = 1;

    return data;
}

uint8_t inputs_rotation(void){
    int8_t old_r = rotation;

    rotation = 0;
    return old_r;
}

void ui_init(void){
    DDRB = 0;  // every pin is an input

    PCMSK0 = 0b00000011;    // enables interrupt for pins PCINT0 and PCINT1 (PORTB 0 and 1)
    PCICR = 1;              // enables Pin Change Interrupt for pins PCINT 0 to 7

    // reads both pins to update their status
    if(PINB & 0b00000001)
        pin_a = 1;
    if(PINB & 0b00000010)
        pin_b = 1;
}

