#include <Arduino.h>

// pin numbers
#define VOICEINT    24  // pin used to signal interruption
#define LED         13  // onboard led pin

// software definitions
#define CHOIRSIZE   32      // max voices
#define RANGE       128     // 7bit range of each voice (12bits / 32)
#define MINFREQ     20
#define MAXFREQ     20000
#define REFRESH     100000  // frequency with 10us interruption

// 32 voice interruption
IntervalTimer timer32v;

// cluster of voice properties
int32_t value[CHOIRSIZE];           // value of each voice
int32_t counter[CHOIRSIZE];         // count each 1us to achieve step update
int32_t period[CHOIRSIZE];          // counter limit
int32_t stepCounter[CHOIRSIZE];     // count each period to step the ramp up
int32_t stepSize[CHOIRSIZE][RANGE]; // remap of analog values
int32_t stepLimit[CHOIRSIZE];       // stepcounter limit

// choir properties
int32_t voiceNumber = 0;    // total number of active voices
int32_t voiceMult = 1;      // output multiplier
int32_t choir = 0;          // output value

// input variables
int32_t slider = 0;     // soft pot input
int32_t slider0 = 0;    // past value of slider
int32_t s_error = 0;    // difference between slider and slider0

// setup one voice
void setVoice(int32_t frequency, int32_t slot){
    // verify if frequency is compatible with interruption and your ear
    if((frequency >= MINFREQ) && (frequency <= MAXFREQ)){
        // if period is -1 that slot was empty
        if(period[slot] == -1)
            voiceNumber++;
        // tries to achieve 7 bits of resolution
        period[slot] = REFRESH/(RANGE * frequency);
        // increase the refresh period trying to achieve a more precise signal
        period[slot]++;
        // tests if less than 128 steps would be a better approach
        stepLimit[slot] = REFRESH/(period[slot] * frequency);
        // if signal is generated with less than 7 bits, scale the ramp back to a 1/32 of output equivalent
        for(int32_t i = 0; i < stepLimit[slot]; i++)
            stepSize[slot][i] = (i * RANGE) / stepLimit[slot];
        // resets the signal in case of occupied slot
        counter[slot] = 0;
        stepCounter[slot] = 0;
        // interruption uses "==" test for efficiency so it turns 1 - 10 into 0 - 9
        stepLimit[slot]--;
        period[slot]--;
        // scale the signal up in case of less than 32 voices on
        voiceMult = CHOIRSIZE / voiceNumber;
    }
}

// clear voice slot
void nullVoice(int32_t slot){
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
    voiceNumber--;
}

// 32 voices choir interruption (SHOULD EXECUTE IN LESS THAN 10us)
void voicesInterruption(){
    // makes it easy to measure interruption time
    digitalWriteFast(VOICEINT, 1);

    // cleans the output variable
    choir = 0;
    // process and sums all of the 32 voices
    for(int32_t i = 0; i < CHOIRSIZE; i++){
        // if counter hits the period its time to rise one step in the ramp
        if(counter[i] == period[i]){
            counter[i] = 0;
            // reaches last steps and resets ramp
            if(stepCounter[i] == stepLimit[i])
                stepCounter[i] = 0;
            else
                stepCounter[i]++;
            // updates voice value
            value[i] = stepSize[i][stepCounter[i]];
        }
        else{
            counter[i]++;
        }
        // updates output variable
        choir += value[i];
    }
    // reajust choir amplitude in case of least than 32 voices
    choir *= voiceMult;
    analogWriteDAC1(choir);

    // makes it easy to measure interruption time
    digitalWriteFast(VOICEINT, 0);
}

// sync all voices
void syncVoices(void){
    timer32v.end();
    for(int32_t i = 0; i < CHOIRSIZE; i++){
        counter[i] = 0;
        stepCounter[i] = 0;
    }
    timer32v.begin(voicesInterruption, 10);
}

void setup(){
    // setup pins
    pinMode(VOICEINT, OUTPUT);
    pinMode(LED, OUTPUT);

    // initialize period variable
    for(int32_t i = 0; i < 32; i++)
        period[i] = -1;

/*
    for(int32_t i = 0; i < 32; i++)
        setVoice(20 + 100 * i, i);
    syncVoices();*/

    // define 
    analogReadResolution(13);

    // setup interruption in 10us
    timer32v.begin(voicesInterruption, 10);
    timer32v.priority(0);
}

void loop(){
    slider = 0;
    for(int32_t i = 0; i < 1000; i++)
        slider += analogRead(A0);
    slider /= 999;
    s_error = slider - slider0;
    if(s_error < 0)
        s_error *= -1;
    if(s_error > 15){
        setVoice(slider + 20, 0);
        slider0 = slider;
    }

/*
    digitalWriteFast(LED, 1);
    delay(100);
    digitalWriteFast(LED, 0);
    delay(100);*/
}
