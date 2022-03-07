#ifndef __ONBOARD_H
#define __ONBOARD_H

#include <Arduino.h>
#include "stdint.h"

void led_toggle(void);

void led_output(uint8_t bit);

void led_off(void);

void led_on(void);

void onboard_init();

#endif /* __ONBOARD_H */