#include <Arduino.h>
#include <SPI.h>

int32_t output = 0;
int32_t mode = 0;

void setup() {
}

void loop() {
    if(mode == 1)
        if(output < 4095)
            output++;
        else
            mode = 0;
    else
        if(output > 0)
            output--;
        else
            mode = 1;
    analogWriteDAC0(output);
}
