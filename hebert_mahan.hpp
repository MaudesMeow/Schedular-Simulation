/*
 * Authors:	Jordan Hebert & Oliver Mahan
 * Date:		23 June 2022
 * Purpose:	Scheduler simulation
 */

#ifndef HEBERT_MAHAN_H
#define HEBERT_MAHAN_H

#include <queue>

#define INPUT_MAX 200			// to read in the first x values (at most) from the ProcessArray

#define ISTEST 3 // Whether to use the test file or the actual file
#if ISTEST == 1 
		#define FILE_LOCATION "10_processes.txt"
#elif ISTEST == 2
		#define FILE_LOCATION "500k_Processes_Raw.txt"
#elif ISTEST == 3
		#define FILE_LOCATION "1m_Processes.txt"
#endif

#define BASE 2 // factor by which each queue's time quantum increases going down.

typedef struct process{
		int pid; 			//ID of the process.
		int arrival; 	//When the process arrived to the scheduler.
		int burst;		//How much CPU time the proces still needs.
		int ageing; 	//To keep track of how long it has been in the lowest queue.
		int initialBurst; //To calculate wait time.
} Process;


/*
 tokenizes line based on tabs
 stores each number into the values array
 */
void splitLine(std::string line, int values[] );


/*
Used for the sort() function to compare two processes based on arrival time.
True(1) if a < b, False(0) otherwise
 */
bool compareArrival(process a, process b);


/*
Used for the sort() function to compare two processes base on PID.
True(1) if a < b, False(0) otherwise
 */
bool comparePID(process a, process b);


#endif // #ifndef HEBERT_MAHAN_H

