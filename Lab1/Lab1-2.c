#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>

void squareWave();

int main(){

	int numinput = -1;

	wiringPiSetupGpio();

	//Push Buttons
	pinMode(16, INPUT);
	pinMode(17, INPUT);
	pinMode(18, INPUT);
	pinMode(19, INPUT);
	pinMode(20, INPUT);

	//Speaker
	pinMode(6, OUTPUT);

	printf("\nEnter a number between 1 and 5: ");
	while (numinput < 1 || numinput > 5){
		scanf("%d", &numinput);	
	}

	switch (numinput){
		case 1:
			while (!digitalRead(16)){}
			squareWave();
			break;
		case 2:
			while (!digitalRead(17)){}
			squareWave();		
			break;
		case 3:
			while (!digitalRead(18)){}
			squareWave();
			break;
		case 4:
			while (!digitalRead(19)){}
			squareWave();
			break;
		case 5:
			while (!digitalRead(20)){}
			squareWave();
			break;
	}

	return 0;
}

void squareWave(){

	while (1){
		digitalWrite(6, 1);
		usleep(400);
		digitalWrite(6, 0);
		usleep(400);
	}
}
