
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


	int i;

	start = get_time();

	for (i = 0; i < worldSize; i++){
		if (i != broadcaster){
			//------------------------------------------------------------------------------------------------------
			//send: int MPI_Send(void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm);
			//------------------------------------------------------------------------------------------------------
			MPI_Send(&whoAmI, 1, MPI_INT, i, 0, MPI_COMM_WORLD);

		}
	}
	if (whoAmI != broadcaster){
		//-------------------------------------------------------------------------------------------------------------------------------
		//Receive: int MPI_Recv(void *buf,	int count,	MPI_Datatype datatype,	int source,	int tag, MPI_Comm comm, MPI_Status *status);
		//-------------------------------------------------------------------------------------------------------------------------------
		MPI_Recv(rankReceiver, 1, MPI_INT, broadcaster, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		//NOTE THAT IN THIS CASE THE SOURCE PROCESS OF THE MESSAGE MUS BE NOW BY ALL THE RECEIVING PROCESS AT THE SAME TIME
	}
	

	//Print broadcasted rank in all the processes
	MPI_Barrier(MPI_COMM_WORLD);

	end = get_time();
	elapsed_time = end - start;


	printf("WhoAmI = %d | rankreceiver = %d\n", whoAmI, *rankReceiver);

	delay(1000);

	if (whoAmI == 0){
		printf("Broadcasting time = %f\n\n", elapsed_time);
	}


	MPI_Finalize();
	return 0;
}
