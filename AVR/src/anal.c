#include "anal.h"
/*
    PORTF:
        0   bend wheel
        1   bend mod
        2   slider 1
        3   slider 2
        4   slider 3
        5   slider 4
        6   slider 5

*/

static const uint8_t FILTER = 20;       // amount of times an analog value is averaged out
static const uint8_t HYSTERESIS = 1;    // minimum difference between a signal's value over time to trigger hysteresis

static uint16_t old_values[7] = {0};  // previous analog readings for hysteresis algorithm

// executes one read on an analog pin
static uint16_t adc_return(uint8_t pin){
    uint16_t result = 0;

    ADMUX = 0b01000000;             // voltage reference
    ADMUX |= pin;                   // select the channel via internal mux
    ADCSRA |= 0b01000000;           // start single conversion
    while (ADCSRA & 0b01000000);    // wait for conversion to finish
    //ADCSRA |= 0b00010000;           // clear conversion flag

    result = ADCL | (ADCH << 8);
    return result;
}

void adc_init(void){
    ADMUX = 0b01000000;     // voltage reference
    ADCSRA = 0b10000100;    // turns ADC on with 16 prescaler
    DDRF = 0;               // all pins are inputs
    DIDR0 = 0xFF;           // turns digital input off
    adc_return(0);          // performs first conversion to initialize the ADC
}

// read an analog pin, filter it and then runs it through an hysteresis window
uint8_t adc_read(uint8_t pin){
    uint8_t i = 0;
    uint32_t total = 0;

    for(i = 0; i < FILTER; i++)
        total += adc_return(pin);
    total /= FILTER;

    // 10-bit hysteresis
    if((total < (old_values[pin] - HYSTERESIS)) || (total > (old_values[pin] + HYSTERESIS)))
        old_values[pin] = (uint16_t)total;
    else
        total = old_values[pin];

    total >>= 2; // convert filtered and post-hysteresis values to 8 bits
    return (uint8_t)total;
}

