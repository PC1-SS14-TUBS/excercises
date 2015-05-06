
#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

int main(int argc, char* argv[])
{
    int rank;

    //Initialize source code to recognize MPI statements
	MPI_Init(&argc, &argv);

	//Get process ID
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	//Initilize resource allocation for Data send and receive
	double* value_to_send = (double*)malloc (sizeof(double));
	double* value_receiver = (double*)malloc(sizeof(double));

	*value_to_send = rank;
	*value_receiver = -1;

	//State and process of value receiver before communication
    printf("Init - Reporting from process %d - value_receiver = %f)\n",  rank, *value_receiver);

	
	//Perform communication
	if (rank == 0){
		MPI_Send(value_to_send, 1, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD);
		MPI_Recv(value_receiver, 1, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}
	else{
		MPI_Recv(value_receiver, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Send(value_to_send, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
	}

	//State and process of value receiver after communication
	printf("End - Reporting from process %d - value_receiver = %f)\n", rank, *value_receiver);


	//End and teminate MPI section - Any MPI command after this part will be executed
    MPI_Finalize();
	

	//Free memory at process completion
	free(value_to_send);
	free(value_receiver);

    return 0;
}
