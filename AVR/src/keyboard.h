#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#include <Arduino.h>
#include "stdint.h"

void keyboard_init(void);

void keyboard_write(uint8_t position);

uint8_t keyboard_read(void);

#endif /* __KEYBOARD_H */