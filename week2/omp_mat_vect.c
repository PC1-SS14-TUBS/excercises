/* File:     
 *     pth_mat_vect_rand.c 
 *
 * Purpose:  
 *     Computes a parallel matrix-vector product.  Matrix
 *     is logically distributed by block rows.  Vectors are logically
 *     distributed by blocks.  This version uses a random number 
 *     generator to generate A and x.
 *
 * Input:
 *     none
 *
 * Output:
 *     y: the product vector
 *     Elapsed time for the computation
 *
 * Compile:  
 *    gcc -g -Wall -o pth_mat_vect_rand pth_mat_vect_rand.c -lpthread -fopenmp
 * Usage:
 *    ./pth_mat_vect_rand <thread_count> <m> <n>
 *
 * Notes:  
 *     1.  Local storage for A, x, y is dynamically allocated.
 *     2.  Number of threads (thread_count) should evenly divide
 *         m.  The program doesn't check for this.
 *     3.  We use a 1-dimensional array for A and compute subscripts
 *         using the formula A[i][j] = A[i*n + j]
 *     4.  Distribution of A, x, and y is logical:  all three are 
 *         globally shared.
 *     5.  Compile with -DDEBUG for information on generated data
 *         and product.
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "timerTool.h"
#include <omp.h>

//Initialize variables
double start_time = 0;
double stop_time  =0;
double end_time = 0;
//Function to get the current time in seconds


/* Global variables */
int     thread_count;
int     m, n;
double* A;
double* x;
double* y;

/* Serial functions */
void Usage(char* prog_name);
void Gen_matrix(double A[], int m, int n);
void Read_matrix(char* prompt, double A[], int m, int n);
void Gen_vector(double x[], int n);
void Read_vector(char* prompt, double x[], int n);
void Print_matrix(char* title, double A[], int m, int n);
void Print_vector(char* title, double y[], double m);


/*------------------------------------------------------------------*/
int main(int argc, char* argv[]) {

   int i=0;
   int j=0;

   if (argc != 4) Usage(argv[0]);
   thread_count = strtol(argv[1], NULL, 10);   //Get number of threads from the arguments
   m = strtol(argv[2], NULL, 10);              //Get number of rows from the arguments
   n = strtol(argv[3], NULL, 10);              //Get number of columns from the arguments

#  ifdef DEBUG
   printf("thread_count =  %d, m = %d, n = %d\n", thread_count, m, n);
#  endif

   
   A = malloc(m*n*sizeof(double));                           //Allocate memory for the input matrix
   x = malloc(n*sizeof(double));                             //Allocate memory for the input vector
   y = malloc(m*sizeof(double));                             //Allocate memory for the output vector
   
   srandom(1);
   Gen_matrix(A, m, n);
#  ifdef DEBUG
   Print_matrix("We generated", A, m, n); 
#  endif

   Gen_vector(x, n);
#  ifdef DEBUG
   Print_vector("We generated", x, n); 
#  endif


   omp_set_num_threads(thread_count);
   start_time = get_time();

#pragma omp parallel 
   {
	   #pragma omp for private(j)
		   for (i = 0; i < m; i++) {

#  ifdef DEBUG
				printf("Row %d processed from thread %d\n", i, omp_get_thread_num());
#  endif

				y[i] = 0.0;
				for (j = 0; j < n; j++){
					y[i] += A[i*n + j] * x[j];
				}
		   }
   }

   stop_time = get_time();
   end_time = stop_time - start_time;


#  ifdef DEBUG
   Print_vector("The product is", y, m); 
#  endif
   printf("Elapsed time = %e seconds\n", end_time);

   free(A);
   free(x);
   free(y);
 

   return 0;
}  /* main */


/*------------------------------------------------------------------
 * Function:  Usage
 * Purpose:   print a message showing what the command line should
 *            be, and terminate
 * In arg :   prog_name
 */
void Usage (char* prog_name) {
   fprintf(stderr, "usage: %s <thread_count> <m> <n>\n", prog_name);
   exit(0);
}  /* Usage */

/*------------------------------------------------------------------
 * Function:    Read_matrix
 * Purpose:     Read in the matrix
 * In args:     prompt, m, n
 * Out arg:     A
 */
void Read_matrix(char* prompt, double A[], int m, int n) {
   int             i, j;

   printf("%s\n", prompt);
   for (i = 0; i < m; i++) 
      for (j = 0; j < n; j++)
         scanf("%lf", &A[i*n+j]);
}  /* Read_matrix */

/*------------------------------------------------------------------
 * Function: Gen_matrix
 * Purpose:  Use the random number generator random to generate
 *    the entries in A
 * In args:  m, n
 * Out arg:  A
 */
void Gen_matrix(double A[], int m, int n) {
   int i, j;

   for (i = 0; i < m; i++)
      for (j = 0; j < n; j++)
         A[i*n+j] = random()/((double) RAND_MAX);
}  /* Gen_matrix */

/*------------------------------------------------------------------
 * Function: Gen_vector
 * Purpose:  Use the random number generator random to generate
 *    the entries in x
 * In arg:   n
 * Out arg:  A
 */
void Gen_vector(double x[], int n) {
   int i;
   for (i = 0; i < n; i++)
      x[i] = random()/((double) RAND_MAX);
}  /* Gen_vector */

/*------------------------------------------------------------------
 * Function:        Read_vector
 * Purpose:         Read in the vector x
 * In arg:          prompt, n
 * Out arg:         x
 */
void Read_vector(char* prompt, double x[], int n) {
   int   i;

   printf("%s\n", prompt);
   for (i = 0; i < n; i++) 
      scanf("%lf", &x[i]);
}  /* Read_vector */

/*------------------------------------------------------------------
 * Function:    Print_matrix
 * Purpose:     Print the matrix
 * In args:     title, A, m, n
 */
void Print_matrix( char* title, double A[], int m, int n) {
   int   i, j;

   printf("%s\n", title);
   for (i = 0; i < m; i++) {
      for (j = 0; j < n; j++)
         printf("%6.3f ", A[i*n + j]);
      printf("\n");
   }
}  /* Print_matrix */


/*------------------------------------------------------------------
 * Function:    Print_vector
 * Purpose:     Print a vector
 * In args:     title, y, m
 */
void Print_vector(char* title, double y[], double m) {
   int   i;

   printf("%s\n", title);
   for (i = 0; i < m; i++)
      printf("%6.3f ", y[i]);
   printf("\n");
}  /* Print_vector */
