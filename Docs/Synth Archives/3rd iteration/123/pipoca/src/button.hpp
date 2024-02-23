#include <Arduino.h>

#ifndef BUTTON_H
#define BUTTON_H

#define BUTPIN  25

void buttonInit();

int32_t unbounceButton();

#endif
