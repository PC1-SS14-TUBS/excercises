
#include <sys/time.h> 
#include <stdio.h>
#include "mpi.h"
#include <stdlib.h>
#include <time.h>
#include "timerTool.h"
#include <math.h>


#define true 1
#define false 0

typedef int bool;



void delay(unsigned long ms) //Sleep the process for ms miliseconds
{
	usleep(ms * 1000); //Receive microseconds as argument
}




int main(int argc, char **argv) {
  

	//Declaration of variables to store neighborgs ranks
	int mRank=-1;
	int* neighborsRank;


	//Declaration of variables to store Comm/Group size
	int worldSize;
	

	MPI_Init(&argc, &argv);

	//Initialization of parameters
	neighborsRank = (int*)malloc(sizeof(int)*4);
	neighborsRank[0] = -1;
	neighborsRank[1] = -1;
	neighborsRank[2] = -1;
	neighborsRank[3] = -1;

	//memset(x, (char)0, sizeof(double)*M*N);
	MPI_Comm_rank(MPI_COMM_WORLD, &mRank);
	MPI_Comm_size(MPI_COMM_WORLD, &worldSize);

	//Get Coordinates of the process in the "Processes Matrix". The process is located in a conceptually conceived matrix
	int pMatSize = 4;
	int rankX = mRank % pMatSize;
	int rankY = (int)floor(mRank / pMatSize);


	//TEST:Print the coordinates of the current Process
	printf("I am Process  %d | x = %d | y = %d\n",mRank,rankX,rankY);

	delay(1000);

	
	//Declaration and initialization of flags to indicate if communicates with lower indexes
	bool commLowerY = false;
	bool commLowerX = false;

	if (rankX > 0)commLowerX = true;
	if (rankY > 0)commLowerY = true;


	//Declaration and initialization of flags to indicate if communicates with upper indexes

	bool commUpperY = false;
	bool commUpperX = false;

	if (rankX < (pMatSize - 1))commUpperX = true;
	if (rankY < (pMatSize - 1))commUpperY = true;


	//Set Process color (Red/Black)
	bool isRed = false;
	if (rankY % 2 == 0){
		isRed = (rankX + 1) % 2;
	}
	else{
		isRed = rankX % 2;
	}



	//Perform Neighbor data sharing depending o

	if (isRed){//Behaviour for red processes
		if (commUpperX == true){
			//Communication with Right Neighbor
			MPI_Recv(neighborsRank, 1, MPI_INT, mRank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);//Receive from right Neighbor
			MPI_Send(&mRank, 1, MPI_INT, mRank + 1, 0, MPI_COMM_WORLD); //Send to right Neighbor
		}

		if (commLowerX == true){
			//Communication with Left Neighbor
			MPI_Recv(neighborsRank + 1, 1, MPI_INT, mRank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);//Receive from left Neighbor
			MPI_Send(&mRank, 1, MPI_INT, mRank - 1, 0, MPI_COMM_WORLD); //Send to right Neighbor
		}



		if (commUpperY == true){
			//Communication with Neighbor Below
			MPI_Recv(neighborsRank + 2, 1, MPI_INT, mRank + pMatSize, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);//Receive from right Neighbor
			MPI_Send(&mRank, 1, MPI_INT, mRank + pMatSize, 0, MPI_COMM_WORLD); //Send to right Neighbor
		}

		if (commLowerY == true){
			//Communication with Neighbor Above
			MPI_Recv(neighborsRank + 3, 1, MPI_INT, mRank - pMatSize, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);//Receive from left Neighbor
			MPI_Send(&mRank, 1, MPI_INT, mRank - pMatSize, 0, MPI_COMM_WORLD); //Send to right Neighbor
		}


		

	}
	else{//Behaviour for Black processes
		if (commLowerX == true){
			//Communication with Left Neighbor
			MPI_Send(&mRank, 1, MPI_INT, mRank - 1, 0, MPI_COMM_WORLD); //Send to left Neighbor
			MPI_Recv(neighborsRank + 1, 1, MPI_INT, mRank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);//Receive from left Neighbor
		}

		if (commUpperX == true){
			//Communication with right Neighbor
			MPI_Send(&mRank, 1, MPI_INT, mRank + 1, 0, MPI_COMM_WORLD); //Send to right Neighbor
			MPI_Recv(neighborsRank, 1, MPI_INT, mRank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);//Receive from right Neighbor
		}


		if (commLowerY == true){
			//Communication with Neighbor Above
			MPI_Send(&mRank, 1, MPI_INT, mRank - pMatSize, 0, MPI_COMM_WORLD); //Send to left Neighbor
			MPI_Recv(neighborsRank + 3, 1, MPI_INT, mRank - pMatSize, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);//Receive from left Neighbor
		}

		if (commUpperY == true){
			//Communication with Neighbor Below
			MPI_Send(&mRank, 1, MPI_INT, mRank + pMatSize, 0, MPI_COMM_WORLD); //Send to right Neighbor
			MPI_Recv(neighborsRank + 2, 1, MPI_INT, mRank + pMatSize, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);//Receive from right Neighbor
		}



	}


		
	MPI_Barrier(MPI_COMM_WORLD);

	delay(1000);

	//Print Neighbors for all processes
	printf("Process = %d | Left = %d | Right = %d | Up = %d | Down = %d\n", mRank, *(neighborsRank + 1), *(neighborsRank + 0), *(neighborsRank + 3), *(neighborsRank + 2));

	

	MPI_Finalize();

	free(neighborsRank);
	return 0;
}
