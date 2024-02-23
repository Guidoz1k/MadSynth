#include "voices.hpp"

// cluster of voice properties
int32_t value[CHOIRSIZE];           // value of each voice
int32_t counter[CHOIRSIZE];         // count each 1us to achieve step update
int32_t period[CHOIRSIZE];          // counter limit (-1 for no signal)
int32_t stepCounter[CHOIRSIZE];     // count each period to step the ramp up
int32_t stepSize[CHOIRSIZE][RANGE]; // remap of analog values
int32_t stepLimit[CHOIRSIZE];       // stepcounter limit

// transition variables
int32_t newVoice[CHOIRSIZE];
int32_t newPeriod[CHOIRSIZE];
int32_t newStepLimit[CHOIRSIZE];

// choir properties
int32_t voiceNumber = 0;        // total number of active voices
int32_t voiceMult = 1;          // output multiplier
int32_t choir = 0;              // output value
int32_t volSlots[CHOIRSIZE];    // bit tracks of the 12bit channel (32 MAX)

void choirInit(){
    pinMode(SYNCPIN, OUTPUT);
    digitalWrite(SYNCPIN, 1);
    // initialize period variable
    for(int32_t i = 0; i < 32; i++){
        period[i] = -1;
        newPeriod[i] = 0;
        newStepLimit[i] = 0;
        newVoice[i] = 0;
        value[i] = 0;
        counter[i] = 0;
        stepCounter[i] = 0;
        stepLimit[i] = 0;
        volSlots[i] = 0;
    }
}

// setup one voice
int32_t setVoice(int32_t frequency, int32_t slot, int32_t volume){
    // variable that signals if the operation was executed
    // (heavily influenced by newVoice status)
    int32_t async = 0;

    // verify if frequency is compatible with interruption and your ear
    if((frequency >= MINFREQ) && (frequency <= MAXFREQ) && ((voiceNumber + volume) < CHOIRSIZE) && (volume > 0)){
        // if period is -1 that slot was empty, if not there should be syncing
        if(period[slot] == -1){
            voiceNumber += volume;
            volSlots[slot] = volume;
            // scale the signal up in case of less than 32 voices on
            voiceMult = CHOIRSIZE / voiceNumber;
            // tries to achieve 7 bits of resolution
            period[slot] = REFRESH/(RANGE * frequency);
            // increase the refresh period trying to achieve a more precise signal
            period[slot]++;
            // tests if less than 128 steps would be a better approach
            stepLimit[slot] = REFRESH/(period[slot] * frequency);
            // if signal is generated with less than 7 bits, scale the ramp back to a 1/32 of output equivalent
            for(int32_t i = 0; i < stepLimit[slot]; i++)
                stepSize[slot][i] = ((i * RANGE) / stepLimit[slot]) * volume;
            // interruption uses "==" test for efficiency so it turns 1 - 10 into 0 - 9
            stepLimit[slot]--;
            period[slot]--;
            async = 1;
        }
        else
            // checks if there is no update to be executed
            if(newVoice[slot] != 1){
                voiceNumber += volume - volSlots[slot];
                volSlots[slot] = volume;
                // scale the signal up in case of less than 32 voices on
                voiceMult = CHOIRSIZE / voiceNumber;
                // tries to achieve 7 bits of resolution
                newPeriod[slot] = REFRESH/(RANGE * frequency);
                // increase the refresh period trying to achieve a more precise signal
                newPeriod[slot]++;
                // tests if less than 128 steps would be a better approach
                newStepLimit[slot] = REFRESH/(newPeriod[slot] * frequency);
                // if signal is generated with less than 7 bits, scale the ramp back to a 1/32 of output equivalent
                // (there is no newStepSize, so this may generate some weird glitches)
                for(int32_t i = 0; i < newStepLimit[slot]; i++)
                    stepSize[slot][i] = ((i * RANGE) / newStepLimit[slot]) * volume;
                // interruption uses "==" test for efficiency so it turns 1 - 10 into 0 - 9
                newStepLimit[slot]--;
                newPeriod[slot]--;
                newVoice[slot] = 1;
                async = 1;
            }
    }


    return async;
}

// clear voice slot
void nullVoice(int32_t slot){
    // checks if there is a voice in the slot
    if(period[slot] != -1){
        // makes the period in that slot disabled
        period[slot] = -1;
        // clears everything
        value[slot] = 0;
        counter[slot] = 0;
        stepCounter[slot] = 0;
        stepLimit[slot] = 0;
        for(int32_t i = 0; i < RANGE; i++)
            stepSize[slot][i] = 0;
        // signals the vacant slot
        voiceNumber -= volSlots[slot];
        volSlots[slot] = 0;
    }
}
