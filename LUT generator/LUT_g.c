#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <conio.h>
#include <math.h>

#define MAX		65535
#define SIZE	2000
#define PI		3.14159265

int main(){
	int32_t counter = 0;
	int32_t integer = 0;
	float temp_number = 0;

	int32_t option = 2; // 0 = sine, 1 = square, 2 = sawtooth, 3 = triangle, 4 = inverted sawtooth

	for(counter = 0; counter < SIZE; counter++){
		switch(option){
			case 0:
				temp_number = sin( ((2*PI)/SIZE) * counter );
				integer = (MAX / 2) * temp_number;
				break;
			case 1:
				if(counter < 1000)
					integer = MAX / 2;
				else
					integer = - MAX / 2;
				break;
			case 2:
				integer = counter * (float)((float)(MAX - 1) / (float)(SIZE - 1));
				if(counter >= 1000)
					integer -= MAX;
				break;
			case 3:
				if(counter < 500){
					temp_number = counter * (float)((float)(MAX / 2) / (float)(SIZE / 4));
					integer = temp_number;
				}
				if((counter >= 500) && (counter < 1500)){
					temp_number = (counter - 500) * (float)((float)( MAX / 2) / (float)(SIZE / 4));
					integer = (MAX / 2) - temp_number;
				}
				if(counter >= 1500){
					temp_number = (counter - 1500) * (float)((float)(MAX / 2) / (float)(SIZE / 4));
					integer = - (MAX / 2) + temp_number;
				}
				break;
			case 4:
				integer = - counter * (float)((float)(MAX - 1) / (float)(SIZE - 1));
				if(counter >= 1000)
					integer += MAX;
			default:
				break;
		}
		if((counter % 20) == 0)
			printf("\n");
		printf("%6d, ", integer);
	}
	getch();

}
