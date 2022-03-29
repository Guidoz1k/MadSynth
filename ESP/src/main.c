#include "main.h"

timer_config_t setts;

void IRAM_ATTR onTimer0(void *param){
    const uint32_t setPin0 = 1 << (SYNCPIN0 - 32);
    const uint32_t clearPin0 = !(1 << (SYNCPIN0 - 32));

    // makes it easy to measure interruption time
    REG_WRITE(GPIO_OUT1_REG, REG_READ(GPIO_OUT1_REG) | setPin0); // gpio_set_level(SYNCPIN0, 1);
    // clear interrupt status
    timer_group_clr_intr_status_in_isr(TIMER_GROUP_0, TIMER_0);

    task0();

    // reenables the alarm
    timer_group_enable_alarm_in_isr(TIMER_GROUP_0, TIMER_0);
    // makes it easy to measure interruption time
    REG_WRITE(GPIO_OUT1_REG, REG_READ(GPIO_OUT1_REG) & clearPin0); // gpio_set_level(SYNCPIN0, 0);
}

void IRAM_ATTR onTimer1(void *param){
    const uint32_t setPin1 = 1 << (SYNCPIN1 - 32);
    const uint32_t clearPin1 = !(1 << (SYNCPIN1 - 32));

    // makes it easy to measure interruption time
    REG_WRITE(GPIO_OUT1_REG, REG_READ(GPIO_OUT1_REG) | setPin1); // gpio_set_level(SYNCPIN1, 1);
    // clear interrupt status
    timer_group_clr_intr_status_in_isr(TIMER_GROUP_0, TIMER_1);

    task1();

    // reenables the alarm
    timer_group_enable_alarm_in_isr(TIMER_GROUP_0, TIMER_1);
    // makes it easy to measure interruption time
    REG_WRITE(GPIO_OUT1_REG, REG_READ(GPIO_OUT1_REG) & clearPin1); // gpio_set_level(SYNCPIN1, 0);
}

void core1Task(void* parameter){
    timer_init(TIMER_GROUP_0, TIMER_1, &setts);
    timer_set_counter_value(TIMER_GROUP_0, TIMER_1, 0);
    timer_isr_register(TIMER_GROUP_0, TIMER_1, &onTimer1, NULL, ESP_INTR_FLAG_IRAM, NULL);
    timer_set_alarm_value(TIMER_GROUP_0, TIMER_1, TIMER1T);
    timer_enable_intr(TIMER_GROUP_0, TIMER_1);
    timer_start(TIMER_GROUP_0, TIMER_1);

    /* all core 1 operations will be executed in task1()
    while(1)
        core1_loop();
    */
    vTaskDelete(NULL);
}

void app_main(){
    gpio_config_t outputs;

    i2s_init(); // initialization of I2S codec
    serial0_init(); // initialization of USB UART
    serial1_init(); // initialization of AVR UART

    { // control pins initialization
        outputs.pin_bit_mask = ((uint64_t)1 << SYNCPIN0) | ((uint64_t)1 << SYNCPIN1) | ((uint64_t)1 << LEDPIN);
        outputs.mode = GPIO_MODE_OUTPUT;
        outputs.pull_up_en = GPIO_PULLUP_DISABLE;
        outputs.pull_down_en = GPIO_PULLDOWN_DISABLE;
        outputs.intr_type = GPIO_INTR_DISABLE;
        gpio_config(&outputs);

        gpio_set_level(SYNCPIN0, 0);
        gpio_set_level(SYNCPIN1, 0);
        gpio_set_level(LEDPIN, 0);
    }

    { // alarm settings and timer 0 initialization
        setts.alarm_en = true;                      // enables the alarm
        setts.auto_reload = TIMER_AUTORELOAD_EN;    // reloads after interrupting
        setts.counter_dir = TIMER_COUNT_UP;         // timer counts up
        setts.counter_en = false;                   // enables the timer
        setts.divider = 80;                         // sets the pre-scaler
        setts.intr_type = TIMER_INTR_LEVEL;         // sets type of interruption

        timer_init(TIMER_GROUP_0, TIMER_0, &setts);
        timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0);
        timer_isr_register(TIMER_GROUP_0, TIMER_0, &onTimer0, NULL, ESP_INTR_FLAG_IRAM, NULL);
        timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, TIMER0T);
        timer_enable_intr(TIMER_GROUP_0, TIMER_0);
        timer_start(TIMER_GROUP_0, TIMER_0);
    }

    // core 1 task creation
    xTaskCreatePinnedToCore(core1Task, "timer1Creator", 10000, NULL, 1, NULL, 1);
 
    task_init();    // initialization of task.c variables

    // core 0 task termination
    vTaskDelete(NULL);
}
