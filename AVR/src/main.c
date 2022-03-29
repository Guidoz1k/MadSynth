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

// ============================================================ accessory functions

// function that clears the pip screen
void ui_clear_pip(void){
    lcd_write_string("             ", 0, 27);
    lcd_write_string("             ", 1, 27);
    lcd_write_string("             ", 2, 27);
    lcd_write_string("             ", 3, 27);
}

// function that allows user to set all digits in a parameter ################################################################
void ui_digit_set(void){

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
    uint8_t button_pressed = 0;     // records the value of the button each iteration
    uint8_t encoder_rotation = 0;   // records the value of the accumulated encoder rotations
    static uint8_t button_hold = 0;    // counter to limit the button interaction
    static uint8_t menu_state = 0;     // UI machine state variable

    typedef enum _mod{
        none = 0,
        lfo1 = 1,
        lfo2 = 2,
        noiser = 3,
        note = 4,
        pitch_w = 5,
        mod_w = 6,
        veloc = 7
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

    int8_t volume = 50;  // 0 ... 99
    enum _stereo{
        l_r,
        l_only,
        r_only
    } stereo = l_r;

    // uint8_t osc_count = MAX NUMBER
    // uint8_t max_osc_mode = enum

    /* oct_trans
    piano has 9 octaves, our synth has 5, it starts at the second already
    0 ... (4 * 12) = 48
    */
    uint8_t oct_trans = 24; // 2 * 12
    uint8_t mixer1 = 50; // mixer's first channel
    uint8_t mixer2 = 50; // mixer's second channel

    SHAPE osc1_shape = sinusoid;
    uint8_t osc1_osc_count = 0; // 0 ... 5(?)
    O_MODE osc1_osc_mode = frees;
    uint8_t osc1_osc_unison = 0; // 0 ... 100
    int8_t osc1_trans = 0;  // transpose -24 ... 24
    int8_t osc1_cent = 0;   // 0 ... 100
    MODIFIER osc1_trans_mod = none; // modifier for osc trans
    MODIFIER osc1_cent_mod = none;  // modifier for osc cent

    SHAPE osc2_shape = sinusoid;
    uint8_t osc2_osc_count = 0; // 0 ... 5(?)
    O_MODE osc2_osc_mode = frees;
    uint8_t osc2_osc_unison = 0; // 0 ... 100
    int8_t osc2_trans = 0;  // transpose -24 ... 24
    int8_t osc2_cent = 0;   // 0 ... 100
    MODIFIER osc2_trans_mod = none; // modifier for osc trans
    MODIFIER osc2_cent_mod = none;  // modifier for osc cent

    uint16_t adsr_a = 0;    // 0 ... 10000 ms
    uint16_t adsr_d = 0;    // 0 ... 10000 ms
    uint16_t adsr_s = 0;    // 0 ... 10000 ms
    uint16_t adsr_sl = 0;   // 0 ... 100%
    uint16_t adsr_r = 0;    // 0 ... 10000 ms
    MODIFIER adsr_a_mod = none;    // modifier for adsr A parameter
    MODIFIER adsr_d_mod = none;    // modifier for adsr D parameter
    MODIFIER adsr_s_mod = none;    // modifier for adsr S parameter
    MODIFIER adsr_sl_mod = none;   // modifier for adsr SL parameter
    MODIFIER adsr_r_mod = none;    // modifier for adsr R parameter

    /*  MODIFIERS
        Pitch bend amplitude
        Pitch bend control
        Pitch mod  amplitude
        Pitch mod  control
        Key press amplitude
        Key press control
    */

    uint8_t lfo1_state = 0; // 0 - off, 1 - on
    O_MODE lfo1_mode = frees;
    SHAPE lfo1_shape = sinusoid;
    uint8_t lfo1_amp = 0;   // 0 ... 100
    uint8_t lfo1_freq = 0;  // 0 ... 250
    MODIFIER lfo1_amp_mod = none;  // modifier for lfo1 amplitude
    MODIFIER lfo1_freq_mod = none; // modifier for lfo1 frequency

    uint8_t lfo2_state = 0; // 0 - off, 1 - on
    O_MODE lfo2_mode = frees;
    SHAPE lfo2_shape = sinusoid;
    uint8_t lfo2_amp = 0;   // 0 ... 100
    uint8_t lfo2_freq = 0;  // 0 ... 250
    MODIFIER lfo2_amp_mod = none;  // modifier for lfo2 amplitude
    MODIFIER lfo2_freq_mod = none; // modifier for lfo2 frequency

    uint8_t rand_state = 0; // 0 - off, 1 - on
    uint8_t rand_amp = 0;   // 0 ... 100

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
            serial1_transmit(i + i, key_detection[i].counter);
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
            serial1_transmit(i + i, 0);
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

    // UI machine state with all the screen texts and DDS configurations
    switch(menu_state){
    case 0: // Master channel setup
        lcd_write_string("00 Master channel setup  ~", 0, 0);
        switch(button_pressed){
        case 1:
            break;
        case 2:
            break;
        case 3:
            lcd_write_string("  ", 0, 24);
            menu_state = 30;
            break;
        case 4:
            menu_state = 1;
            break;
        case 5:
            break;
        default:
            break;
        }
        break;
    case 1: // Oscillators allocation
        lcd_write_string("01 OSC allocation        ~", 0, 0);
        lcd_write_char(127, 0, 24);
        switch(button_pressed){
        case 1:
            break;
        case 2:
            break;
        case 3:
            lcd_write_string("  ", 0, 24);
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
    case 2: // Keybed offset config
        lcd_write_string("02 Keybed offset config  ~", 0, 0);
        lcd_write_char(127, 0, 24);
        switch(button_pressed){
        case 1:
            break;
        case 2:
            break;
        case 3:
            lcd_write_string("  ", 0, 24);
            menu_state = 50;
            break;
        case 4:
            menu_state = 3;
            break;
        case 5:
            menu_state = 1;
            break;
        default:
            break;
        }
        break;
    case 3: // Stereo Oscillator Mixer
        lcd_write_string("03 Oscillators mixer     ~", 0, 0);
        lcd_write_char(127, 0, 24);
        switch(button_pressed){
        case 1:
            break;
        case 2:
            break;
        case 3:
            lcd_write_string("  ", 0, 24);
            menu_state = 60;
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
        lcd_write_string("04 OSC 1 controls        ~", 0, 0);
        lcd_write_char(127, 0, 24);
        switch(button_pressed){
        case 1:
            break;
        case 2:
            break;
        case 3:
            lcd_write_string("  ", 0, 24);
            menu_state = 70;
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
        lcd_write_string("05 OSC 2 controls        ~", 0, 0);
        lcd_write_char(127, 0, 24);
        switch(button_pressed){
        case 1:
            break;
        case 2:
            break;
        case 3:
            lcd_write_string("  ", 0, 24);
            menu_state = 90;
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
        lcd_write_string("06 ADSR-5 parameters     ~", 0, 0);
        lcd_write_char(127, 0, 24);
        switch(button_pressed){
        case 1:
            break;
        case 2:
            break;
        case 3:
            lcd_write_string("  ", 0, 24);
            menu_state = 110;
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
        lcd_write_string("07 Control modifiers     ~", 0, 0);
        lcd_write_char(127, 0, 24);
        switch(button_pressed){
        case 1:
            break;
        case 2:
            break;
        case 3:
            lcd_write_string("  ", 0, 24);
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
        lcd_write_string("08 LFO 1 Controls        ~", 0, 0);
        lcd_write_char(127, 0, 24);
        switch(button_pressed){
        case 1:
            break;
        case 2:
            break;
        case 3:
            lcd_write_string("  ", 0, 24);
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
        lcd_write_string("09 LFO 2 Controls        ~", 0, 0);
        lcd_write_char(127, 0, 24);
        switch(button_pressed){
        case 1:
            break;
        case 2:
            break;
        case 3:
            lcd_write_string("  ", 0, 24);
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
        lcd_write_string("10 Randomizer controls    ", 0, 0);
        lcd_write_char(127, 0, 24);
        switch(button_pressed){
        case 1:
            break;
        case 2:
            break;
        case 3:
            lcd_write_string("  ", 0, 24);
            menu_state = 170;
            break;
        case 4:
            break;
        case 5:
            menu_state = 9;
            break;
        default:
            break;
        }
        break;

    { // Master channel setup
    case 30:
        lcd_write_string("00 Volume                ~", 2, 0);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            lcd_write_string("                          ", 2, 0);
            menu_state = 0;
            break;
        case 3:
            break;
        case 4:
            menu_state = 31;
            break;
        case 5:
            break;
        default:
            break;
        }
        break;
    case 31:
        lcd_write_string("01 Stereo                 ", 2, 0);
        lcd_write_char(127, 2, 24);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            lcd_write_string("                          ", 2, 0);
            menu_state = 0;
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            menu_state = 30;
            break;
        default:
            break;
        }
        break;
    }

    {}

    { // Oscillators allocation
    case 40:
        lcd_write_string("00 Osc count limit       ~", 2, 0);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            lcd_write_string("                          ", 2, 0);
            menu_state = 1;
            break;
        case 3:
            break;
        case 4:
            menu_state = 41;
            break;
        case 5:
            break;
        default:
            break;
        }
        break;
    case 41:
        lcd_write_string("01 Max oscillator mode    ", 2, 0);
        lcd_write_char(127, 2, 24);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            lcd_write_string("                          ", 2, 0);
            menu_state = 1;
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            menu_state = 40;
            break;
        default:
            break;
        }
        break;
    }

    {}

    { // Keybed offset config
    case 50:
        lcd_write_string("00 Octave                ~", 2, 0);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            lcd_write_string("                          ", 2, 0);
            menu_state = 2;
            break;
        case 3:
            break;
        case 4:
            menu_state = 51;
            break;
        case 5:
            break;
        default:
            break;
        }
        break;
    case 51:
        lcd_write_string("01 Transpose              ", 2, 0);
        lcd_write_char(127, 2, 24);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            lcd_write_string("                          ", 2, 0);
            menu_state = 2;
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            menu_state = 50;
            break;
        default:
            break;
        }
        break;
    }

    {}

    { // Stereo Oscillator Mixer
    case 60:
        lcd_write_string("00 Mixer Aconfig         ~", 2, 0);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            lcd_write_string("                          ", 2, 0);
            menu_state = 3;
            break;
        case 3:
            break;
        case 4:
            menu_state = 61;
            break;
        case 5:
            break;
        default:
            break;
        }
        break;
    case 61:
        lcd_write_string("01 Oscillator 1          ~", 2, 0);
        lcd_write_char(127, 2, 24);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            lcd_write_string("                          ", 2, 0);
            menu_state = 3;
            break;
        case 3:
            break;
        case 4:
            menu_state = 62;
            break;
        case 5:
            menu_state = 60;
            break;
        default:
            break;
        }
        break;
    case 62:
        lcd_write_string("02 Oscillator 2           ", 2, 0);
        lcd_write_char(127, 2, 24);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            lcd_write_string("                          ", 2, 0);
            menu_state = 3;
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            menu_state = 61;
            break;
        default:
            break;
        }
        break;
    }

    {}

    { // Oscillator 1 controls
    case 70:
        lcd_write_string("00 Osc 1 Aconfig         ~", 2, 0);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            lcd_write_string("                          ", 2, 0);
            menu_state = 4;
            break;
        case 3:
            break;
        case 4:
            menu_state = 71;
            break;
        case 5:
            break;
        default:
            break;
        }
        break;
    case 71:
        lcd_write_string("01 Shape                 ~", 2, 0);
        lcd_write_char(127, 2, 24);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            lcd_write_string("                          ", 2, 0);
            menu_state = 4;
            break;
        case 3:
            break;
        case 4:
            menu_state = 72;
            break;
        case 5:
            menu_state = 70;
            break;
        default:
            break;
        }
        break;
    case 72:
        lcd_write_string("02 Sub-oscillators       ~", 2, 0);
        lcd_write_char(127, 2, 24);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            lcd_write_string("                          ", 2, 0);
            menu_state = 4;
            break;
        case 3:
            break;
        case 4:
            menu_state = 73;
            break;
        case 5:
            menu_state = 71;
            break;
        default:
            break;
        }
        break;
    case 73:
        lcd_write_string("03 Sub-oscillators mode  ~", 2, 0);
        lcd_write_char(127, 2, 24);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            lcd_write_string("                          ", 2, 0);
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
        lcd_write_string("04 Sub-osc unison        ~", 2, 0);
        lcd_write_char(127, 2, 24);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            lcd_write_string("                          ", 2, 0);
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
    case 75:
        lcd_write_string("05 Transpose             ~", 2, 0);
        lcd_write_char(127, 2, 24);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            lcd_write_string("                          ", 2, 0);
            menu_state = 4;
            break;
        case 3:
            break;
        case 4:
            menu_state = 76;
            break;
        case 5:
            menu_state = 74;
            break;
        default:
            break;
        }
        break;
    case 76:
        lcd_write_string("06 Cent variation         ", 2, 0);
        lcd_write_char(127, 2, 24);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            lcd_write_string("                          ", 2, 0);
            menu_state = 4;
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            menu_state = 75;
            break;
        default:
            break;
        }
        break;
    }

    {}

    { // Oscillator 2 controls
    case 90:
        lcd_write_string("00 Osc 2 Aconfig         ~", 2, 0);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            lcd_write_string("                          ", 2, 0);
            menu_state = 5;
            break;
        case 3:
            break;
        case 4:
            menu_state = 91;
            break;
        case 5:
            break;
        default:
            break;
        }
        break;
    case 91:
        lcd_write_string("01 Shape                 ~", 2, 0);
        lcd_write_char(127, 2, 24);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            lcd_write_string("                          ", 2, 0);
            menu_state = 5;
            break;
        case 3:
            break;
        case 4:
            menu_state = 92;
            break;
        case 5:
            menu_state = 90;
            break;
        default:
            break;
        }
        break;
    case 92:
        lcd_write_string("02 Sub-oscillators       ~", 2, 0);
        lcd_write_char(127, 2, 24);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            lcd_write_string("                          ", 2, 0);
            menu_state = 5;
            break;
        case 3:
            break;
        case 4:
            menu_state = 93;
            break;
        case 5:
            menu_state = 91;
            break;
        default:
            break;
        }
        break;
    case 93:
        lcd_write_string("03 Sub-oscillators mode  ~", 2, 0);
        lcd_write_char(127, 2, 24);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            lcd_write_string("                          ", 2, 0);
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
        lcd_write_string("04 Sub-osc unison        ~", 2, 0);
        lcd_write_char(127, 2, 24);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            lcd_write_string("                          ", 2, 0);
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
    case 95:
        lcd_write_string("05 Transpose             ~", 2, 0);
        lcd_write_char(127, 2, 24);
        switch(button_pressed){
        case 1:
            //reak;
        case 2:
            lcd_write_string("                          ", 2, 0);
            menu_state = 5;
            break;
        case 3:
            break;
        case 4:
            menu_state = 96;
            break;
        case 5:
            menu_state = 94;
            break;
        default:
            break;
        }
        break;
    case 96:
        lcd_write_string("06 Cent variation         ", 2, 0);
        lcd_write_char(127, 2, 24);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            lcd_write_string("                          ", 2, 0);
            menu_state = 5;
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            menu_state = 95;
            break;
        default:
            break;
        }
        break;
    }

    {}

    { // ADSR-5 parameters
    case 110:
        lcd_write_string("00 ADSR Aconfig          ~", 2, 0);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            lcd_write_string("                          ", 2, 0);
            menu_state = 6;
            break;
        case 3:
            break;
        case 4:
            menu_state = 111;
            break;
        case 5:
            break;
        default:
            break;
        }
        break;
    case 111:
        lcd_write_string("01 Attack time           ~", 2, 0);
        lcd_write_char(127, 2, 24);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            lcd_write_string("                          ", 2, 0);
            menu_state = 6;
            break;
        case 3:
            break;
        case 4:
            menu_state = 112;
            break;
        case 5:
            menu_state = 110;
            break;
        default:
            break;
        }
        break;
    case 112:
        lcd_write_string("02 Decay time            ~", 2, 0);
        lcd_write_char(127, 2, 24);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            lcd_write_string("                          ", 2, 0);
            menu_state = 6;
            break;
        case 3:
            break;
        case 4:
            menu_state = 113;
            break;
        case 5:
            menu_state = 111;
            break;
        default:
            break;
        }
        break;
    case 113:
        lcd_write_string("03 Sustain level         ~", 2, 0);
        lcd_write_char(127, 2, 24);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            lcd_write_string("                          ", 2, 0);
            menu_state = 6;
            break;
        case 3:
            break;
        case 4:
            menu_state = 114;
            break;
        case 5:
            menu_state = 112;
            break;
        default:
            break;
        }
        break;
    case 114:
        lcd_write_string("04 Sustain time          ~", 2, 0);
        lcd_write_char(127, 2, 24);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            lcd_write_string("                          ", 2, 0);
            menu_state = 6;
            break;
        case 3:
            break;
        case 4:
            menu_state = 115;
            break;
        case 5:
            menu_state = 113;
            break;
        default:
            break;
        }
        break;
    case 115:
        lcd_write_string("05 Release time           ", 2, 0);
        lcd_write_char(127, 2, 24);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            lcd_write_string("                          ", 2, 0);
            menu_state = 6;
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            menu_state = 114;
            break;
        default:
            break;
        }
        break;
    }

    {}

    { // Modifiers bend/mod/press
    case 130:
        lcd_write_string("00 Pitch bend amplitude  ~", 2, 0);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            lcd_write_string("                          ", 2, 0);
            menu_state = 7;
            break;
        case 3:
            break;
        case 4:
            menu_state = 131;
            break;
        case 5:
            break;
        default:
            break;
        }
        break;
    case 131:
        lcd_write_string("01 Pitch bend control    ~", 2, 0);
        lcd_write_char(127, 2, 24);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            lcd_write_string("                          ", 2, 0);
            menu_state = 7;
            break;
        case 3:
            break;
        case 4:
            menu_state = 132;
            break;
        case 5:
            menu_state = 130;
            break;
        default:
            break;
        }
        break;
    case 132:
        lcd_write_string("02 Pitch mod  amplitude  ~", 2, 0);
        lcd_write_char(127, 2, 24);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            lcd_write_string("                          ", 2, 0);
            menu_state = 7;
            break;
        case 3:
            break;
        case 4:
            menu_state = 133;
            break;
        case 5:
            menu_state = 131;
            break;
        default:
            break;
        }
        break;
    case 133:
        lcd_write_string("03 Pitch mod  control    ~", 2, 0);
        lcd_write_char(127, 2, 24);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            lcd_write_string("                          ", 2, 0);
            menu_state = 7;
            break;
        case 3:
            break;
        case 4:
            menu_state = 134;
            break;
        case 5:
            menu_state = 132;
            break;
        default:
            break;
        }
        break;
    case 134:
        lcd_write_string("04 Key press amplitude   ~", 2, 0);
        lcd_write_char(127, 2, 24);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            lcd_write_string("                          ", 2, 0);
            menu_state = 7;
            break;
        case 3:
            break;
        case 4:
            menu_state = 135;
            break;
        case 5:
            menu_state = 133;
            break;
        default:
            break;
        }
        break;
    case 135:
        lcd_write_string("05 Key press control      ", 2, 0);
        lcd_write_char(127, 2, 24);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            lcd_write_string("                          ", 2, 0);
            menu_state = 7;
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            menu_state = 134;
            break;
        default:
            break;
        }
        break;
    }

    {}

    { // LFO 1 controls
    case 150:
        lcd_write_string("00 AControl              ~", 2, 0);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            lcd_write_string("                          ", 2, 0);
            menu_state = 8;
            break;
        case 3:
            break;
        case 4:
            menu_state = 151;
            break;
        case 5:
            break;
        default:
            break;
        }
        break;
    case 151:
        lcd_write_string("01 Shape                 ~", 2, 0);
        lcd_write_char(127, 2, 24);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            lcd_write_string("                          ", 2, 0);
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
    case 152:
        lcd_write_string("02 Mode                  ~", 2, 0);
        lcd_write_char(127, 2, 24);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            lcd_write_string("                          ", 2, 0);
            menu_state = 8;
            break;
        case 3:
            break;
        case 4:
            menu_state = 153;
            break;
        case 5:
            menu_state = 151;
            break;
        default:
            break;
        }
        break;
    case 153:
        lcd_write_string("03 Amplitude             ~", 2, 0);
        lcd_write_char(127, 2, 24);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            lcd_write_string("                          ", 2, 0);
            menu_state = 8;
            break;
        case 3:
            break;
        case 4:
            menu_state = 153;
            break;
        case 5:
            menu_state = 151;
            break;
        default:
            break;
        }
        break;
    case 154:
        lcd_write_string("04 Frequency              ", 2, 0);
        lcd_write_char(127, 2, 24);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            lcd_write_string("                          ", 2, 0);
            menu_state = 8;
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            menu_state = 152;
            break;
        default:
            break;
        }
        break;
    }

    {}

    { // LFO 2 controls
    case 160:
        lcd_write_string("00 AControl              ~", 2, 0);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            lcd_write_string("                          ", 2, 0);
            menu_state = 9;
            break;
        case 3:
            break;
        case 4:
            menu_state = 161;
            break;
        case 5:
            break;
        default:
            break;
        }
        break;
    case 161:
        lcd_write_string("01 Shape                 ~", 2, 0);
        lcd_write_char(127, 2, 24);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            lcd_write_string("                          ", 2, 0);
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
    case 162:
        lcd_write_string("02 Mode                  ~", 2, 0);
        lcd_write_char(127, 2, 24);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            lcd_write_string("                          ", 2, 0);
            menu_state = 8;
            break;
        case 3:
            break;
        case 4:
            menu_state = 163;
            break;
        case 5:
            menu_state = 161;
            break;
        default:
            break;
        }
        break;
    case 163:
        lcd_write_string("03 Amplitude             ~", 2, 0);
        lcd_write_char(127, 2, 24);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            lcd_write_string("                          ", 2, 0);
            menu_state = 9;
            break;
        case 3:
            break;
        case 4:
            menu_state = 163;
            break;
        case 5:
            menu_state = 161;
            break;
        default:
            break;
        }
        break;
    case 164:
        lcd_write_string("04 Frequency              ", 2, 0);
        lcd_write_char(127, 2, 24);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            lcd_write_string("                          ", 2, 0);
            menu_state = 9;
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            menu_state = 162;
            break;
        default:
            break;
        }
        break;
    }

    {}

    { // Randomizer controls
    case 170:
        lcd_write_string("00 Control               ~", 2, 0);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            lcd_write_string("                          ", 2, 0);
            menu_state = 10;
            break;
        case 3:
            break;
        case 4:
            menu_state = 171;
            break;
        case 5:
            break;
        default:
            break;
        }
        break;
    case 171:
        lcd_write_string("01 Amplitude              ", 2, 0);
        lcd_write_char(127, 2, 24);
        switch(button_pressed){
        case 1:
            //break;
        case 2:
            lcd_write_string("                          ", 2, 0);
            menu_state = 10;
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            menu_state = 170;
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

    lcd_write_string("AVR/ESP  Ctrlr based synth", 3, 7);
    lcd_write_char(0b11100100, 3, 15);

    lag(3000);
    lcd_write_string("                                        ", 0, 0);
    lcd_write_string("                                        ", 2, 0);
    lcd_write_string("                                        ", 3, 0);
    lcd_write_char(255, 0, 26);
    for(i = 0; i <= 26; i++)
        lcd_write_char(255, 1, i);
    lcd_write_char(255, 2, 26);
    lcd_write_char(255, 3, 26);
    }

    while(1){
        loop();
    }
}









