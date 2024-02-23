#include "hardSPI.hpp"

void SPIInit(){
    pinMode(SLAVEP, OUTPUT);
    digitalWrite(SLAVEP, 1);
    pinMode(MDOPIN, OUTPUT);
    digitalWrite(MDOPIN, 0);
    pinMode(CLKPIN, OUTPUT);
    digitalWrite(CLKPIN, 0);
    //SPI1.begin();
}

void hardSPI(int32_t datum){
    int32_t controlBit = 0x8000;
    int32_t counter = 0;

    //datum &= 0xFFFF;
    datum |= 0x3000;
    digitalWrite(SLAVEP, 0);

    for(counter = 0; counter < 16; counter++){
        GPIO.out_w1tc = 1 << CLKPIN;//digitalWrite(CLKPIN, 0);
        if(datum & controlBit)
            digitalWrite(MDOPIN, 1);
        else
            digitalWrite(MDOPIN, 0);
        GPIO.out_w1ts = 1 << CLKPIN;//digitalWrite(CLKPIN, 1);
        //asm("nop");
        controlBit >>= 1;
    }

    digitalWrite(SLAVEP, 1);
}

/*
void writeSPI(int32_t datum){
    datum |= 0x3000;

    digitalWrite(SYNCPIN, 0);   // SPI pin signal
    SPI1.beginTransaction(spiSets);
    digitalWrite(SLAVEP, 0);    // CS pin
    SPI1.transfer16(datum);
    digitalWrite(SLAVEP, 1);
    SPI1.endTransaction();
    digitalWrite(SYNCPIN, 1);
}
*/

/*
int32_t bitCheck(int32_t datum, int32_t bit){
    int32_t result = 0;

    if((datum & (1 << bit)) != 0)
        result = 1;

    return result;
}

void writeTeensy(int32_t datum){
    datum &= 0x0FFF;
    digitalWrite(DACPIN00, bitCheck(datum, 0x00));
    digitalWrite(DACPIN01, bitCheck(datum, 0x01));
    digitalWrite(DACPIN02, bitCheck(datum, 0x02));
    digitalWrite(DACPIN03, bitCheck(datum, 0x03));
    digitalWrite(DACPIN04, bitCheck(datum, 0x04));
    digitalWrite(DACPIN05, bitCheck(datum, 0x05));
    digitalWrite(DACPIN06, bitCheck(datum, 0x06));
    digitalWrite(DACPIN07, bitCheck(datum, 0x07));
    digitalWrite(DACPIN08, bitCheck(datum, 0x08));
    digitalWrite(DACPIN09, bitCheck(datum, 0x09));
    digitalWrite(DACPIN10, bitCheck(datum, 0x0A));
    digitalWrite(DACPIN11, bitCheck(datum, 0x0B));
    digitalWrite(DACCLOCK, 1);
    delayMicroseconds(1);
    digitalWrite(DACCLOCK, 0);
}

void teensyInit(){
    pinMode(DACPIN00, OUTPUT);
    pinMode(DACPIN01, OUTPUT);
    pinMode(DACPIN02, OUTPUT);
    pinMode(DACPIN03, OUTPUT);
    pinMode(DACPIN04, OUTPUT);
    pinMode(DACPIN05, OUTPUT);
    pinMode(DACPIN06, OUTPUT);
    pinMode(DACPIN07, OUTPUT);
    pinMode(DACPIN08, OUTPUT);
    pinMode(DACPIN09, OUTPUT);
    pinMode(DACPIN10, OUTPUT);
    pinMode(DACPIN11, OUTPUT);
    pinMode(DACCLOCK, OUTPUT);
    digitalWrite(DACPIN00, 0);
    digitalWrite(DACPIN01, 0);
    digitalWrite(DACPIN02, 0);
    digitalWrite(DACPIN03, 0);
    digitalWrite(DACPIN04, 0);
    digitalWrite(DACPIN05, 0);
    digitalWrite(DACPIN06, 0);
    digitalWrite(DACPIN07, 0);
    digitalWrite(DACPIN08, 0);
    digitalWrite(DACPIN09, 0);
    digitalWrite(DACPIN10, 0);
    digitalWrite(DACPIN11, 0);
    digitalWrite(DACCLOCK, 0);
}
*/
