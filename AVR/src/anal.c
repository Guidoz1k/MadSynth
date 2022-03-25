#include "anal.h"
/*
    AREF: DOES IT NEED ANY CONNECTION?
    PORTF:
        0   bend wheel
        1   bend mod
        2   slider 1
        3   slider 2
        4   slider 3
        5   slider 4
        6   slider 5

*/

static uint16_t old_values[7];

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
    uint8_t i = 0;

    for(i = 0; i <= 6; i++)
        old_values[i] = 0;

    ADMUX = 0b01000000;     // voltage reference
    ADCSRA = 0b10000100;    // turns ADC on with 16 prescaler
    DDRF = 0;               // all pins are inputs
    DIDR0 = 0xFF;           // turns digital input off
    adc_return(0);            // performs first conversion to initialize the ADC
}

uint8_t adc_read(uint8_t pin){
    uint8_t i = 0;
    uint32_t total = 0;

    for(i = 0; i < FILTER; i++)
        total += adc_return(pin);
    total /= FILTER;

    // 10-bit histeresis
    if((total < (old_values[pin] - HISTERESIS)) || (total > (old_values[pin] + HISTERESIS)))
        old_values[pin] = (uint16_t)total;
    else
        total = old_values[pin];

    total >>= 2; // convert filtered and post-histeresis values to 8 bits
    return (uint8_t)total;
}

