#include "timer.h"

// 1ms interrupt initialization
void timer_init(void){
    // interrupt config (1ms)
	TCCR0A  = 0x02;		// timer0 CTC mode
	TCCR0B  = 0x03;		// timer0 prescaler 64
	OCR0A   = 0xF9;		// 249 in hex
	TIMSK0  = 0x02;		// timer0 compareA interrupt enable
}

