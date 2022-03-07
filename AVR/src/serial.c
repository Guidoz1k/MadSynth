#include "serial.h"

// initialize the serial peripheral
void serial_init(void){
	UBRR0H = 0x00;
	UBRR0L = 0x22;		// baud rate of 57600 equals to UBRR = 34
	UCSR0A = 0x02;		// U2Xn = 1 so the baud rate is B = (f_osc / (UBRR + 1)) / 8
	UCSR0B = 0x18;		// enables RX and TX
	UCSR0C = 0x06;		// set frame format: 8data

//	UCSR0B |= 1 << 7;						// enables RX complete interruption
}

// test if serial is ready to transmit
static uint8_t serialWriteAvailable(){
	uint8_t data = 0;	// return flag value

	if(UCSR0A & 0x20)	// USART data register empty (UDRE0) is 1
	data = 1;

	return data;
}

// serial transmit
static void serialWrite(byte data){
	UDR0 = data;
	}

// blocking function that sends one character over UART
void serial_transmit(uint8_t character){
	while(!serialWriteAvailable());
	serialWrite(character);
}
