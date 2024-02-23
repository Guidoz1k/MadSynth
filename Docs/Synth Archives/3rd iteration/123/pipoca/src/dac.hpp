#include <Arduino.h>

#ifndef DAC_H
#define DAC_H

#define DACCSP  23
#define DACCLK  33
#define DACUDB  32
#define MAXVOL  31

extern int32_t volume;

void dacSet(int32_t direction, int32_t steps);

void volumeSet(int32_t volumeSP);

void volumeInc();

void volumeDec();

void dacInit();

#endif
