#include "button.hpp"

void buttonInit(){
    pinMode(BUTPIN, INPUT); // pullup is broken somehow
}

int32_t unbounceButton(){
    int32_t result = 0;

    if(digitalRead(BUTPIN) == 0){
        delay(5);
        if(digitalRead(BUTPIN) == 0){
            result = 1;
        }
    }

    return result;
}
