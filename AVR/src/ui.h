#ifndef __UI_H
#define __UI_H

#include <Arduino.h>
#include "stdint.h"

uint8_t inputs_read(void);

int8_t inputs_rotation(void);

void ui_init(void);

#endif /* __UI_H */