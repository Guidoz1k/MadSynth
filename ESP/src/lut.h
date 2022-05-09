#ifndef __LUT_H
#define __LUT_H

#include <stdio.h>
#include "driver/gpio.h"
#include "driver/timer.h"
#include <esp_intr_alloc.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"

#define LUTSIZE     2000

#include "lut.h"

int16_t lut_sinuso[LUTSIZE];
int16_t lut_square[LUTSIZE];
int16_t lut_triang[LUTSIZE];
int16_t lut_sawtoo[LUTSIZE];
int16_t lut_invsaw[LUTSIZE];

int16_t lut_l[LUTSIZE];
int16_t lut_r[LUTSIZE];

#endif /* __LUT_H */
