#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

struct Data{
	int rows;
	int columns;
	int** matrix;
	float time;
	int** count;
} typedef DataMatrix;


int main(int argc, char **argv){

	//Variable initalization
	FILE *pFile;
	DataMatrix data;
	char filename[25];
	int i,r,c;

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
			printf("Error! Filename too long. Quitting...");
			sleep(2);
			return 0;
		}
	}

	printf("\nOpening file %s...\n", filename);	

	//Open the file
	pFile = fopen(filename, "r");

	//Get number of rows and columns
	fscanf(pFile, "%d", &(data.rows));
	fscanf(pFile, "%d", &(data.columns));

	//Allocate memory for the 2D array
	data.matrix = (int**)malloc(sizeof(int) * data.rows);
	for (i = 0; i < data.rows; i++){
		*(data.matrix + i)  = (int*)malloc(sizeof(int) * data.columns);
	}

	//Read in values
	for (r = 0; r < data.rows; r++){
		for (c = 0; c < data.columns; c++){
			fscanf(pFile, "%d", *(data.matrix + r) + c);
		}
	}	

	//Display the numbers
	printf("\nThe numbers are - \n");	
	for (r = 0; r < data.rows; r++){
		for (c = 0; c < data.columns; c++){
			printf("%d ", *(*(data.matrix + r) + c) );
		}
		printf("\n");
	}
	
	
	




	//Free up the allocated memory
	for (i = 0; i < data.rows; i++){
		free(*(data.matrix + i));
	}
	free(data.matrix);

	//Close the file
	fclose(pFile);


	return 0;
}
