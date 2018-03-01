#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include "ece4220lab3.h"
#include <wiringPi.h>
#include <sched.h>
#include <stdint.h>
#include <sys/timerfd.h>
#include <fcntl.h>


//Definitions
#define RED 2
#define YELLOW 3
#define GREEN 4
#define PBUTTON 16


int main(int argc, char** argv){

	int period;
	int cycle = 0;

	wiringPiSetupGpio();

	pinMode(RED, OUTPUT);
	pinMode(YELLOW, OUTPUT);
	pinMode(GREEN, OUTPUT);
	pinMode(PBUTTON, INPUT);

	if (argc != 2){
		printf("\nPlease enter a time period: ");
		scanf("%d", &period);
	} else {
		period = atoi(*(argv + 1));
	}
	
	while (1){
		if (check_button() == 1){
			digitalWrite(RED, 1);
			sleep(period);
			digitalWrite(RED, 0);
			clear_button();	
		}

		if (cycle == 0){
			digitalWrite(GREEN, 1);
			sleep(period);
			digitalWrite(GREEN, 0);
			cycle = 1;
		} else {
			digitalWrite(YELLOW, 1);
			sleep(period);
			digitalWrite(YELLOW, 0);
			cycle = 0;
		}
	}
		
	return 0;
}



