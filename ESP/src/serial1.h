#ifndef __SERIAL1_H
#define __SERIAL1_H

#include <stdio.h>
#include "driver/gpio.h"
#include "driver/timer.h"
#include <esp_intr_alloc.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"

#include "driver/uart.h"

void serial1_init();

int8_t serial1_read(uint8_t* buffer, uint8_t bytes);

#endif /* SERIAL1 */

