#include "i2s_imp.h"

#define SRFREQ    40000

void i2s_init(){
    i2s_config_t i2s_config = {
        .mode = I2S_MODE_MASTER | I2S_MODE_TX,
        .sample_rate = SRFREQ,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .tx_desc_auto_clear = false,
        .dma_buf_count = 8,
        .dma_buf_len = 64,
        .use_apll = true,
        .intr_alloc_flags = ESP_INTR_FLAG_INTRDISABLED  // Interrupt level 1, default 0
    };
    static const i2s_pin_config_t pin_config = {
        .bck_io_num = BCK_PIN,
        .ws_io_num = LCK_PIN,
        .data_out_num = DIN_PIN,
        .data_in_num = I2S_PIN_NO_CHANGE
    };

    i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_NUM_0, &pin_config);
}

void i2s_output(int16_t data1, int16_t data2){
    int16_t data[2];
    size_t i2s_bytes_write = 0;

    data[0] = data1;
    data[1] = data2;

    i2s_write(I2S_NUM_0, data, 4, &i2s_bytes_write, 0);
}
