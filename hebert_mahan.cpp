#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <math.h>
#include <algorithm>
#include "hebert_mahan.hpp"
using namespace std;

int main() {

// variables 
		int numberOfQueues;
		int timeQuantumValue;
		int ageingValue;
		vector<process> allProcesses;
		
		// prompting user for necessary parameters

		cout << "Enter number of queues: " << endl;
		cin >> numberOfQueues;
		while  (numberOfQueues < 3 || 5 < numberOfQueues){
				cout << "Number of queues must be greater than 2 but less than 6." << endl;
				cin >> numberOfQueues;
		}

		// create the queues and a vector to hold them
		vector<queue<process> > queueArray;	

		for (int i = 0; i < (numberOfQueues - 1); i++) {
				queue<process> temp;
				queueArray.push_back(temp);
		}

		cout << "Enter time quantum for top queue: " << endl;
		cin >> timeQuantumValue;
	
		cout << "Enter ageing interval: " << endl;
		cin >> ageingValue;

		//taking in the document to read for lines 

		string fileName = "";
		cout << "Enter file name: " << endl;
		cin >> fileName;

		ifstream scheduledJobs(fileName);
		int retry = 0;

		while (!scheduledJobs.is_open()) {
				cout << "That file doesn't exist. Press 1 to retry, or any other key to use default (" << FILE_LOCATION << "): " << endl;
				cin >> retry;
				if (retry == 1) {
						cout << "Enter file name: " << endl;
						cin >> fileName;
						scheduledJobs.open(fileName);
				} else {
						scheduledJobs.open(FILE_LOCATION);
				}
		}

		string line;

		// to skip the column titles:
		getline(scheduledJobs, line);

		int values [3];

		while (getline(scheduledJobs,line)){

				splitLine(line, values); 
				
				if (values[0] > 0) { // only add the process if it is valid (no negative burst or arrival times).

					// create the process and store the values in it.
						process temp;
						temp.pid = values[0];
						temp.burst = values[1];
						temp.arrival = values[2];
						temp.initialBurst = values[1];

						allProcesses.push_back(temp);	// add the process to the vector containing all the processes.
				}

		}


		// figure out how many processes are actually in the array
		int numProcesses = allProcesses.size();

//	sort all processes by arrival time
		sort(allProcesses.begin(), allProcesses.end(), compareArrival);

		int globalTicks = 0; 	// GLOBAL TICK COUNTER *************************
		int iterator = 0; 		// never reset this, will make sure we only add each process once and don't miss any
		vector< process > currentTickProcess;
		int processorTimeRemaining = 0;
		int previousQueue = 0;		// keeps track of which queue the process was in before it went to the CPU
		process CPU [1];
		vector<process> lastQueue;
		int nextProcessQueue = 0;
		bool rrflag; // to indicate whether the round robin queues are empty. True: they are empty
		long int totalWaitTime = 0;
		long int totalTurnaroundTime = 0;

// create a "null" placeholder process
		process null;
		null.pid = -1;
		null.burst = -1;
		null.arrival = -1;
		null.initialBurst = -1;

		CPU[0] = null;

while (1) { // global while loop

		// while loop to add all processes where arrival time == current global tick		//
		while (allProcesses[iterator].pid != '\0') { 																		//
				if (allProcesses[iterator].arrival == globalTicks) {												//
						currentTickProcess.push_back(allProcesses[iterator]);										//
						iterator++;																															//
				}																																						//
				else {																																			//
						break;																																	//
				}																																						//
		}																																								// adding new processes
																																										//
		// sort the processes in the arrival buffer by PID															//
		sort(currentTickProcess.begin(), currentTickProcess.end(), comparePID);					//
																																										//
		// adds the processes in order to the first queue																//
		while (!currentTickProcess.empty()) {																						//
				queueArray[0].push(currentTickProcess.back());															//
				cout << "Process " << currentTickProcess.back().pid << ":\tarrives @\t" << globalTicks << endl;
				currentTickProcess.pop_back();																							//
		}																																								//
																																										
		if (CPU[0].pid != -1) { // valid process in CPU																			//
				if (processorTimeRemaining > 1 && CPU[0].burst > 1) {// still work to do				//
						processorTimeRemaining--;																										//
						CPU[0].burst--;																															//
																																												//
				} else if (CPU[0].burst == 1) {	// IT IS DONE																		//
						cout << "Process " << CPU[0].pid << ":\tfinished @\t" << globalTicks << endl;
						int processTurnaroundTime = globalTicks - CPU[0].arrival;
						int processWaitTime = processTurnaroundTime - CPU[0].initialBurst;
						totalWaitTime += processWaitTime;
						totalTurnaroundTime += processTurnaroundTime;
						CPU[0] = null;																															//
																																												//
				} else { // KICK IT OUT																													//
						CPU[0].burst--;
						if (previousQueue == (numberOfQueues - 2)) { // the second to last queue		//
								CPU[0].ageing = ageingValue + 1; // since it decrements immediately			//
								lastQueue.push_back(CPU[0]);																						//
						}																																						//
						else {	// to the next queue down																						//
								queueArray[previousQueue + 1].push(CPU[0]);															//
						}																																						//
						cout << "Process " << CPU[0].pid << ":\tkicked @\t" << globalTicks << endl;	//
						CPU[0] = null;																															//	
						processorTimeRemaining = 0;
				}																																								//
																																												//
		}

	 	if (CPU[0].pid == -1) {	// CPU has 'null' process (empty)														//
																																												//
				rrflag = true;																																	//
				for (int i = 0; i < (numberOfQueues - 1); i++) {																//	
						if (!queueArray[i].empty()){																								//
								nextProcessQueue = i;																										//		
								rrflag = false;																													//
								break;																																	//
						}																																						//
				}																																								//	
																																												//
				if (rrflag) {																																		//
						if (!lastQueue.empty()) {																										//
								nextProcessQueue = (numberOfQueues - 1);																//		
						}	else {																																		//
								nextProcessQueue = -1;																									//		
						}																																						//
				}																																								//
																																												//
				if (nextProcessQueue == -1 && globalTicks > 700) { // all queues empty, we are done									//
						float avgWait = ((float) totalWaitTime) / numProcesses;											//
						float avgTurnaround = ((float) totalTurnaroundTime) / numProcesses;					//
						cout << "Avg. wait time = " << avgWait << "\t\t" << "Avg. turnaround time = " << avgTurnaround << endl;
						cout << "Total processes scheduled = " << numProcesses << endl;							//
																																												//
						return 0;																																		//
																																												//
				} else if (!queueArray[nextProcessQueue].empty()){ 																																				//
						if (nextProcessQueue < numberOfQueues - 1) { // not the last queue					//
								CPU[0] = queueArray[nextProcessQueue].front();													//
								queueArray[nextProcessQueue].pop();																			// dealing with the CPU
						}	else {	// last queue																											//
								CPU[0] = lastQueue[0];																									//
								lastQueue.erase(lastQueue.begin());																			//
						}																																						//
						cout << "Process " << CPU[0].pid << ":\truns @\t\t" << globalTicks << endl;	//
						previousQueue = nextProcessQueue;																						//
						if (nextProcessQueue == numberOfQueues - 1) { // comes from last queue			//
								processorTimeRemaining = 10000;
								cout << "from last queue" << endl;
						} else {
								processorTimeRemaining = timeQuantumValue * pow(BASE,(nextProcessQueue));				//	
						}
				} else {}																																				//
		}


		for (int i = 0; i < lastQueue.size(); i++) {													//
				if (lastQueue[i].ageing > 1) { // so it gets aged up this tick		//
						lastQueue[i].ageing--;																				//
				} else { 																													// ageing the last queue
						queueArray[numberOfQueues - 2].push(lastQueue[i]);						//
						lastQueue.erase(lastQueue.begin() + i);												//
						i--; // since we deleted an element														//
				}																																	//	
		}																																			//


	globalTicks++;

} // global while loop

		return 0;
}



void splitLine(string line, int values[]) {
		size_t i = 0;
		string temp;
		int index = 0;

		if (line.size() > 0) {
				while (line[i] != '\0' && index < 3) {

// this 'if' statement is only for this specific project, since the only time you see a '-', it means a negative number.						
						if (line[i] == '-') {
								values[0] = -1;
								return;
						}
// Comment this out if you want to include the correct negative values in the file 


						if (line[i] == '0' || line[i] == '1' || line[i] == '2' || line[i] == '3' || line[i] == '4' || line[i] == '5' || line[i] == '6' || line[i] == '7' || line[i] == '8' || line[i] == '9'){

/*	if you want to send back negative values. inefficient for this project
								if (i > 0 && line[i-1] == '-') {
										temp.append(1, '-');
								}
*/

								temp.append(1, line[i]);
						}
						else if (line[i] == '\t') {
								values[index] = stoi(temp);
								if (index == 2) {
										return;
								}
								index++;
								temp = "";
						}
						i++;
				}
				values[index] = stoi(temp);
		}
}


bool compareArrival(process a, process b) {
		if (a.arrival < b.arrival) {
				return 1;
		}
		else {
				return 0;
		}
}

bool comparePID(process a, process b) {
		if (a.pid > b.pid) {
				return 1;
		}
		else {
				return 0;
		}
}



