#ifndef __LCD_H
#define __LCD_H

#include <Arduino.h>
#include "stdint.h"

void lcd_flush(void);

void lcd_write(uint8_t data);

void lcd_pos(uint8_t line, uint8_t pos);

void display_init();

#endif /* __LCD_H */