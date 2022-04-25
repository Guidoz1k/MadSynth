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

    // updates the pins states
    if(PINB & 0b00000001)
        new_a = 1;
    if(PINB & 0b00000010)
        new_b = 1;

/* the original code counted half-steps
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
*/
    // this code only takes full-steps into account
    if(new_a != pin_a){     // pin a switched
        if(new_a == pin_b)  // pins are at diff states
            rotation--;
        pin_a = new_a;
    }
    else
        if(new_b != pin_b){     // pin b switched
            if(new_b == pin_a)  // pins are at diff states
                rotation++;
            pin_b = new_b;

        }
}

// reads all UI inputs excluding the encoder rotation pins
uint8_t inputs_read(void){
/*  input   button
    1       encoder
    2       up
    3       down
    4       right
    5       left
    0       invalid or none
*/
    uint8_t input = 0;
    uint8_t counter = 0;

    if(!(PINB & (0b00000100 << 0))){
        input = 1;
        counter++;
    }
    if(!(PINB & (0b00000100 << 1))){
        input = 2;
        counter++;
    }
    if(!(PINB & (0b00000100 << 2))){
        input = 3;
        counter++;
    }
    if(!(PINB & (0b00000100 << 3))){
        input = 4;
        counter++;
    }
    if(!(PINB & (0b00000100 << 4))){
        input = 5;
        counter++;
    }

    // it's invalid to press more than one button at a time
    if(counter > 1)
        input = 0;

    return input;
}

// return the accumulated rotation count
int8_t inputs_rotation(void){
    int8_t old_r = rotation;

    // returns the encoder rotation count and resets it
    rotation = 0;
    return old_r;
}

// initializes pins and variables
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

