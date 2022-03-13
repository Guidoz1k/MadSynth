#include "lcd.h"
/*
    PORTC: data 0 - 7
    PORTE:
        3   RS pin
        4   E1 pin
    	5   E2 pin
*/

// makeshift kludge 1us imprecise delay
static void nopi(void){
    __asm__("nop\n\t");
    __asm__("nop\n\t");
    __asm__("nop\n\t");
    __asm__("nop\n\t");
    __asm__("nop\n\t");
}

// ouput data bus
static void lcdData(uint8_t data){
	PORTC = data;
}

// output RS bit
static void lcdRS(uint8_t RS){
    if(RS)
        PORTE |= 0x08;
    else
        PORTE &= 0xF7;
}

// output E1 bit
static void lcdE1(uint8_t E){
    if(E)
        PORTC |= 0x10;
    else
        PORTC &= 0xEF;
}

// output the LCD enable UP pulse
static void lcdEnablePulse1(void){
    uint8_t i;

    lcdE1(1);
	nopi();						// 1us min delay
	lcdE1(0);
	for(i = 0; i < 36; i++)     // 40us min delay
		nopi();
}

// output E1 bit
static void lcdE2(uint8_t E){
    if(E)
        PORTC |= 0x20;
    else
        PORTC &= 0xDF;
}

// output the LCD enable DOWN pulse
static void lcdEnablePulse2(void){
    uint8_t i;

    lcdE2(1);
	nopi();						// 1us min delay
	lcdE2(0);
	for(i = 0; i < 36; i++)     // 40us min delay
		nopi();
}

// configure LCD through data bus
static void lcdConfig(uint8_t data, uint8_t sector){
    lcdRS(0);
	lcdData(data);
	nopi();
	if(sector == 0)
		lcdEnablePulse1();
	else
		lcdEnablePulse2();
	lcdData(0x00);
}

// resets the LCD
static void lcd_flush(void){
    uint16_t i;

    lcdRS(0);
	lcdData(0x01);
    lcdE1(1);
	nopi();         // 1us min delay
	lcdE1(0);
	nopi();
    lcdE2(1);
	nopi();         // 1us min delay
	lcdE2(0);

	for(i = 0; i < 1400; i++)	// 1.65ms min delay
		nopi();
}

// write to the LCD screen
static void lcd_write(uint8_t data, uint8_t sector){
    lcdRS(0);
	lcdData(data);
	lcdRS(1);
	if(sector == 0)
		lcdEnablePulse1();
	else
		lcdEnablePulse2();
	lcdRS(0);
	lcdData(0x00);
}

// sets LCD cursor position
static void lcd_pos(uint8_t line, uint8_t pos){
	if(line)			// display second line
		pos |= 0x40;	// pos 0 of second line is memory position 0x40
	pos |= 0x80;		// config bit set

	if(line < 2)
		lcdConfig(pos, 0);
	else{
		line -= 2;
		lcdConfig(pos, 1);
	}
}

// initialize LCD functions
void display_init(){
	DDRA = 0xFF;	// PORTB is output
	DDRC |= 0x03;	// PORTC is output (bits 0 and 1)

	// first sector
	lcdConfig(0x06, 0);	// display automatic cursor increment
	lcdConfig(0x0c, 0);	// active display with hidden cursor
	lcdConfig(0x38, 0);	// bit and pixel format
	// second sector
	lcdConfig(0x06, 1);	// display automatic cursor increment
	lcdConfig(0x0c, 1);	// active display with hidden cursor
	lcdConfig(0x38, 1);	// bit and pixel format

	lcd_flush();
}

void lcd_write_string(const char *pointer, uint8_t line, uint8_t pos){
	uint8_t sector = 0;
	uint8_t counter = 0;

	if(line > 1)
		sector = 1;
	lcd_pos(line, pos);

	while((counter++ < (40 - pos)) && (*pointer != '\0')){
		lcd_write(*(pointer++), sector);
	}
}

void lcd_write_number(uint16_t number, uint8_t size, uint8_t line, uint8_t pos){
	uint8_t sector = 0;
	char character = 0;
	uint32_t ten = 0;
	uint8_t i, j;

	if(line > 1)
		sector = 1;
	lcd_pos(line, pos);

	for(i = size; i >= 0; i--){
		ten = 1;
		for(j = 1; j <= i; j++)
			ten *= 10;

		if(i < size)
			character = ((number / ten) % 10) + 48;
		else
			character = (number / ten) + 48;

		lcd_write(character, sector);
	}

}

