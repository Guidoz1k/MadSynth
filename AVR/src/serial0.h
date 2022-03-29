#ifndef __SERIAL0_H
#define __SERIAL0_H

#include <Arduino.h>
#include "stdint.h"

void serial0_init(void);

void serial0_write_string(const char *pointer, uint8_t newline);

void serial0_write_number(uint16_t number, uint8_t size, uint8_t newline);

#endif /* __SERIAL0_H */