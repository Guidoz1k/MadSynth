#include "keyboard.h"
/*
    PORTA:
        0   Column pin 0
        1   Column pin 1
        2   Column pin 2
        3   Row pin 0
        4   Row pin 1
        5   Row pin 2
        6   Row pin 3
        7   Output pin
*/

void keyboard_init(void){
    DDRA = 0b01111111;
    PORTA = 0;
}

// sets the output to select the right mux channel
void keyboard_write(uint8_t position){
    // PORTA = column number | (Row number << 3); for more information check the "1 - keyboard layout.xlsx"
    switch(position){
    // first edge (x)
    // first octave, x edge
    case  0:
        PORTA = 4 | (6 << 3);
        break;
    case  1:
        PORTA = 5 | (6 << 3);
        break;
    case  2:
        PORTA = 6 | (6 << 3);
        break;
    case  3:
        PORTA = 7 | (6 << 3);
        break;
    case  4:
        PORTA = 0 | (5 << 3);
        break;
    case  5:
        PORTA = 1 | (5 << 3);
        break;
    case  6:
        PORTA = 2 | (5 << 3);
        break;
    case  7:
        PORTA = 3 | (5 << 3);
        break;
    case  8:
        PORTA = 4 | (5 << 3);
        break;
    case  9:
        PORTA = 5 | (5 << 3);
        break;
    case 10:
        PORTA = 6 | (5 << 3);
        break;
    case 11:
        PORTA = 7 | (5 << 3);
        break;
    // second octave, x edge
    case 12:
        PORTA = 0 | (4 << 3);
        break;
    case 13:
        PORTA = 1 | (4 << 3);
        break;
    case 14:
        PORTA = 2 | (4 << 3);
        break;
    case 15:
        PORTA = 3 | (4 << 3);
        break;
    case 16:
        PORTA = 4 | (4 << 3);
        break;
    case 17:
        PORTA = 5 | (4 << 3);
        break;
    case 18:
        PORTA = 6 | (4 << 3);
        break;
    case 19:
        PORTA = 7 | (4 << 3);
        break;
    case 20:
        PORTA = 0 | (3 << 3);
        break;
    case 21:
        PORTA = 1 | (3 << 3);
        break;
    case 22:
        PORTA = 2 | (3 << 3);
        break;
    case 23:
        PORTA = 3 | (3 << 3);
        break;
    // third octave, x edge
    case 24:
        PORTA = 4 | (3 << 3);
        break;
    case 25:
        PORTA = 5 | (3 << 3);
        break;
    case 26:
        PORTA = 6 | (3 << 3);
        break;
    case 27:
        PORTA = 7 | (3 << 3);
        break;
    case 28:
        PORTA = 0 | (2 << 3);
        break;
    case 29:
        PORTA = 1 | (2 << 3);
        break;
    case 30:
        PORTA = 2 | (2 << 3);
        break;
    case 31:
        PORTA = 3 | (2 << 3);
        break;
    case 32:
        PORTA = 4 | (2 << 3);
        break;
    case 33:
        PORTA = 5 | (2 << 3);
        break;
    case 34:
        PORTA = 6 | (2 << 3);
        break;
    case 35:
        PORTA = 7 | (2 << 3);
        break;
    // fourth octave, x edge
    case 36:
        PORTA = 0 | (1 << 3);
        break;
    case 37:
        PORTA = 1 | (1 << 3);
        break;
    case 38:
        PORTA = 2 | (1 << 3);
        break;
    case 39:
        PORTA = 3 | (1 << 3);
        break;
    case 40:
        PORTA = 4 | (1 << 3);
        break;
    case 41:
        PORTA = 5 | (1 << 3);
        break;
    case 42:
        PORTA = 6 | (1 << 3);
        break;
    case 43:
        PORTA = 7 | (1 << 3);
        break;
    case 44:
        PORTA = 0 | (0 << 3);
        break;
    case 45:
        PORTA = 1 | (0 << 3);
        break;
    case 46:
        PORTA = 2 | (0 << 3);
        break;
    case 47:
        PORTA = 3 | (0 << 3);
        break;
    // fifth octave, x edge
    case 48:
        PORTA = 4 | (0 << 3);
        break;
    case 49:
        PORTA = 5 | (0 << 3);
        break;
    case 50:
        PORTA = 6 | (0 << 3);
        break;
    case 51:
        PORTA = 7 | (0 << 3);
        break;
    case 52:
        PORTA = 0 | (7 << 3);
        break;
    case 53:
        PORTA = 1 | (7 << 3);
        break;
    case 54:
        PORTA = 2 | (7 << 3);
        break;
    case 55:
        PORTA = 3 | (7 << 3);
        break;
    case 56:
        PORTA = 4 | (7 << 3);
        break;
    case 57:
        PORTA = 5 | (7 << 3);
        break;
    case 58:
        PORTA = 6 | (7 << 3);
        break;
    case 59:
        PORTA = 7 | (7 << 3);
        break;
    // sixth octave, x edge
    case 60:
        PORTA = 0 | (6 << 3);
        break;
    // second edge (y)
    // first octave, y edge 
    case 61:
        PORTA = 4 | ((6 + 8) << 3);
        break;
    case 62:
        PORTA = 5 | ((6 + 8) << 3);
        break;
    case 63:
        PORTA = 6 | ((6 + 8) << 3);
        break;
    case 64:
        PORTA = 7 | ((6 + 8) << 3);
        break;
    case 65:
        PORTA = 0 | ((5 + 8) << 3);
        break;
    case 66:
        PORTA = 1 | ((5 + 8) << 3);
        break;
    case 67:
        PORTA = 2 | ((5 + 8) << 3);
        break;
    case 68:
        PORTA = 3 | ((5 + 8) << 3);
        break;
    case 69:
        PORTA = 4 | ((5 + 8) << 3);
        break;
    case 70:
        PORTA = 5 | ((5 + 8) << 3);
        break;
    case 71:
        PORTA = 6 | ((5 + 8) << 3);
        break;
    case 72:
        PORTA = 7 | ((5 + 8) << 3);
        break;
    // second octave, y edge 
    case 73:
        PORTA = 0 | ((4 + 8) << 3);
        break;
    case 74:
        PORTA = 1 | ((4 + 8) << 3);
        break;
    case 75:
        PORTA = 2 | ((4 + 8) << 3);
        break;
    case 76:
        PORTA = 3 | ((4 + 8) << 3);
        break;
    case 77:
        PORTA = 4 | ((4 + 8) << 3);
        break;
    case 78:
        PORTA = 5 | ((4 + 8) << 3);
        break;
    case 79:
        PORTA = 6 | ((4 + 8) << 3);
        break;
    case 80:
        PORTA = 7 | ((4 + 8) << 3);
        break;
    case 81:
        PORTA = 0 | ((3 + 8) << 3);
        break;
    case 82:
        PORTA = 1 | ((3 + 8) << 3);
        break;
    case 83:
        PORTA = 2 | ((3 + 8) << 3);
        break;
    case 84:
        PORTA = 3 | ((3 + 8) << 3);
        break;
    // third octave, y edge 
    case 85:
        PORTA = 4 | ((3 + 8) << 3);
        break;
    case 86:
        PORTA = 5 | ((3 + 8) << 3);
        break;
    case 87:
        PORTA = 6 | ((3 + 8) << 3);
        break;
    case 88:
        PORTA = 7 | ((3 + 8) << 3);
        break;
    case 89:
        PORTA = 0 | ((2 + 8) << 3);
        break;
    case 90:
        PORTA = 1 | ((2 + 8) << 3);
        break;
    case 91:
        PORTA = 2 | ((2 + 8) << 3);
        break;
    case 92:
        PORTA = 3 | ((2 + 8) << 3);
        break;
    case 93:
        PORTA = 4 | ((2 + 8) << 3);
        break;
    case 94:
        PORTA = 5 | ((2 + 8) << 3);
        break;
    case 95:
        PORTA = 6 | ((2 + 8) << 3);
        break;
    case 96:
        PORTA = 7 | ((2 + 8) << 3);
        break;
    // fourth octave, y edge 
    case 97:
        PORTA = 0 | ((1 + 8) << 3);
        break;
    case 98:
        PORTA = 1 | ((1 + 8) << 3);
        break;
    case 99:
        PORTA = 2 | ((1 + 8) << 3);
        break;
    case 100:
        PORTA = 3 | ((1 + 8) << 3);
        break;
    case 101:
        PORTA = 4 | ((1 + 8) << 3);
        break;
    case 102:
        PORTA = 5 | ((1 + 8) << 3);
        break;
    case 103:
        PORTA = 6 | ((1 + 8) << 3);
        break;
    case 104:
        PORTA = 7 | ((1 + 8) << 3);
        break;
    case 105:
        PORTA = 0 | ((0 + 8) << 3);
        break;
    case 106:
        PORTA = 1 | ((0 + 8) << 3);
        break;
    case 107:
        PORTA = 2 | ((0 + 8) << 3);
        break;
    case 108:
        PORTA = 3 | ((0 + 8) << 3);
        break;
    // fifth octave, y edge 
    case 109:
        PORTA = 4 | ((0 + 8) << 3);
        break;
    case 110:
        PORTA = 5 | ((0 + 8) << 3);
        break;
    case 111:
        PORTA = 6 | ((0 + 8) << 3);
        break;
    case 112:
        PORTA = 7 | ((0 + 8) << 3);
        break;
    case 113:
        PORTA = 0 | ((7 + 8) << 3);
        break;
    case 114:
        PORTA = 1 | ((7 + 8) << 3);
        break;
    case 115:
        PORTA = 2 | ((7 + 8) << 3);
        break;
    case 116:
        PORTA = 3 | ((7 + 8) << 3);
        break;
    case 117:
        PORTA = 4 | ((7 + 8) << 3);
        break;
    case 118:
        PORTA = 5 | ((7 + 8) << 3);
        break;
    case 119:
        PORTA = 6 | ((7 + 8) << 3);
        break;
    case 120:
        PORTA = 7 | ((7 + 8) << 3);
        break;
    // sixth octave, y edge 
    case 121:
        PORTA = 0 | ((6 + 8) << 3);
        break;
    default:
        break;
    }

    __asm__("nop\n\t"); // delay for reading
    __asm__("nop\n\t"); // delay for reading
    __asm__("nop\n\t"); // delay for reading
    __asm__("nop\n\t"); // delay for reading
    __asm__("nop\n\t"); // delay for reading
    __asm__("nop\n\t"); // delay for reading
}

// reads the output of the mux
uint8_t keyboard_read(void){
    uint8_t output = 0;

    if(PINA & 0b10000000)
        output = 1;

    return output;
}
