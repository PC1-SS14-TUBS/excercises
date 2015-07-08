#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>



#define CONST_TOL 0.00001
#define MPI_RANK_ROOT 0
#define MAX_ITER 1000



void readFile(char *fileName, int *n, double **value, int **colind, int **rbegin, double **b){
    FILE *fp=fopen(fileName,"r");
	char buf[200];
    int i,j,k,l;
    int p,q;
    double w;
    int m;


    if ((fp=fopen(fileName,"r"))==NULL){
        fprintf(stderr,"Open file error, check filename please.\n");
    }


	//fgets functions will read the next line in the file datafile
    fgets(buf,200,fp);

	//Set to n the first integer of the first line in given data file
	//This represents the matrix Size
    *n=atoi(buf);


	//Read the second integer in the first line in the given data file
	//This correspon to the number of non zero values
    l=0;
    while(buf[l++]!=' ');
    m=atoi(&buf[l]);


    printf("Matrix size: %d, #Non-zeros: %d\n",*n,m);

	//Allocate memory for the values in the compressed matrix
    (*value)=(double*)malloc(sizeof(double)*m);
    
	//Allocate memory for the column indices in the compressed matrix
	(*colind)=(int*)malloc(sizeof(int)*m);


	//Allocat memory for 
    (*rbegin)=(int*)malloc(sizeof(int)*((*n)+1));


	//Allocate memory and Initialize b with the initial condition
	(*b) = (double*)malloc(sizeof(double)*(*n));
    for(i=0;i<(*n);i++){
        (*b)[i]=1.0;
    }


    k=-1;

	//Iterate throuhg nonzero number of lines in data file
    for(i=0;i<m;i++){

		//Read and Store a new line in buffer
        fgets(buf,200,fp);
        l=0;
		
		//Get first element of line
		p=atoi(&buf[l]);

		//get second element of line
        while(buf[l++]!=' '); 
		q=atoi(&buf[l]);

		//get third element of line
        while(buf[l++]!=' '); 
		w=atof(&buf[l]);


		//Update value array with new value
        (*value)[i]=w;

		//Update column index for the new value
        (*colind)[i]=q;


		//Row Index Value??
		//What whould you think is happening here?
        if(p!=k){
            k=p;
            (*rbegin)[p]=i;
        }

    }

    (*rbegin)[*n]=m;
    
	//No more data, then close file
	fclose(fp);
}
void scatterData(int *n, int *m, double **value, int **colind, int **rbegin, double **b, int rank, int nproc){
    int np;    //Rows/columns per process
    int *sendcnts;
    int *displs;

    double *gvalue;  //Will copy value
    int *gcolind;    //Will copy colind
    int *grbegin;    //Will copy rbegin
    double *gb;      //Will copy b

    int i,j;


	//Initilize and prepare the data distribution process. Done in the Master Process
    if (rank==MPI_RANK_ROOT){

		//Aloccate memory for the variable scatter sized and pointers for the data to processes distribution
        sendcnts=(int*)malloc(sizeof(int)*(nproc)); //Below explained
        displs=(int*)malloc(sizeof(int)*(nproc));	//Below explained
        np=(*n)/nproc;								//Number of rows per process


		
		//Make copies of the complete data and leave available the original pointers which exist in each process to receive the corresponding rows data.
        gvalue=(*value);    (*value)=NULL;
        gcolind=(*colind);  (*colind)=NULL;
        grbegin=(*rbegin);  (*rbegin)=NULL;
        gb=(*b);(*b)=NULL;



		//Calculate the number of elements to be send to each process representing the data in their assigned rows.
        for(i=0;i<nproc;i++){							//Go through each processor rank
            sendcnts[i]=0;								//Initialize number of elements to send to the process i
            for(j=i*np;j<(i+1)*np;j++){					//Go through each rows per process	
                sendcnts[i]+=grbegin[j+1]-grbegin[j];	//Store the number of elements to be send to each processor.
														//Note that because grbegin holds the row offsets, the shown substraction represent the number of nonzero elements of the correspondent row
            }
        }

		//We now from sendcnts the number of elements but we need the starting point adresses in the original array
        displs[0]=0;
        for(i=1;i<nproc;i++){
            displs[i]=displs[i-1]+sendcnts[i-1];
        }
    }




    fflush(stdout);

    MPI_Bcast(n,1,MPI_INT,MPI_RANK_ROOT,MPI_COMM_WORLD); //Broadcast matrix size to each process
    MPI_Scatter(sendcnts,1,MPI_INT, m , 1 , MPI_INT ,MPI_RANK_ROOT, MPI_COMM_WORLD); //Scatter to each process its correspondent number of elements of the matrix to receive 
    np=(*n)/nproc;
    printf("Process %d get n=%d, m=%d\n",rank,*n,*m);
    fflush(stdout);


    (*value)=(double*)malloc(sizeof(double)*(*m));
    (*colind)=(int*)malloc(sizeof(int)*(*m));
    (*rbegin)=(int*)malloc(sizeof(int)*(np+1));
    (*b)=(double*)malloc(sizeof(double)*np);
    
	


	/*MPI_Scatterv arguments:
	*	sbuf		address of send buffer,
	*	scounts		integer array specifying the number of elements to send to each process,
	*	displs		array specifying the displacement relative to sbuf from which to take the data going out to the corresponding process,
	*	stype		data type of send buffer elements,
	*	rbuf		address of receive buffer,
	*	rcount		number of elements in receive buffer,
	*	rtype		data type of receive buffer elements,
	*	root		rank of sending process,
	*	comm		group communicator
	*/
	MPI_Scatterv(gvalue, sendcnts, displs, MPI_DOUBLE, (*value), (*m),MPI_DOUBLE, MPI_RANK_ROOT, MPI_COMM_WORLD);
	MPI_Scatterv(gcolind, sendcnts, displs, MPI_INT, (*colind), (*m),MPI_INT, MPI_RANK_ROOT, MPI_COMM_WORLD);
    


	/*MPI_Scatter arguments:
	*	sbuf		address of send buffer(choice, significant only at root)
	*	scounts		number of elements sent to each process(integer, significant only at root)
	*	stype		data type of send buffer elements(significant only at root) (handle)
	*	rbuf		address of receive buffer (choice)
	*	rcount		number of elements in receive buffer(integer)
	*	rtype		data type of receive buffer elements(handle)
	*	root		rank of sending process,
	*	comm		group communicator
	*/
	MPI_Scatter(grbegin, np, MPI_INT, (*rbegin), np, MPI_INT, MPI_RANK_ROOT, MPI_COMM_WORLD);
	MPI_Scatter(gb, np, MPI_DOUBLE, (*b), np, MPI_DOUBLE, MPI_RANK_ROOT, MPI_COMM_WORLD);
    
	
	int offset=(*rbegin)[0];
    for(i=0;i<np;i++){
        (*rbegin)[i]-=offset;
    }
    (*rbegin)[np]=(*m);
    if (rank==MPI_RANK_ROOT){
        free(gvalue);
        free(gcolind);
        free(grbegin);
        free(gb);
        free(sendcnts);
        free(displs);
    }
}



//This method will write the resul matrix data to a text file 
void writeFile(int n,double *answer){
    FILE *fp=fopen("output.dat","w");
    int i;
    for(i=0;i<n;i++){
        fprintf(fp,"%.10f\n",answer[i]);
    }
    fclose(fp);
}




//Method that corresponde to the actual solver. This is the method you will need to implement.
//FURTHER COMMENTS TO BE MADE
double* cg(int n, double *value, int* colind, int* rbegin, double *b, int rank, int nproc){
    double *answer;

    // Your CODE here. NOTICE: Data is already Scattered. Function IS already Called.

    return answer;
}




/*Main code stream of the program. Controls the time line of:	Reading the problem's data
																scatter problem's data to processes
																Run the solver
																Write back the result values.
*/
int main(int argc, char* argv[]){
    int n;
    int m;
    double *value=NULL;
    int *colind=NULL;
    int *rbegin=NULL;
    double *answer=NULL;
    double *b=NULL;
    int nproc,rank,namelen;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&nproc);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Get_processor_name(processor_name,&namelen);
    printf("Process %d on %s out of %d\n",rank, processor_name, nproc);

    fflush(stdout);

	//MPI_RANK_ROOT defined above manually
    if (rank==MPI_RANK_ROOT){
		//argv[1] -> command line argument that contains the name of the data file to be read.
		//   n    -> Size of the matrix (n*n)  
		// value  -> Pointer to array that will contain the values of the matrix in compressed format  
		// colind -> Pointer to array that will contain the column Indexes
		// rbegin -> Later explanined
		//   b    -> Pointer to array that will contain the state initialization  

        readFile(argv[1],&n,&value,&colind,&rbegin,&b);


		//   m    ->	Number of nonzero elements that will consider each process after assigning rows to processes
		//				NOTE: Note that this m has a different meaning from the m in the readFile function	
        scatterData(&n,&m,&value,&colind,&rbegin,&b,rank,nproc);
    }
    else{
        scatterData(&n,&m,&value,&colind,&rbegin,&b,rank,nproc);
    }


    MPI_Barrier(MPI_COMM_WORLD);

	//  tv1    -> Measured start time (Seconds)
	//  tv2    -> Measured finish time (Seconds)
    double tv1,tv2;
    
	//    MPI_Wtime   -> Returns an elapsed time on the calling processor (Seconds).
	tv1=MPI_Wtime();
    
	
	//HERE THE FUNCTION TO IMPLEMENT IN THE HOMEWORK
	answer = cg(n,value,colind,rbegin,b,rank,nproc);
    
	
	tv2=MPI_Wtime();


	//Finalize and cleanup code
    if (rank==MPI_RANK_ROOT){
        printf("Process %d takes %.10f seconds\n",rank,tv2-tv1);
    }
    if (value!=NULL) {
        free(value);
    }
    if (colind!=NULL) {
        free(colind);
    }
    if (rbegin!=NULL) {
        free(rbegin);
    }
    if (b!=NULL) {
        free(b);
    }
    if (rank==MPI_RANK_ROOT) {
        writeFile(n,answer);
    }
    if (answer!=NULL) {
        free(answer);
    }
    MPI_Finalize();
    return 1;
}	
		
