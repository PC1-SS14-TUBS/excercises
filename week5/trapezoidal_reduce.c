#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include <math.h>
#include "timerTool.h"
#include <sys/time.h> 
#include <unistd.h> //Library for use usleep -> This example only works in unix OS


void delay(unsigned long ms) //Sleep the process for ms miliseconds
{
	usleep(ms * 1000); //Receive microseconds as argument
}


double f(double x){
	return x*x;
}

double local_trap(double a, double b, int n, int process_count, int My_rank){
	double h; //Step in x due to the Subinterval partitions
	double my_result; //Return variable to hold the integral over the subinterval limits
	double local_a; 
	double local_b;
	int local_n;
	int i;

	//Define and initialize local parameters (Integral subsection)
	h = (b - a) / n;                  //Distance between partitions
	local_n = n / process_count;      //Number of partitions to consider each process
	local_a = a + My_rank*local_n*h;  //Local to process lower limit
	local_b = local_a + local_n*h;    //Local to process upper limit

	
	
	//Actual trapezoidal rule implementation
	//h*[f(x0)/2 + f(x1) + f(x2)+ ... + f(xn-1)+ f(xn)/2]
	my_result = f(local_a) / 2;
	for (i = 1; i < local_n; i++){
		my_result += f(local_a + i*h);
	}
	my_result += f(local_b) / 2;
	my_result = my_result*h;
	
	delay(1000);

	printf("Process = %d | local_a = %f | local_b = %f | local_n %d | local_result = %f\n", My_rank, local_a, local_b, local_n, my_result);

	return my_result;
}






int main(int argc , char * argv[]){
	double start;
	double end;
	double elapsed_time;
	
	double * local_result;
	double * global_result;
	
	double a; //Global Lower limit for integration
	double b; //Global Upper limit for integration
	int n;    //Number of partitions that will be distributed along subintervals
	
	int process_count = -1; //Stores the number of Processes that represent the nomber of 
						    //subintervals to consider for the integration.
	int My_rank = -1; //Rank Id of the process running this code
	
	
	MPI_Init(&argc, &argv); //Initialization of the MPI interface
	

	//INITIALIZE PROBLEM PARAMETERS
	MPI_Comm_rank(MPI_COMM_WORLD, &My_rank); //Get process Id within the GLobal Communicator
	if (argc < 4){
		if (My_rank == 0){
			printf("\nThe current program calculates the integral of the function f(x)=x^2\n");
			printf("between the limits values given as command line parameters for a and b.\n");
			printf("\nUsage: \"Executable File\" [a] [b] [n]\n");
			printf("         a: Lower Integration Limit:\n");
			printf("         b: Upper Integration Limit:\n");
			printf("         n: Number of partitions to be used in the integration process:\n");
		}
		MPI_Finalize();
		return 0;
	}

	MPI_Comm_size(MPI_COMM_WORLD, &process_count);
	global_result = (double*)malloc(sizeof(double));
	local_result = (double*)malloc(sizeof(double));
	
	*global_result = -1;
	*local_result = -1;
	a = atof(argv[1]);
	b = atof(argv[2]);
	n = atoi(argv[3]);

	

	//RUN TRAPEZOIDAL RULE IN process_count NUMBER OF PROCESSES
	start = get_time();

	//Subinterval integral calculation
	*local_result = local_trap(a, b, n, process_count, My_rank);
	
	MPI_Barrier(MPI_COMM_WORLD);
	
	//Reduce all the calculated local results to the global result

	MPI_Reduce(
		local_result,	 //Pointer to Reduce data
		global_result,	 //Pointer to Reduced data
		1,				 //data elements to reduce
		MPI_DOUBLE,		 //Data type of the reduce data
		MPI_SUM,         //Reduce operation
		0,               //Root process where the data is going to get reduced
		MPI_COMM_WORLD);  //Communicator

	MPI_Barrier(MPI_COMM_WORLD);

	end = get_time();
	elapsed_time = end - start;


	delay(1000);

	//Print the obtained result
	if (My_rank == 0){
		printf("\nWith n = %d trapezoids, the integral estimate is = %f\n", n, *global_result);
		printf("Processing time = %f\n\n", elapsed_time);
	}

	delay(1000);

	printf("Process = %d | Global Result = %f\n",My_rank, *global_result);
	

	MPI_Finalize();
	free(local_result);
	free(global_result);

	return 0;
}