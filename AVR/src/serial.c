#include "serial.h"

void serialInit(void){
	// serial config
/*
	UBRR0H = 0x00;		// baud rate of 19200 equals to UBRR 0x34
	UBRR0L = 0x34;		//
	UCSR0B = 0x18;		// enables RX and TX
	UCSR0C = 0x06;		// set frame format: 8data

	UCSR0B |= 1 << 7;						// enables RX complete interruption
	for(byte i = 0; i < S_INBUFFER; i++)	// cleans serial buffers vectors
		serialInBuffer[i] = '\0';
	for(twobyte i = 0; i < S_OUTBUFFER; i++)
		serialOutBuffer[i] = '\0';
*/
}
