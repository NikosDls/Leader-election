/*	Dollas Nikolaos A.M.: 2113007
 *	Lesson: Distributed Systems
 * 	Semester Project
 *	Subject: Leader Election (node with the lowest ID wins the election)
 *	Itah & Rodeh Algorithm
 *	Lamia 2016-2017
 */

#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef enum {false, true} bool;	// new type bool with values: false or true

int main(int argc, char **argv){
	int rank, size, retVal;	// process rank, number of processes and return value from mpi initialize
	int min, localCounter;	// minimun value for the election and counter
	
	int id;				// the node ID
	int inMessage[2];	// in going message in place 0, and counter in place 1
  	int outMessage[2];	// out going message in place 0, and counter in place 1
	
	MPI_Status status;	// mpi status
	
	time_t t;	// t variable for srand
			
	bool stop = false;	// boolean variable for the loop
	
	retVal = MPI_Init(&argc, &argv);		// we initialize the mpi
	if(retVal != MPI_SUCCESS){				// we check if mpi started correctly
		printf("MPI cannot be started!\n");
		MPI_Abort(MPI_COMM_WORLD, retVal);	// we stop the mpi
	}
	
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);	// we save the process rank into rank variable
	MPI_Comm_size(MPI_COMM_WORLD, &size);	// we save the number or processes into size variable
	
	srand(time(NULL) + rank);	// we are creating the seed for the random number
	id = rand() % 80;			// we set random number to ID
	/* id = rand() % size */	// the right node ID setup. ID between 0-N(number or nodes(size variable))
	
	outMessage[0] = id;		// set node id equal to id
  	outMessage[1] = 0;		// message counter (if inMessage counter = N(number of processes) we know that message did his cycle)
  	
  	min = id; 			// initialize minimun value equal to node own id
  	localCounter = 0; 	// local counter of received messages inside the loop
  	
  	MPI_Send(&outMessage, 2, MPI_INT, (rank + 1) % size, 1, MPI_COMM_WORLD);	// send my ID with counter to the right	
  	while(!stop){	// loop while process cannot find any leader
  		MPI_Recv(&inMessage, 2, MPI_INT, (rank - 1) % size, 1, MPI_COMM_WORLD, &status);	// receive ID message with counter from left node
	    localCounter++;	// increase local counter
	    
	    if(min > inMessage[0]){	// if incomming message is greater than minimum value
			min = inMessage[0];	// we update minimum value
		}
      		
    	inMessage[1] = inMessage[1] + 1; // increase the message counter before forwarding
    	
    	if(inMessage[0] == id && localCounter == (inMessage[1])){	// if ID is equal with process rank AND localCounter is equal with message counter
      		printf("Node %d (ID %d):\tnode with ID = %d is the leader!\n", rank, id, min);
      		stop = true;	// current process fund the node with smaller ID
    	}
    	
    	/*
    	if(inMessage[0] == id && localCounter != (inMessage[1])){	// if ID is equal with process rank AND localCounter isnt equal with message counter means that we have processes with the same ID
      		// code to reset the MPI
    	}
    	*/
   		
	    MPI_Send(&inMessage, 2, MPI_INT, (rank + 1) % size, 1, MPI_COMM_WORLD); // send the received ID with counter to the right
	}
	
	MPI_Finalize();	// we finish and close the mpi 
	
	if(id == min){	// the winner node announce his election
		printf("\n\nNODE %d IS THE LEADER!\n\n", rank);
	}
	
    return 0;
}

