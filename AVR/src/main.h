#ifndef __MAIN_H
#define __MAIN_H

#include <Arduino.h>
#include "lcd.h"
#include "onboard.h"
#include "timer.h"
#include "serial.h"
#include "keyboard.h"
#include "ui.h"
#include "anal.h"

#define MAXKEYS     122 // maximum number of keys in the keybed
#define MENUSIZE    25  // maximum number of character in a menu title
#define INTERFACE   10  // interface delay in ms
#define BUTTONHOLD  40  // 40 * INTERFACE = 400ms of delay between valid button presses


#endif /* __MAIN_H */