#ifndef __TASKS_H
#define __TASKS_H

#include <stdio.h>
#include "driver/gpio.h"
#include "driver/timer.h"
#include <esp_intr_alloc.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"

#include "i2s_imp.h"
#include "lut.h"

#define LUTSIZE     2000
#define LEDPIN      2       // built-in led pin
#define VOICEM      61
#define BROKELF     150
#define BROKEHF     80000
#define KEYOFFSET   24

#define PI          3.1415926

void task0(void);

void task1(void);

//void core1_loop(void);

void task_init(void);

#endif /* __TASKS_H */