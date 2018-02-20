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

#define THREAD1INITIALSECONDS 1
#define THREAD1INITIALNANOSECONDS 100000
#define THREAD1PERIODSECONDS 0
#define THREAD1PERIODNANOSECONDS 200000

#define THREAD2INITIALSECONDS 1
#define THREAD2INITIALNANOSECONDS 200000
#define THREAD2PERIODSECONDS 0
#define THREAD2PERIODNANOSECONDS 200000

#define THREAD3INITIALSECONDS 1
#define THREAD3INITIALNANOSECONDS 150000
#define THREAD3PERIODSECONDS 0
#define THREAD3PERIODNANOSECONDS 50000

//Structures
struct Threadargs{
	char *buffer, *filename;
	char **song;
	time_t is, ps;
	long in, pn;
}typedef threadargs;

//Function prototypes
void* songfixer(void *args);
void* songmixer(void *args);

int main(){
	
	char *tempbuffer;
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
	tempbuffer = malloc(50 * sizeof(char));

	//Allocate space for the thread arguments
	args1 = malloc(sizeof(threadargs));
	args1->filename = malloc(sizeof(char) * 20);
	args2 = malloc(sizeof(threadargs));
	args2->filename = malloc(sizeof(char) * 20);
	args3 = malloc(sizeof(threadargs));
	
	//Set the thread arguments
	*(args1->filename) = 'f';
	*(args1->filename + 1) = 'i';
	*(args1->filename + 2) = 'r';
	*(args1->filename + 3) = 's';
	*(args1->filename + 4) = 't';
	*(args1->filename + 5) = '.';
	*(args1->filename + 6) = 't';
        *(args1->filename + 7) = 'x';
        *(args1->filename + 8) = 't';
        *(args1->filename + 9) = '\0';

        *(args2->filename) = 's';
        *(args2->filename + 1) = 'e';
        *(args2->filename + 2) = 'c';
        *(args2->filename + 3) = 'o';
        *(args2->filename + 4) = 'n';
        *(args2->filename + 5) = 'd';
	*(args2->filename + 6) = '.';
        *(args2->filename + 7) = 't';
        *(args2->filename + 8) = 'x';
        *(args2->filename + 9) = 't';
        *(args2->filename + 10) = '\0';


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
	args3->buffer = tempbuffer;
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
		printf("Line %d: %s", i + 1, *(Song+i));
	}	

	//Free the allocated memory
	for (i = 0; i < 20; i++){
		free(*(Song + i));
	}
	free(Song);
	free(args1->filename);
	free(args1);
	free(args2->filename);
	free(args2);
	free(args3);
	free(tempbuffer);

	return 0;
}


void* songfixer(void *args){
	int i, j = 0, timer = timerfd_create(CLOCK_MONOTONIC, 0);
	struct itimerspec timer_value;
	struct sched_param param;	
	uint64_t periods = 0;	
	char temp;
	FILE* pFile;
	pFile = fopen(((threadargs*)args)->filename, "r");

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

	for (i = 0; i < 8; i++){
		//Read the period variable
		read(timer, &periods, sizeof(periods));

		fscanf(pFile, "%c", &temp);

		*(((threadargs*)args)->buffer + j) = temp;
		while (temp != '\n'){
			j++;
			fscanf(pFile, "%c", (((threadargs*)args)->buffer + j));
                	temp = *(((threadargs*)args)->buffer + j);
		}
		j = 0;
	printf("Fixer run %d finished\n", i+1);
	}

	fclose(pFile);
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

		for (j = 0; *(((threadargs*)args)->buffer + j) != '\n'; j++){
			*(*(((threadargs*)args)->song + i) + j) = *(((threadargs*)args)->buffer + j);
		}
		*(*(((threadargs*)args)->song + i) + j) = '\n';

	printf("Mixer run %d finished\n", i+1);
	}
	
	pthread_exit(0);
}





