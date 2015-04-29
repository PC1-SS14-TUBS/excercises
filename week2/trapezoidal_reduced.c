#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>
#include "timerTool.h"
#include <unistd.h> //Library for use usleep -> This example only works in unix OS


void delay(unsigned long ms) //Sleep the process for ms miliseconds
{
	usleep(ms * 1000); //Receive microseconds as argument
}

double local_trap(double a, double b, int n){
	double h; 
	double my_result;
	double local_a;
	double local_b;
	int i;
	int local_n;
	int my_rank = omp_get_thread_num();
	int thread_count = omp_get_num_threads();


	//Define and initialize local parameters (Integral subsection)
	h = (b - a) / n;
	local_n = n / thread_count;
	local_a = a + my_rank*local_n*h;
	local_b = local_a + local_n*h;

	printf("Thread = %d local_a = %f local_b = %f local_n %d\n", my_rank, local_a, local_b, local_n);
	
	//Actual trapezoidal rule implementation
	//h*[f(x0)/2 + f(x1) + f(x2)+ ... + f(xn-1)+ f(xn)/2]
	my_result = sin(local_a) / 2;
	for (i = 1; i < local_n; i++){
		my_result += sin(local_a + i*h);
	}
	my_result += sin(local_b) / 2;
	my_result = my_result*h;
	
	delay(1000);

	return my_result;
}




int main(int argc , char * arg[]){
	double start;
	double end;
	double elapsed_time;
	double global_result = 0.0f;
	double a;
	double b;
	int n;
	int thread_count;

	//Initialize the problem parameters
	printf("\nThe current program will calculate the integral of the function f(x)=sin(x)\n");
	printf("for a given values x = a and x = b\n");
	printf("\nPlease type a value for a:\n");
	scanf("%lf",&a);
	printf("\nPlease type a value for b:\n");
	scanf("%lf", &b);
	printf("\nPlease type a number n of interval subdivision:\n");
	scanf("%d", &n);
	printf("\nPlease type the number of threads to use for the calculation:\n");
	scanf("%d", &thread_count);


	printf("\nSelected parameters are:\n");
	printf("a = %f b = %f n = %d Num threads = %d \n", a , b , n , thread_count);


	start = get_time();
	//Run trapezoidal rule in thread_count number of threads
#pragma omp parallel num_threads(thread_count) reduction(+: global_result)
	{
			global_result += local_trap(a, b, n);
	}
	end = get_time();
	elapsed_time = end - start;


	//Print the obtained result
	printf("\nWith n = %d trapezoids, the integral estimate is = %f\n", n, global_result);
	printf("Processing time = %f\n", elapsed_time);


	return 0;
}