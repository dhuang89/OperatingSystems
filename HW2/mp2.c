//Dennis Huang
//dlh4fx
//CS 4414-001

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>


int main() {

	int input[255];
	char breaker[20];
	int holder;
	int i = 0;
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
	printf("here\n");
	int j;
	for (j = 0; j < i; j++) {
		printf("Number in array: %d\n", input[j]);
	}
	

}
