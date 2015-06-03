
#include <sys/time.h> 
#include <stdio.h>
#include "mpi.h"
#include <stdlib.h>
#include <time.h>
#include "timerTool.h"



void delay(unsigned long ms) //Sleep the process for ms miliseconds
{
	usleep(ms * 1000); //Receive microseconds as argument
}


//Log base 2 implementation
double my_log2(double x)
{
	return log(x) / log(2);
}


int main(int argc, char **argv) {
  
	//Time measurement variable declaration
	double start;
	double end;
	double elapsed_time;

	//Declaration of variables to store ranks
	int whoAmI=-1;
	int* rankReceiver;


	//Declaration of variables to store Comm/Group size
	int worldSize;
	

	MPI_Init(&argc, &argv);

	//Initialization of parameters
	rankReceiver = (int*)malloc(sizeof(int));
	*rankReceiver = -1;
	MPI_Comm_rank(MPI_COMM_WORLD, &whoAmI);
	MPI_Comm_size(MPI_COMM_WORLD, &worldSize);

	int broadcaster = (int)(worldSize/2);


	//Pareparation for reduction->move data to rank 0 if not in rank 0

	start = get_time();

	if (broadcaster != 0){
		if (whoAmI == 0){
			MPI_Recv(rankReceiver, 1, MPI_INT, broadcaster, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
		if (whoAmI == broadcaster){
			MPI_Send(&whoAmI, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
			
		}
	}

	int threeLevels = ceil(my_log2(worldSize));
	int i;
	for (i = threeLevels; i > 0; i--){
		int level = pow(2, i);
		int nextlevel = pow(2, i-1);
		if (whoAmI%level == 0){
			MPI_Send(rankReceiver, 1, MPI_INT, whoAmI+nextlevel, 0, MPI_COMM_WORLD);
		}
		if ((whoAmI - nextlevel) % level == 0){
			MPI_Recv(rankReceiver, 1, MPI_INT, whoAmI-nextlevel, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
		MPI_Barrier(MPI_COMM_WORLD);
	}

		
	MPI_Barrier(MPI_COMM_WORLD);

	end = get_time();
	elapsed_time = end - start;

	//Print broadcasted rank in all the processes
	printf("WhoAmI = %d | rankreceiver = %d\n", whoAmI, *rankReceiver);

	delay(1000);

	if (whoAmI == 0){
		printf("Broadcasting time = %f\n\n", elapsed_time);
	}

	MPI_Finalize();
	return 0;
}
