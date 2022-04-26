#ifndef __ANAL_H
#define __ANAL_H

#include <Arduino.h>
#include "stdint.h"

#define PW_MAX  225
#define PW_MIN  43
#define MW_MAX  250
#define MW_MIN  10

void adc_init(void);

uint8_t adc_read(uint8_t pin);

#endif /* __ANAL_H */