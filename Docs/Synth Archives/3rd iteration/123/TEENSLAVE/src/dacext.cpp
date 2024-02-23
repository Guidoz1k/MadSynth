#include <Arduino.h>

#define CLOCK    12
#define PIN11    11
#define PIN10    10
#define PIN09    9
#define PIN08    8
#define PIN07    7
#define PIN06    6
#define PIN05    5
#define PIN04    4
#define PIN03    3
#define PIN02    2
#define PIN01    1
#define PIN00    0
#define OUTPIN   13

int32_t output2 = 0;

void update(){
    digitalWriteFast(OUTPIN, 1);
    output2 = 0;
    output2 |= digitalReadFast(PIN00) << 0;
    output2 |= digitalReadFast(PIN01) << 1;
    output2 |= digitalReadFast(PIN02) << 2;
    output2 |= digitalReadFast(PIN03) << 3;
    output2 |= digitalReadFast(PIN04) << 4;
    output2 |= digitalReadFast(PIN05) << 5;
    output2 |= digitalReadFast(PIN06) << 6;
    output2 |= digitalReadFast(PIN07) << 7;
    output2 |= digitalReadFast(PIN08) << 8;
    output2 |= digitalReadFast(PIN09) << 9;
    output2 |= digitalReadFast(PIN10) << 10;
    output2 |= digitalReadFast(PIN11) << 11;
    analogWriteDAC0(output2);
    digitalWriteFast(OUTPIN, 0);
}

void dacInit(){
    for(int i = 0; i <= 12; i++)
        pinMode(i, INPUT);
    pinMode(OUTPIN, OUTPUT);
    attachInterrupt(digitalPinToInterrupt(CLOCK), update, RISING);
}