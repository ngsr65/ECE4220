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
#define PBUTTON 16

//Structures
struct Threadargs{
	int *pbuffer;
	time_t *t;
}typedef threadargs;


//Function prototypes


//Global variables
sem_t my_semaphore;


int main(int argc, char** argv){
	
	//Variables
	int dummy, buffer;
	int N_pipe1, N_pipe2;
	time_t ctime;	
	threadargs *args1;

	//Set thread arguments
	args1->pbuffer = &buffer;
	args1->t = &ctime;	

	if ((N_pipe1 = open("/tmp/N_pipe1", O_RDONLY)) < 0){
                printf("Error creating N_pipe1! Exiting program\n");
                exit(-1);
        }
	
	
	//Setup the hardware
	wiringPiSetupGpio();
	pinMode(PBUTTON, INPUT);
	
	
	
	//Recieve data
	while(1){
		if (read(N_pipe1, &buffer, sizeof(int)) < 0){
			printf("N_pipe1 read data error!\n");
			exit(-1);
		} else {
			printf("%d\n", buffer);
		}
		 
	}
	
	
	return 0;
}


































