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
	int i;
	int j;
	for (i = 0; i < inputLength; i++) {
		isValid = false;

		for (j = 0; j < 71; j++) {
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
	int status; //used to wait for processes to finish
	int forkNum = 0; //number of forks
	int ii = 0; //used to keep track of command index
	int lineLength = length; //number of arguments in command
	int pipefd[2 * pipeNum]; //create necessary size for pipes
	int output_file;
	bool output = false;
	char* output_name;
	int input_file;
	bool input = false;
	char* input_name;
	int pipeCount = 0;
	int pipeNumClone = pipeNum;
	int readIndex = -2;
	int writeIndex = -1;

	//create the pipes
	int k;
	for (k = 0; k < pipeNum; k++) {
		pipe(pipefd + 2*k);
	}

	//while there are still forks present
	while (forkNum < pipeNum + 1) {
		char *args[200];
		int argIndex = 0;
		int argLen = 0;
		bool write = false;
		bool both = false;
		bool read = false;

		if (pipeCount == pipeNum) {
			// You've already passed the last pipe. Next args will only read from pipe.
			read = true;
		}

		// iterates through command, checks for pipe character
		for (; ii < lineLength; ii++) {
			if (strcmp(line[ii], "|") == 0 && read == false) { //if a pipe character is found and not reading
				pipeCount++; //increment to keep track of number of pipes
				if (pipeCount == 1) { //first pipe, must be writing
					write = true;
				}
				if (pipeCount > 1) { //not the first pipe and not reading, must be doing both
					both = true;
				}
				break;
			}
			argLen++; //keep track of number of arguments
			args[argIndex] = line[ii]; //add word to new array
			argIndex++; //track of argument index
		}
		args[argIndex] = NULL; //make last character in array null
		ii++; //increment past the pipe (if any) so next iteration will avoid pipe character

		//increment through the array and check for input/output redirection
		int zz;
		for (zz = 0; zz < argLen; zz++) {
			if (strcmp(args[zz], ">") == 0) {
				output = true;
			}
			if (strcmp(args[zz], "<") == 0) {
				input = true;
			}
		}

		//create files for redirection
		if (output) {
			int i;
			for (i = 0; i < argLen; i++) {
				if (strcmp(args[i], ">") == 0) {
					output_name = args[i+1];
					output_file = open(output_name, O_CREAT | O_RDWR, 0777);
					args[i] = NULL;
					args[i+1] = NULL;
					break;
				}
			}
		} 
	
		if (input) {
			int j;
			for (j = 0; j < argLen; j++) {
				if (strcmp(args[j], "<") == 0) {
					input_name = args[j+1];
					input_file = open(input_name, O_CREAT | O_RDONLY, 0777);
					args[j] = NULL;
					args[j+1] = NULL;
					break;
				}
			}
		}

		forkNum++;

		//for piping. if performing any action, increment the index by 2 for the next pipe
		if (write == true) {
			writeIndex += 2;
		}

		if (read == true) {
			readIndex += 2;
		}

		if (both == true) {
			writeIndex+=2;
			readIndex+=2;
		}

		int pid = fork();

		if (pid == 0) {
			//for redirection
			if (output) {
				dup2(output_file, 1);
			}

			if (input) {
				dup2(input_file, 0);
			}

			//for piping
			if (pipeNumClone > 0) {
				if (write == true) { //redirecting stdout for next pipe
					if (dup2(pipefd[writeIndex], 1) == -1) {
						perror("Dup2 error writing");
					}

				} else if (write == false && both == false) { //reading stdin from previous pipe
					if (dup2(pipefd[readIndex], 0) == -1) {
						perror("Dup2 error reading");
						_exit(1);
					}
				} else if (both == true) {
					if(dup2(pipefd[readIndex], 0) == -1) { //reading stdin from previous pipe and redirecting stdout
						perror("Dup2 error reading for both");
					}
					if(dup2(pipefd[writeIndex], 1) == -1) {
						perror("Dup2 error writing for both");
					}
				}
				//close the pipes
				int xx;
				for (xx = 0; xx < 2*pipeNum; xx++) {
					close(pipefd[xx]);
				}


			}
			//execute the command
			execvp(args[0], args);
			printf("ERROR: Command failed.\n");
			exit(0);

		} 

	}

	//make sure all pipes are closed
	int yy; 
	for (yy = 0; yy < 2*pipeNum; yy++) {
		close(pipefd[yy]);
	}
	//wait for processes to finish
	int jj;
	for (jj = 0; jj < pipeNumClone + 1; jj++) {
		wait(&status);
	}
	

	//close files if necessary
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
		printf("Enter a command: ");
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

				if (strcmp(strArray[i], ">") == 0 && i < arrayLen - 2) {
					if (strcmp(strArray[i + 2], "|") == 0) {
						printf("ERROR: Name of text file cannot be written into a pipe.\n");
						
					}
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
				//  char* innerTok = strArray[i];
				//  int innerTokLen = strlen(innerTok);

				//  if (innerTok[z] == "|" && innerTokLen > 0) {
				//      execute = false;
				//      printf("ERROR: Pipe operator cannot be part of a word.\n");
				//  }
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