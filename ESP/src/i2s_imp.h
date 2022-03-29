#ifndef __I2S_I_H
#define __I2S_I_H

#include <stdio.h>
#include "driver/gpio.h"
#include "driver/timer.h"
#include <esp_intr_alloc.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"

#include "serial0.h"
#include "serial1.h"
#include "i2s_mod/i2s_mod.h" // i2s_write modified

#define BCK_PIN     5
#define DIN_PIN     18
#define LCK_PIN     19

void i2s_init();

void i2s_output(int16_t data, int16_t data2);

#endif /* __I2S_I_H */
