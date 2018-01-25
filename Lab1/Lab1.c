#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>

#define RED 2
#define YELLOW 3

int main(){

	wiringPiSetupGpio();

	pinMode(RED, OUTPUT);	   //Red LED
	pinMode(YELLOW, OUTPUT);   //Yellow LED

	while (1){
		digitalWrite(RED,1); 	 //Pin, Value
		digitalWrite(YELLOW,0);
		sleep(1);	  	 //Pause the program for one second
		digitalWrite(RED,0);
		digitalWrite(YELLOW,1);
		sleep(1);
	}

	return 0;
}
