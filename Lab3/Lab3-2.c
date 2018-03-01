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
#include <semaphore.h>

//Definitions
#define REDLED 2
#define YELLOWLED 3
#define GREENLED 4
#define PBUTTON 16

//Function prototypes
void* runLights(void* args);

//Thread argument struct
struct Threadargs{
	int tp, light_ID, plight_ID, pri;
}typedef threadargs;

//Global variables
sem_t my_semaphore;


int main(int argc, char** argv){

	int period, p1, p2, p3;
	threadargs *args1, *args2, *args3;
	pthread_t thread1, thread2, thread3;

	//Get time period
	if (argc != 5){
		printf("\nError - sudo ./a.out [Time] [PPL] [PTL1] [PTL2]\n");
		return 0;
	} else {
		period = atoi(*(argv + 1));
		p1 = atoi(*(argv + 2));
		p2 = atoi(*(argv + 3));
		p3 = atoi(*(argv + 4));
	}

	//Setup hardware
	wiringPiSetupGpio();
	pinMode(REDLED, OUTPUT);
	pinMode(YELLOWLED, OUTPUT);
        pinMode(GREENLED, OUTPUT);
        pinMode(PBUTTON, INPUT);
	
	//Allocated space for the thread arguments
	args1 = malloc(sizeof(threadargs));
	args2 = malloc(sizeof(threadargs));
	args3 = malloc(sizeof(threadargs));

	//Set thread arguments
	args1->tp = period;
	args2->tp = period;
	args3->tp = period;
	args1->light_ID = REDLED;
	args2->light_ID = YELLOWLED;
	args3->light_ID = GREENLED;
	args1->plight_ID = REDLED;
        args2->plight_ID = REDLED;
        args3->plight_ID = REDLED;
	args1->pri = p1;
        args2->pri = p2;
        args3->pri = p3;
		
	//Start the semaphore
	sem_init(&my_semaphore, 0, 1);	
	
	
	//Start the threads
	pthread_create(&thread1, NULL, runLights, (void*)args1);
	pthread_create(&thread2, NULL, runLights, (void*)args2);
	pthread_create(&thread3, NULL, runLights, (void*)args3);
	
	
	
	//Wait for the threads to finish
	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);
	pthread_join(thread3, NULL);
	
	//Free space reserved for thread arguments
	free(args1);
	free(args2);
	free(args3);
		
	return 0;
}


void* runLights(void* args){

	int light = ((threadargs*)args)->light_ID, period = ((threadargs*)args)->tp;
	int plight = ((threadargs*)args)->plight_ID, priority = ((threadargs*)args)->pri;
	struct sched_param param;
	param.sched_priority = priority;
	sched_setscheduler(0, SCHED_FIFO, &param);

	while (1){
		//If it is the pedestrian light	
		if (light == 2){
			sem_wait(&my_semaphore);
			if (check_button() == 1){
				digitalWrite(light, 1);
				sleep(period);
				digitalWrite(light, 0);
				clear_button();
			}
			sem_post(&my_semaphore);
			usleep(1000);
		} else {
			sem_wait(&my_semaphore);
			digitalWrite(light, 1);
			sleep(period);		
			digitalWrite(light, 0);
			sem_post(&my_semaphore);
			usleep(1000);
		}				
	}
	
	pthread_exit(0);
}


