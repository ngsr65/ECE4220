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

//Function prototypes


//Global variables
sem_t my_semaphore;


int main(int argc, char** argv){
	
	//Variables
	int dummy, data[8], i = 0;
	int N_pipe1, N_pipe2;	
	
	dummy = system("mkfifo N_pipe1");
	dummy = system("mkfifo N_pipe2");
	
	if ((N_pipe1 = open("N_pipe1", O_RDONLY)) < 0){
		printf("Error creating N_pipe1! Exiting program\n");
		exit(-1);
	}

	if ((N_pipe2 = open("N_pipe2", O_RDONLY)) < 0){
                printf("Error creating N_pipe2! Exiting program\n");
                exit(-1);
        }

	
	//Setup the hardware
	wiringPiSetupGpio();
	pinMode(PBUTTON, INPUT);
	
	

	//Recieve data
	while(1){
		if (read(N_pipe1, data + i, sizeof(int)) < 0){
			printf("N_pipe1 read data error!\n");
			exit(-1);
		}
	}
	
	
	return 0;
}







