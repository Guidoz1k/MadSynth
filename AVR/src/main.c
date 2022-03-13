#include "main.h"
/*  Arduino Mega Board:
    PORTA: 8 pins --------------> Keyboard
    PORTB: 8 pins --------------> UI buttons
    PORTC: 8 pins --------------> LCD data pins
    PORTD: 5 pins
    PORTE: 3 pins (U,U,3,4,5) U = UART0 + LCD control pins
    PORTF: 8 analog pins -------> analog inputs
    PORTG: 4 pins (0,1,2,5)
    PORTH: 6 pins
    PORTI: Does not exist
    PORTJ: 2 pins (0,1)
    PORTK: 8 analog pins
    PORTL: 8 pins
*/

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
    if(timing)
        time_counter++;
}

// asynchronous main loop
void loop(){
    lag(1000);
    serial_transmit('x');
    serial_transmit('\n');
}

// main function
int main(){

    display_init();     // initialize the display pins and functions
    onboard_init();     // initialize the onboard components pins and functions
    serial_init();      // initialize the serial pins and functions
    keyboard_init();    // initialize the keyboard pins and functions
    ui_init();          // initialize the UI buttons and encoder pins and functions

    timer_init();       // initialize the timer functions
	sei();				// global interrupts enable

    while(1){
        loop();
    }
}