#ifndef __SERIAL1_H
#define __SERIAL1_H

#include <Arduino.h>
#include "stdint.h"

void serial1_init(void);

static void serial1_transmit(uint8_t character);

#endif /* __SERIAL1_H */