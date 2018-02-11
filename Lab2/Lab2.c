#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>



//Structure definitions
struct Data{
	int rows, columns;
	int** matrix;
	float time;
	int* count;
} typedef DataMatrix;

struct Threadarguments{
	DataMatrix *data;
	int choice, r, c;
} typedef Threadargs;

//Function prototypes
void search(int choice, DataMatrix data, int times);
void* threadsearch(void *args);



int main(int argc, char **argv){

	//Variable initalization
	FILE *pFile;
	DataMatrix data;
	char filename[25];
	int i,r,c, times;
	struct timespec time;
	long timediff;


	if (argc == 1){
		printf("Please enter the name of the file you wish to open: ");
		scanf( "%s", filename);
	} else {
		i = 0;
		while ((argv[1][i] != '\0') && i < 24){
			filename[i] = argv[1][i];
			i++;
		}
		if (i < 25){
			filename[i] = '\0';
		} else {
			printf("Error! Filename too long. Quitting...\n");
			return 0;
		}
	}

	//Open the file
	if ((pFile = fopen(filename, "r")) == NULL){
		printf("Error opening file %s! Exiting program", filename);
		return 0;
	}

	//Get number of rows and columns
	fscanf(pFile, "%d", &(data.rows));
	fscanf(pFile, "%d", &(data.columns));

	//Allocate memory for the 2D array
	data.matrix = (int**)malloc(sizeof(int*) * data.rows);
	for (i = 0; i < data.rows; i++){
		*(data.matrix + i) = (int*)malloc(sizeof(int) * data.columns);
	}

	//Allocate memory for the count array
	data.count = (int*)malloc(sizeof(int) * 11);

	//Read in values and display
	printf("\n\nThe numbers are - \n");
	for (r = 0; r < data.rows; r++){
		for (c = 0; c < data.columns; c++){
			fscanf(pFile, "%d", *(data.matrix + r) + c);
			printf("%d ", *(*(data.matrix + r) + c));
		}
		printf("\n");
	}	

	printf("\n\nHow many times would you like to run each search?");
	printf("\nPlease enter a positive integer: ");
	scanf("%d", &times);

	//Searches
	printf("\n\nSearching the data with one thread %d times...\n", times);
	search(0, data, times);
	printf("\n\n\nSearching the data with %d threads, one for each row, %d times...\n", data.rows, times);
	search(1, data, times);
	printf("\n\n\nSearching the data with %d threads, one for each column, %d times...\n", data.columns, times);
	search(2, data, times);
	printf("\n\n\nSearching the data with %d threads, one for each element, %d times...\n", data.rows * data.columns, times);
	search(3, data, times);
	printf("\n");


	//Free up the allocated memory
	for (i = 0; i < data.rows; i++){
		free(*(data.matrix + i));
	}
	free(data.matrix);
	free(data.count);

	//Close the file
	fclose(pFile);


	return 0;
}

void search(int choice, DataMatrix data, int times){

	//Variable initalization
	int i, j, k, threads;
	long starttime, difftime[times];
	double timeavg;
	struct timespec time;
	Threadargs *args;
	pthread_t *thread;

	//Run the search multiple times
	for (i = 0; i < times; i++){

		//Set the count array's values to zero
		for (j = 0; j < 11; j++){
			*(data.count + j) = 0;
		}

		//Run with the correct amount of threads for each test
		switch (choice){

			//One single thread
			case 0:
				//Allocate space for the thread
				thread = malloc(sizeof(pthread_t) * 1);
				args = malloc(sizeof(Threadargs) * 1);
				threads = 1;
				
				//Set the thread's arguments
				args->data = &data;
				args->choice = 0;

				//Start the clock
				clock_gettime(CLOCK_MONOTONIC, &time);
				starttime = time.tv_nsec;

				//Create the thread
				pthread_create(thread, NULL, threadsearch, (void*)args);

				//Wait for the thread to finish
				for (j = 0; j < threads; j++){
					pthread_join(*(thread + j), NULL);
				}
				break;

			//One thread for each row
			case 1:
				//Allocate space for the threads
				thread = malloc(sizeof(pthread_t) * data.rows);
				args = malloc(sizeof(Threadargs) * data.rows);
				threads = data.rows;

				//Set the threads arguments
				for (j = 0; j < threads; j++){
					(args + j)->data = &data;
					(args + j)->choice = 1;
					(args + j)->r = j;
				}

				//Start the clock
				clock_gettime(CLOCK_MONOTONIC, &time);
				starttime = time.tv_nsec;

				//Create the threads
				for (j = 0; j < threads; j++){
					pthread_create(thread + j, NULL, threadsearch, (void*)(args+j));
				}

				//Wait for the threads to finish
				for (j = 0; j < threads; j++){
					pthread_join(*(thread + j), NULL);
				}

				break;

			//One thread for each column
			case 2:
				//Allocate space for the threads
				thread = malloc(sizeof(pthread_t) * data.columns);
				args = malloc(sizeof(Threadargs) * data.columns);
				threads = data.columns;

				//Set the threads arguments
				for (j = 0; j < threads; j++){
					(args + j)->data = &data;
					(args + j)->choice = 2;
					(args + j)->c = j;
				}

				//Start the clock
				clock_gettime(CLOCK_MONOTONIC, &time);
				starttime = time.tv_nsec;

				//Create the threads
				for (j = 0; j < threads; j++){
					pthread_create(thread + j, NULL, threadsearch, (void*)(args+j));
				}

				//Wait for the threads to finish
				for (j = 0; j < threads; j++){
					pthread_join(*(thread + j), NULL);
				}
				break;

			//One thread for each element
			case 3:
				//Allocate space for the threads
				thread = malloc(sizeof(pthread_t) * data.rows * data.columns);
				args = malloc(sizeof(Threadargs) * data.rows * data.columns);
				threads = data.rows * data.columns;

				//Set the threads arguments
				for (j = 0; j < data.rows; j++){
					for (k = 0; k < data.columns; k++){
						(args + k + (j * data.rows))->data = &data;
						(args + k + (j * data.rows))->choice = 3;
						(args + k + (j * data.rows))->r = j;
						(args + k + (j * data.rows))->c = k;
					}
				}

				//Start the clock
				clock_gettime(CLOCK_MONOTONIC, &time);
				starttime = time.tv_nsec;

				//Create the threads
				for (j = 0; j < threads; j++){
					pthread_create(thread + j, NULL, threadsearch, (void*)(args+j));
				}

				//Wait for the threads to finish
				for (j = 0; j < threads; j++){
					pthread_join(*(thread + j), NULL);
				}
				break;
		}

		//Stop the clock and figure out the difference in time
		clock_gettime(CLOCK_MONOTONIC, &time);
		difftime[i] = time.tv_nsec - starttime;

		//Free the memory used by the threads
		free(thread);
		free(args);
	}

	//Calculate the average time
	timeavg = 0;
	for (i = 0; i < times; i++){
		timeavg += difftime[i];
	}
	timeavg /= (double)times;
	timeavg /= 1000000;
	printf("\n\nAverage time for %d runs with %d threads is %.4lf milliseconds", times, threads, timeavg);
	
	//Output the count
	printf("\nCount  0    1    2    3    4    5    6    7    8    9    10");
	printf("\n       %-4d ", *(data.count));
	for (i = 1; i < 11; i++){
		printf("%-4d ", *(data.count + i));
	}

}

void* threadsearch(void *args){
	
	//Variables
	int i, j;
	int rows = ((Threadargs*)args)->data->rows, columns = ((Threadargs*)args)->data->columns, choice = ((Threadargs*)args)->choice;
	int r = ((Threadargs*)args)->r, c = ((Threadargs*)args)->c;

	switch(choice){
		case 0:
			for (i = 0; i < rows; i++){
				for (j = 0; j < columns; j++){
					//Increment the counter value
				        (*(((Threadargs*)args)->data->count + *(*(((Threadargs*)args)->data->matrix + i) + j)))++;
				}
			}
			break;

		case 1:
			for (i = 0; i < columns; i++){
				//Increment the counter value
				(*(((Threadargs*)args)->data->count + *(*(((Threadargs*)args)->data->matrix + r) + i)))++;
			}
			break;

		case 2:
			for (i = 0; i < rows; i++){
				//Increment the counter value
				(*(((Threadargs*)args)->data->count + *(*(((Threadargs*)args)->data->matrix + i) + c)))++;
			}
			break;

		case 3:
			(*(((Threadargs*)args)->data->count + *(*(((Threadargs*)args)->data->matrix + r) + c)))++;
			break;
	}

	pthread_exit(0);
}
