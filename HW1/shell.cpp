//Dennis Huang
//dlh4fx
//CS 4414-001

/** Questions
 * max length for standard input
 * null character counts as a char
 * EOF
 * new line delimiter
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <cstring>

//contains all valid chars
char reference[71] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
        			'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
					'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.', '-', '_', '<', '>', '|', '/', '\n', ' '};

//check input command for invalid chars
bool validate(char* input) {
	bool isValid;
	int inputLength = strlen(input); //length of parameter

	//iterate through all chars
	for (int i = 0; i < inputLength; i++) {
		isValid = false;

		for (int j = 0; j < 71; j++) {
			if (input[i] == reference[j]) { //mark char as valid
				isValid = true;
				break;
			}
		}
		//invalid char detected
		if (isValid == false) {
			break;
		}
	}

	return isValid;
}

//function to interpret a command typed in the shell
void interpret(char* line[]) {
	int status;
	//int lineLength = strlen(line);

	//printf("Interpret this: %s\n", line);

	// char* argv[1];
	// argv[0] = line[0];

	char* args[4];
	args[0] = line[0];
	args[1] = "-l";
	args[2] = "-i";
	args[3] = NULL;

	int pid = fork();

	if (pid == 0) {
		execvp(args[0], args);
		printf("$ERROR: Command failed.\n");
		exit(0);
	} 

	wait();
}

int main () {
	char command[255]; //initializing string of command
	int length; //length of command
	char *new_command;
	bool execute;
	
	while (true) {
		printf("$"); //start shell
		execute = true;
		fgets(command, 255, stdin); //read in standard input
		new_command = strtok(command, "\n");
		length = strlen(new_command); //length of command input

		if (length > 100) { //if command is more than 100 characters, print out an error
			printf("$ERROR: Exceeded maximum length of 100 characters.\n");
			
		}
		else if (strcmp(new_command, "exit") == 0) { //break out of shell command
			break;
		} 
		else if (!validate(new_command)) { //invalid char is input
			printf("$ERROR: Invalid character found.\n");
		}
		else {
			//parse each line into an array of strings
			const char s[2] = " "; //split command by spaces
			char *strArray[50]; //store into preset array with length of 50
			int i = 0;
			int arrayLen = 0; //track array's length
			char *token;
			token = strtok(new_command, s);

			while (token != NULL) {
				strArray[i++] = token;
				token = strtok(NULL, s);
				arrayLen++;
			}

			//go through array and validate syntax
			for (i = 0; i < arrayLen; i++) {
				printf("Array: %s\n", strArray[i]);

				if (i == 0 && (strcmp(strArray[i], ">") == 0 || strcmp(strArray[i], "<") == 0 || strcmp(strArray[i], "|") == 0)) {
					//if operator is part of a word
					// if (strlen(strArray[i]) > 1) {
					// 	printf("$ERROR: Operator cannot be part of a word.\n");
					// 	break; 
					// }
					execute = false;
					printf("$ERROR: Input must begin with a word.\n");
				}

				if (strcmp(strArray[i], ">>") == 0 || strcmp(strArray[i], "<<") == 0 || strcmp(strArray[i], "<>") == 0 || strcmp(strArray[i], "><") == 0) {
					execute = false;
					printf("$ERROR: Output redirection operators cannot be next to one another.\n");
				}

				if (strcmp(strArray[i], "<") == 0 || strcmp(strArray[i], ">") == 0 || strcmp(strArray[i], "|") == 0) {
					if (strcmp(strArray[i + 1], "|") == 0 || strcmp(strArray[i + 1], "<") == 0 || strcmp(strArray[i + 1], ">") == 0) {
						execute = false;
						printf("$ERROR: Operators cannot be succeeded with another operator.\n");
					}
				}

			}

			//interpret the command
			if (execute == true) {
				interpret(strArray);
			}


		}
	}
	return (0);
}
