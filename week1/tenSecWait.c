#include <unistd.h> //Library for use usleep -> This example only works in unix OS
#include <stdio.h>

//#include <sys/time.h> 
//#include <time.h>
//#include <stdlib.h>

#include "timerTool.h"

void delay(unsigned long ms) //Sleep the process for ms miliseconds
{
	usleep(ms * 1000); //Receive microseconds as argument
}


int main(){
	
	//#TODO
	//Save initial time
	
	double start = get_time();
	printf("\nWaiting...\n");
	delay(10000);
	printf("\nTime up!!\n");
	
	//#TODO
	//Save ending time, calculate the elapsed time and print to console the result
	
	return 0;
}