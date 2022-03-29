#ifndef __SERIAL0_H
#define __SERIAL0_H

#include <stdio.h>
#include "driver/gpio.h"
#include "driver/timer.h"
#include <esp_intr_alloc.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"

#include "driver/uart.h"

void serial0_init();

void serial0_write_number(uint_fast8_t number);

void serial0_write_hex(uint_fast32_t number);

void serial0_new_line(void);

#endif /* SERIAL0 */

