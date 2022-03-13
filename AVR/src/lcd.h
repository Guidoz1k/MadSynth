#ifndef __LCD_H
#define __LCD_H

#include <Arduino.h>
#include "stdint.h"

void lcd_write_string(const char *pointer, uint8_t line, uint8_t pos);

void lcd_write_number(uint16_t number, uint8_t size, uint8_t line, uint8_t pos);

void display_init();

#endif /* __LCD_H */