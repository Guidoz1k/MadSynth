#ifndef __SERIAL_H
#define __SERIAL_H

#include <Arduino.h>
#include "stdint.h"

void serial_init(void);

void serial_write_string(const char *pointer, uint8_t newline);

void serial_write_number(uint16_t number, uint8_t size, uint8_t newline);

#endif /* __TIME__SERIAL_HR_H */