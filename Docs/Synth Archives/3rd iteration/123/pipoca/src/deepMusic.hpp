#include <Arduino.h>

#ifndef DEEPMUSIC_H
#define DEEPMUSIC_H

#define RANDMIN     100
#define RANDMAX     200
#define D1          37
#define D2          73
#define A2          110
#define D3          147
#define A3          220
#define D4          294
#define A4          440
#define D5          587
#define A5          880
#define D6          1175
#define F6          1397

/*
    originally the first 3 voices were double and the rest was tripled
*/
extern int32_t deepNotes[11];

extern int32_t deepVols[11];

void musicInit();

/*
// sync all voices
void syncVoices(void){
    timerAlarmDisable(timer);
    for(int32_t i = 0; i < CHOIRSIZE; i++){
        counter[i] = 0;
        stepCounter[i] = 0;
    }
    timerAlarmEnable(timer);
    timerRestart(timer);
}
*/

#endif