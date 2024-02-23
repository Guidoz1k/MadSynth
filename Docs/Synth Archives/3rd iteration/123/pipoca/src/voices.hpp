#include <Arduino.h>

#ifndef VOICES_H
#define VOICES_H

// software definitions
#define CHOIRSIZE   32      // max voices
#define RANGE       128     // 7bit range of each voice (12bits / 32)
#define MINFREQ     20
#define MAXFREQ     2000
#define REFRESH     100000  // frequency with 10us interruption
#define SYNCPIN     26

// cluster of voice properties
extern int32_t value[CHOIRSIZE];           // value of each voice
extern int32_t counter[CHOIRSIZE];         // count each 1us to achieve step update
extern int32_t period[CHOIRSIZE];          // counter limit (-1 for no signal)
extern int32_t stepCounter[CHOIRSIZE];     // count each period to step the ramp up
extern int32_t stepSize[CHOIRSIZE][RANGE]; // remap of analog values
extern int32_t stepLimit[CHOIRSIZE];       // stepcounter limit

// transition variables
extern int32_t newVoice[CHOIRSIZE];
extern int32_t newPeriod[CHOIRSIZE];
extern int32_t newStepLimit[CHOIRSIZE];

// choir properties
extern int32_t voiceNumber;             // total number of active voices
extern int32_t voiceMult;               // output multiplier
extern int32_t choir;                   // output value
extern int32_t volSlots[CHOIRSIZE];     // bit tracks of the 12bit channel (32 MAX)

void choirInit();

int32_t setVoice(int32_t frequency, int32_t slot, int32_t volSlots);

void nullVoice(int32_t slot);

#endif
