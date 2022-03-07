#ifndef __SERIAL_H
#define __SERIAL_H

#include <Arduino.h>
#include "stdint.h"

void serial_init(void);

void serial_transmit(uint8_t character);

#endif /* __TIME__SERIAL_HR_H */