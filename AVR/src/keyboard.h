#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#include <Arduino.h>
#include "stdint.h"

void keyboard_init(void);

uint8_t keyboard_read(uint8_t position);

#endif /* __KEYBOARD_H */