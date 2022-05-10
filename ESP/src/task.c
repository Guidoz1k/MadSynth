#include "task.h"

// DDS main variables
volatile uint_fast32_t increment[VOICEM] = {0};    // 52.2 * freq
volatile uint_fast32_t counter[VOICEM] = {0};      // position in LUT
/* channel allocation
    0 - unallocated
    1 - A
    2 - D
    3 - S
    4 - off by user
    5 - R
    6 - off by DDS
*/
volatile uint_fast8_t channel_alloc[VOICEM] = {0};

uint_fast32_t bobMax = LUTSIZE << 10;
uint_fast8_t voice_counter = 0;       // 0 - VOICEM number of voices
const uint32_t keys_freq[KEYOFFSET + 108 + KEYOFFSET] = {
    BROKELF,BROKEHF,BROKEHF,BROKEHF,BROKEHF,BROKEHF,BROKEHF,BROKEHF,BROKEHF,BROKEHF,BROKEHF,BROKEHF,
    BROKELF,BROKELF,BROKELF,BROKELF,BROKELF,BROKELF,BROKELF,BROKELF,BROKELF,BROKELF,BROKELF,BROKELF,
    164,    173,    184,    194,    206,    218,    231,    245,    260,    275,    291,    309,
    327,    346,    367,    389,    412,    437,    462,    490,    519,    550,    583,    617,
    654,    693,    734,    778,    824,    873,    925,    980,    1038,   1100,   1165,   1235,
    1308,   1386,   1468,   1556,   1648,   1746,   1850,   1960,   2077,   2200,   2331,   2470,
    2616,   2772,   2937,   3111,   3296,   3492,   3670,   3920,   4153,   4400,   4662,   4939,
    5233,   5544,   5873,   6223,   6593,   6985,   7400,   7840,   8306,   8800,   9323,   9878,
    10465,  11087,  11747,  12445,  13185,  13969,  14800,  15680,  16612,  17600,  18647,  19755,
    20930,  22175,  23493,  24890,  26370,  27938,  29600,  31360,  33224,  35200,  37293,  39511,
    41860,  44349,  46986,  49780,  52740,  55877,  59109,  62719,  66449,  70400,  74586,  79021,
    BROKEHF,BROKEHF,BROKEHF,BROKEHF,BROKEHF,BROKEHF,BROKEHF,BROKEHF,BROKEHF,BROKEHF,BROKEHF,BROKEHF,
    BROKEHF,BROKEHF,BROKEHF,BROKEHF,BROKEHF,BROKEHF,BROKEHF,BROKEHF,BROKEHF,BROKEHF,BROKEHF,BROKEHF,
};
const uint8_t agm[VOICEM] = {
    0,
    7, 7, 7, 7,
    6, 6, 6, 6,
    5, 5, 5, 5, 
    4, 4, 4,
    3, 3, 3, 3, 3,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};

int_fast8_t master_volume = 50;
int_fast8_t mixer1 = 50;
int_fast8_t mixer2 = 50;
int_fast32_t vca1 = 0;
int_fast32_t vca2 = 0;
uint8_t oct_trans = 2;
int8_t note_trans = 0;
int16_t note_cent = 0;
uint8_t stereo = 0; // 0 - LR, 1 - L, 2 - R
uint8_t key_tune = KEYOFFSET + (2 * 12);

uint8_t shape1 = 0;
uint8_t shape2 = 0;
uint8_t sub_osc1 = 0;
uint8_t sub_osc2 = 0;
uint16_t harm_atten[5][16] = {
    {1000,   568,   400,   309,   252,   213,   184,   162,   145,   131,   120,   110,   102,   95,    89,    83 }, // sinusoidal
    {1000,   666,   500,   375,   307,   263,   227,   200,   179,   162,   148,   136,   126,   117,   110,   103}, // triangular
    {1000,   500,   333,   250,   200,   166,   142,   125,   111,   100,   90,    83,    76,    71,    66,    62 }, // square
    {1000,   500,   333,   250,   200,   166,   142,   125,   111,   100,   90,    83,    76,    71,    66,    62 }, // sawtooth
    {1000,   500,   333,   250,   200,   166,   142,   125,   111,   100,   90,    83,    76,    71,    66,    62 }, // inverted sawtooth
};
/*
uint16_t harm_atten_old[5][16] = {
    {1000,   769,   693,   654,   631,   616,   605,   597,   591,   585,   581,   578,   575,   572,   570,   568}, // sinusoidal
    {1000, 1.000,   856,   856,   811,   811,   791,   788,   774,   774,   765,   765,   759,   759,   754,   754}, // triangular
    {1000,   666,   545,   480,   437,   408,   385,   367,   353,   341,   331,   322,   314,   307,   301,   295}, // square
    {1000,   666,   545,   480,   437,   408,   385,   367,   353,   341,   331,   322,   314,   307,   301,   295}, // sawtooth
    {1000,   666,   545,   480,   437,   408,   385,   367,   353,   341,   331,   322,   314,   307,   301,   295}, // inverted sawtooth
};
*/

// low-pass filter variables
uint8_t filter_state = 0;
int32_t cutoff_freq = 10000; // variavel real
int32_t filter_alpha = 0;
int32_t filter_factor = 10000;
int32_t inp_factor = 0;
int32_t out_factor = 0;

// reverb fx variables
uint8_t reverb_state = 0;

// ADSR variables
uint16_t a_timer = 0;
uint16_t d_timer = 0;
uint16_t s_level = 128;
uint16_t s_timer = 0;
uint16_t r_timer = 0;
uint16_t adsr_counter[VOICEM] = {0};
uint16_t adsr_vca[VOICEM] = {0}; // 0 - 128
uint16_t adsr_crop = 0;

// MOD ENV variables
uint16_t me_status = 0; // on or off
uint16_t me_keys = 0;   // number of keys
uint16_t me_max = 250;        // 0 - 250
uint16_t me_a_timer = 0;
uint16_t me_d_timer = 0;
uint16_t me_s_level = 100;
uint16_t me_s_timer = 0;
uint16_t me_r_timer = 0;
uint16_t me_crop = 0;
uint16_t me_state = 5;      // 0 reset and go, 1 2 3 4 ADSR, 5 finished
uint16_t me_counter = 0;
uint16_t me_output = 0;

// LFO variables
uint8_t lfo1_state = 0; // 0 - off, 1 - on
uint8_t lfo1_mode = 0;
uint8_t lfo1_shape = 0;
uint8_t lfo1_amp = 0;   // 0 ... 100
uint16_t lfo1_counter = 1;  // 0 ... 25
uint16_t lfo1_inc = 0;
int16_t lfo1_input = 0;
int16_t lfo1_output = 0;

uint8_t lfo2_state = 0; // 0 - off, 1 - on
uint8_t lfo2_mode = 0;
uint8_t lfo2_shape = 0;
uint8_t lfo2_amp = 0;   // 0 ... 100
uint16_t lfo2_counter = 1;  // 0 ... 25
uint16_t lfo2_inc = 0;
int16_t lfo2_input = 0;
int16_t lfo2_output = 0;  

// analog control variables
uint8_t amp_pw = 0;  // 0 ... 99
uint8_t amp_mw = 0;  // 0 ... 99
uint8_t amp_kn = 0;  // 0 ... 99
uint8_t amp_ks = 0;  // 0 ... 99
uint8_t amp_s1 = 0;  // 0 ... 99
uint8_t amp_s2 = 0;  // 0 ... 99
uint8_t amp_s3 = 0;  // 0 ... 99
uint8_t amp_s4 = 0;  // 0 ... 99
uint8_t amp_s5 = 0;  // 0 ... 99

// control variables

// ============================================================ DDS PERIPHERAL FUNCTIONS

void lfo_freq(uint8_t lfo, uint16_t freq){
    switch(lfo){
    case 1:
        lfo1_inc = freq * 2;
        break;
    case 2:
        lfo2_inc =  freq * 2;
        break;
    default:
        break;
    }
}

void lfo_set(uint8_t lfo, uint8_t state){
    switch(lfo){
    case 1:
        if(state == 0)
            lfo1_state = 0;
        else{
            lfo1_state = 1;
            if(lfo1_mode == 1)
                lfo1_counter = 0;
        }
        break;
    case 2:
        if(state == 0)
            lfo2_state = 0;
        else{
            lfo2_state = 1;
            if(lfo2_mode == 1)
                lfo2_counter = 0;
        }
        break;
    default:
        break;
    }
}

void me_start(void){
    if(me_keys == 0){
        me_state = 0;
        me_keys++;
    }
}

void me_stop(void){
    if(me_keys == 1){
        me_state = 4;
        me_keys--;
    }
}

uint16_t power2(uint16_t degree){
    uint16_t result = 1;
    uint8_t aux = 0;

    for(aux = 1; aux <= degree; aux++)
        result *= 2;

    return result;
}

void adjust_filter(int32_t new_freq, uint8_t byte){
    if(byte == 0)
        cutoff_freq = new_freq;
    else
        if(byte == 1){
        cutoff_freq |= new_freq << 8;
        filter_alpha = (filter_factor * (2 * PI * cutoff_freq)) / 40000;
        inp_factor = ((filter_factor * filter_alpha) / (filter_factor + filter_alpha));
        out_factor = ((filter_factor * filter_factor) / (filter_factor + filter_alpha));
        }

}

// ============================================================ DDS CORE FUNCTIONS

void update_tune(void){
    key_tune = KEYOFFSET + (12 * oct_trans) + note_trans;
}

void change_frequency(uint32_t frequency, uint16_t number){
    increment[number] = 256 * frequency / 50;
}

void set_frequency(uint32_t frequency, uint16_t number){
    counter[number] = 0;
    increment[number] = 256 * frequency / 50;
    channel_alloc[number] = 1;
    adsr_counter[number] = 0;
}

void change_shape(uint8_t channel, uint8_t shape, uint8_t sub_socs){
    uint16_t i, j, harm_acc;
    int16_t *pointer_in = lut_sinuso;
    int16_t *pointer_out = lut_r;
    int32_t temp = 0;

    switch(shape){
    case 0:
        pointer_in = lut_sinuso;
        break;
    case 1:
        pointer_in = lut_triang;
        break;
    case 2:
        pointer_in = lut_square;
        break;
    case 3:
        pointer_in = lut_sawtoo;
        break;
    case 4:
        pointer_in = lut_invsaw;
        break;
    }
    if(channel == 1){
        pointer_out = lut_r;
        shape1 = shape;
        sub_osc1 = sub_socs;
    }
    else
        if(channel == 2){
            pointer_out = lut_l;
            shape2 = shape;
            sub_osc2 = sub_socs;
        }
    for(i = 0; i < LUTSIZE; i++)
        pointer_out[i] = 0;

    for(j = 0; j <= sub_socs; j++){
        harm_acc = 0;
        for(i = 0; i < LUTSIZE; i++){
            temp = harm_atten[shape][sub_socs] * pointer_in[harm_acc] ;
            temp /= 1000 * VOICEM;//(j + 1) * 1000;
            pointer_out[i] += (int16_t)temp;
            harm_acc += power2(j);
            if(harm_acc >= LUTSIZE)
                harm_acc -= LUTSIZE;
        }
    }
}

uint32_t cent_voice(uint32_t freq, uint8_t key){
    if(note_cent != 0){
        if(note_cent > 0)
            freq +=  (note_cent * (keys_freq[key_tune + key + 1] - keys_freq[key_tune + key])) / 100;
        else
            freq -=  (uint32_t)((uint8_t)(note_cent * -1) * (keys_freq[key_tune + key] - keys_freq[key_tune + key - 1])) / 100;
    }
    return freq;
}

void change_voice(uint8_t key){
    uint32_t aux_freq = keys_freq[key_tune + key];

    aux_freq = cent_voice(aux_freq, key);
    change_frequency(aux_freq, key);
}

void set_voice(uint8_t key){
    uint32_t aux_freq = keys_freq[key_tune + key];

    aux_freq = cent_voice(aux_freq, key);
    set_frequency(aux_freq, key);
}

void remove_voice(uint16_t number){
    if((channel_alloc[number] < 4) && (channel_alloc[number] != 0))
        channel_alloc[number] = 4;
}

// ============================================================ TASK 1 SERIAL FUNCTIONS

void serial_command(uint8_t uart_code, uint8_t uart_message){
    uint8_t aux;

    switch(uart_code){
    { // main codes
    case 110:
        master_volume = uart_message;
        break;
    case 111:
        stereo = uart_message;
        break;
    case 114:
        oct_trans = uart_message;
        update_tune();
        for(aux = 0; aux < VOICEM; aux++)
            if(channel_alloc[aux] != 0)
                change_voice(aux);
        break;
    case 116:
        mixer1 = uart_message;
        break;
    case 117:
        mixer2 = uart_message;
        break;
    case 120:
        change_shape(1, uart_message, sub_osc1);
        break;
    case 121:
        change_shape(1, shape1, uart_message);
        break;
    case 124: // used to be 115 but system can only handle 61 osc
        note_trans = uart_message;
        update_tune();
        for(aux = 0; aux < VOICEM; aux++)
            if(channel_alloc[aux] != 0)
                change_voice(aux);
        break;
    case 125:
        note_cent = (int8_t)uart_message;
        update_tune();
        for(aux = 0; aux < VOICEM; aux++)
            if(channel_alloc[aux] != 0)
                change_voice(aux);
        break;
    case 130:
        change_shape(2, uart_message, sub_osc2);
        break;
    case 131:
        change_shape(2, shape2, uart_message);
        break;
    }

    { // ADSR
    case 140:
        a_timer = uart_message;
        break;
    case 142:
        d_timer = uart_message;
        break;
    case 144:
        s_timer = uart_message;
        break;
    case 146:
        s_level = uart_message;
        break;
    case 148:
        r_timer = uart_message;
        break;
    }

    { // LPF
    case 220:
        filter_state = uart_message;
        break;
    case 221:
        adjust_filter(uart_message, 0);
        break;
    case 222:
        adjust_filter(uart_message, 1);
        break;
    }

    { // MOD ENV
    case 199:
        me_status = uart_message;
        break;
    case 200:
        me_a_timer = uart_message;
        break;
    case 202:
        me_d_timer = uart_message;
        break;
    case 204:
        me_s_timer = uart_message;
        break;
    case 206:
        me_s_level = uart_message;
        break;
    case 208:
        me_r_timer = uart_message;
        break;
    case 215:
        me_max = uart_message;
        break;
    }

    { // LFO
    case 160:
        lfo_set(1, uart_message);
        break;
    case 161:
        lfo1_mode = uart_message;
        break;
    case 162:
        lfo1_shape = uart_message;
        break;
    case 163:
        lfo1_amp = uart_message;
        break;
    case 164:
        lfo_freq(1, uart_message);
        break;
    case 170:
        lfo_set(2, uart_message);
        break;
    case 171:
        lfo2_mode = uart_message;
        break;
    case 172:
        lfo2_shape = uart_message;
        break;
    case 173:
        lfo2_amp = uart_message;
        break;
    case 174:
        lfo_freq(2, uart_message);
        break;
    }

    { // controls
    case 180:
        amp_pw = uart_message;
        break;
    case 181:
        amp_mw = uart_message;
        break;
    case 182:
        amp_kn = uart_message;
        break;
    case 183:
        amp_ks = uart_message;
        break;
    case 184:
        amp_s1 = uart_message;
        break;
    case 185:
        amp_s2 = uart_message;
        break;
    case 186:
        amp_s3 = uart_message;
        break;
    case 187:
        amp_s4 = uart_message;
        break;
    case 188:
        amp_s5 = uart_message;
        break;
    }

    default:
        break;
    }
}

void serial_ops(void){
    uint8_t uart_code = 0;
    uint8_t uart_message = 0;
    uint8_t uart_count = 0;

    uart_count += serial1_read(&uart_code, 1);
    uart_count += serial1_read(&uart_message, 1);

    if(uart_count == 2){
        if((uart_code >= 1) && (uart_code <= 108)){
            uart_code -= 1;
            if(uart_message == 0){
                remove_voice(uart_code);
                me_stop();
            }
            else{
                set_voice(uart_code);
                me_start();
            }
        }
        else
            serial_command(uart_code, uart_message);
    }
}

// ============================================================ MAIN FUNCTIONS

void task0(void){ // can't use float in ISR
    int_fast16_t out_r = 0, out_l = 0;
    int_fast16_t master_r = 0, master_l = 0;
    int_fast16_t mono = 0;
    uint_fast8_t aux = 0;
    uint_fast32_t localCounter = 0;

    // low-pass filter variables
    static int_fast32_t filter_l = 0;
    static int_fast32_t filter_r = 0;
    int_fast16_t filter_out_r = 0, filter_out_l = 0;

    // reverb variables
    int_fast16_t reverb_out_r = 0, reverb_out_l = 0;

    // DDS engine
    for(aux = 0; aux < VOICEM; aux++){
        localCounter = counter[aux];
        localCounter += increment[aux];
        if(localCounter >= bobMax){
            localCounter -= bobMax;
            if(channel_alloc[aux] == 6){
                increment[aux] = 0;
                localCounter = 0;
                channel_alloc[aux] = 0;
            }
        }
        out_r += (adsr_vca[aux] * lut_r[localCounter >> 10]) >> 7;
        out_l += (adsr_vca[aux] * lut_l[localCounter >> 10]) >> 7;
        counter[aux] = localCounter;
    }
    out_r *= vca1;
    out_l *= vca2;

    // low-pass filter
    filter_r = ((inp_factor * out_r)/filter_factor) + ((out_factor * filter_r)/filter_factor);
    filter_l = ((inp_factor * out_l)/filter_factor) + ((out_factor * filter_l)/filter_factor);
    if(filter_state == 0){
        filter_out_r = out_r;
        filter_out_l = out_l;
    }
    else{
        filter_out_r = filter_r;
        filter_out_l = filter_l;
    }

    // reverb fx
    if(reverb_state == 0){
        master_r = filter_out_r;
        master_l = filter_out_l;
    }
    else{
        master_r = reverb_out_r;
        master_l = reverb_out_l;
    }

    // stereo config
    mono = (master_l + master_r) / 2;
    switch(stereo){
    case 0:
        i2s_output(master_r, master_l);
        break;
    case 1:
        i2s_output(0, mono);
        break;
    case 2:
        i2s_output(mono, 0);
        break;
    default:
        break;
    }
}

void task1(void){
    uint8_t aux = 24;

    // SERIAL IN
    while((serial1_check() / 2) > 0){
        serial_ops();
    }

    voice_counter = 0;
    for(aux = 0; aux < VOICEM; aux++)
        if((channel_alloc[aux] != 0) && (channel_alloc[aux] != 6))
            voice_counter++;

    // AGM
    if(voice_counter == 0){
        vca1 = 0;
        vca2 = 0;
    }
    else{
        vca1 = (agm[voice_counter] * mixer1 * master_volume) / (99 * 99);
        vca2 = (agm[voice_counter] * mixer2 * master_volume) / (99 * 99);
        }

    // ADSR
    for(aux = 0; aux < VOICEM; aux++){
        if(channel_alloc[aux] == 0){
            adsr_counter[aux] = 0;
            adsr_vca[aux] = 0;
        }
        if(channel_alloc[aux] == 1){
            if(adsr_counter[aux] < a_timer){
                adsr_counter[aux]++;
                adsr_vca[aux] = (adsr_counter[aux] * 128) / a_timer;
            }
            else{
                adsr_vca[aux] = 128;
                adsr_counter[aux] = 0;
                channel_alloc[aux] = 2;
            }
        }
        if(channel_alloc[aux] == 2){
            if(adsr_counter[aux] < d_timer){
                adsr_counter[aux]++;
                adsr_vca[aux] = 128 - (((128 - s_level) * adsr_counter[aux]) / (d_timer - 1));
            }
            else{
                adsr_counter[aux] = 0;
                channel_alloc[aux] = 3;
            }
        }
        if(channel_alloc[aux] == 3){
            adsr_vca[aux] = s_level;
            adsr_crop = s_level;
            if(s_timer != 0){
                if(adsr_counter[aux] < s_timer){
                    adsr_counter[aux]++;
                }
                else{
                    adsr_counter[aux] = 0;
                    channel_alloc[aux] = 5;
                }
            }
        }
        if(channel_alloc[aux] == 4){
            adsr_counter[aux] = 0;
            adsr_crop = adsr_vca[aux];
            channel_alloc[aux] = 5;// transição de quebra
        }
        if(channel_alloc[aux] == 5){
            if(adsr_counter[aux] < r_timer){
                adsr_counter[aux]++;
                adsr_vca[aux] = (adsr_crop *(r_timer - adsr_counter[aux])) / r_timer;
            }
            else{
                adsr_counter[aux] = 0;
                channel_alloc[aux] = 6;
            }
        }
    }

    { // LFO1
    switch(lfo1_shape){
    case 0:
        lfo1_input = lut_sinuso[lfo1_counter];
        break;
    case 1:
        lfo1_input = lut_triang[lfo1_counter];
        break;
    case 2:
        lfo1_input = lut_square[lfo1_counter];
        break;
    case 3:
        lfo1_input = lut_sawtoo[lfo1_counter];
        break;
    case 4:
        lfo1_input = lut_invsaw[lfo1_counter];
        break;
    default:
        break;
    }
    lfo1_input = (lfo1_amp * (lfo1_input >> 8)) / 128;
    if(lfo1_counter < LUTSIZE)
        lfo1_counter += lfo1_inc;
    else
        lfo1_counter -= LUTSIZE;
    if(lfo1_state == 1)
        lfo1_output = lfo1_input;
    else
        lfo1_output = 0;
    }

    { // LFO2
    switch(lfo2_shape){
    case 0:
        lfo2_input = lut_sinuso[lfo2_counter];
        break;
    case 1:
        lfo2_input = lut_triang[lfo2_counter];
        break;
    case 2:
        lfo2_input = lut_square[lfo2_counter];
        break;
    case 3:
        lfo2_input = lut_sawtoo[lfo2_counter];
        break;
    case 4:
        lfo2_input = lut_invsaw[lfo2_counter];
        break;
    default:
        break;
    }
    lfo2_input = (lfo2_amp * (lfo2_input >> 8)) / 128;
    if(lfo2_counter < LUTSIZE)
        lfo2_counter += lfo2_inc;
    else
        lfo2_counter -= LUTSIZE;
    if(lfo2_state == 1)
        lfo2_output = lfo2_input;
    else
        lfo2_output = 0;
    }

    if(me_status == 1){// MOD_ENV
        if(me_state == 0){
            me_counter = 0;
            me_output = 0;
            me_state = 1;
        }
        if(me_state == 1){
            if(me_counter < me_a_timer){
                me_counter++;
                me_output = (me_counter * 100) / me_a_timer;
            }
            else{
                me_output = 100;
                me_counter = 0;
                me_state = 2;
            }
        }
        if(me_state == 2){
            if(me_counter < me_d_timer){
                me_counter++;
                me_output = 100 - (((100 - me_s_level) * me_counter) / (me_d_timer - 1));
            }
            else{
                me_counter = 0;
                me_state = 3;
            }
        }
        if(me_state == 3){
            me_output = me_s_level;
            if(me_s_timer != 0){
                if(me_counter < me_s_timer){
                    me_counter++;
                }
                else{
                    me_counter = 0;
                    me_state = 4;
                }
            }
        }
        if(me_state == 4){
            me_crop = me_output;
            if(me_counter < me_r_timer){
                me_counter++;
                me_output = (me_crop *(me_r_timer - me_counter)) / me_r_timer;
            }
            else{
                me_state = 5;
            }
        }
        me_output = (me_max * me_output) / 100;
    }
    else{
        me_counter = 0;
        me_output = 250;
    }

    {// controls
    /*
    none = 0,
    lfo1 = 1,
    lfo2 = 2,
    noiser = 3,
    note = 4,
    speed = 5,
    pitch_w = 6,
    mod_w = 7,
    slider1 = 8,
    slider2 = 9,
    slider3 = 10,
    slider4 = 11,
    slider5 = 12
    */
    }
}

/* all core 1 operations will be executed in task1()
void core1_loop(void){
    vTaskDelay(1);
}
*/

void task_init(void){
    uint16_t aux = 0;

    uint8_t reduction = VOICEM;

    for(aux = 0; aux < LUTSIZE; aux++){
        lut_l[aux] /= reduction;
        lut_r[aux] /= reduction;
    }

    adjust_filter(0b00010000, 0); // 10000 = 0b 0010 0111 0001 0000
    adjust_filter(0b00100111, 0);
}
