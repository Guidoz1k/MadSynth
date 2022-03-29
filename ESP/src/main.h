#ifndef __MAIN_H
#define __MAIN_H

#include <stdio.h>
#include "driver/gpio.h"
#include "driver/timer.h"
#include <esp_intr_alloc.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"

#include "task.h"

#define SYNCPIN0    32  // interruption 0 work pin
#define SYNCPIN1    33  // interruption 1 work pin

#define TIMER0T     25      // timer 0 alarm value
#define TIMER1T     1000    // timer 1 alarm value

#endif /* __MAIN_H */