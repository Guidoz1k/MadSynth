#ifndef __MAIN_H
#define __MAIN_H

#include <Arduino.h>
#include "lcd.h"
#include "onboard.h"
#include "timer.h"
#include "serial0.h"
#include "serial1.h"
#include "keyboard.h"
#include "ui.h"
#include "anal.h"

#define MAXKEYS     122 // maximum number of keys in the keybed
#define DIGIT_POS   15  // position for numerical inputs
#define MAXVOICES   61  // maximum number of audio channels
#define MAXSVOICES  15  // maximum number of audio channels per oscillator
#define MAXTRANS    48  // maximum number of notes a osc can be transposed
#define MAXADSR     250 // maximum amount of ms a parameter of ADSR can have

#endif /* __MAIN_H */