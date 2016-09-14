//Dennis Huang
//dlh4fx
//CS 4414-001

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>

int i = 0;

void* maximum(void *a) {
	int size = i;
	int* arr = (int*) a;
	int x;
	int max = arr[0];
	int current;

	for (x = 0; x < size; x++) {
		current = arr[x];
		if (current > max) {
			max = current;
		}
	}

	printf("Max is: %d\n", max);	
}

int main() {

	int input[255];
	char breaker[20];
	int holder;
	bool takeInput = true;

	while (takeInput == true) {
		printf("Enter a number: ");
		fgets(breaker, 20, stdin);

		if (strcmp(breaker, "\n") == 0) {
			printf("breaking\n");
			break;
		}

		int holder = atoi(breaker);
		input[i] = holder;
		i++;
	}

	pthread_t thread1;

	pthread_create(&thread1, NULL, &maximum, (void*) input);
	pthread_join(thread1, NULL);

}
