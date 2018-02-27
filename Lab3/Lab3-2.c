#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <ece4220lab3.h>
#include <wiringPi.h>
#include <sched.h>
#include <stdint.h>
#include <sys/timerfd.h>
#include <fcntl.h>


//Definitions
#define REDLED 2
#define YELLOWLED 3
#define GREENLED 4
#define PBUTTON 16

#define PRIORITY 51

//Function prototypes
void* runLights(void* args);

//Thread argument struct
struct Threadargs{
	int ps, pn, is, in;
}typedef threadargs;



int main(int argc, char** argv){

	int period;



	if (argc != 2){
		printf("\nPlease enter a time period: ");
		scanf("%d", &period);
	} else {
		period = atoi(*(argv + 1));
	}
	
	
	
	
		
	return 0;
}


void* runLights(void* args){
	
	int timerfd_create(CLOCK_MONOTONIC, 0);
	struct itimerspex timer_value;
	struct sched_param param;
	uint64_t periods = 0;

	//Set all of the timer values
	timer_value.it_interval.tv_sec = ((threadargs*)args)->ps;
        timer_value.it_interval.tv_nsec = ((threadargs*)args)->pn;
        timer_value.it_value.tv_sec = ((threadargs*)args)->is;
        timer_value.it_value.tv_nsec = ((threadargs*)args)->in;

	//Start the timer
	timerfd_settime(timer, 0, &timer_value, NULL);
		
		
	
	pthread_exit(0);
}

void* pedLight(void* args){


	pthread_exit(0);
}






