//Dennis Huang
//dlh4fx
//CS 4414-001

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>

int i = 0;
int input[255];
pthread_barrier_t barrier;
int rounds = 0;
int finArr[255];
int countCopy;
int finalMax;

typedef struct threadStruct {
	pthread_t id;
	int max;
	int index;
	int otherIndex;
} ThreadData;

void* maximum(void *a) {
	ThreadData *myThread = (ThreadData*) a;
	int max2 = input[myThread->index];

	if (input[myThread->index+1] > max2) {
		max2 = input[myThread->index+1];
	}
	myThread->max = max2;
	int numRounds = rounds;
	finArr[myThread->otherIndex] = myThread->max;
	pthread_barrier_wait(&barrier);

	while (numRounds != 0) {
		if (myThread->otherIndex % 2 == 0) {
			if (finArr[myThread->otherIndex] < finArr[myThread->otherIndex+1]) {
				if (myThread->otherIndex > 0) {
					if (finArr[myThread->otherIndex/2] < finArr[myThread->otherIndex+1]) {
						finArr[myThread->otherIndex/2] = finArr[myThread->otherIndex+1];
					}
					
				} else {
					finArr[myThread->otherIndex] = finArr[myThread->otherIndex+1];
				}
				
			}

			if (finArr[myThread->otherIndex] > finArr[myThread->otherIndex+1]) {
				if (myThread->otherIndex > 0) {
					if (finArr[myThread->otherIndex/2] > finArr[myThread->otherIndex+1]) {
						finArr[myThread->otherIndex/2] = finArr[myThread->otherIndex+1];
					}
					finArr[myThread->otherIndex/2] = finArr[myThread->otherIndex];
				}
			}
		}

		pthread_barrier_wait(&barrier);
		numRounds--;
	}

	printf("Max is: %d\n", max2);	

	
}

int main() {
	char breaker[20];
	int holder;
	bool takeInput = true;
	int ret = -1;
	int assignedIndex = 0;
	int finalIndex = 0;


	while (takeInput == true) {
		printf("Enter a number: ");
		fgets(breaker, 20, stdin);

		if (strcmp(breaker, "\n") == 0) {
			break;
		}

		int holder = atoi(breaker);
		input[i] = holder;
		i++;
	}

	int count = i / 2;
	countCopy = count;
	rounds = count/2;
	pthread_barrier_init(&barrier, NULL, count);
	ThreadData threads[count];
	int j;

	for (j = 0; j < count; j++) {
		printf("thread %d\n", j);
		threads[j].index = assignedIndex;
		threads[j].otherIndex = finalIndex;
		finalIndex++;
		assignedIndex += 2; 
		ret = pthread_create(&(threads[j].id), NULL, &maximum, (void*) (threads+j));

		if (ret != 0) {
			printf("Error creating thread\n");
			exit(1);
		}
	}	



	int k;

	for (k = 0; k < count; k++) {
		printf("joining thread %d\n", k);
		pthread_join(threads[k].id, NULL);
	}
	finalMax = finArr[0];
	printf("Final max is: %d\n", finalMax);

}
