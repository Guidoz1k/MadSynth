#include "lcd.h"
/*
Display:
	PORTA 0 - 7     ==> data 0 - 7
	PORTC 0, 1  	==> E, RS
*/

static void nopi(void){  // 1us
    __asm__("nop\n\t");
    __asm__("nop\n\t");
    __asm__("nop\n\t");
    __asm__("nop\n\t");
    __asm__("nop\n\t");
}

static void lcdData(uint8_t data){
	PORTA = data;
}

static void lcdRS(uint8_t RS){
    if(RS)
        PORTC |= 0x02;
    else
        PORTC &= 0xFD;
}

static void lcdE(uint8_t E){
    if(E)
        PORTC |= 0x01;
    else
        PORTC &= 0xFE;
}

static void lcdEnablePulse(void){
    uint8_t i;

    lcdE(1);
	nopi();						// 1us min delay
	lcdE(0);
	for(i = 0; i < 36; i++)     // 40us min delay
		nopi();
}

static void lcdConfig(uint8_t data){
    lcdRS(0);
	lcdData(data);
	nopi();
	lcdEnablePulse();
	lcdData(0x00);
}

void lcdFlush(void){
    uint16_t i;

    lcdRS(0);
	lcdData(0x01);
    lcdE(1);
	nopi();         // 1us min delay
	lcdE(0);

	for(i = 0; i < 1400; i++)	// 1.65ms min delay
		nopi();
}

void lcdWrite(uint8_t data){
    lcdRS(0);
	lcdData(data);
	lcdRS(1);
	lcdEnablePulse();
	lcdRS(0);
	lcdData(0x00);
}

void lcdPos(uint8_t line, uint8_t pos){
	if(line)			// display second line
		pos |= 0x40;	// pos 0 of second line is memory position 0x40
	pos |= 0x80;		// config bit set
	lcdConfig(pos);
}

void displayInit(){
	DDRA = 0xFF;	// PORTB is output
	DDRC |= 0x03;	// PORTC is output (bits 0 and 1)

	lcdConfig(0x06);	// display automatic cursor increment
	lcdConfig(0x0c);	// active display with hidden cursor
	lcdConfig(0x38);	// bit and pixel format
	lcdFlush();
}


