#ifndef __ANAL_H
#define __ANAL_H

#include <Arduino.h>
#include "stdint.h"

#define FILTER      20
#define HISTERESIS  2

void adc_init(void);

uint8_t adc_read(uint8_t pin);

#endif /* __ANAL_H */