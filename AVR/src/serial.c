#include "serial.h"
/*
    PORTE:
        0   RX pin
        1   TX pin
*/

static const uint8_t MAXSIZE = 20; // max size of serial 0 message

// initialize the serial peripheral
void serial_init(void){
	UBRR0H = 0x00;
	UBRR0L = 0x10;		// baud rate of 115200 equals to UBRR = 16
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
static void serial_transmit(uint8_t character){
	while(!serialWriteAvailable());
	serialWrite(character);
}

// prints to serial 0 a string
void serial_write_string(const char *pointer, uint8_t newline){
	uint8_t counter = 0;

	while((counter++ < MAXSIZE) && (*pointer != '\0')){
		serial_transmit(*(pointer++));
	}
	if(newline)
		serial_transmit('\n');
}

// prints to serial a integer
void serial_write_number(uint16_t number, uint8_t size, uint8_t newline){
	char character = 0;
	uint32_t ten = 0;
	int8_t i, j;

	for(i = size - 1; i >= 0; i--){
		ten = 1;
		for(j = 1; j <= i; j++)
			ten *= 10;

		if(i < size)
			character = ((number / ten) % 10) + 48;
		else
			character = (number / ten) + 48;

		serial_transmit(character);
	}
	if(newline)
		serial_transmit('\n');

}