#include <Arduino.h>

#ifndef HARDSPI_H
#define HARDSPI_H

// SPI definitions
//#define SPICLK      15000000
#define SLAVEP      27
#define MDOPIN      13
#define CLKPIN      14

// SPI variables
//SPIClass SPI1(HSPI);
//SPISettings spiSets = SPISettings(SPICLK, MSBFIRST, SPI_MODE0);

/* pins for parallel comm
    #define DACCLOCK    23
    #define DACPIN11    22
    #define DACPIN10    1
    #define DACPIN09    3
    #define DACPIN08    21
    #define DACPIN07    19
    #define DACPIN06    18
    #define DACPIN05    5
    #define DACPIN04    17
    #define DACPIN03    16
    #define DACPIN02    4
    #define DACPIN01    2
    #define DACPIN00    15
*/

void SPIInit();

void hardSPI(int32_t datum);

#endif
