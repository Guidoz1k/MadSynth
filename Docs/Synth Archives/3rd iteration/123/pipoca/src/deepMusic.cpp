#include "deepMusic.hpp"

int32_t deepNotes[11];

int32_t deepVols[11];

void musicInit(){
    deepNotes[0] = D1;
    deepNotes[1] = D2;
    deepNotes[2] = A2;
    deepNotes[3] = D3;
    deepNotes[4] = A3;
    deepNotes[5] = D4;
    deepNotes[6] = A4;
    deepNotes[7] = D5;
    deepNotes[8] = A5;
    deepNotes[9] = D6;
    deepNotes[10] = F6;
    deepVols[0] = 2;
    deepVols[1] = 5;
    deepVols[2] = 5;
    deepVols[3] = 4;
    deepVols[4] = 4;
    deepVols[5] = 3;
    deepVols[6] = 3;
    deepVols[7] = 2;
    deepVols[8] = 2;
    deepVols[9] = 1;
    deepVols[10] = 1;
}