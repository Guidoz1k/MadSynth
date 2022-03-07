#ifndef __LCD_H
#define __LCD_H

#include <Arduino.h>
#include "stdint.h"

void lcdFlush(void);

void lcdWrite(uint8_t data);

void lcdPos(uint8_t line, uint8_t pos);

void displayInit();

#endif /* __CUSTOM_H */