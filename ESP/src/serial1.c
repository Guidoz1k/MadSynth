#include "serial1.h"

void serial1_init(){
    uart_config_t configs;

    configs.baud_rate = 115200;
    configs.data_bits = UART_DATA_8_BITS;
    configs.parity = UART_PARITY_DISABLE;
    configs.stop_bits = UART_STOP_BITS_1;
    configs.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
    configs.rx_flow_ctrl_thresh = 122;

    uart_set_pin(UART_NUM_2, 17, 16, -1, -1);
    uart_param_config(UART_NUM_2, &configs);
    uart_driver_install(UART_NUM_2, 2048, 0, 0, NULL, 0);
}

int8_t serial1_read(uint8_t* buffer, uint8_t bytes){
    int8_t bytes_read = 0;

    bytes_read = uart_read_bytes(UART_NUM_2, buffer, bytes, 0);

    return bytes_read;
}

uint8_t serial1_check(void){
    uint8_t buffer_size = 0;

    uart_get_buffered_data_len(UART_NUM_2, (size_t *)&buffer_size);

    return buffer_size;
}
