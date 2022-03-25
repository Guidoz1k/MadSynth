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

// main loop delay variables
volatile uint8_t timing = 0;
volatile uint16_t time_counter = 0;

// ============================================================ main functions

// delay function for the main loop
static void lag(uint16_t time){
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

    // key state machine to serial routine
    for(i = 0; i < (MAXKEYS / 2); i++)
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

/*
    lcd_write_string("UI BUTTONS: ", 0, 1);
    switch(inputs_read()){
    case 0:
        lcd_write_string("INVALID", 0, 13);
        break;
    case 1:
        lcd_write_string("Encoder", 0, 13);
        break;
    case 2:
        lcd_write_string("UP", 0, 13);
        break;
    case 3:
        lcd_write_string("DOWN", 0, 13);
        break;
    case 4:
        lcd_write_string("LEFT", 0, 13);
        break;
    case 5:
        lcd_write_string("RIGHT", 0, 13);
        break;
    default:
        break;
    }
    lcd_write_string("ROTATION: ", 0, 24);
    lcd_write_number(inputs_rotation(), 3, 0, 34);
*/
    serial_write_number(inputs_read(), 1, 0);
    serial_write_string(" - ", 0);
    serial_write_number(inputs_rotation(), 5, 0);
    serial_write_string(" - ", 0);
    for(i = 0; i <= 6; i++){
        serial_write_number(adc_read(i), 4, 0);
        serial_write_string(" - ", 0);
    }
    serial_write_string(" ", 1);

    lag(250);
}

// main function
int main(){
    uint8_t i = 0;

    display_init();     // initialize the display pins and functions
    onboard_init();     // initialize the onboard components pins and functions
    serial_init();      // initialize the serial pins and functions
    keyboard_init();    // initialize the keyboard pins and functions
    ui_init();          // initialize the UI buttons and encoder pins and functions
    adc_init();         // initialize the ADC pins and functions

    timer_init();       // initialize the timer functions
	sei();				// global interrupts enable

    // variables initialization
    for(i = 0; i < MAXKEYS; i++){
        key_detection[i].counter = 0;
        key_detection[i].stateMachine = 0;
    }

    lag(1000);

    while(1){
        loop();
    }
}