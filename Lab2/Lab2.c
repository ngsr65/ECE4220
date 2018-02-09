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
void threadsearch(void *args);



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

	printf("\nOpening file %s...\n", filename);	

	//Open the file
	pFile = fopen(filename, "r");

	//Get number of rows and columns
	fscanf(pFile, "%d", &(data.rows));
	fscanf(pFile, "%d", &(data.columns));

//Errorcheck
printf("\nThere are %d rows and %d columns.\n", data.rows, data.columns);

	//Allocate memory for the 2D array
	data.matrix = (int**)malloc(sizeof(int*) * data.rows);
	for (i = 0; i < data.rows; i++){
		*(data.matrix + i) = (int*)malloc(sizeof(int) * data.columns);
	}

//Errorcheck
printf("\nAllocation ran %d times\n", i);

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

	//First search
	printf("\nSearching the data with one thread %d times...\n", times);
	search(0, data, times);
	




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
	int i,j, threads;
	long starttime, difftime[times], timeavg;
	struct timespec time;
	Threadargs args;
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
				//Allocate space for the threads
				thread = (pthread_t*)malloc(sizeof(pthread_t) * 1);
				threads = 1;
				
				//Set the threads arguments
				args.data = &data;
				args.choice = 0;

				//Start the clock
				clock_gettime(CLOCK_MONOTONIC, &time);
				starttime = time.tv_nsec;

				//Create the thread
				pthread_create(&thread, NULL, &threadsearch, (void*)&args);

				//Wait for the thread to finish
				pthread_join(&thread, NULL);

				break;

			//One thread for each row
			case 1:
				
				break;

			//One thread for each column
			case 2:
				
				break;

			//One thread for each element
			case 3:
				
				break;
		}

		//Stop the clock and figure out the difference in time
		clock_gettime(CLOCK_MONOTONIC, &time);
		difftime[i] = time.tv_nsec - starttime;

		//Free the memory used by the threads
		free(thread);

		//Output the count and the time it took
		printf("\n\nTime = %ld nanoseconds", difftime[i]);
		printf("\nCount  0   1   2   3   4   5   6   7   8   9   10");
		printf("\n       %-3d ", *(data.count));
		for (j = 1; j < 11; j++){
			printf("%-3d ", *(data.count + j));
		}
	}

	//Calculate the average time
	timeavg = 0;
	for (i = 0; i < times; i++){
		timeavg += difftime[i];
	}
	timeavg /= (long)times;

}

void threadsearch(void *args){
	
	//int i, j, rows = *(args->data->rows), columns = *(args->data->columns), choice = *(args->choice);
	int i, j, rows = ((Threadargs*)args)->data->rows, columns = ((Threadargs*)args)->data->columns, choice = ((Threadargs*)args)->choice;

	printf("\nRows = %d, Columns = %d, Choice = %d", rows, columns, choice);


	switch(choice){
		case 0:
			for (i = 0; i < rows; i++){
				for (j = 0; j < columns; j++){
					//Increment the counter value
					//*(((Threadargs*)args)->data->count + *(*(((Threadargs*)args)->data->matrix + i) + j))++;
				}
			}
		break;
	}

	pthread_exit(0);
}
