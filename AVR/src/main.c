#include "main.h"
/*
    Arduino Mega Board:
    PORTA: 8 pins --------------> Keyboard
    PORTB: 8 pins --------------> UI buttons
    PORTC: 8 pins --------------> LCD data pins
    PORTD: 5 pins (0,1,2,3,7)
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
    uint8_t i = 0;
    uint8_t button_pressed = 0; // records the value of the button each iteration
    static uint8_t button_hold = 0;    // counter to limit the button interaction
    static uint8_t menu_state = 0;     // UI machine state variable

    // key state machine to serial routine
    for(i = 0; i < (MAXKEYS / 2); i++){
        switch(key_detection[i].stateMachine){
        case 0:
            break;
        case 1:
            break;
        case 2: // key is on
            serial_write_number(i, 2, 0);
            serial_write_string(" ON = ", 0);
            serial_write_number(key_detection[i].counter, 3, 1);
            key_detection[i].stateMachine = 3;
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        case 6: // key is off
            serial_write_number(i, 2, 0);
            serial_write_string(" OFF ", 1);
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
            break;
        case 4:
            break;
        case 5:
            break;
        default:
            break;
        }
        break;
    case 1: // Oscillators allocation
        lcd_write_string("01 OSC allocation       ⌂~", 0, 0);
        switch(button_pressed){
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        default:
            break;
        }
        break;
    case 2: // Keybed offset config
        lcd_write_string("02 Keybed offset config ⌂~", 0, 0);
        switch(button_pressed){
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        default:
            break;
        }
        break;
    case 3: // Triple Oscillator Mixer
        lcd_write_string("03 Oscillators mixer    ⌂~", 0, 0);
        switch(button_pressed){
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        default:
            break;
        }
        break;
    case 4: // Oscillator 1 controls
        lcd_write_string("04 OSC 1 controls       ⌂~", 0, 0);
        switch(button_pressed){
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        default:
            break;
        }
        break;
    case 5: // Oscillator 2 controls
        lcd_write_string("05 OSC 2 controls       ⌂~", 0, 0);
        switch(button_pressed){
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        default:
            break;
        }
        break;
    case 6: // Bass oscillator controls
        lcd_write_string("06 Bass osc controls    ⌂~", 0, 0);
        switch(button_pressed){
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        default:
            break;
        }
        break;
    case 7: // ADSR-5 parameters
        lcd_write_string("07 ADSR-5 parameters    ⌂~", 0, 0);
        switch(button_pressed){
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        default:
            break;
        }
        break;
    case 8: // Modifiers bend/mod/press
        lcd_write_string("08 Control modifiers    ⌂~", 0, 0);
        switch(button_pressed){
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        default:
            break;
        }
        break;
    case 9: // LFO controls
        lcd_write_string("09 LFO controls         ⌂~", 0, 0);
        switch(button_pressed){
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        default:
            break;
        }
        break;
    case 10: // Randomizer controls
        lcd_write_string("10 Randomizer controls  ⌂ ", 0, 0);
        switch(button_pressed){
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        default:
            break;
        }
        break;
    case 20: // Master channel setup
        lcd_write_string("00 Volume                ~", 0, 0);
        switch(button_pressed){
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        default:
            break;
        }
        break;
    case 21: // Master channel setup
        lcd_write_string("01 Stereo               ⌂ ", 0, 0);
        switch(button_pressed){
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        default:
            break;
        }
        break;
    case 22: // Oscillators allocation
        lcd_write_string("00 Osc count limit       ~", 0, 0);
        switch(button_pressed){
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        default:
            break;
        }
        break;
    case 23: // Oscillators allocation
        lcd_write_string("01 Max oscillator mode  ⌂ ", 0, 0);
        switch(button_pressed){
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        default:
            break;
        }
        break;
    case 24: // Keybed offset config
        lcd_write_string("00 Octave                ~", 0, 0);
        switch(button_pressed){
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        default:
            break;
        }
        break;
    case 25: // Keybed offset config
        lcd_write_string("01 Transpose            ⌂ ", 0, 0);
        switch(button_pressed){
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        default:
            break;
        }
        break;
    case 26: // ADSR-5 parameters
        lcd_write_string("00 ADSR mult-config     ⌂ ", 0, 0);
        switch(button_pressed){
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        default:
            break;
        }
        break;
    case 27: // ADSR-5 parameters
        lcd_write_string("01 Attack time           ~", 0, 0);
        switch(button_pressed){
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        default:
            break;
        }
        break;
    case 28: // ADSR-5 parameters
        lcd_write_string("02 Decay time           ⌂~", 0, 0);
        switch(button_pressed){
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        default:
            break;
        }
        break;
    case 29: // ADSR-5 parameters
        lcd_write_string("03 Sustain level        ⌂~", 0, 0);
        switch(button_pressed){
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        default:
            break;
        }
        break;
    case 30: // ADSR-5 parameters
        lcd_write_string("04 Sustain time         ⌂~", 0, 0);
        switch(button_pressed){
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        default:
            break;
        }
        break;
    case 31: // ADSR-5 parameters
        lcd_write_string("05 Release time         ⌂~", 0, 0);
        switch(button_pressed){
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        default:
            break;
        }
        break;
    case 32: // Modifiers bend/mod/press
        lcd_write_string("00 Pitch bend amplitude  ~", 0, 0);
        switch(button_pressed){
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        default:
            break;
        }
        break;
    case 33: // Modifiers bend/mod/press
        lcd_write_string("01 Pitch bend control   ⌂~", 0, 0);
        switch(button_pressed){
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        default:
            break;
        }
        break;
    case 34: // Modifiers bend/mod/press
        lcd_write_string("02 Pitch mod  amplitude ⌂~", 0, 0);
        switch(button_pressed){
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        default:
            break;
        }
        break;
    case 35: // Modifiers bend/mod/press
        lcd_write_string("03 Pitch mod  control   ⌂~", 0, 0);
        switch(button_pressed){
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        default:
            break;
        }
        break;
    case 36: // Modifiers bend/mod/press
        lcd_write_string("04 Key press amplitude  ⌂~", 0, 0);
        switch(button_pressed){
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        default:
            break;
        }
        break;
    case 37: // Modifiers bend/mod/press
        lcd_write_string("05 Key press control    ⌂ ", 0, 0);
        switch(button_pressed){
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        default:
            break;
        }
        break;
    case 38: // LFO controls
        lcd_write_string("00 Amplitude             ~", 0, 0);
        switch(button_pressed){
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        default:
            break;
        }
        break;
    case 39: // LFO controls
        lcd_write_string("01 Control              ⌂ ", 0, 0);
        switch(button_pressed){
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        default:
            break;
        }
        break;
    case 40: // Randomizer controls
        lcd_write_string("00 Amplitude             ~", 0, 0);
        switch(button_pressed){
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        default:
            break;
        }
        break;
    case 41: // Randomizer controls
        lcd_write_string("01 Control              ⌂ ", 0, 0);
        switch(button_pressed){
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        default:
            break;
        }
        break;
    case 42: // Triple Oscillator Mixer
        lcd_write_string("00 Oscillator 1         ⌂ ", 0, 0);
        switch(button_pressed){
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        default:
            break;
        }
        break;
    case 43: // Triple Oscillator Mixer
        lcd_write_string("01 Oscillator 2         ⌂~", 0, 0);
        switch(button_pressed){
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        default:
            break;
        }
        break;
    case 44: // Triple Oscillator Mixer
        lcd_write_string("02 Bass oscillator       ~", 0, 0);
        switch(button_pressed){
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        default:
            break;
        }
        break;
    case 45: // Oscillator 1 controls
        lcd_write_string("00 Shape                 ~", 0, 0);
        switch(button_pressed){
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        default:
            break;
        }
        break;
    case 46: // Oscillator 1 controls
        lcd_write_string("01 Sub-oscillators      ⌂~", 0, 0);
        switch(button_pressed){
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        default:
            break;
        }
        break;
    case 47: // Oscillator 1 controls
        lcd_write_string("02 Sub-oscillators mode ⌂~", 0, 0);
        switch(button_pressed){
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        default:
            break;
        }
        break;
    case 48: // Oscillator 1 controls
        lcd_write_string("03 Transpose            ⌂~", 0, 0);
        switch(button_pressed){
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        default:
            break;
        }
        break;
    case 49: // Oscillator 1 controls
        lcd_write_string("04 Cent variation       ⌂ ", 0, 0);
        switch(button_pressed){
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        default:
            break;
        }
        break;
    case 50: // Oscillator 2 controls
        lcd_write_string("00 Shape                 ~", 0, 0);
        switch(button_pressed){
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        default:
            break;
        }
        break;
    case 51: // Oscillator 2 controls
        lcd_write_string("01 Sub-oscillators      ⌂~", 0, 0);
        switch(button_pressed){
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        default:
            break;
        }
        break;
    case 52: // Oscillator 2 controls
        lcd_write_string("02 Sub-oscillators mode ⌂~", 0, 0);
        switch(button_pressed){
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        default:
            break;
        }
        break;
    case 53: // Oscillator 2 controls
        lcd_write_string("03 Transpose            ⌂~", 0, 0);
        switch(button_pressed){
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        default:
            break;
        }
        break;
    case 54: // Oscillator 2 controls
        lcd_write_string("04 Cent variation       ⌂ ", 0, 0);
        switch(button_pressed){
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        default:
            break;
        }
        break;
    case 55: // Bass oscillator controls
        lcd_write_string("00 Shape                 ~", 0, 0);
        switch(button_pressed){
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        default:
            break;
        }
        break;
    case 56: // Bass oscillator controls
        lcd_write_string("03 Transpose            ⌂~", 0, 0);
        switch(button_pressed){
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        default:
            break;
        }
        break;
    case 57: // Bass oscillator controls
        lcd_write_string("04 Cent variation       ⌂ ", 0, 0);
        switch(button_pressed){
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        default:
            break;
        }
        break;
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
    serial_init();      // initialize the serial pins and functions
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