//Dennis Huang
//dlh4fx
//CS 4414-001

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

//global variables
int i = 0; //counts number of input ints there are
int input[255]; //array that contains user input
//pthread_barrier_t barrier;
int rounds = 0; //number of rounds of max computing there will be
int finArr[255]; //array that holds ints and is used to compute the max
int countCopy; //copy of the int count
int finalMax; //final max integer printed out to stdout
sem_t mutex; //mutex semaphore
sem_t barrier; //turnstile semaphores
sem_t barrier2;
int semcount;

//struct of each thread
typedef struct threadStruct {
	pthread_t id; //id of pthread
	int max; //max int
	int index; //index used for array
	int otherIndex; //index used for finArr
} ThreadData;

//method that is used for threads, computes the maximum
void* maximum(void *a) {
	ThreadData *myThread = (ThreadData*) a; //cast to struct
	int max2 = input[myThread->index]; 

	//each thread is responsible for two indices, therefore two integers
	//find the max and store it in the struct
	if (input[myThread->index+1] > max2) {
		max2 = input[myThread->index+1];
	}
	myThread->max = max2;
	int numRounds = rounds; //number of rounds of comparing integers to find max
	finArr[myThread->otherIndex] = myThread->max; //store value into finArr


	sem_wait(&mutex);
	semcount++;
	if (semcount == countCopy) {
		sem_wait(&barrier2); //lock second barrier
		sem_post(&barrier); //unlock first
	}
	sem_post(&mutex); 
	sem_wait(&barrier); //signal first barrier
	sem_post(&barrier);

	//max finding algorithm. after every round, numRounds is decremented
	while (numRounds != 0) {
		if (myThread->otherIndex % 2 == 0) { //only need half of threads after initial round
			if (finArr[myThread->otherIndex] < finArr[myThread->otherIndex+1]) { //if int is less than int next to it
				if (myThread->otherIndex > 0) { //if not in index of 0
					//check to make sure index writing to does not contain greater int
					if (finArr[myThread->otherIndex/2] < finArr[myThread->otherIndex+1]) { 
						finArr[myThread->otherIndex/2] = finArr[myThread->otherIndex+1];
					}
					
				} else {
					finArr[myThread->otherIndex] = finArr[myThread->otherIndex+1];
				}
				
			}
			//if int is greater than int next to it
			if (finArr[myThread->otherIndex] > finArr[myThread->otherIndex+1]) {
				if (myThread->otherIndex > 0) { 
					//check to make sure index writing to is actually greater than current int
					if (finArr[myThread->otherIndex/2] > finArr[myThread->otherIndex+1]) {
						finArr[myThread->otherIndex/2] = finArr[myThread->otherIndex+1];
					}
					finArr[myThread->otherIndex/2] = finArr[myThread->otherIndex];
				}
			}
		}

		sem_wait(&mutex);
		semcount--;
		if (semcount == 0) {
			sem_wait(&barrier); //lock first barrier
			sem_post(&barrier2); //unlock second
		}
		sem_post(&mutex);
		sem_wait(&barrier2); //signal second barrier
		sem_post(&barrier2);
		numRounds--;
	}
	
}

int main() {
	char breaker[20]; //string char looking for newline character
	int holder; //int being typed in 
	bool takeInput = true;
	int ret = -1;
	int assignedIndex = 0;
	int finalIndex = 0;

	//keep accepting user input until newline char
	while (takeInput == true) {
		fgets(breaker, 20, stdin);

		if (strcmp(breaker, "\n") == 0) {
			break;
		}

		int holder = atoi(breaker);
		input[i] = holder;
		i++;
	}

	int count = i / 2; //determines number of threads needed
	countCopy = count;
	rounds = count/2;
	//pthread_barrier_init(&barrier, NULL, count);
	ThreadData threads[count]; //initialize array of structs
	sem_init(&mutex, 0, 1); //intialize needed semaphores
	sem_init(&barrier, 0, 0);
	sem_init(&barrier2, 0, 1);
	
	int j;
	//create threads
	for (j = 0; j < count; j++) {
		threads[j].index = assignedIndex; //assign initial values
		threads[j].otherIndex = finalIndex;
		finalIndex++;
		assignedIndex += 2; 
		//creates thread, tells it to go to maximum function
		ret = pthread_create(&(threads[j].id), NULL, &maximum, (void*) (threads+j));

		if (ret != 0) {
			printf("Error creating thread\n");
			exit(1);
		}
	}	


	//joins threads at the end
	int k;
	for (k = 0; k < count; k++) {
		pthread_join(threads[k].id, NULL);
	}
	finalMax = finArr[0];

	//print out absolute max to stdout
	printf("%d\n", finalMax);

}
