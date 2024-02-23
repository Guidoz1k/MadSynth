/*
#include <Arduino.h>

#define DIG         24
#define LED         13

#define CHOIRSIZE   32

#define MAXFREQ     4000

IntervalTimer interrupter;
int32_t aux = 1000000/3000;

volatile int32_t value[CHOIRSIZE];
volatile int32_t counter[CHOIRSIZE];
volatile int32_t period[CHOIRSIZE];

volatile int32_t voiceNumber = 0;
volatile int32_t choir = 0;

void setVoice(int32_t frequency, int32_t slot){
    if(period[slot] == -1)
        voiceNumber++;
    if(frequency <= MAXFREQ){
        period[slot] = 100000/(frequency * 2);
        counter[slot] = 0;
    }
}

void nullVoice(int32_t slot){
    period[slot] = -1;
    value[slot] = 0;
    counter[slot] = 0;
    voiceNumber--;
}

void interrupt(){
    digitalWriteFast(DIG, 1);

    choir = 0;
    for(int32_t i = 0; i < 32; i++){
        if(counter[i] == period[i]){
            counter[i] = 0;
            value[i] ^= 127;
        }
        else
            counter[i]++;
        choir += value[i];
    }
    analogWriteDAC1(choir);

    digitalWriteFast(DIG, 0);
}

void setup(){
    pinMode(DIG, OUTPUT);
    pinMode(LED, OUTPUT);
    interrupter.begin(interrupt, 10);
    interrupter.priority(0);
    for(int32_t i = 0; i < 32; i++)
        period[i] = -1;

    //Serial.begin(9600);
    //for(int32_t i = 0; i < 32; i++)
    //    setVoice(20 + 100 * i, i);
}

void loop(){
    digitalWriteFast(LED, 1);
    setVoice(1000, 0);
    delay(5);
    digitalWriteFast(LED, 0);
    setVoice(2000, 0);
    delay(5);
}
*/
