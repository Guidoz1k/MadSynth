#include "main.h"
/*
    Arduino Mega Board:
    PORTA: 8 pins --------------> Keyboard
    PORTB: 8 pins --------------> UI buttons
    PORTC: 8 pins --------------> LCD data pins
    PORTD: 5 pins (0,1,2,3,7) --> pins 2 + 3 are UART1
    PORTE: 3 pins (U,U,3,4,5) --> U = UART0 + LCD control pins
    PORTF: 8 analog pins -------> analog inputs
    PORTG: 4 pins (0,1,2,5)
    PORTH: 6 pins (0,1,3,4,5,6)
    PORTI: Does not exist
    PORTJ: 2 pins (0,1)
    PORTK: 8 analog pins
    PORTL: 8 pins
*/

// keyboard dual press state machine
struct _key{
    //volatile uint_fast8_t pinX;
    //volatile uint_fast8_t pinY;
    volatile uint_fast8_t counter;      // time between key's pins x and y press
    volatile uint_fast8_t stateMachine; // key's state machine
} key_detection[MAXKEYS];               // all x's and y's

// main loop lag function variables
volatile uint8_t timing = 0;
volatile uint16_t time_counter = 0;

// UI variables
uint8_t pip_mod = 0;
uint8_t menu_state = 0;     // UI machine state variable
uint8_t modifier_allocation[14] = {
    18, // 00 - none
    0, // 01 - lfo1
    0, // 02 - lfo2
    0, // 03 - noiser
    0, // 04 - note
    0, // 05 - speed
    0, // 06 - pitch_w
    0, // 07 - mod_w
    0, // 08 - slider1
    0, // 09 - slider2
    0, // 10 - slider3
    0, // 11 - slider4
    0, // 12 - slider5
    0, // 13 - MOD ENV
};
typedef enum _texts{
    modifier_t = 0,
    shape_t = 1,
    oscm_t = 2,
    stereo_t = 3,
    soscm_t = 4,
    state_t = 5
} TEXTS;
char MODIFIER_TEXT[14][26] = {
    " none                     ",
    " LFO 1                    ",
    " LFO 2                    ",
    " Noiser   X               ",
    " Note     X               ",
    " Speed    X               ",
    " Pitch Wheel              ",
    " Pitch Mod                ",
    " Slider 1                 ",
    " Slider 2                 ",
    " Slider 3                 ",
    " Slider 4                 ",
    " Slider 5                 ",
    " Mod Envelope             "
};
char SHAPE_TEXT[5][26] = {
    " Sinusoid                 ",
    " Triangle                 ",
    " Square                   ",
    " Sawtooth                 ",
    " Inverted Sawtooth        "
};
char O_MODE_TEXT[2][26] = {
    " Free                     ",
    " Retrigger                "
};
char STEREO_TEXT[3][26] = {
    " Left + Right             ",
    " Left Only                ",
    " Right Only               "
};
/* all sub oscillators are harmonic
char SO_MODE_TEXT[2][26] = {
    " Harmonic                 ",
    " Independent              "
};
*/
char STATE_TEXT[2][26] = {
    " Off                      ",
    " On                       "
};
// ============================================================ accessory functions

void ui_pip_label(void){
    lcd_write_string("M     R   L  ", 13, 0, 27);
    lcd_write_string("OSC1         ", 13, 1, 27);
    lcd_write_string("OSC2         ", 13, 2, 27);
}

void ui_pip_update( uint8_t volume, uint8_t mixer1, uint8_t mixer2, uint8_t osc1_shape,
                    uint8_t osc1_osc_count, uint8_t osc2_shape, uint8_t osc2_osc_count,
                    uint8_t lfo1_state, uint8_t lfo2_state, uint8_t me_state){
    if(pip_mod == 1){
        lcd_write_number(volume, 2, 0, 28);
        lcd_write_number(mixer1, 2, 0, 34);
        lcd_write_number(mixer2, 2, 0, 38);
        /*
        typedef enum _shape{
            sinusoid = 0,
            triangle = 1,
            square = 2,
            sawtooth = 3,
            inv_sawtooth = 4
        } SHAPE;
        */
        switch(osc1_shape){
        case 0:
            lcd_write_string("sinus", 5, 1, 32);
            break;
        case 1:
            lcd_write_string("trian", 5, 1, 32);
            break;
        case 2:
            lcd_write_string("squar", 5, 1, 32);
            break;
        case 3:
            lcd_write_string("sawto", 5, 1, 32);
            break;
        case 4:
            lcd_write_string("invsa", 5, 1, 32);
            break;
        default:
            break;
        }
        switch(osc2_shape){
        case 0:
            lcd_write_string("sinus", 5, 2, 32);
            break;
        case 1:
            lcd_write_string("trian", 5, 2, 32);
            break;
        case 2:
            lcd_write_string("squar", 5, 2, 32);
            break;
        case 3:
            lcd_write_string("sawto", 5, 2, 32);
            break;
        case 4:
            lcd_write_string("invsa", 5, 2, 32);
            break;
        default:
            break;
        }
        lcd_write_number(osc1_osc_count + 1, 2, 1, 38);
        lcd_write_number(osc2_osc_count + 1, 2, 2, 38);
        if(lfo1_state)
            lcd_write_string("LFO1", 4, 3, 27);
        else
            lcd_write_string("    ", 4, 3, 27);
        if(lfo2_state)
            lcd_write_string("LFO2", 4, 3, 32);
        else
            lcd_write_string("    ", 4, 3, 32);
        if(me_state)
            lcd_write_string("MOD", 3, 3, 37);
        else
            lcd_write_string("   ", 3, 3, 37);
        pip_mod = 0;
    }
}

// function that clears the pip screen
void ui_pip_clear(void){
    lcd_write_string("             ", 13, 0, 27);
    lcd_write_string("             ", 13, 1, 27);
    lcd_write_string("             ", 13, 2, 27);
    lcd_write_string("             ", 13, 3, 27);
}

// function that allows user to set all digits in a parameter
uint16_t ui_digit_set(uint16_t old_value, int8_t ticks, uint8_t digits, uint16_t min, uint16_t max){
    int16_t unlimited = (int16_t)old_value;
    uint16_t new_value = old_value;

    if(ticks != 0){
        unlimited += (int16_t)ticks;
        if(unlimited >= (int16_t)min){
            if(unlimited <= (int16_t)max)
                new_value = (uint16_t)unlimited;
            else
                new_value = max;
        }
        else
            new_value = min;

        lcd_write_number(new_value, digits, 3, DIGIT_POS - digits);
    }
    return new_value;
}

// function that allows user to set all digits in a parameter
int16_t ui_signed_digit_set(int16_t old_value, int8_t ticks, uint8_t digits, int16_t min, int16_t max){
    int16_t unlimited = (int16_t)old_value;
    int16_t new_value = old_value;

    if(ticks != 0){
        unlimited += (int16_t)ticks;
        if(unlimited >= (int16_t)min){
            if(unlimited <= (int16_t)max)
                new_value = (uint16_t)unlimited;
            else
                new_value = max;
        }
        else
            new_value = min;

        lcd_signed_number(new_value, digits, 3, DIGIT_POS - digits - 1);
    }
    return new_value;
}

uint8_t ui_text_set(uint8_t old_value, int8_t ticks, TEXTS text_type){
    int8_t unlimited = (int8_t)old_value;
    uint8_t new_value = old_value;

    if(ticks != 0)
        switch(text_type){
        case modifier_t:
            unlimited += ticks;
            if(unlimited >= 0){
                if(unlimited <= 13)
                    new_value = (uint8_t)unlimited;
                else
                    new_value = 13;
            }
            else
                new_value = 0;

            lcd_write_string(MODIFIER_TEXT[new_value], 26, 3, 0);
            break;
        case shape_t:
            unlimited += ticks;
            if(unlimited >= 0){
                if(unlimited < 4)
                    new_value = (uint16_t)unlimited;
                else
                    new_value = 4;
            }
            else
                new_value = 0;

            lcd_write_string(SHAPE_TEXT[new_value], 26, 3, 0);
            break;
        case oscm_t:
            unlimited += ticks;
            if(unlimited >= 0){
                if(unlimited <= 1)
                    new_value = (uint16_t)unlimited;
                else
                    new_value = 1;
            }
            else
                new_value = 0;

            lcd_write_string(O_MODE_TEXT[new_value], 26, 3, 0);
            break;
        case stereo_t:
            unlimited += ticks;
            if(unlimited >= 0){
                if(unlimited <= 2)
                    new_value = (uint16_t)unlimited;
                else
                    new_value = 2;
            }
            else
                new_value = 0;

            lcd_write_string(STEREO_TEXT[new_value], 26, 3, 0);
            break;
        case soscm_t:
        /* all sub oscillators are harmonic
            unlimited += ticks;
            if(unlimited >= 0){
                if(unlimited <= 2)
                    new_value = (uint8_t)unlimited;
                else
                    new_value = 2;
            }
            else
                new_value = 0;

            lcd_write_string(SO_MODE_TEXT[new_value], 26, 3, 0);
            break;
        */
        case state_t:
            unlimited += ticks;
            if(unlimited >= 0){
                if(unlimited <= 1)
                    new_value = (uint8_t)unlimited;
                else
                    new_value = 1;
            }
            else
                new_value = 0;

            lcd_write_string(STATE_TEXT[new_value], 26, 3, 0);
            break;
        }

    return new_value;
}

void control_update(void){
    if(modifier_allocation[6] != 0)
        serial1_transmit(200, adc_read(0));
    if(modifier_allocation[7] != 0)
        serial1_transmit(201, adc_read(1));
    if(modifier_allocation[8] != 0)
        serial1_transmit(202, adc_read(2));
    if(modifier_allocation[9] != 0)
        serial1_transmit(203, adc_read(3));
    if(modifier_allocation[10] != 0)
        serial1_transmit(204, adc_read(4));
    if(modifier_allocation[11] != 0)
        serial1_transmit(205, adc_read(5));
    if(modifier_allocation[12] != 0)
        serial1_transmit(206, adc_read(6));
}

void change_submenu(uint8_t submenu){
    lcd_write_string("                          ", 26, 3, 0);
    lcd_write_string("                          ", 26, 2, 0);
    menu_state = submenu;
}

void transmit_on_change(uint16_t code, uint8_t message, uint8_t size){
    static uint16_t old_value = 0xFF;
    static uint8_t old_state = 0;

    if(menu_state != old_state){
        old_value = 0xFF;
        old_state = menu_state;
        pip_mod = 1;
    }

    switch(size){
    case 1:
        if(message != old_value){
            serial1_transmit(code, message);
            old_value = message;
            pip_mod = 1;
        }
        break;
    case 2:
        if(message != old_value){
            serial1_transmit(code, (0x00FF) & message);
            serial1_transmit(code + 1, message >> 8);
            old_value = message;
            pip_mod = 1;
        }
        break;
    default:
        break;
    }
}

// ============================================================ main core functions

// delay function for the main loop
void lag(uint16_t time){
    timing = 1;

    while(time_counter != time);

    timing = 0;
    time_counter = 0;
}

// 1ms interrupt function
ISR(TIMER0_COMPA_vect){
    // 100 * interrupt time = 100ms is the maximum time between x - y signals
    static const uint8_t MAXPRESS = 100;

    uint8_t i = 0;
    uint8_t key = 0;

    if(timing)
        time_counter++;

    led_on();
    for(i = 0; i < MAXKEYS; i++){
        keyboard_write(i);
        if(i < 61){     // key's x pin
            if(keyboard_read()) // key's x == 1
                switch(key_detection[i].stateMachine){
                case 0:
                    key_detection[i].stateMachine = 1;
                    break;
                case 1:
                    key_detection[i].counter++;
                    if(key_detection[i].counter == MAXPRESS)
                        key_detection[i].stateMachine = 2;
                    break;
                case 2:
                    break;
                case 3:
                    break;
                case 4:
                    break;
                case 5:
                    break;
                case 6:
                    break;
                default:
                    break;
                }
            else                // key's x == 0
                switch(key_detection[i].stateMachine){
                case 0:
                    break;
                case 1:
                    // key_detection[i].stateMachine = 6; IS THIS GOOD ACTUALLY?
                    // actually not using this implements a debounce (sort of)
                    break;
                case 2:
                    break;
                case 3:
                    key_detection[i].stateMachine = 4;
                    break;
                case 4:
                    break;
                case 5:
                    key_detection[i].stateMachine = 6;
                    break;
                case 6:
                    break;
                default:
                    break;
                }
        }
        else{           // key's y pin
            key = i - 61;
            if(keyboard_read()) // key's y == 1
                switch(key_detection[key].stateMachine){
                case 0:
                    break;
                case 1:
                    key_detection[key].stateMachine = 2;
                    break;
                case 2:
                    break;
                case 3:
                    break;
                case 4:
                    break;
                case 5:
                    break;
                case 6:
                    break;
                default:
                    break;
                }
            else                // key's y == 0
                switch(key_detection[key].stateMachine){
                case 0:
                    break;
                case 1:
                    break;
                case 2:
                    break;
                case 3:
                    key_detection[key].stateMachine = 5;
                    break;
                case 4:
                    key_detection[key].stateMachine = 6;
                    break;
                case 5:
                    break;
                case 6:
                    break;
                default:
                    break;
                }
        }
    }
    led_off();
}

// asynchronous main loop
void loop(){
    static const uint8_t INTERFACE = 10;    // interface delay in ms
    static const uint8_t BUTTONHOLD = 15;   // 15 * INTERFACE = 150ms of delay between valid button presses

    uint8_t i = 0;
    uint8_t modification = 0;

    uint8_t button_pressed = 0;     // records the value of the button each iteration
    uint8_t encoder_rotation = 0;   // records the value of the accumulated encoder rotations
    static uint8_t button_hold = 0; // counter to limit the button interaction

    typedef enum _mod{
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
        slider5 = 12,
        mod_env = 13
    } MODIFIER;
    typedef enum _shape{
        sinusoid = 0,
        triangle = 1,
        square = 2,
        sawtooth = 3,
        inv_sawtooth = 4
    } SHAPE;
    typedef enum _oscm{
        frees = 0,
        retriggers = 1
    } O_MODE;
    /* all sub oscillators are harmonic
    typedef enum _soscm{
        harmonic = 0,
        independent = 1
    } SO_MODE;
    */
    typedef enum _state{
        off_t = 0,
        on_t = 1
    } STATE;
    static uint8_t volume = 50;  // 0 ... 99
    static enum _stereo{
        l_r,
        l_only,
        r_only
    } stereo = l_r;

    //static uint8_t osc_count_limit = 1;
    //static uint8_t max_osc_mode = frees;

    /* oct_trans
    piano has 9 octaves, our synth has 5, it starts at the second already
    0 ... (4 * 12) = 48
    */
    static uint8_t oct_trans = 2; // 2 * 12
    //static int8_t note_trans = 0; // -12 ... 12
    static uint8_t mixer1 = 50; // mixer's first channel
    static uint8_t mixer2 = 50; // mixer's second channel

    static SHAPE osc1_shape = sinusoid;
    static uint8_t osc1_osc_count = 0; // 0 ... 5(?)
    //static SO_MODE osc1_osc_mode = harmonic;              all sub oscillators are harmonic
    //static uint8_t osc1_osc_unison = 0; // 0 ... 100
    static int8_t osc1_trans = 0;  // transpose -24 ... 24
    static int8_t osc1_cent = 0;   // -99 ... 99
    static MODIFIER osc1_trans_mod = none; // modifier for osc trans
    static MODIFIER osc1_cent_mod = none;  // modifier for osc cent

    static SHAPE osc2_shape = sinusoid;
    static uint8_t osc2_osc_count = 0; // 0 ... 5(?)
    //static SO_MODE osc2_osc_mode = harmonic;              all sub oscillators are harmonic
    //static uint8_t osc2_osc_unison = 0; // 0 ... 100
    //static int8_t osc2_trans = 0;  // transpose -24 ... 24
    //static int8_t osc2_cent = 0;   // -99 ... 99
    //static MODIFIER osc2_trans_mod = none; // modifier for osc trans
    //static MODIFIER osc2_cent_mod = none;  // modifier for osc cent

    /* in case one day OSC 3 is implemented
    static SHAPE osc3_shape = sinusoid;
    static uint8_t osc3_osc_count = 0; // 0 ... 5(?)
    //static SO_MODE osc3_osc_mode = harmonic;              all sub oscillators are harmonic
    //static uint8_t osc3_osc_unison = 0; // 0 ... 100
    static int8_t osc3_trans = 0;  // transpose -24 ... 24
    static int8_t osc3_cent = 0;   // 0 ... 100
    static MODIFIER osc3_trans_mod = none; // modifier for osc trans
    static MODIFIER osc3_cent_mod = none;  // modifier for osc cent
    */

    static uint16_t adsr_a = 0;    // 0 ... 10000 ms
    static uint16_t adsr_d = 0;    // 0 ... 10000 ms
    static uint16_t adsr_s = 0;    // 0 ... 10000 ms
    static uint16_t adsr_sl = 0;   // 0 ... 100%
    static uint16_t adsr_r = 1;    // 1 ... 10000 ms
    static MODIFIER adsr_a_mod = none;    // modifier for adsr A parameter
    static MODIFIER adsr_d_mod = none;    // modifier for adsr D parameter
    static MODIFIER adsr_s_mod = none;    // modifier for adsr S parameter
    static MODIFIER adsr_sl_mod = none;   // modifier for adsr SL parameter
    static MODIFIER adsr_r_mod = none;    // modifier for adsr R parameter

    static uint8_t amp_pw = 0;  // 0 ... 99
    static uint8_t amp_mw = 0;  // 0 ... 99
    static uint8_t amp_kn = 0;  // 0 ... 99
    static uint8_t amp_ks = 0;  // 0 ... 99
    static uint8_t amp_s1 = 0;  // 0 ... 99
    static uint8_t amp_s2 = 0;  // 0 ... 99
    static uint8_t amp_s3 = 0;  // 0 ... 99
    static uint8_t amp_s4 = 0;  // 0 ... 99
    static uint8_t amp_s5 = 0;  // 0 ... 99

    static STATE lfo1_state = off_t; // 0 - off, 1 - on
    static O_MODE lfo1_mode = frees;
    static SHAPE lfo1_shape = sinusoid;
    static uint8_t lfo1_amp = 0;   // 0 ... 100
    static uint8_t lfo1_freq = 0;  // 0 ... 250
    static MODIFIER lfo1_amp_mod = none;  // modifier for lfo1 amplitude
    static MODIFIER lfo1_freq_mod = none; // modifier for lfo1 frequency

    static STATE lfo2_state = off_t; // 0 - off, 1 - on
    static O_MODE lfo2_mode = frees;
    static SHAPE lfo2_shape = sinusoid;
    static uint8_t lfo2_amp = 0;   // 0 ... 100
    static uint8_t lfo2_freq = 0;  // 0 ... 250
    static MODIFIER lfo2_amp_mod = none;  // modifier for lfo2 amplitude
    static MODIFIER lfo2_freq_mod = none; // modifier for lfo2 frequency

    /* ainda nao implementado
    static uint8_t rand_state = 0; // 0 - off, 1 - on
    static uint8_t rand_amp = 0;   // 0 ... 100
    */

    // MOD ENV variables
    static STATE me_state = off_t;      // 0 - off, 1 - on
    static uint16_t me_a = 0;       // 0 ... 10000 ms
    static uint16_t me_d = 0;       // 0 ... 10000 ms
    static uint16_t me_s = 0;       // 0 ... 10000 ms
    static uint16_t me_sl = 0;      // 0 ... 100%
    static uint16_t me_r = 1;       // 1 ... 10000 ms
    static MODIFIER me_a_mod = none;    // modifier for MOD ENV A parameter
    static MODIFIER me_d_mod = none;    // modifier for MOD ENV D parameter
    static MODIFIER me_s_mod = none;    // modifier for MOD ENV S parameter
    static MODIFIER me_sl_mod = none;   // modifier for MOD ENV SL parameter
    static MODIFIER me_r_mod = none;    // modifier for MOD ENV R parameter

    // low-pass filter variables
    //static STATE filter_state = off_t;
    //static uint16_t cutoff_freq = 0;

    // reverb effect variables
    // SOCORR

    // key state machine to serial routine
    for(i = 0; i < (MAXKEYS / 2); i++){
        switch(key_detection[i].stateMachine){
        case 0:
            break;
        case 1:
            break;
        case 2: // key is on
            //serial0_write_number(i, 2, 0);
            //serial0_write_string(" ON = ", 0);
            //serial0_write_number(key_detection[i].counter, 3, 1);
            if(key_detection[i].counter == 0)
                key_detection[i].counter = 1;
            serial1_transmit(i + 1, key_detection[i].counter);
            key_detection[i].stateMachine = 3;
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        case 6: // key is off
            //serial0_write_number(i, 2, 0);
            //serial0_write_string(" OFF ", 1);
            serial1_transmit(i + 1, 0);
            key_detection[i].stateMachine = 0;
            key_detection[i].counter = 0;
            break;
        default:
            break;
        }
    }

    // limits the amount of button presses over the short time of the interface delay (INTERFACE)
    if(button_hold != 0){
        button_hold--;
    }
    else{
        button_pressed = inputs_read();
        if(button_pressed != 0)
            button_hold = BUTTONHOLD;
        else
            encoder_rotation = inputs_rotation();   // gets the rotation of the encoder
    }

    control_update();
    ui_pip_update(  volume, mixer1, mixer2,
                    osc1_shape, osc1_osc_count, osc2_shape, osc2_osc_count,
                    lfo1_state, lfo2_state, me_state);

    // UI machine state with all the screen texts and DDS configurations
    switch(menu_state){
    { // MAIN MENU
    case 0: // Master channel setup
        lcd_write_string("00 Master channel setup  ~", 26, 0, 0);
        switch(button_pressed){
        case 1:
            break;
        case 2:
            break;
        case 3:
            lcd_write_string("  ", 26, 0, 24);
            lcd_write_number(volume, 2, 3, DIGIT_POS - 2);
            menu_state = 30;
            break;
        case 4:
            menu_state = 2;
            break;
        case 5:
            break;
        default:
            break;
        }
        break;
    /*
    case 1: // Oscillators allocation
        lcd_write_string("01 OSC allocation        ~", 26, 0, 0);
        lcd_write_char(127, 0, 24);
        switch(button_pressed){
        case 1:
            break;
        case 2:
            break;
        case 3:
            lcd_write_string("  ", 26, 0, 24);
            lcd_write_number(osc_count_limit, 2, 3, DIGIT_POS - 2);
            menu_state = 40;
            break;
        case 4:
            menu_state = 2;
            break;
        case 5:
            menu_state = 0;
            break;
        default:
            break;
        }
        break;
    */
    case 2: // Keyboard offset config
        lcd_write_string("01 Keyboard config       ~", 26, 0, 0);
        lcd_write_char(127, 0, 24);
        switch(button_pressed){
        case 1:
            break;
        case 2:
            break;
        case 3:
            lcd_write_string("  ", 26, 0, 24);
            lcd_write_number(oct_trans, 2, 3, DIGIT_POS - 2);
            menu_state = 50;
            break;
        case 4:
            menu_state = 3;
            break;
        case 5:
            menu_state = 0;
            break;
        default:
            break;
        }
        break;
    case 3: // Stereo Oscillator Mixer
        lcd_write_string("02 Oscillators mixer     ~", 26, 0, 0);
        lcd_write_char(127, 0, 24);
        switch(button_pressed){
        case 1:
            break;
        case 2:
            break;
        case 3:
            lcd_write_string("  ", 26, 0, 24);
            lcd_write_number(mixer1, 2, 3, DIGIT_POS - 2);
            menu_state = 61; // 5A
            break;
        case 4:
            menu_state = 4;
            break;
        case 5:
            menu_state = 2;
            break;
        default:
            break;
        }
        break;
    case 4: // Oscillator 1 controls
        lcd_write_string("03 OSC 1 controls        ~", 26, 0, 0);
        lcd_write_char(127, 0, 24);
        switch(button_pressed){
        case 1:
            break;
        case 2:
            break;
        case 3:
            lcd_write_string("  ", 26, 0, 24);
            lcd_write_string(SHAPE_TEXT[osc1_shape], 26, 3, 0);
            menu_state = 71; // 5A
            break;
        case 4:
            menu_state = 5;
            break;
        case 5:
            menu_state = 3;
            break;
        default:
            break;
        }
        break;
    case 5: // Oscillator 2 controls
        lcd_write_string("04 OSC 2 controls        ~", 26, 0, 0);
        lcd_write_char(127, 0, 24);
        switch(button_pressed){
        case 1:
            break;
        case 2:
            break;
        case 3:
            lcd_write_string("  ", 26, 0, 24);
            lcd_write_string(SHAPE_TEXT[osc2_shape], 26, 3, 0);
            menu_state = 91; // 5A
            break;
        case 4:
            menu_state = 6;
            break;
        case 5:
            menu_state = 4;
            break;
        default:
            break;
        }
        break;
    case 6: // ADSR-5 parameters
        lcd_write_string("05 ADSR-5 parameters     ~", 26, 0, 0);
        lcd_write_char(127, 0, 24);
        switch(button_pressed){
        case 1:
            break;
        case 2:
            break;
        case 3:
            lcd_write_string("  ", 26, 0, 24);
            lcd_write_number(adsr_a, 3, 3, DIGIT_POS - 3);
            menu_state = 111; // 5A
            break;
        case 4:
            menu_state = 7;
            break;
        case 5:
            menu_state = 5;
            break;
        default:
            break;
        }
        break;
    case 7: // Modifiers bend/mod/press
        lcd_write_string("06 Control modifiers     ~", 26, 0, 0);
        lcd_write_char(127, 0, 24);
        switch(button_pressed){
        case 1:
            break;
        case 2:
            break;
        case 3:
            lcd_write_string("  ", 26, 0, 24);
            lcd_write_number(amp_pw, 2, 3, DIGIT_POS - 2);
            menu_state = 130;
            break;
        case 4:
            menu_state = 8;
            break;
        case 5:
            menu_state = 6;
            break;
        default:
            break;
        }
        break;
    case 8: // LFO controls
        lcd_write_string("07 LFO 1 Controls        ~", 26, 0, 0);
        lcd_write_char(127, 0, 24);
        switch(button_pressed){
        case 1:
            break;
        case 2:
            break;
        case 3:
            lcd_write_string("  ", 26, 0, 24);
            lcd_write_string(STATE_TEXT[lfo1_state], 26, 3, 0);
            menu_state = 150;
            break;
        case 4:
            menu_state = 9;
            break;
        case 5:
            menu_state = 7;
            break;
        default:
            break;
        }
        break;
    case 9: // LFO controls
        lcd_write_string("08 LFO 2 Controls        ~", 26, 0, 0);
        lcd_write_char(127, 0, 24);
        switch(button_pressed){
        case 1:
            break;
        case 2:
            break;
        case 3:
            lcd_write_string("  ", 26, 0, 24);
            lcd_write_string(STATE_TEXT[lfo2_state], 26, 3, 0);
            menu_state = 160;
            break;
        case 4:
            menu_state = 10;
            break;
        case 5:
            menu_state = 8;
            break;
        default:
            break;
        }
        break;
    case 10: // Randomizer controls
        lcd_write_string("09 Noise controls      X ~", 26, 0, 0);
        lcd_write_char(127, 0, 24);
        switch(button_pressed){
        case 1:
            break;
        case 2:
            break;
        case 3:
            /* not yet ready
            lcd_write_string("  ", 26, 0, 24);
            menu_state = 170;
            */
            break;
        case 4:
            menu_state = 11;
            break;
        case 5:
            menu_state = 9;
            break;
        default:
            break;
        }
        break;
    case 11: // Modifier Envelope controls
        lcd_write_string("10 MOD ENV controls       ", 26, 0, 0);
        lcd_write_char(127, 0, 24);
        switch(button_pressed){
        case 1:
            break;
        case 2:
            break;
        case 3:
            lcd_write_string("  ", 26, 0, 24);
            lcd_write_string(STATE_TEXT[me_state], 26, 3, 0);
            menu_state = 180;
            break;
        case 4:
            break;
        case 5:
            menu_state = 10;
            break;
        default:
            break;
        }
        break;
    
    }

    {}

    { // Master channel setup
    case 30:
        lcd_write_string("00 Volume                ~", 26, 2, 0);
        volume = ui_digit_set(volume, encoder_rotation, 2, 0, 99);
        transmit_on_change(110, volume, 1);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            change_submenu(0);
            break;
        case 3:
            break;
        case 4:
            change_submenu(31);
            lcd_write_string(STEREO_TEXT[stereo], 26, 3, 0);
            break;
        case 5:
            break;
        default:
            break;
        }
        break;
    case 31:
        lcd_write_string("01 Stereo                 ", 26, 2, 0);
        lcd_write_char(127, 2, 24);
        stereo = ui_text_set(stereo, encoder_rotation, stereo_t);
        transmit_on_change(111, stereo, 1);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            change_submenu(0);
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            change_submenu(30);
            lcd_write_number(volume, 2, 3, DIGIT_POS - 2);
            break;
        default:
            break;
        }
        break;
    }

/*
    {}

    { // Oscillators allocation NOT IMPLEMENTED
    case 40:
        lcd_write_string("00 Osc count limit       ~", 26, 2, 0);
        osc_count_limit = ui_digit_set(osc_count_limit, encoder_rotation, 2, 1, MAXVOICES);
        transmit_on_change(112, osc_count_limit, 1);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            change_submenu(1);
            break;
        case 3:
            break;
        case 4:
            change_submenu(41);
            lcd_write_string(O_MODE_TEXT[max_osc_mode], 26, 3, 0);
            break;
        case 5:
            break;
        default:
            break;
        }
        break;
    case 41:
        lcd_write_string("01 Max oscillator mode    ", 26, 2, 0);
        lcd_write_char(127, 2, 24);
        max_osc_mode = ui_text_set(max_osc_mode, encoder_rotation, oscm_t);
        transmit_on_change(113, max_osc_mode, 1);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            change_submenu(1);
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            change_submenu(40);
            lcd_write_number(osc_count_limit, 2, 3, DIGIT_POS - 2);
            break;
        default:
            break;
        }
        break;
    }
*/
    {}


    { // Keyboard offset config
    case 50:
        lcd_write_string("00 Octave                 ", 26, 2, 0);
        oct_trans = ui_digit_set(oct_trans, encoder_rotation, 2, 0, 4);
        transmit_on_change(114, oct_trans, 1);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            change_submenu(2);
            break;
        case 3:
            break;
        case 4:
            /* only 61 oscillators
            change_submenu(51);
            lcd_signed_number(note_trans, 2, 3, DIGIT_POS - 3);
            */
            break;
        case 5:
            break;
        default:
            break;
        }
        break;
    /* only 61 oscillators
    case 51:
        lcd_write_string("01 Transpose              ", 26, 2, 0);
        lcd_write_char(127, 2, 24);
        note_trans = ui_signed_digit_set(note_trans, encoder_rotation, 2, -12, 12);
        transmit_on_change(115, note_trans, 1);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            change_submenu(2);
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            change_submenu(50);
            lcd_write_number(oct_trans, 2, 3, DIGIT_POS - 2);
            break;
        default:
            break;
        }
        break;
    */
    }

    {}

    { // Stereo Oscillator Mixer
    /* 5A
    case 60:
        lcd_write_string("00 Mixer Aconfig         ~", 26, 2, 0);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            change_submenu(3);
            break;
        case 3:
            break;
        case 4:
            change_submenu(61);
            lcd_write_number(mixer1, 2, 3, DIGIT_POS - 2);
            break;
        case 5:
            break;
        default:
            break;
        }
        break;
    */
    case 61:
        lcd_write_string("01 Oscillator 1          ~", 26, 2, 0);
        lcd_write_char(127, 2, 24);
        mixer1 = ui_digit_set(mixer1, encoder_rotation, 2, 0, 99);
        transmit_on_change(116, mixer1, 1);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            change_submenu(3);
            break;
        case 3:
            break;
        case 4:
            change_submenu(62);
            lcd_write_number(mixer2, 2, 3, DIGIT_POS - 2);
            break;
        case 5:
            //change_submenu(60);
            break;
        default:
            break;
        }
        break;
    case 62:
        lcd_write_string("02 Oscillator 2           ", 26, 2, 0);
        lcd_write_char(127, 2, 24);
        mixer2 = ui_digit_set(mixer2, encoder_rotation, 2, 0, 99);
        transmit_on_change(117, mixer2, 1);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            change_submenu(3);
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            change_submenu(61);
            lcd_write_number(mixer1, 2, 3, DIGIT_POS - 2);
            break;
        default:
            break;
        }
        break;
    }

    {}

    { // Oscillator 1 controls
    /* 5A
    case 70:
        lcd_write_string("00 Osc 1 Aconfig         ~", 26, 2, 0);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            lcd_write_string("                          ", 26, 2, 0);
            menu_state = 4;
            break;
        case 3:
            break;
        case 4:
            change_submenu(71);
            break;
        case 5:
            break;
        default:
            break;
        }
        break;
    */
    case 71:
        lcd_write_string("01 Shape                 ~", 26, 2, 0);
        //lcd_write_char(127, 2, 24);
        osc1_shape = ui_text_set(osc1_shape, encoder_rotation, shape_t);
        transmit_on_change(120, osc1_shape, 1);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            change_submenu(4);
            break;
        case 3:
            break;
        case 4:
            change_submenu(72);
            lcd_write_number(osc1_osc_count, 2, 3, DIGIT_POS - 2);
            break;
        case 5:
            //change_submenu(70);
            break;
        default:
            break;
        }
        break;
    case 72:
        lcd_write_string("02 Sub-oscillators       ~", 26, 2, 0);
        lcd_write_char(127, 2, 24);
        osc1_osc_count  = ui_digit_set(osc1_osc_count, encoder_rotation, 2, 0, MAXSVOICES);
        transmit_on_change(121, osc1_osc_count, 1);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            change_submenu(4);
            break;
        case 3:
            break;
        case 4:
            change_submenu(75);
            lcd_signed_number(osc1_trans, 2, 3, DIGIT_POS - 3);
            break;
        case 5:
            change_submenu(71);
            lcd_write_string(SHAPE_TEXT[osc1_shape], 26, 3, 0);
            break;
        default:
            break;
        }
        break;
    /* all sub oscillators are harmonic
    case 73:
        lcd_write_string("03 Sub-oscillators mode  ~", 26, 2, 0);
        lcd_write_char(127, 2, 24);
        //lcd_write_char(127, 2, 24);
        osc1_osc_mode  = ui_text_set(osc1_osc_mode , encoder_rotation, soscm_t);
        serial1_transmit(120, osc1_osc_mode );
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            lcd_write_string("                          ", 26, 2, 0);
            menu_state = 4;
            break;
        case 3:
            break;
        case 4:
            menu_state = 74;
            break;
        case 5:
            menu_state = 72;
            break;
        default:
            break;
        }
        break;
    case 74:
        lcd_write_string("04 Sub-osc unison        ~", 26, 2, 0);
        lcd_write_char(127, 2, 24);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            lcd_write_string("                          ", 26, 2, 0);
            menu_state = 4;
            break;
        case 3:
            break;
        case 4:
            menu_state = 75;
            break;
        case 5:
            menu_state = 73;
            break;
        default:
            break;
        }
        break;
    */
    case 75:
        lcd_write_string("05 Transpose             ~", 26, 2, 0);
        lcd_write_char(127, 2, 24);
        osc1_trans = ui_signed_digit_set(osc1_trans, encoder_rotation, 2, -MAXTRANS / 2, MAXTRANS / 2);
        transmit_on_change(124, osc1_trans, 1);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            change_submenu(4);
            break;
        case 3:
            break;
        case 4:
            change_submenu(76);
            lcd_signed_number(osc1_cent, 2, 3, DIGIT_POS - 3);
            break;
        case 5:
            change_submenu(72);
            lcd_write_number(osc1_osc_count, 2, 3, DIGIT_POS - 2);
            break;
        default:
            break;
        }
        break;
    case 76:
        lcd_write_string("06 Cent variation        ~", 26, 2, 0);
        lcd_write_char(127, 2, 24);
        osc1_cent = ui_signed_digit_set(osc1_cent, encoder_rotation, 2, -99, 99);
        transmit_on_change(125, osc1_cent, 1);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            change_submenu(4);
            break;
        case 3:
            break;
        case 4:
            change_submenu(77);
            lcd_write_string(MODIFIER_TEXT[osc1_trans_mod], 26, 3, 0);
            break;
        case 5:
            change_submenu(75);
            lcd_signed_number(osc1_trans, 2, 3, DIGIT_POS - 3);
            break;
        default:
            break;
        }
        break;
    case 77:
        lcd_write_string("07 Transpose control     ~", 26, 2, 0);
        lcd_write_char(127, 2, 24);
        modification = ui_text_set(osc1_trans_mod, encoder_rotation, modifier_t);
        if(modification != osc1_trans_mod){
            modifier_allocation[osc1_trans_mod]--;
            modifier_allocation[modification]++;
            osc1_trans_mod = modification;
        }
        transmit_on_change(126, osc1_trans_mod, 1);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            change_submenu(4);
            break;
        case 3:
            break;
        case 4:
            change_submenu(78);
            lcd_write_string(MODIFIER_TEXT[osc1_cent_mod], 26, 3, 0);
            break;
        case 5:
            change_submenu(76);
            lcd_signed_number(osc1_cent, 2, 3, DIGIT_POS - 3);
            break;
        default:
            break;
        }
        break;
    case 78:
        lcd_write_string("08 Cent control           ", 26, 2, 0);
        lcd_write_char(127, 2, 24);
        modification = ui_text_set(osc1_cent_mod, encoder_rotation, modifier_t);
        if(modification != osc1_cent_mod){
            modifier_allocation[osc1_cent_mod]--;
            modifier_allocation[modification]++;
            osc1_cent_mod = modification;
        }
        transmit_on_change(127, osc1_cent_mod, 1);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            change_submenu(4);
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            change_submenu(77);
            lcd_write_string(MODIFIER_TEXT[osc1_trans_mod], 26, 3, 0);
            break;
        default:
            break;
        }
        break;
    }

    {}

    { // Oscillator 2 controls
    /* 5A
    case 90:
        lcd_write_string("00 Osc 2 Aconfig         ~", 26, 2, 0);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            lcd_write_string("                          ", 26, 2, 0);
            menu_state = 5;
            break;
        case 3:
            break;
        case 4:
            change_submenu(91);
            break;
        case 5:
            break;
        default:
            break;
        }
        break;
    */
    case 91:
        lcd_write_string("01 Shape                 ~", 26, 2, 0);
        //lcd_write_char(127, 2, 24);
        osc2_shape = ui_text_set(osc2_shape, encoder_rotation, shape_t);
        transmit_on_change(130, osc2_shape, 1);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            change_submenu(5);
            break;
        case 3:
            break;
        case 4:
            change_submenu(92);
            lcd_write_number(osc2_osc_count, 2, 3, DIGIT_POS - 2);
            break;
        case 5:
            //change_submenu(90);
            break;
        default:
            break;
        }
        break;
    case 92:
        lcd_write_string("02 Sub-oscillators       ~", 26, 2, 0);
        lcd_write_char(127, 2, 24);
        osc2_osc_count = ui_digit_set(osc2_osc_count, encoder_rotation, 2, 0, MAXSVOICES);
        transmit_on_change(131, osc2_osc_count, 1);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            change_submenu(5);
            break;
        case 3:
            break;
        case 4:
            change_submenu(95);
            lcd_signed_number(osc1_trans, 2, 3, DIGIT_POS - 3);
            break;
        case 5:
            change_submenu(91);
            lcd_write_string(SHAPE_TEXT[osc2_shape], 26, 3, 0);
            break;
        default:
            break;
        }
        break;
    /* all sub oscillators are harmonic
    case 93:
        lcd_write_string("03 Sub-oscillators mode  ~", 26, 2, 0);
        lcd_write_char(127, 2, 24);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            lcd_write_string("                          ", 26, 2, 0);
            menu_state = 5;
            break;
        case 3:
            break;
        case 4:
            menu_state = 94;
            break;
        case 5:
            menu_state = 92;
            break;
        default:
            break;
        }
        break;
    case 94:
        lcd_write_string("04 Sub-osc unison        ~", 26, 2, 0);
        lcd_write_char(127, 2, 24);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            lcd_write_string("                          ", 26, 2, 0);
            menu_state = 5;
            break;
        case 3:
            break;
        case 4:
            menu_state = 95;
            break;
        case 5:
            menu_state = 93;
            break;
        default:
            break;
        }
        break;
    */
    case 95:
        lcd_write_string("05 Transpose             ~", 26, 2, 0);
        lcd_write_char(127, 2, 24);
        /* only 61 oscillators
        osc2_trans = ui_signed_digit_set(osc2_trans, encoder_rotation, 2, -MAXTRANS / 2, MAXTRANS / 2);
        transmit_on_change(134, osc2_trans, 1);
        */
        switch(button_pressed){
        case 1:
            //reak;
        case 2:
            change_submenu(5);
            break;
        case 3:
            break;
        case 4:
            change_submenu(96);
            lcd_signed_number(osc1_cent, 2, 3, DIGIT_POS - 3);
            break;
        case 5:
            change_submenu(92);
            lcd_write_number(osc1_osc_count, 2, 3, DIGIT_POS - 2);
            break;
        default:
            break;
        }
        break;
    case 96:
        lcd_write_string("06 Cent variation        ~", 26, 2, 0);
        lcd_write_char(127, 2, 24);
        /* only 61 oscillators
        osc2_cent = ui_signed_digit_set(osc2_cent, encoder_rotation, 2, -99, 99);
        transmit_on_change(135, osc2_cent, 1);
        */
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            change_submenu(5);
            break;
        case 3:
            break;
        case 4:
            change_submenu(97);
            lcd_write_string(MODIFIER_TEXT[osc1_trans_mod], 26, 3, 0);
            break;
        case 5:
            change_submenu(95);
            lcd_signed_number(osc1_trans, 2, 3, DIGIT_POS - 3);
            break;
        default:
            break;
        }
        break;
    case 97:
        lcd_write_string("07 Transpose control     ~", 26, 2, 0);
        lcd_write_char(127, 2, 24);
        /* only 61 oscillators
        modification = ui_text_set(osc2_trans_mod, encoder_rotation, modifier_t);
        if(modification != osc2_trans_mod){
            modifier_allocation[osc2_trans_mod]--;
            modifier_allocation[modification]++;
            osc2_trans_mod = modification;
        }
        transmit_on_change(136, osc2_trans_mod, 1);
        */
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            change_submenu(5);
            break;
        case 3:
            break;
        case 4:
            change_submenu(98);
            lcd_write_string(MODIFIER_TEXT[osc1_cent_mod], 26, 3, 0);
            break;
        case 5:
            change_submenu(96);
            lcd_signed_number(osc1_cent, 2, 3, DIGIT_POS - 3);
            break;
        default:
            break;
        }
        break;
    case 98:
        lcd_write_string("08 Cent control           ", 26, 2, 0);
        lcd_write_char(127, 2, 24);
        /* only 61 oscillators
        modification = ui_text_set(osc2_cent_mod, encoder_rotation, modifier_t);
        if(modification != osc2_cent_mod){
            modifier_allocation[osc2_cent_mod]--;
            modifier_allocation[modification]++;
            osc2_cent_mod = modification;
        }
        transmit_on_change(137, osc2_cent_mod, 1);
        */
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            change_submenu(5);
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            change_submenu(97);
            lcd_write_string(MODIFIER_TEXT[osc1_trans_mod], 26, 3, 0);
            break;
        default:
            break;
        }
        break;
    }

    {}

    { // ADSR-5 parameters
    /* 5A
    case 110:
        lcd_write_string("00 ADSR Aconfig          ~", 26, 2, 0);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            lcd_write_string("                          ", 26, 2, 0);
            menu_state = 6;
            break;
        case 3:
            break;
        case 4:
            change_submenu(111);
            break;
        case 5:
            break;
        default:
            break;
        }
        break;
    */
    case 111:
        lcd_write_string("01 Attack time           ~", 26, 2, 0);
        //lcd_write_char(127, 2, 24);
        adsr_a = ui_digit_set(adsr_a, encoder_rotation, 3, 0, MAXADSR);
        transmit_on_change(140, adsr_a, 1);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            change_submenu(6);
            break;
        case 3:
            break;
        case 4:
            change_submenu(112);
            lcd_write_number(adsr_d, 3, 3, DIGIT_POS - 3);
            break;
        case 5:
            //change_submenu(110);
            break;
        default:
            break;
        }
        break;
    case 112:
        lcd_write_string("02 Decay time            ~", 26, 2, 0);
        lcd_write_char(127, 2, 24);
        adsr_d = ui_digit_set(adsr_d, encoder_rotation, 3, 0, MAXADSR);
        transmit_on_change(142, adsr_d, 1);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            change_submenu(6);
            break;
        case 3:
            break;
        case 4:
            change_submenu(113);
            lcd_write_number(adsr_sl, 3, 3, DIGIT_POS - 3);
            break;
        case 5:
            change_submenu(111);
            lcd_write_number(adsr_a, 3, 3, DIGIT_POS - 3);
            break;
        default:
            break;
        }
        break;
    case 113:
        lcd_write_string("03 Sustain level         ~", 26, 2, 0);
        lcd_write_char(127, 2, 24);
        adsr_sl = ui_digit_set(adsr_sl, encoder_rotation, 3, 0, 100);
        transmit_on_change(146, adsr_sl, 1);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            change_submenu(6);
            break;
        case 3:
            break;
        case 4:
            change_submenu(114);
            lcd_write_number(adsr_s, 3, 3, DIGIT_POS - 3);
            break;
        case 5:
            change_submenu(112);
            lcd_write_number(adsr_d, 3, 3, DIGIT_POS - 3);
            break;
        default:
            break;
        }
        break;
    case 114:
        lcd_write_string("04 Sustain time          ~", 26, 2, 0);
        lcd_write_char(127, 2, 24);
        adsr_s = ui_digit_set(adsr_s, encoder_rotation, 3, 0, MAXADSR);
        transmit_on_change(144, adsr_s, 1);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            change_submenu(6);
            break;
        case 3:
            break;
        case 4:
            change_submenu(115);
            lcd_write_number(adsr_r, 3, 3, DIGIT_POS - 3);
            break;
        case 5:
            change_submenu(113);
            lcd_write_number(adsr_sl, 3, 3, DIGIT_POS - 3);
            break;
        default:
            break;
        }
        break;
    case 115:
        lcd_write_string("05 Release time          ~", 26, 2, 0);
        lcd_write_char(127, 2, 24);
        adsr_r = ui_digit_set(adsr_r, encoder_rotation, 3, 1, MAXADSR);
        transmit_on_change(148, adsr_r, 1);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            change_submenu(6);
            break;
        case 3:
            break;
        case 4:
            change_submenu(116);
            lcd_write_string(MODIFIER_TEXT[adsr_a_mod ], 26, 3, 0);
            break;
        case 5:
            change_submenu(114);
            lcd_write_number(adsr_s, 3, 3, DIGIT_POS - 3);
            break;
        default:
            break;
        }
        break;
    case 116:
        lcd_write_string("06 Attack control        ~", 26, 2, 0);
        lcd_write_char(127, 2, 24);
        modification = ui_text_set(adsr_a_mod, encoder_rotation, modifier_t);
        if(modification != adsr_a_mod){
            modifier_allocation[adsr_a_mod]--;
            modifier_allocation[modification]++;
            adsr_a_mod = modification;
        }
        transmit_on_change(150, adsr_a_mod, 1);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            change_submenu(6);
            break;
        case 3:
            break;
        case 4:
            change_submenu(117);
            lcd_write_string(MODIFIER_TEXT[adsr_d_mod ], 26, 3, 0);
            break;
        case 5:
            change_submenu(115);
            lcd_write_number(adsr_r, 3, 3, DIGIT_POS - 3);
            break;
        default:
            break;
        }
        break;
    case 117:
        lcd_write_string("07 Decay control         ~", 26, 2, 0);
        lcd_write_char(127, 2, 24);
        modification = ui_text_set(adsr_d_mod, encoder_rotation, modifier_t);
        if(modification != adsr_d_mod){
            modifier_allocation[adsr_d_mod]--;
            modifier_allocation[modification]++;
            adsr_d_mod = modification;
        }
        transmit_on_change(151, adsr_d_mod, 1);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            change_submenu(6);
            break;
        case 3:
            break;
        case 4:
            change_submenu(118);
            lcd_write_string(MODIFIER_TEXT[adsr_sl_mod ], 26, 3, 0);
            break;
        case 5:
            change_submenu(116);
            lcd_write_string(MODIFIER_TEXT[adsr_a_mod ], 26, 3, 0);
            break;
        default:
            break;
        }
        break;
    case 118:
        lcd_write_string("08 Sustain lvl control   ~", 26, 2, 0);
        lcd_write_char(127, 2, 24);
        modification = ui_text_set(adsr_sl_mod, encoder_rotation, modifier_t);
        if(modification != adsr_sl_mod){
            modifier_allocation[adsr_sl_mod]--;
            modifier_allocation[modification]++;
            adsr_sl_mod = modification;
        }
        transmit_on_change(152, adsr_sl_mod, 1);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            change_submenu(6);
            break;
        case 3:
            break;
        case 4:
            change_submenu(119);
            lcd_write_string(MODIFIER_TEXT[adsr_s_mod ], 26, 3, 0);
            break;
        case 5:
            change_submenu(117);
            lcd_write_string(MODIFIER_TEXT[adsr_d_mod ], 26, 3, 0);
            break;
        default:
            break;
        }
        break;
    case 119:
        lcd_write_string("09 Sustain control       ~", 26, 2, 0);
        lcd_write_char(127, 2, 24);
        modification = ui_text_set(adsr_s_mod, encoder_rotation, modifier_t);
        if(modification != adsr_s_mod){
            modifier_allocation[adsr_s_mod]--;
            modifier_allocation[modification]++;
            adsr_s_mod = modification;
        }
        transmit_on_change(153, adsr_s_mod, 1);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            change_submenu(6);
            break;
        case 3:
            break;
        case 4:
            change_submenu(120);
            lcd_write_string(MODIFIER_TEXT[adsr_r_mod ], 26, 3, 0);
            break;
        case 5:
            change_submenu(118);
            lcd_write_string(MODIFIER_TEXT[adsr_sl_mod ], 26, 3, 0);
            break;
        default:
            break;
        }
        break;
    case 120:
        lcd_write_string("10 Release control       ", 26, 2, 0);
        lcd_write_char(127, 2, 24);
        modification = ui_text_set(adsr_r_mod, encoder_rotation, modifier_t);
        if(modification != adsr_r_mod){
            modifier_allocation[adsr_r_mod]--;
            modifier_allocation[modification]++;
            adsr_r_mod = modification;
        }
        transmit_on_change(154, adsr_r_mod, 1);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            change_submenu(6);
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            change_submenu(119);
            lcd_write_string(MODIFIER_TEXT[adsr_s_mod ], 26, 3, 0);
            break;
        default:
            break;
        }
        break;
    }

    {}

    { // Modifiers bend/mod/press/speed/sliders
    case 130:
        lcd_write_string("00 Pitch bend amplitude  ~", 26, 2, 0);
        amp_pw = ui_digit_set(amp_pw, encoder_rotation, 2, 0, 99);
        transmit_on_change(180, amp_pw, 1);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            change_submenu(7);
            break;
        case 3:
            break;
        case 4:
            change_submenu(131);
            lcd_write_number(amp_mw, 2, 3, DIGIT_POS - 2);
            break;
        case 5:
            break;
        default:
            break;
        }
        break;
    case 131:
        lcd_write_string("01 Pitch mod amplitude   ~", 26, 2, 0);
        lcd_write_char(127, 2, 24);
        amp_mw = ui_digit_set(amp_mw, encoder_rotation, 2, 0, 99);
        transmit_on_change(181, amp_mw, 1);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            change_submenu(7);
            break;
        case 3:
            break;
        case 4:
            change_submenu(132);
            lcd_write_number(amp_kn, 2, 3, DIGIT_POS - 2);
            break;
        case 5:
            change_submenu(130);
            lcd_write_number(amp_pw, 2, 3, DIGIT_POS - 2);
            break;
        default:
            break;
        }
        break;
    case 132:
        lcd_write_string("02 Key note amplitude    ~", 26, 2, 0);
        lcd_write_char(127, 2, 24);
        amp_kn = ui_digit_set(amp_kn, encoder_rotation, 2, 0, 99);
        transmit_on_change(187, amp_kn, 1);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            change_submenu(7);
            break;
        case 3:
            break;
        case 4:
            change_submenu(133);
            lcd_write_number(amp_ks, 2, 3, DIGIT_POS - 2);
            break;
        case 5:
            change_submenu(131);
            lcd_write_number(amp_mw, 2, 3, DIGIT_POS - 2);
            break;
        default:
            break;
        }
        break;
    case 133:
        lcd_write_string("03 Key speed amplitude   ~", 26, 2, 0);
        lcd_write_char(127, 2, 24);
        amp_ks = ui_digit_set(amp_ks, encoder_rotation, 2, 0, 99);
        transmit_on_change(188, amp_ks, 1);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            change_submenu(7);
            break;
        case 3:
            break;
        case 4:
            change_submenu(134);
            lcd_write_number(amp_s1, 2, 3, DIGIT_POS - 2);
            break;
        case 5:
            change_submenu(132);
            lcd_write_number(amp_kn, 2, 3, DIGIT_POS - 2);
            break;
        default:
            break;
        }
        break;
    case 134:
        lcd_write_string("04 Slider 1 amplitude    ~", 26, 2, 0);
        lcd_write_char(127, 2, 24);
        amp_s1 = ui_digit_set(amp_s1, encoder_rotation, 2, 0, 99);
        transmit_on_change(182, amp_s1, 1);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            change_submenu(7);
            break;
        case 3:
            break;
        case 4:
            change_submenu(135);
            lcd_write_number(amp_s2, 2, 3, DIGIT_POS - 2);
            break;
        case 5:
            change_submenu(133);
            lcd_write_number(amp_ks, 2, 3, DIGIT_POS - 2);
            break;
        default:
            break;
        }
        break;
    case 135:
        lcd_write_string("05 Slider 2 amplitude    ~", 26, 2, 0);
        lcd_write_char(127, 2, 24);
        amp_s2 = ui_digit_set(amp_s2, encoder_rotation, 2, 0, 99);
        transmit_on_change(183, amp_s2, 1);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            change_submenu(7);
            break;
        case 3:
            break;
        case 4:
            change_submenu(136);
            lcd_write_number(amp_s3, 2, 3, DIGIT_POS - 2);
            break;
        case 5:
            change_submenu(134);
            lcd_write_number(amp_s1, 2, 3, DIGIT_POS - 2);
            break;
        default:
            break;
        }
        break;
    case 136:
        lcd_write_string("06 Slider 3 amplitude    ~", 26, 2, 0);
        lcd_write_char(127, 2, 24);
        amp_s3 = ui_digit_set(amp_s3, encoder_rotation, 2, 0, 99);
        transmit_on_change(184, amp_s3, 1);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            change_submenu(7);
            break;
        case 3:
            break;
        case 4:
            change_submenu(137);
            lcd_write_number(amp_s4, 2, 3, DIGIT_POS - 2);
            break;
        case 5:
            change_submenu(135);
            lcd_write_number(amp_s2, 2, 3, DIGIT_POS - 2);
            break;
        default:
            break;
        }
        break;
    case 137:
        lcd_write_string("07 Slider 4 amplitude    ~", 26, 2, 0);
        lcd_write_char(127, 2, 24);
        amp_s4 = ui_digit_set(amp_s4, encoder_rotation, 2, 0, 99);
        transmit_on_change(185, amp_s4, 1);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            change_submenu(7);
            break;
        case 3:
            break;
        case 4:
            change_submenu(138);
            lcd_write_number(amp_s5, 2, 3, DIGIT_POS - 2);
            break;
        case 5:
            change_submenu(136);
            lcd_write_number(amp_s3, 2, 3, DIGIT_POS - 2);
            break;
        default:
            break;
        }
        break;
    case 138:
        lcd_write_string("08 Slider 5 amplitude     ", 26, 2, 0);
        lcd_write_char(127, 2, 24);
        amp_s5 = ui_digit_set(amp_s5, encoder_rotation, 2, 0, 99);
        transmit_on_change(186, amp_s5, 1);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            change_submenu(7);
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            change_submenu(137);
            lcd_write_number(amp_s4, 2, 3, DIGIT_POS - 2);
            break;
        default:
            break;
        }
        break;
    }

    {}

    { // LFO 1 controls
    case 150:
        lcd_write_string("00 State                 ~", 26, 2, 0);
        lfo1_state = ui_text_set(lfo1_state, encoder_rotation, state_t);
        transmit_on_change(160, lfo1_state, 1);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            change_submenu(8);
            break;
        case 3:
            break;
        case 4:
            change_submenu(152);
            lcd_write_string(SHAPE_TEXT[lfo1_shape], 26, 3, 0);
            break;
        case 5:
            break;
        default:
            break;
        }
        break;
    /* 5A
    case 151:
        lcd_write_string("01 AControl              ~", 26, 2, 0);
        lcd_write_char(127, 2, 24);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            lcd_write_string("                          ", 26, 2, 0);
            menu_state = 8;
            break;
        case 3:
            break;
        case 4:
            menu_state = 152;
            break;
        case 5:
            menu_state = 150;
            break;
        default:
            break;
        }
        break;
    */
    case 152:
        lcd_write_string("02 Shape                 ~", 26, 2, 0);
        lcd_write_char(127, 2, 24);
        lfo1_shape = ui_text_set(lfo1_shape, encoder_rotation, shape_t);
        transmit_on_change(162, lfo1_shape, 1);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            change_submenu(8);
            break;
        case 3:
            break;
        case 4:
            change_submenu(153);
            lcd_write_string(O_MODE_TEXT[lfo1_mode], 26, 3, 0);
            break;
        case 5:
            change_submenu(150);
            lcd_write_string(STATE_TEXT[lfo1_state], 26, 3, 0);
            break;
        default:
            break;
        }
        break;
    case 153:
        lcd_write_string("03 Mode                  ~", 26, 2, 0);
        lcd_write_char(127, 2, 24);
        lfo1_mode = ui_text_set(lfo1_mode, encoder_rotation, oscm_t);
        transmit_on_change(161, lfo1_mode, 1);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            change_submenu(8);
            break;
        case 3:
            break;
        case 4:
            change_submenu(154);
            lcd_write_number(lfo1_amp, 2, 3, DIGIT_POS - 2);
            break;
        case 5:
            change_submenu(152);
            lcd_write_string(SHAPE_TEXT[lfo1_shape], 26, 3, 0);
            break;
        default:
            break;
        }
        break;
    case 154:
        lcd_write_string("04 Amplitude             ~", 26, 2, 0);
        lcd_write_char(127, 2, 24);
        lfo1_amp = ui_digit_set(lfo1_amp, encoder_rotation, 2, 1, 99);
        transmit_on_change(163, lfo1_amp, 1);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            change_submenu(8);
            break;
        case 3:
            break;
        case 4:
            change_submenu(155);
            lcd_write_number(lfo1_freq, 3, 3, DIGIT_POS - 3);
            break;
        case 5:
            change_submenu(153);
            lcd_write_string(O_MODE_TEXT[lfo1_mode], 26, 3, 0);
            break;
        default:
            break;
        }
        break;
    case 155:
        lcd_write_string("05 Frequency             ~", 26, 2, 0);
        lcd_write_char(127, 2, 24);
        lfo1_freq = ui_digit_set(lfo1_freq, encoder_rotation, 3, 1, 250);
        transmit_on_change(164, lfo1_freq, 1);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            change_submenu(8);
            break;
        case 3:
            break;
        case 4:
            change_submenu(156);
            lcd_write_string(MODIFIER_TEXT[lfo1_amp_mod], 26, 3, 0);
            break;
        case 5:
            change_submenu(154);
            lcd_write_number(lfo1_amp, 2, 3, DIGIT_POS - 2);
            break;
        default:
            break;
        }
        break;
    case 156:
        lcd_write_string("06 Amplitude control     ~", 26, 2, 0);
        lcd_write_char(127, 2, 24);
        modification = ui_text_set(lfo1_amp_mod, encoder_rotation, modifier_t);
        if(modification != lfo1_amp_mod){
            modifier_allocation[lfo1_amp_mod]--;
            modifier_allocation[modification]++;
            lfo1_amp_mod = modification;
        }
        transmit_on_change(165, lfo1_amp_mod, 1);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            change_submenu(8);
            break;
        case 3:
            break;
        case 4:
            change_submenu(157);
            lcd_write_string(MODIFIER_TEXT[lfo1_freq_mod], 26, 3, 0);
            break;
        case 5:
            change_submenu(155);
            lcd_write_number(lfo1_freq, 3, 3, DIGIT_POS - 3);
            break;
        default:
            break;
        }
        break;
    case 157:
        lcd_write_string("07 Frequency control      ", 26, 2, 0);
        lcd_write_char(127, 2, 24);
        modification = ui_text_set(lfo1_freq_mod, encoder_rotation, modifier_t);
        if(modification != lfo1_freq_mod){
            modifier_allocation[lfo1_freq_mod]--;
            modifier_allocation[modification]++;
            lfo1_freq_mod = modification;
        }
        transmit_on_change(166, lfo1_freq_mod, 1);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            change_submenu(8);
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            change_submenu(156);
            lcd_write_string(MODIFIER_TEXT[lfo1_amp_mod], 26, 3, 0);
            break;
        default:
            break;
        }
        break;
    }

    {}

    { // LFO 2 controls
    case 160:
        lcd_write_string("00 State                 ~", 26, 2, 0);
        lfo2_state = ui_text_set(lfo2_state, encoder_rotation, state_t);
        transmit_on_change(170, lfo2_state, 1);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            change_submenu(9);
            break;
        case 3:
            break;
        case 4:
            change_submenu(162);
            lcd_write_string(SHAPE_TEXT[lfo2_shape], 26, 3, 0);
            break;
        case 5:
            break;
        default:
            break;
        }
        break;
    /* 5A
    case 161:
        lcd_write_string("01 AControl              ~", 26, 2, 0);
        lcd_write_char(127, 2, 24);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            lcd_write_string("                          ", 26, 2, 0);
            menu_state = 9;
            break;
        case 3:
            break;
        case 4:
            menu_state = 162;
            break;
        case 5:
            menu_state = 160;
            break;
        default:
            break;
        }
        break;
    */
    case 162:
        lcd_write_string("02 Shape                 ~", 26, 2, 0);
        lcd_write_char(127, 2, 24);
        lfo2_shape = ui_text_set(lfo2_shape, encoder_rotation, shape_t);
        transmit_on_change(172, lfo2_shape, 1);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            change_submenu(9);
            break;
        case 3:
            break;
        case 4:
            change_submenu(163);
            lcd_write_string(O_MODE_TEXT[lfo2_mode], 26, 3, 0);
            break;
        case 5:
            change_submenu(160);
            lcd_write_string(STATE_TEXT[lfo2_state], 26, 3, 0);
            break;
        default:
            break;
        }
        break;
    case 163:
        lcd_write_string("03 Mode                  ~", 26, 2, 0);
        lcd_write_char(127, 2, 24);
        lfo2_mode = ui_text_set(lfo2_mode, encoder_rotation, oscm_t);
        transmit_on_change(171, lfo2_mode, 1);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            change_submenu(9);
            break;
        case 3:
            break;
        case 4:
            change_submenu(164);
            lcd_write_number(lfo2_amp, 2, 3, DIGIT_POS - 2);
            break;
        case 5:
            change_submenu(162);
            lcd_write_string(SHAPE_TEXT[lfo2_shape], 26, 3, 0);
            break;
        default:
            break;
        }
        break;
    case 164:
        lcd_write_string("04 Amplitude             ~", 26, 2, 0);
        lcd_write_char(127, 2, 24);
        lfo2_amp = ui_digit_set(lfo2_amp, encoder_rotation, 2, 1, 99);
        transmit_on_change(173, lfo2_amp, 1);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            change_submenu(9);
            break;
        case 3:
            break;
        case 4:
            change_submenu(165);
            lcd_write_number(lfo2_freq, 3, 3, DIGIT_POS - 3);
            break;
        case 5:
            change_submenu(163);
            lcd_write_string(O_MODE_TEXT[lfo2_mode], 26, 3, 0);
            break;
        default:
            break;
        }
        break;
    case 165:
        lcd_write_string("05 Frequency             ~", 26, 2, 0);
        lcd_write_char(127, 2, 24);
        lfo2_freq = ui_digit_set(lfo2_freq, encoder_rotation, 3, 1, 250);
        transmit_on_change(174, lfo2_freq, 1);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            change_submenu(9);
            break;
        case 3:
            break;
        case 4:
            change_submenu(166);
            lcd_write_string(MODIFIER_TEXT[lfo2_amp_mod], 26, 3, 0);
            break;
        case 5:
            change_submenu(164);
            lcd_write_number(lfo2_amp, 2, 3, DIGIT_POS - 2);
            break;
        default:
            break;
        }
        break;
    case 166:
        lcd_write_string("06 Amplitude control     ~", 26, 2, 0);
        lcd_write_char(127, 2, 24);
        modification = ui_text_set(lfo2_amp_mod, encoder_rotation, modifier_t);
        if(modification != lfo2_amp_mod){
            modifier_allocation[lfo2_amp_mod]--;
            modifier_allocation[modification]++;
            lfo1_freq_mod = modification;
        }
        transmit_on_change(175, lfo2_amp_mod, 1);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            change_submenu(9);
            break;
        case 3:
            break;
        case 4:
            change_submenu(167);
            lcd_write_string(MODIFIER_TEXT[lfo2_freq_mod], 26, 3, 0);
            break;
        case 5:
            change_submenu(165);
            lcd_write_number(lfo2_freq, 3, 3, DIGIT_POS - 3);
            break;
        default:
            break;
        }
        break;
    case 167:
        lcd_write_string("07 Frequency control      ", 26, 2, 0);
        lcd_write_char(127, 2, 24);
        modification = ui_text_set(lfo2_freq_mod, encoder_rotation, modifier_t);
        if(modification != lfo2_freq_mod){
            modifier_allocation[lfo2_freq_mod]--;
            modifier_allocation[modification]++;
            lfo2_freq_mod = modification;
        }
        transmit_on_change(176, lfo2_freq_mod, 1);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            change_submenu(9);
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            change_submenu(166);
            lcd_write_string(MODIFIER_TEXT[lfo2_amp_mod], 26, 3, 0);
            break;
        default:
            break;
        }
        break;
    }

    {}

    /* not yet ready
    { // Randomizer controls
    case 170:
        lcd_write_string("00 Control               ~", 26, 2, 0);
        rand_state = ui_digit_set(rand_state, encoder_rotation, 1, 0, 1);
        serial1_transmit(180, rand_state);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            change_submenu(10);
            break;
        case 3:
            break;
        case 4:
            change_submenu(171);
            lcd_write_number(rand_amp, 2, 3, DIGIT_POS - 2);
            break;
        case 5:
            break;
        default:
            break;
        }
        break;
    case 171:
        lcd_write_string("01 Amplitude              ", 26, 2, 0);
        lcd_write_char(127, 2, 24);
        rand_amp = ui_digit_set(rand_amp, encoder_rotation, 2, 0, 99);
        serial1_transmit(180, rand_amp);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            change_submenu(10);
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            change_submenu(170);
            lcd_write_number(rand_state, 1, 3, DIGIT_POS - 1);
            break;
        default:
            break;
        }
        break;
    }
    */

    {}

    { // MOD ENV parameters
    case 180:
        lcd_write_string("00 MOD ENV State         ~", 26, 2, 0);
        me_state = ui_text_set(me_state, encoder_rotation, state_t);
        transmit_on_change(199, me_state, 1);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            change_submenu(11);
            break;
        case 3:
            break;
        case 4:
            change_submenu(182);
            lcd_write_number(me_a, 3, 3, DIGIT_POS - 3);
            break;
        case 5:
            break;
        default:
            break;
        }
        break;
    /* 5A
    case 181:
        lcd_write_string("01 MOD ENV Aconfig       ~", 26, 2, 0);
        lcd_write_char(127, 2, 24);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            lcd_write_string("                          ", 26, 2, 0);
            lcd_write_string("                          ", 26, 3, 0);
            menu_state = 11;
            break;
        case 3:
            break;
        case 4:
            lcd_write_string("                          ", 26, 3, 0);
            menu_state = 182;
            break;
        case 5:
            lcd_write_string("                          ", 26, 3, 0);
            menu_state = 180;
            break;
        default:
            break;
        }
        break;
    */
    case 182:
        lcd_write_string("02 Attack time           ~", 26, 2, 0);
        lcd_write_char(127, 2, 24);
        me_a = ui_digit_set(me_a, encoder_rotation, 3, 0, MAXADSR);
        transmit_on_change(200, me_a, 1);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            change_submenu(11);
            break;
        case 3:
            break;
        case 4:
            change_submenu(183);
            lcd_write_number(me_d, 3, 3, DIGIT_POS - 3);
            break;
        case 5:
            change_submenu(180);
            lcd_write_string(STATE_TEXT[me_state], 26, 3, 0);
            break;
        default:
            break;
        }
        break;
    case 183:
        lcd_write_string("03 Decay time            ~", 26, 2, 0);
        lcd_write_char(127, 2, 24);
        me_d = ui_digit_set(me_d, encoder_rotation, 3, 0, MAXADSR);
        transmit_on_change(202, me_d, 1);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            change_submenu(11);
            break;
        case 3:
            break;
        case 4:
            change_submenu(184);
            lcd_write_number(me_sl, 3, 3, DIGIT_POS - 3);
            break;
        case 5:
            change_submenu(182);
            lcd_write_number(me_a, 3, 3, DIGIT_POS - 3);
            break;
        default:
            break;
        }
        break;
    case 184:
        lcd_write_string("04 Sustain level         ~", 26, 2, 0);
        lcd_write_char(127, 2, 24);
        me_sl = ui_digit_set(me_sl, encoder_rotation, 3, 0, 100);
        transmit_on_change(206, me_sl, 1);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            change_submenu(11);
            break;
        case 3:
            break;
        case 4:
            change_submenu(185);
            lcd_write_number(me_s, 3, 3, DIGIT_POS - 3);
            break;
        case 5:
            change_submenu(183);
            lcd_write_number(me_d, 3, 3, DIGIT_POS - 3);
            break;
        default:
            break;
        }
        break;
    case 185:
        lcd_write_string("05 Sustain time          ~", 26, 2, 0);
        lcd_write_char(127, 2, 24);
        me_s = ui_digit_set(me_s, encoder_rotation, 3, 0, MAXADSR);
        transmit_on_change(204, me_s, 1);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            change_submenu(11);
            break;
        case 3:
            break;
        case 4:
            change_submenu(186);
            lcd_write_number(me_r, 3, 3, DIGIT_POS - 3);
            break;
        case 5:
            change_submenu(184);
            lcd_write_number(me_sl, 3, 3, DIGIT_POS - 3);
            break;
        default:
            break;
        }
        break;
    case 186:
        lcd_write_string("06 Release time          ~", 26, 2, 0);
        lcd_write_char(127, 2, 24);
        me_r = ui_digit_set(me_r, encoder_rotation, 3, 1, MAXADSR);
        transmit_on_change(208, me_r, 1);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            change_submenu(11);
            break;
        case 3:
            break;
        case 4:
            change_submenu(187);
            lcd_write_string(MODIFIER_TEXT[me_a_mod], 26, 3, 0);
            break;
        case 5:
            change_submenu(185);
            lcd_write_number(me_s, 3, 3, DIGIT_POS - 3);
            break;
        default:
            break;
        }
        break;
    case 187:
        lcd_write_string("07 Attack control        ~", 26, 2, 0);
        lcd_write_char(127, 2, 24);
        modification = ui_text_set(me_a_mod, encoder_rotation, modifier_t);
        if(modification != me_a_mod){
            modifier_allocation[me_a_mod]--;
            modifier_allocation[modification]++;
            me_a_mod = modification;
        }
        transmit_on_change(210, me_a_mod, 1);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            change_submenu(11);
            break;
        case 3:
            break;
        case 4:
            change_submenu(188);
            lcd_write_string(MODIFIER_TEXT[me_d_mod], 26, 3, 0);
            break;
        case 5:
            change_submenu(186);
            lcd_write_number(me_r, 3, 3, DIGIT_POS - 3);
            break;
        default:
            break;
        }
        break;
    case 188:
        lcd_write_string("08 Decay control         ~", 26, 2, 0);
        lcd_write_char(127, 2, 24);
        modification = ui_text_set(me_d_mod, encoder_rotation, modifier_t);
        if(modification != me_d_mod){
            modifier_allocation[me_d_mod]--;
            modifier_allocation[modification]++;
            me_d_mod = modification;
        }
        transmit_on_change(211, me_d_mod, 1);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            change_submenu(11);
            break;
        case 3:
            break;
        case 4:
            change_submenu(189);
            lcd_write_string(MODIFIER_TEXT[me_sl_mod], 26, 3, 0);
            break;
        case 5:
            change_submenu(187);
            lcd_write_string(MODIFIER_TEXT[me_a_mod], 26, 3, 0);
            break;
        default:
            break;
        }
        break;
    case 189:
        lcd_write_string("09 Sustain lvl control   ~", 26, 2, 0);
        lcd_write_char(127, 2, 24);
        modification = ui_text_set(me_sl_mod, encoder_rotation, modifier_t);
        if(modification != me_sl_mod){
            modifier_allocation[me_sl_mod]--;
            modifier_allocation[modification]++;
            me_sl_mod = modification;
        }
        transmit_on_change(212, me_sl_mod, 1);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            change_submenu(11);
            break;
        case 3:
            break;
        case 4:
            change_submenu(190);
            lcd_write_string(MODIFIER_TEXT[me_s_mod], 26, 3, 0);
            break;
        case 5:
            change_submenu(188);
            lcd_write_string(MODIFIER_TEXT[me_d_mod], 26, 3, 0);
            break;
        default:
            break;
        }
        break;
    case 190:
        lcd_write_string("10 Sustain control       ~", 26, 2, 0);
        lcd_write_char(127, 2, 24);
        modification = ui_text_set(me_s_mod, encoder_rotation, modifier_t);
        if(modification != me_s_mod){
            modifier_allocation[me_s_mod]--;
            modifier_allocation[modification]++;
            me_s_mod = modification;
        }
        transmit_on_change(213, me_s_mod, 1);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            change_submenu(11);
            break;
        case 3:
            break;
        case 4:
            change_submenu(191);
            lcd_write_string(MODIFIER_TEXT[me_r_mod], 26, 3, 0);
            break;
        case 5:
            change_submenu(189);
            lcd_write_string(MODIFIER_TEXT[me_sl_mod], 26, 3, 0);
            break;
        default:
            break;
        }
        break;
    case 191:
        lcd_write_string("11 Release control       ", 26, 2, 0);
        lcd_write_char(127, 2, 24);
        modification = ui_text_set(me_r_mod, encoder_rotation, modifier_t);
        if(modification != me_r_mod){
            modifier_allocation[me_r_mod]--;
            modifier_allocation[modification]++;
            me_r_mod = modification;
        }
        transmit_on_change(214, me_r_mod, 1);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            change_submenu(11);
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            change_submenu(190);
            lcd_write_string(MODIFIER_TEXT[me_s_mod], 26, 3, 0);
            break;
        default:
            break;
        }
        break;
    }

    default:
        break;
    }

    lag(INTERFACE);
}

// main function
int main(){
    uint8_t i = 0;

    // variables initialization
    for(i = 0; i < MAXKEYS; i++){
        key_detection[i].counter = 0;
        key_detection[i].stateMachine = 0;
    }

    display_init();     // initialize the display pins and functions
    onboard_init();     // initialize the onboard components pins and functions
    serial0_init();     // initialize the serial pins and functions
    serial1_init();     // initialize the serial pins and functions
    keyboard_init();    // initialize the keyboard pins and functions
    ui_init();          // initialize the UI buttons and encoder pins and functions
    adc_init();         // initialize the ADC pins and functions

    timer_init();       // initialize the timer functions
	sei();				// global interrupts enable

    { // INTRO SCREEN
    lag(100);

    lcd_write_char('M', 0, 17);
    lcd_write_char(0b11001010, 0, 19);  // A
    lcd_write_char(0b00101001, 0, 21);  // D (0b10111100 and 0b10111010 were another good options)

    lcd_write_char('S', 1, 15);
    lcd_write_char(0b10111111, 1, 17);  // Y
    lcd_write_char(0b11101110, 1, 19);  // N
    lcd_write_char(0b11001110, 1, 21);  // T
    lcd_write_char(0b10110110, 1, 23);  // H

    lcd_write_string("AVR/ESP  Ctrlr based synth", 26, 3, 7);
    lcd_write_char(0b11100100, 3, 15);

    lag(3000);
    lcd_write_string("                                        ", 40, 0, 0);
    lcd_write_string("                                        ", 40, 2, 0);
    lcd_write_string("                                        ", 40, 3, 0);
    lcd_write_char(255, 0, 26);
    for(i = 0; i <= 26; i++)
        lcd_write_char(255, 1, i);
    lcd_write_char(255, 2, 26);
    lcd_write_char(255, 3, 26);
    }

    { // PIP SCREEN
    ui_pip_label();
    lcd_write_number(50, 2, 0, 28);
    lcd_write_number(50, 2, 0, 34);
    lcd_write_number(50, 2, 0, 38);
    lcd_write_string("sinus", 5, 1, 32);
    lcd_write_string("sinus", 5, 2, 32);
    lcd_write_number(1, 2, 1, 38);
    lcd_write_number(1, 2, 2, 38);
    }

    while(1){
        loop();
    }
}
