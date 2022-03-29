#ifndef __SERIAL1_H
#define __SERIAL1_H

#include <Arduino.h>
#include "stdint.h"

#define HEADBYTE    0xff

void serial1_init(void);

void serial1_transmit(uint8_t byte0, uint8_t byte1);

#endif /* __SERIAL1_H */