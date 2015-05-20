/*NOTES OF IMPLEMENTATION */
/*This example is meant to be for a World Comm of size 20*/


#include <sys/time.h> 
#include <stdio.h>
#include "mpi.h"
#include <stdlib.h>
#include <time.h>
#define FALSE 0
#define TRUE 1
typedef int bool;

int main(int argc, char **argv) {

	//----------------------------------------------
	//Added: rankReceiver
	//----------------------------------------------
	int * rankReceiver;
	rankReceiver = (int *)malloc(sizeof(int));
	*rankReceiver = -1;
  



	//Declaration of groups to be used
	MPI_Group worldGroup; //Group abstraction from the World Communicator
	MPI_Group firstGroup; //Group of processors with Even Rank in World Group
	MPI_Group secondGroup;//Group of processors with Odd Rank in World Group
	bool hasFirstGroup = FALSE;
	bool hasSecondGroup = FALSE;


	//Declaration of communicator that will be tied to the new Groups groups
	MPI_Comm myFirstComm;
	MPI_Comm mySecondComm;

	
	
	//Declaration of variables to store ranks per communicator	
	int whoAmI_WorldComm=-1;
	int whoAmI_FirstComm=-1;
	int whoAmI_SecondComm=-1;

	


	//Declaration of variables to store Comm/Group sizes
	int worldSize;
	int firstSize;
	int secondSize;

	MPI_Init(&argc, &argv);


	//Build array with the rank list for each new group
	MPI_Comm_size(MPI_COMM_WORLD, &worldSize);

	int  ranksFirstGroupA[12] = {0,2,4,6,8,10,12,14,16,18,9,11}; // Odd/Even ranks + the 2 ranks to represent an intersection
	int  ranksFirstGroupB[8] = {1,3,5,7,13,15,17,19}; // Odd/Even ranks + the 2 ranks to represent an intersection

	int ranksSecondGroupA[12] = {1,3,5,7,9,11,13,15,17,19,8,10}; // Odd/Even ranks + the 2 ranks to represent an intersection
	int ranksSecondGroupB[8] = {0,2,4,6,12,14,16,18}; // Odd/Even ranks + the 2 ranks to represent an intersection


	//Get my rank in World Communicator
	MPI_Comm_rank(MPI_COMM_WORLD, &whoAmI_WorldComm);	


	//Extract the Group from the World Communicator
	//--------------------------------------------------------------
	//int MPI_Comm_group(  MPI_Comm comm  , 	MPI_Group *group);
	//--------------------------------------------------------------
	MPI_Comm_group(MPI_COMM_WORLD, &worldGroup);


	//Include odd and even processes in WorldGroup to its new belonging group
	//---------------------------------------------------------------------------------
	//int MPI_Group_incl(	MPI_Group group, int n, int *ranks,	MPI_Group *newgroup	);
	//---------------------------------------------------------------------------------

	//Code for First Group
	if (whoAmI_WorldComm % 2 == 0 || whoAmI_WorldComm == 9 || whoAmI_WorldComm == 11){
		//printf("1A Rank = %d\n", whoAmI_WorldComm);
		MPI_Group_incl(worldGroup, 12, ranksFirstGroupA, &firstGroup);
		hasFirstGroup = TRUE;
	}
	else{
		//printf("1B Rank = %d\n", whoAmI_WorldComm);
		MPI_Group_incl(worldGroup, 8, ranksFirstGroupB, &firstGroup);
	}


	//Code for Second Group
	if (whoAmI_WorldComm % 2 == 1 || whoAmI_WorldComm == 8 || whoAmI_WorldComm == 10){
		//printf("2A Rank = %d\n", whoAmI_WorldComm);
		MPI_Group_incl(worldGroup, 12, ranksSecondGroupA, &secondGroup);
		hasSecondGroup = TRUE;
	}
	else{
		//printf("2B Rank = %d\n", whoAmI_WorldComm);
		MPI_Group_incl(worldGroup, 8, ranksSecondGroupB, &secondGroup);
	}

	

	MPI_Barrier(MPI_COMM_WORLD);

	

	//Create first and second communicator 
	//--------------------------------------------------------------------------------
	//int MPI_Comm_create(MPI_Comm comm,	MPI_Group group, MPI_Comm *newcomm	);
	//--------------------------------------------------------------------------------
	MPI_Comm_create(MPI_COMM_WORLD, firstGroup, &myFirstComm);
	MPI_Comm_create(MPI_COMM_WORLD, secondGroup, &mySecondComm);


	//MPI_Barrier(MPI_COMM_WORLD);


	//Get Processor Name and new ranks
	int nameLength;
	char name[MPI_MAX_PROCESSOR_NAME];
	MPI_Get_processor_name(name, &nameLength);

	MPI_Comm_rank(myFirstComm, &whoAmI_FirstComm);
	MPI_Comm_rank(mySecondComm, &whoAmI_SecondComm);



	//-------------------------------------------------------------------------------
	//Added: Bradcast of rank within the Second Group (Odd numbers + 8 and 10)
	//-------------------------------------------------------------------------------
	if (hasSecondGroup){
		MPI_Comm_size(mySecondComm , &secondSize);
		int broadcaster = (int)(secondSize/2);
		if (whoAmI_SecondComm == broadcaster){
			*rankReceiver = whoAmI_SecondComm;
		}
		MPI_Bcast(rankReceiver, 1, MPI_INT, broadcaster, mySecondComm);
	}

	//Next is printed all the available processors in COMM_WORLD plus the value for rankReceiver. We should notice that only the Processes
	//belongint to the Second communicator, with which the broadcast was used, should have a value different to -1 for rankReceiver.
	printf("Proc name = %s | World rank = %d | rankReceiver = %d\n", name, whoAmI_WorldComm, *rankReceiver);



	MPI_Finalize();
	return 0;
}
