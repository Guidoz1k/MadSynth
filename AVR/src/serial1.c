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
112	--> osc count limit	--> 0 ... MAXOSC	X
113	--> max osc mode	--> 0 or 1			X
114	--> octave			--> 0 ... 8 octaves to the right
115	--> trans			--> OSC1 trans
116 --> mixer channel 1 --> 0 ... 99?
117 --> mixer channel 1 --> 0 ... 99?

120	-->	OSC1 shape
121	-->	OSC1 # sub-osc
122	-->	OSC1 sub-osc mode   X
123	-->	OSC1 sub-osc unison X
124	-->	OSC1 transpose
125	-->	OSC1 cent
126 --> OSC1 trans_mod
127 --> OSC1 cent_mod
130	-->	OSC2 shape
131	-->	OSC2 # sub-osc
132	-->	OSC2 sub-osc mode   X
133	-->	OSC2 sub-osc unison X
134	-->	OSC2 transpose		X
135	-->	OSC2 cent			X
136	-->	OSC2 trans_mod		X
137	-->	OSC2 cent_mod		X

140	--> ADSR A time	0	-->	first byte
141	--> ADSR A time	1	--> second byte when necessary
142	--> ADSR D time	0	-->	first byte
143	--> ADSR D time	1	--> second byte when necessary
144	--> ADSR S time	0	-->	first byte
145	--> ADSR S time	1	--> second byte when necessary
146	--> ADSR S level 0	-->	first byte
147	--> ADSR S level 1	--> second byte when necessary
148	--> ADSR R time	0	-->	first byte
149	--> ADSR R time	1	--> second byte when necessary
150	--> ADSR A_mod
151	--> ADSR D_mod
152	--> ADSR S time_mod
153	--> ADSR S_mod
154	--> ADSR R_mod

160 --> LFO1 state
161 --> LFO1 mode
162 --> LFO1 shape
163 --> LFO1 amp
164 --> LFO1 freq
165 --> LFO1 amp_mod
166 --> LFO1 freq_mod
170 --> LFO2 state
171 --> LFO2 mode
172 --> LFO2 shape
173 --> LFO2 amp
174 --> LFO2 freq
175 --> LFO2 amp_mod
176 --> LFO2 freq_mod

// manual controls' values
180 --> pitch wheel
181 --> mod wheel
182 --> slider 1
183 --> slider 2
184 --> slider 3
185 --> slider 4
186 --> slider 5
187 --> key note mod amp
188 --> key speed mod amp

190 --> noiser state
191 --> noiser amp

199 --> MOD ENV state
200	--> MOD ENV A time	0	-->	first byte
201	--> MOD ENV A time	1	--> second byte when necessary
202	--> MOD ENV D time	0	-->	first byte
203	--> MOD ENV D time	1	--> second byte when necessary
204	--> MOD ENV S time	0	-->	first byte
205	--> MOD ENV S time	1	--> second byte when necessary
206	--> MOD ENV S level 0	-->	first byte
207	--> MOD ENV S level 1	--> second byte when necessary
208	--> MOD ENV R time	0	-->	first byte
209	--> MOD ENV R time	1	--> second byte when necessary
210	--> MOD ENV A_mod
211	--> MOD ENV D_mod
212	--> MOD ENV S time_mod
213	--> MOD ENV S_mod
214	--> MOD ENV R_mod
215 --> MOD ENV maximum value

220 --> low-pass filter state
221 --> low-pass filter cut-off frequency
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
	//serial1_send(HEADBYTE);
	serial1_send(byte0);
	serial1_send(byte1);
}

