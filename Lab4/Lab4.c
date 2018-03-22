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
#include <unistd.h>
#include <semaphore.h>

//Definitions
#define PBUTTON 16

//Structures
struct Threadargs{
	int *pbuffer;
	int *newDataFlag;
	struct timespec *gpstime;
}typedef threadargs;

struct Threadargs2{
	int oldGPSdata, newGPSdata;
	struct timespec oldGPStime, newGPStime, buttontime;
	struct timespec *timeptr;
	int *dataflag, *data;
}typedef threadargs2;

//Function prototypes
void Process2main();
void* thread0function(void* args);
void* childthreadfunction(void* args);

//Global variables
sem_t my_semaphore;


int main(int argc, char** argv){
	
	//Variables
	int dummy, buffer, df;
	int N_pipe1;
	time_t ctime;	
	threadargs *args0;
	pthread_t thread0;
	struct timespec time;

	//Open a pipe to the GPS data program
	if ((N_pipe1 = open("/tmp/N_pipe1", O_RDONLY)) < 0){
                printf("Error creating N_pipe1! Exiting program\n");
                exit(-1);
        }
	
	//Setup the hardware
	wiringPiSetupGpio();
	pinMode(PBUTTON, INPUT);
	
	//Start the second process
	dummy = fork();

	if (dummy == 0){
		Process2main();
	}
	
	//Setup and start thread0 for process 1
	args0 = malloc(sizeof(threadargs));
	args0->pbuffer = &buffer;
	args0->newDataFlag = &df;
	args0->gpstime = &time;
	pthread_create(&thread0, NULL, thread0function, (void*)args0);
	
	//Recieve data
	while(1){
		if (read(N_pipe1, &buffer, sizeof(int)) < 0){
			printf("N_pipe1 read data error!\n");
			exit(-1);
		} else {
			df = 1;
			clock_gettime(CLOCK_MONOTONIC, &time);
		}
		 
	}
	

	//Free allocated data
	free(args0);
	
	return 0;
}


void Process2main(){
	struct sched_param param;
	int timer = timerfd_create(CLOCK_MONOTONIC, 0);
	int N_pipe2, dummy;
	struct itimerspec timer_value;
	struct timespec currenttime;
	uint64_t periods = 0;

	//Make process realtime
	param.sched_priority = 55;
        sched_setscheduler(0, SCHED_FIFO, &param);

	//Set timer periods
	timer_value.it_interval.tv_sec = 0;
	timer_value.it_interval.tv_nsec = 75000000;
	timer_value.it_value.tv_sec = 0;
	timer_value.it_value.tv_nsec = 1000;

	//Create a pipe for the button event timestamps
        dummy = system("mkfifo N_pipe2");
        N_pipe2 = open("N_pipe2", O_WRONLY);

	//Start timer
	timerfd_settime(timer, 0, &timer_value, NULL);
	read(timer, &periods, sizeof(periods));	

	while (1){
		if (check_button() == 1){
			clock_gettime(CLOCK_MONOTONIC, &currenttime);
			write(N_pipe2, &currenttime, sizeof(currenttime));
			clear_button();	
		}
		
		read(timer, &periods, sizeof(periods));
		usleep(100);
	}

}


void* thread0function(void* args){
	int *df, *data, N_pipe2, readamount, i = 0;
	struct timespec oldtime;	
	pthread_t childthread;
	threadargs2 childargs;
	
	df = ((threadargs*)args)->newDataFlag;
	data = ((threadargs*)args)->pbuffer;

	usleep(100000);
	clear_button();

	if ((N_pipe2 = open("N_pipe2", O_RDONLY)) < 0){
		printf("Error opening N_pipe2!\n");
		sleep(2);
	}

	while (1){

		//If a button event happens
		readamount = read(N_pipe2, &oldtime, sizeof(oldtime));
		if (readamount < 0){
			printf("N_pipe2 read error!\n");
		} else if (readamount > 0){
			*df = 0;
			childargs.oldGPSdata = *data;
			childargs.oldGPStime = *(((threadargs*)args)->gpstime);
			childargs.buttontime = oldtime;
			childargs.timeptr = (((threadargs*)args)->gpstime);
			childargs.data = data;
			childargs.dataflag = df;
			printf("\nButton Pressed! Creating thread...\n");
			if (pthread_create(&childthread, NULL, childthreadfunction, (void*)&childargs) != 0){
				printf("Error creating child thread!\n");
			}
		}
	}

}

void* childthreadfunction(void* args){
	
	int olddata = ((threadargs2*)args)->oldGPSdata, newdata;
	struct timespec oldGPStime = ((threadargs2*)args)->oldGPStime, buttontime = ((threadargs2*)args)->buttontime, newGPStime;
	int *df = ((threadargs2*)args)->dataflag, *d = ((threadargs2*)args)->data, tempi;
	uint64_t old, new;
	long on;
	double slope, intercept, tempd, tempd2, interpolateddata;	

	while (*df == 0){}	//Wait until a new GPS datapoint comes in

	//Get the new data
	newdata = *d;
	newGPStime = *(((threadargs2*)args)->timeptr);

	if (newdata == olddata){
		printf("Data error!\n");
	}

	if (newGPStime.tv_nsec == oldGPStime.tv_nsec){
		printf("Time error!\n");
	}

	//Figure out linear equation
	tempi = olddata - newdata;
	new = ((uint64_t)newGPStime.tv_sec) * 1000000000 + (uint64_t)newGPStime.tv_nsec;
	old = ((uint64_t)oldGPStime.tv_sec) * 1000000000 + (uint64_t)oldGPStime.tv_nsec;
	printf("New Sec -> %d, New NSec -> %ld, New Sec * 1b - %ld\n", newGPStime.tv_sec, newGPStime.tv_nsec, ((long)(newGPStime.tv_sec)) * 1000000000);
	printf("Old Sec -> %d, Old NSec -> %ld\n", oldGPStime.tv_sec, oldGPStime.tv_nsec);
	printf("New - %ld Old - %ld\n", new, old);
	


	on = (long)(on - new);
	slope = (double)tempi/(double)on;

	new = ((uint64_t)oldGPStime.tv_sec) * 1000000000 + (uint64_t)oldGPStime.tv_nsec;
	intercept = (double)olddata - (slope * new);
	new = ((uint64_t)buttontime.tv_sec) * 1000000000 + (uint64_t)buttontime.tv_nsec;

	interpolateddata = (slope * new) + intercept;

	printf("Prev = %d, Next = %d, Data = %lf\n", olddata, newdata, interpolateddata);

	pthread_exit(0);
}


























