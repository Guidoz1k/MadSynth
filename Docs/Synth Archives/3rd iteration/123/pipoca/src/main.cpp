/* DEEP NOTE SYNTH MK I
    ESP32 Dev Kit DOIT
    todas funções executam no core 1 por padrão
    função de varredura executa no core 0
*/

#include <Arduino.h>
#include "hardSPI.hpp"
#include "deepMusic.hpp"
#include "voices.hpp"
#include "button.hpp"
#include "dac.hpp"

#define GSTEPS  64
#define MAXVAR  10

// interruption variable
hw_timer_t *timer = NULL;

// adjustable volume by number of voices
int32_t adjVol = 0;

int32_t teste = 0;

// glissando variables
int32_t frequencies[CHOIRSIZE];
int32_t glissandoInc[11][GSTEPS];

void serialDebug(int32_t number){
    int32_t var[4];
    int32_t aux = 1;

    for(int32_t i = 3; i >= 0; i--){
        aux = 1;
        for(int32_t j = 0; j < i; j++)
            aux *= 10;
        var[3 - i] = (number / aux) % 10;
    }
    for(int32_t i = 0; i <= 3; i++)
        Serial.write(var[i] + 48);
}

int32_t rand(){
    int32_t result = 0;

    result = (esp_random() % MAXVAR) - (MAXVAR / 2);

    return result;
}

void varFreq(int32_t slot){
    frequencies[slot] += rand();
    setVoice(frequencies[slot], slot, 5);
}

void task0(void *pvParameters){
    int32_t aux = 0;

    if(teste == 1){
        volumeSet(10);
        for(aux = 0; aux < 11; aux++){
            setVoice(deepNotes[aux], aux, deepVols[aux]);
        }
        while(1)
            delay(10);
    }
    while(1){
        // waits for command to start cacophony
        while(unbounceButton() == 0)
            delay(3);
        for(aux = 0; aux < 6; aux++){
            frequencies[aux] = 100 + 25 * aux;
            setVoice(frequencies[aux], aux, 5);
            delay(100);
        }
        for(aux = 0; aux <= (MAXVOL / 2); aux++){
            volumeSet(aux);
            delay(250);
        }

        // random variations in frequency untill button is pressed
        while(unbounceButton() == 0){
            for(aux = 0; aux < 6; aux++)
                varFreq(aux);
            delay(100);
        }

        // random low voices go to silence
        for(aux = MAXVOL / 2; aux >= 0; aux--){
            volumeSet(aux);
            delay(100);
        }
        for(aux = 0; aux < CHOIRSIZE; aux++)
            nullVoice(aux);

        // crescendo and glissando
        volumeSet(0);
        for(aux = 0; aux < 11; aux++){
            frequencies[aux] = 100 + (10 * aux);
            Serial.write("AUX = ");
            Serial.write(aux + 48);
            Serial.write("\n glissandos: ");
            //setVoice(frequencies[aux], aux, deepVols[aux]);
            for(int32_t i = 0; i < GSTEPS; i++){
                glissandoInc[aux][i] = frequencies[aux] + (i * (deepNotes[aux] - frequencies[aux])) / GSTEPS;
                serialDebug(glissandoInc[aux][i]);
                Serial.write(", ");
            }
            Serial.write("\n");
        }
        for(int32_t i = 0; i < GSTEPS; i++){
            for(aux = 2; aux < 11; aux++){
                delay(10);
                setVoice(glissandoInc[aux][i], aux, deepVols[aux]);
            }
            volumeSet((i / 2) - 1);
        }

        // finishes Deep Note
        while(unbounceButton() == 0)
            delay(3);
        for(aux = MAXVOL; aux >= 0; aux--){
            volumeSet(aux);
            delay(100);
        }
        for(aux = 10; aux >= 0; aux--){
            nullVoice(aux);
            delay(100);
        }
    }
}

void task0Init(){
    int32_t aux = 0;

    for(aux = 0; aux < CHOIRSIZE; aux++){
        frequencies[aux] = 0;
    }
    for(aux = 0; aux < 11; aux++)
        for(int32_t i; i < GSTEPS; i++)
            glissandoInc[aux][i] = 0;
    xTaskCreatePinnedToCore(task0, "task0", 10000, NULL, 2, NULL, 0);
}

// 32 voices choir interruption (SHOULD EXECUTE IN LESS THAN 10us)
void IRAM_ATTR onTimer(){
    // makes it easy to measure interruption time
    digitalWrite(SYNCPIN, 0);
/*
    // cleans the output variable
    choir = 0;
    // process and sums all of the 32 voices
    for(int32_t i = 0; i < CHOIRSIZE; i++){
        if(period[i] != -1){
            // if counter hits the period its time to rise one step in the ramp
            if(counter[i] == period[i]){
                counter[i] = 0;
                // reaches last steps and resets ramp
                // if there was an update on the frequency, now its the time to update the variables
                if(stepCounter[i] == stepLimit[i]){
                    stepCounter[i] = 0;
                    if(newVoice[i] == 1){
                        period[i] = newPeriod[i];
                        stepLimit[i] = newStepLimit[i];
                        newVoice[i] = 0;
                    }
                }
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
    }
    // reajust choir amplitude in case of least than 32 voices
    if(adjVol == 1)
        choir *= voiceMult;
*/
    // transfer total value to external SPI DAC
    hardSPI(choir);

    // makes it easy to measure interruption time
    digitalWrite(SYNCPIN, 1);
}

void timerInit(){
    timer = timerBegin(0, 80, true);
    timerAttachInterrupt(timer, &onTimer, true);
    timerAlarmWrite(timer, 10, true);
    timerAlarmEnable(timer);
}

void setup() {
    musicInit();
    dacInit();
    buttonInit();
    choirInit();
    SPIInit();
    timerInit();
    task0Init();

    delay(1000);
    Serial.begin(115200);
    Serial.write("Serial ON\n");
}

void loop() {
}
