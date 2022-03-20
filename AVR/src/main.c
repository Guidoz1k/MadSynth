#include "main.h"
/*
    Arduino Mega Board:
    PORTA: 8 pins --------------> Keyboard
    PORTB: 8 pins --------------> UI buttons
    PORTC: 8 pins --------------> LCD data pins
    PORTD: 5 pins (0,1,2,3,7)
    PORTE: 3 pins (U,U,3,4,5) --> U = UART0 + LCD control pins
    PORTF: 8 analog pins -------> analog inputs
    PORTG: 4 pins (0,1,2,5)-
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

    if(timing)
        time_counter++;

    led_on();
    for(i = 1; i < MAXKEYS; i++){
        keyboard_write(i);
        if(i < 61){     // key's x pin
            if(keyboard_read()) // key's x == 1
                switch(key_detection[i].stateMachine){
                case 0:
                    break;
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
        }
        else{           // key's y pin
            if(keyboard_read()) // key's y == 1
                switch(key_detection[i].stateMachine){
                case 0:
                    break;
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
                case 6:
                    break;
                default:
                    break;
                }
            else                // key's y == 0
                switch(key_detection[i].stateMachine){
                case 0:
                    break;
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
    lag(10);

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