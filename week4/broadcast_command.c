
#include <sys/time.h> 
#include <stdio.h>
#include "mpi.h"
#include <stdlib.h>
#include <time.h>


int main(int argc, char **argv) {
  

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
	

	//--------------------------------------------------------------------------------------------------------
	//Broadcast: int MPI_Bcast(void *buffer,	int count,	MPI_Datatype datatype,	int root, MPI_Comm comm);
	//--------------------------------------------------------------------------------------------------------
	if (whoAmI == broadcaster){
		MPI_Bcast(&whoAmI, 1, MPI_INT, broadcaster, MPI_COMM_WORLD);
	}
	else{
		MPI_Bcast(rankReceiver, 1, MPI_INT, broadcaster, MPI_COMM_WORLD);
	}
	

	/*
	if (whoAmI == broadcaster){
		*rankReceiver = whoAmI;
	}	
	MPI_Bcast(rankReceiver, 1, MPI_INT, broadcaster, MPI_COMM_WORLD);
	*/
	

	//Print broadcasted rank in all the processes
	MPI_Barrier(MPI_COMM_WORLD);

		printf("WhoAmI = %d | rankreceiver = %d\n", whoAmI, *rankReceiver);


	MPI_Finalize();
	return 0;
}
