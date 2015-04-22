#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <time.h>


double get_time()
{
	struct timeval time;
	gettimeofday(&time, NULL);
	return (double) time.tv_sec + (time.tv_usec/1000000.0);
}


int main(int argc, char **argv) 
{
	MPI_Init(&argc, &argv);

	int whoAmI;
	MPI_Comm_rank(MPI_COMM_WORLD, &whoAmI);
	printf("I am %d\n", whoAmI);

	MPI_Status stat;

	//Variables for time measurement purpose
	double start_time;
	double end_time;
	float runtime;

	int * dataToSend = (int*)malloc(sizeof(int));
	*dataToSend = whoAmI+1000;

	
	switch (whoAmI) 
	{
		case 0: 
		{
			
			start_time = get_time();
			MPI_Send(dataToSend, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
			end_time = get_time();
			runtime = end_time - start_time; 
			free(dataToSend);
			break;
		}
		case 1: 
		{
			int resultToPrint = *dataToSend;
			MPI_Recv(dataToSend, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &stat);

			resultToPrint = resultToPrint + *dataToSend ;
			printf("The sum of both numbers ia %d\n" , resultToPrint);

			free(dataToSend);
			break;
		}
		default:
			printf("hello. i am your missing child! #%d\n", whoAmI);
	}

	MPI_Finalize();
	return 0;
}
