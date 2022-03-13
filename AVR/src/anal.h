#ifndef __ANAL_H
#define __ANAL_H

#include <Arduino.h>
#include "stdint.h"

void adc_init(void);

uint16_t read_adc(uint8_t pin);

#endif /* __ANAL_H */