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

void adc_init(void){
    // ADMUX = 0b01000000;  // verificar AREF
    ADCSRA = 0b10000000;    // turns ADC on
    DDRF = 0;               // all pins are inputs
    DIDR0 = 0xFF;           // turns digital input off
    read_adc(0);            // performs first conversion to initialize the ADC
}

uint16_t read_adc(uint8_t pin){
    uint16_t result = 0;

    ADMUX |= pin;                   // select the channel via internal mux
    ADCSRA |= 0b01000000;           // start single conversion
    while (ADCSRA & 0b00010000);    // wait for conversion to finish
    ADCSRA |= 0b00010000;           // clear conversion flag

    result = ADCL | (ADCH << 8);
    return result;
}
