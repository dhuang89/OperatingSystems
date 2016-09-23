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

typedef struct threadStruct {
	pthread_t id;
	int max;
	int index;
} ThreadData;

void* maximum(void *a) {
	ThreadData *myThread = (ThreadData*) a;
	int max2 = input[myThread->index];

	if (input[myThread->index+1] > max2) {
		max2 = input[myThread->index+1];
	}
	myThread->max = max2;

	printf("Max is: %d\n", max2);	

	printf("waiting\n");
	pthread_barrier_wait(&barrier);
	printf("done waiting\n");
}

int main() {
	char breaker[20];
	int holder;
	bool takeInput = true;
	int ret = -1;
	int assignedIndex = 0;
	int finalMax;
	int rounds = 0;

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
	int countCopy = count;
	rounds = count/2;
	pthread_barrier_init(&barrier, NULL, count);
	ThreadData threads[count];
	int j;

	for (j = 0; j < count; j++) {
		printf("thread %d\n", j);
		threads[j].index = assignedIndex;
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

	while (rounds != 0) {
		//max function here
		int x;
		int y = 0;
		for (x = 0; x < countCopy; x+=2) {
			if (threads[x].max > threads[x+1].max) {
				threads[y] = threads[x];
			} else {
				threads[y] = threads[x+1];
			}
			y++;
		}

		countCopy = countCopy / 2;
		if (countCopy == 1) {
			finalMax = threads[0].max;
		}
		rounds--;
		printf("rounds: %d\n", rounds);
	}

	printf("Final max is: %d\n", finalMax);

}
