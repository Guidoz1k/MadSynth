#include "serial0.h"
/*
    PORTD:
        2   RX pin
        3   TX pin
*/
/*
0 ... 61	-->	keys pressed	--> 0 = off, 1 - 256 = intensity

110 --> volume			--> 0 ... 99?
111	--> stereo mode		--> L+R, L, R
112	--> osc count limit	--> 0 ... MAXOSC
113	--> max osc mode	--> #MISSING#
114	--> octave + trans	--> 0 ... 110 keys
115	--> ADRS A time		--> \
116	--> ADRS D time		--> |
117	--> ADRS S time		--> > ADSR #SHOULD THIS BE 16 BITS?
118	--> ADRS S level 	--> |
119	--> ADRS A time		--> /

120	-->	OSC1 shape
121	-->	OSC1 # sub-osc
122	-->	OSC1 sub-osc mode
123	-->	OSC1 sub-osc unison
124	-->	OSC1 transpose
125	-->	OSC1 cent
130	-->	OSC2 shape
131	-->	OSC2 # sub-osc
132	-->	OSC2 sub-osc mode
133	-->	OSC2 sub-osc unison
134	-->	OSC2 transpose
135	-->	OSC2 cent

######################################################TBD
	controls
	LFO
	randomizer (noiser)
	mod envelope
	ALL POST-FX
*/

// initialize the serial peripheral
void serial1_init(void){
	UBRR1H = 0x00;
	UBRR1L = 0x10;		// baud rate of 115200 equals to UBRR = 16
	UCSR1A = 0x02;		// U2Xn = 1 so the baud rate is B = (f_osc / (UBRR + 1)) / 8
	UCSR1B = 0x18;		// enables RX and TX
	UCSR1C = 0x06;		// set frame format: 8data
}

// test if ESP serial is ready to transmit
static uint8_t serial1WriteAvailable(){
	uint8_t data = 0;	// return flag value

	if(UCSR1A & 0x20)	// USART data register empty (UDRE0) is 1
	data = 1;

	return data;
}

// ESP serial transmit
static void serial1Write(byte data){
	UDR1 = data;
	}

// blocking function that sends one character over ESP UART
void serial1_transmit(uint8_t character){
	while(!serial1WriteAvailable());
	serial1Write(character);
}


