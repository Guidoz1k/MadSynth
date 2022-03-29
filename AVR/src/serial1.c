#include "serial1.h"
/*
    PORTD:
        2   RX pin
        3   TX pin
*/
/*
1 ... 61	-->	keys pressed	--> 0 = off, 1 - 256 = intensity

110 --> volume			--> 0 ... 99?
111	--> stereo mode		--> L+R, L, R
112	--> osc count limit	--> 0 ... MAXOSC
113	--> max osc mode	--> #MISSING#
114	--> octave + trans	--> 0 ... 110 keys
115	--> ADRS A time	0	-->	first byte
116	--> ADRS A time	1	--> second byte when necessary
117	--> ADRS D time	0	-->	first byte
118	--> ADRS D time	1	--> second byte when necessary
119	--> ADRS S time	0	-->	first byte
120	--> ADRS S time	1	--> second byte when necessary
121	--> ADRS S level 0	-->	first byte
122	--> ADRS S level 1	--> second byte when necessary
123	--> ADRS A time	0	-->	first byte
124	--> ADRS A time	1	--> second byte when necessary

130	-->	OSC1 shape
131	-->	OSC1 # sub-osc
132	-->	OSC1 sub-osc mode
133	-->	OSC1 sub-osc unison
134	-->	OSC1 transpose
135	-->	OSC1 cent
140	-->	OSC2 shape
141	-->	OSC2 # sub-osc
142	-->	OSC2 sub-osc mode
143	-->	OSC2 sub-osc unison
144	-->	OSC2 transpose
145	-->	OSC2 cent

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
static void serial1_send(uint8_t character){
	while(!serial1WriteAvailable());
	serial1Write(character);
}

void serial1_transmit(uint8_t byte0, uint8_t byte1){
	serial1_send(HEADBYTE);
	serial1_send(byte0);
	serial1_send(byte1);
}

