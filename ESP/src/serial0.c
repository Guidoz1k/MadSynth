#include "serial0.h"

void serial0_init(){
    uart_config_t configs;

    configs.baud_rate = 115200;
    configs.data_bits = UART_DATA_8_BITS;
    configs.parity = UART_PARITY_DISABLE;
    configs.stop_bits = UART_STOP_BITS_1;
    configs.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
    configs.rx_flow_ctrl_thresh = 122;

    uart_set_pin(UART_NUM_0, 1, 3, -1, -1);
    uart_param_config(UART_NUM_0, &configs);
    uart_driver_install(UART_NUM_0, 2048, 0, 0, NULL, 0);
}

void serial0_write_number(uint_fast8_t number, uint_fast8_t newline){
    char buffer[6];

    buffer[0] = (number / 10000) + 48;
    buffer[1] = ((number / 1000) % 10) + 48;
    buffer[2] = ((number / 100) % 10) + 48;
    buffer[3] = ((number / 10) % 10) + 48;
    buffer[4] = (number % 10) + 48;
    if(newline)
        buffer[5] = '\n';
    else
        buffer[5] = ' ';

    uart_write_bytes(UART_NUM_0, buffer, 6);
}

void serial0_write_hex(uint_fast32_t number){
    int8_t index = 0;
    int32_t aux = 0;
    char buffer[9];

    for(index = 0; index < 8; index++){
        aux = number >> ((7 - index) * 4);
        aux &= 0xF;
        if(aux < 9)
            buffer[index] = aux + 48;
        else
            buffer[index] = aux - 10 + 65;
    }
    buffer[8] = ' ';

    uart_write_bytes(UART_NUM_0, buffer, 9);
}

void serial0_new_line(void){
    char buffer = '\n';

    uart_write_bytes(UART_NUM_0, &buffer, 1);
}
