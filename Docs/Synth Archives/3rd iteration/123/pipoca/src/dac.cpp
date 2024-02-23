#include "dac.hpp"

int32_t dacVolume = 0;

/*  direction: 0 for increment, 1 for decrement, rest for excrement
    function should only be used by volumeSet, for it has no protection
*/
void dacSet(int32_t direction, int32_t steps){
    int32_t counter = 0;


    digitalWrite(DACCSP, 0);
    asm("nop");
    digitalWrite(DACUDB, direction);
    asm("nop");
    for(counter = 0; counter < steps; counter++){
        digitalWrite(DACCLK, 0);
        asm("nop");
        digitalWrite(DACCLK, 1);
        asm("nop");
    }
    asm("nop");
    digitalWrite(DACCSP, 1);
}

void volumeSet(int32_t volumeSP){
    int32_t direction = 0;
    int32_t value = 0;

    if((dacVolume != volumeSP) && (volumeSP <= MAXVOL) && (volumeSP >= 0)){
        if(volumeSP < dacVolume){
            direction = 1;
            value = dacVolume - volumeSP;
            }
        else
            value = volumeSP - dacVolume;
        dacSet(direction, value);
        dacVolume = volumeSP;
    }
}

void volumeInc(){
    if(dacVolume < MAXVOL){
        volumeSet(++dacVolume);
    }
}

void volumeDec(){
    if(dacVolume > 0){
        volumeSet(--dacVolume);
    }
}

void dacInit(){
    pinMode(DACCSP, OUTPUT);
    digitalWrite(DACCSP, 1);
    pinMode(DACCLK, OUTPUT);
    digitalWrite(DACCLK, 1);
    pinMode(DACUDB, OUTPUT);
    digitalWrite(DACUDB, 0);
    dacSet(1, 128);
}
