//Dennis Huang
//dlh4fx
//CS 4414-001

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <cstring>
#include <sys/stat.h>
#include <sys/types.h>
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
void interpret(char* line[], int length) {
	int status;
	int lineLength = length;
	char* args[lineLength];

	int output_file;
	bool output = false;
	char* output_name;
	// args[0] = line[0];
	// args[1] = "-l";
	// args[2] = "-i";
	// args[3] = NULL;

	for (int i = 0; i < lineLength; i++) {
		//redirection
		args[i] = line[i];

		if (strcmp(line[i], ">") == 0) {
			output = true;
			output_name = line[i+1];
			args[i] = NULL;
			args[i+1] = NULL;
		}

	}
	args[lineLength] = NULL;

	if (output) {
		output_file = open(output_name, O_CREAT | O_RDWR, 0777);
	}

	int pid = fork();

	if (pid == 0) {

		if (output) {
			dup2(output_file, 1);
		}

		execvp(args[0], args);
		printf("ERROR: Command failed.\n");
		exit(0);
	} 

	wait();

	if (output) {
		close(output_file);
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
				interpret(strArray, arrayLen);
			}


		}
	}
	return (0);
}
