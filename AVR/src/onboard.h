#ifndef __ONBOARD_H
#define __ONBOARD_H

#include <Arduino.h>
#include "stdint.h"

void ledTOGGLE(void);

void ledOUTPUT(uint8_t bit);

void ledOFF(void);

void ledON(void);

void onboardInit();

#endif /* __ONBOARD_H */