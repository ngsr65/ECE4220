#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sched.h>
#include <stdint.h>
#include <sys/timerfd.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>

#define Priority 51

#define THREAD1INITIALSECONDS 0
#define THREAD1INITIALNANOSECONDS 50000
#define THREAD1PERIODSECONDS 0
#define THREAD1PERIODNANOSECONDS 50000

#define THREAD2INITIALSECONDS 0
#define THREAD2INITIALNANOSECONDS 100000
#define THREAD2PERIODSECONDS 0
#define THREAD2PERIODNANOSECONDS 50000

#define THREAD3INITIALSECONDS 0
#define THREAD3INITIALNANOSECONDS 75000
#define THREAD3PERIODSECONDS 0
#define THREAD3PERIODNANOSECONDS 25000

//Structures
struct Threadargs{
	FILE *file;
	char *buffer;
	char **song;
	time_t is, ps;
	long in, pn;
}typedef threadargs;

//Function prototypes
void* songfixer(void *args);
void* songmixer(void *args);

int main(){
	
	FILE *pFile1, *pFile2;
	char tempbuffer[50] = {'\0'};
	char **Song;
	int i;
	struct itimerspec timerval;
	threadargs *args1, *args2, *args3;
	pthread_t thread1, thread2, thread3;

	//Allocate space for the song
	Song = malloc(20 * sizeof(char*));
	for (i = 0; i < 20; i++){
		*(Song + i) = malloc(50 * sizeof(char));
	}

	//Open the files
	pFile1 = fopen("first.txt", "r");
	pFile2 = fopen("second.txt", "r");
	
	//Set the thread arguments
	args1->file = pFile1;
	args2->file = pFile2;
	args1->is = THREAD1INITIALSECONDS;
	args2->is = THREAD2INITIALSECONDS;
	args3->is = THREAD3INITIALSECONDS;
	args1->in = THREAD1INITIALNANOSECONDS;
	args2->in = THREAD2INITIALNANOSECONDS;
	args3->in = THREAD3INITIALNANOSECONDS;
	args1->ps = THREAD1PERIODSECONDS;
	args2->ps = THREAD2PERIODSECONDS;
	args3->ps = THREAD3PERIODSECONDS;
	args1->pn = THREAD1PERIODNANOSECONDS;
	args2->pn = THREAD2PERIODNANOSECONDS;
	args3->pn = THREAD3PERIODNANOSECONDS;
	args1->buffer = tempbuffer;
	args2->buffer = tempbuffer;
	args3->song = Song; 
	
	//Create the threads
	pthread_create(&thread1, NULL, songfixer, (void*)args1);
	pthread_create(&thread2, NULL, songfixer, (void*)args2);
	pthread_create(&thread3, NULL, songmixer, (void*)args3);
		
	//Wait for threads to finish
	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);
	pthread_join(thread3, NULL);		
	
	//Print out the song
	for (i = 0; i < 20; i++){
		printf("%s\n", *(Song+i));
	}	

	//Close the files
	fclose(pFile1);
	fclose(pFile2);

	//Free the allocated memory
	for (i = 0; i < 20; i++){
		free(*(Song + i));
	}
	free(Song);

	return 0;
}


void* songfixer(void *args){
	int timer = timerfd_create(CLOCK_MONOTONIC, 0);
	struct itimerspec timer_value;
	struct sched_param param;	
	uint64_t periods = 0;	

	//Set the timer period and intervals
	timer_value.it_interval.tv_sec = ((threadargs*)args)->ps;
	timer_value.it_interval.tv_nsec = ((threadargs*)args)->pn;
	timer_value.it_value.tv_sec = ((threadargs*)args)->is;
	timer_value.it_value.tv_nsec = ((threadargs*)args)->in;

	//Start the timer
	timerfd_settime(timer, 0, &timer_value, NULL);

	//Set priority
	param.sched_priority = Priority;	
	
	//Set the schedular to First in First out
	sched_setscheduler(0, SCHED_FIFO, &param);


	
		//Read the period variable
		read(timer, &periods, sizeof(periods));


	pthread_exit(0);
	
}

void* songmixer(void *args){
	int i, j, timer = timerfd_create(CLOCK_MONOTONIC, 0);
	char temp;
        struct itimerspec timer_value;
        struct sched_param param;
        uint64_t periods = 0;

        //Set the timer period and intervals
        timer_value.it_interval.tv_sec = ((threadargs*)args)->ps;
        timer_value.it_interval.tv_nsec = ((threadargs*)args)->pn;
        timer_value.it_value.tv_sec = ((threadargs*)args)->is;
        timer_value.it_value.tv_nsec = ((threadargs*)args)->in;

        //Start the timer
        timerfd_settime(timer, 0, &timer_value, NULL);

        //Set priority
        param.sched_priority = Priority;

        //Set the schedular to First in First out
        sched_setscheduler(0, SCHED_FIFO, &param);


	for (i = 0; i < 20; i++){
 	        //Read the period variable
      		read(timer, &periods, sizeof(periods));

		for (j = 0; j < 50; j++){
			*(*(((threadargs*)args)->song + i) + j) = *(((threadargs*)args)->buffer + j);
		}

	}
	
	pthread_exit(0);
}





