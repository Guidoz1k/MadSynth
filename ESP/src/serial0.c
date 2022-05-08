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

void serial0_write_number(int_fast32_t number, uint_fast8_t newline){
    char buffer[12];

    if(number >= 0)
        buffer[0] = '+';
    else{
        buffer[0] = '-';
        number *= -1;
    }

    buffer[1] = ((number / (int_fast32_t)1000000000) % 10) + 48;
    buffer[2] = ((number / (int_fast32_t)100000000) % 10) + 48;
    buffer[3] = ((number / (int_fast32_t)10000000) % 10) + 48;
    buffer[4] = ((number / (int_fast32_t)1000000) % 10) + 48;
    buffer[5] = ((number / (int_fast32_t)100000) % 10) + 48;
    buffer[6] = ((number / 10000) % 10) + 48;
    buffer[7] = ((number / 1000) % 10) + 48;
    buffer[8] = ((number / 100) % 10) + 48;
    buffer[9] = ((number / 10) % 10) + 48;
    buffer[10] = (number % 10) + 48;
    if(newline)
        buffer[11] = '\n';
    else
        buffer[11] = ' ';

    uart_write_bytes(UART_NUM_0, buffer, 12);
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

void serial0_write_string(const char *pointer, uint8_t newline){
	uint8_t counter = 0;
    char buffer[MAXSIZE + 1] = {0};

	while((counter < MAXSIZE) && (*pointer != '\0')){
        buffer[counter++] = *(pointer++);
	}
    if(newline == 1)
        buffer[counter] = '\n';
    else
        counter--;

    uart_write_bytes(UART_NUM_0, buffer, counter + 1);
}
