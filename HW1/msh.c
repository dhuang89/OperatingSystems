//Dennis Huang
//dlh4fx
//CS 4414-001

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

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
void interpret(char* line[], int length, int pipeNum) {
	int status;
	int lineLength = length;
	//char* args[lineLength];
	int pipefd[2];
	pipe(pipefd);
	bool pipeBool = false;
	int output_file;
	bool output = false;
	char* output_name;
	int input_file;
	bool input = false;
	char* input_name;
    char *argv1[10];
    char *argv2[10];
    int helper;
    int helper2 = 0;
    int track = 0;

    if (pipeNum > 0) {
    	pipeBool = true;
    	for (track = 0; track < lineLength; track++) {
    		if (strcmp(line[track], "|") == 0) {
    			helper = track;
    			//args[helper] = NULL;
    			for (int jj = 0; jj < helper; jj++) {
    				argv1[jj] = line[jj];

    			}
    		}
    	}

    	for (helper = 4; helper < lineLength; helper++) {
    		argv2[helper2] = line[helper];
    		//args[helper2] = NULL;
    		helper2++;
    	}
    } else {
    	for (int i = 0; i < lineLength; i++) {
    		argv1[i] = line[i];
    	}
    }

	for (int i = 0; i < lineLength; i++) {
		//redirection
		//args[i] = line[i];

		if (strcmp(line[i], ">") == 0) {
			output = true;
			output_name = line[i+1];
			argv1[i] = NULL;
			argv1[i+1] = NULL;
		}

		if (strcmp(line[i], "<") == 0) {
			input = true;
			input_name = line[i+1];
			argv1[i] = NULL;
			argv1[i+1] = NULL;
		}

	}
	argv1[lineLength] = NULL;

	if (output) {
		output_file = open(output_name, O_CREAT | O_RDWR, 0777);
	}

	if (input) {
		input_file = open(input_name, O_CREAT | O_RDONLY, 0777);
	}

	int pid = fork();

	if (pid == 0) {

		if (output) {
			dup2(output_file, 1);
		}

		if (input) {
			dup2(input_file, 0);
		}
		if (pipeBool == true) {
			dup2(pipefd[1], 1);
        	close(pipefd[0]);
		}
		
		execvp(argv1[0], argv1);
		printf("ERROR: Command failed.\n");
		exit(0);
	} 

	int pid2 = fork();
    if (pid2 == 0) {
        dup2(pipefd[0], 0);
        close(pipefd[1]);
        execvp(argv2[0], argv2);
        printf("ERROR: Command failed.\n");
      
    }
       close(pipefd[0]);
       close(pipefd[1]);

	wait(&status);

	if (output) {
		close(output_file);
	}

	if (input) {
		close(input_file);
	}
}

int main () {
	char command[255]; //initializing string of command
	int length; //length of command
	char *new_command;
	bool execute;
	
	while (true) {
		//start shell
		execute = true;
		fgets(command, 255, stdin); //read in standard input
		new_command = strtok(command, "\n");
		length = strlen(new_command); //length of command input

		if (length > 100) { //if command is more than 100 characters, print out an error
			printf("ERROR: Exceeded maximum length of 100 characters.\n");
			
		}
		else if (strcmp(new_command, "exit") == 0) { //break out of shell command
			break;
		} 
		else if (!validate(new_command)) { //invalid char is input
			printf("ERROR: Invalid character found.\n");
		}
		else {
			//parse each line into an array of strings
			const char s[2] = " "; //split command by spaces
			char *strArray[50]; //store into preset array with length of 50
			int i = 0;
			int arrayLen = 0; //track array's length
			int pipeCount = 0;
			char *token;
			token = strtok(new_command, s);

			while (token != NULL) {
				strArray[i++] = token;
				token = strtok(NULL, s);
				arrayLen++;
			}

			//go through array and validate syntax
			for (i = 0; i < arrayLen; i++) {

				int innerLen = strlen(strArray[i]);

				if (strcmp(strArray[i], "|") == 0) {
					pipeCount++;
				}

				if (i == 0 && (strcmp(strArray[i], ">") == 0 || strcmp(strArray[i], "<") == 0 || strcmp(strArray[i], "|") == 0)) {
					execute = false;
					printf("ERROR: Input must begin with a word.\n");
				}

				if (strcmp(strArray[i], ">>") == 0 || strcmp(strArray[i], "<<") == 0 || strcmp(strArray[i], "<>") == 0 || strcmp(strArray[i], "><") == 0) {
					execute = false;
					printf("ERROR: Output redirection operators cannot be next to one another.\n");
				}

				if (strcmp(strArray[i], "<") == 0 || strcmp(strArray[i], ">") == 0 || strcmp(strArray[i], "|") == 0) {
					if (strcmp(strArray[i + 1], "|") == 0 || strcmp(strArray[i + 1], "<") == 0 || strcmp(strArray[i + 1], ">") == 0) {
						execute = false;
						printf("ERROR: Operators cannot be succeeded with another operator.\n");
					}
				}

				if (strcmp(strArray[i], "|") == 0 && i == arrayLen - 1) {
					execute = false;
					printf("ERROR: Pipe command must be separating two words.\n");
				}

				// for (int z = 0; z < innerLen; z++) {
				// 	char* innerTok = strArray[i];
				// 	int innerTokLen = strlen(innerTok);

				// 	if (innerTok[z] == "|" && innerTokLen > 0) {
				// 		execute = false;
				// 		printf("ERROR: Pipe operator cannot be part of a word.\n");
				// 	}
				// }

			}

			//interpret the command
			if (execute == true) {
				interpret(strArray, arrayLen, pipeCount);
			}


		}
	}
	return (0);
}