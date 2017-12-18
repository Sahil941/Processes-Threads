/* Assignment 2
   Authors: Sahil Kumbhani, Sandeep Behera */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>

void compressR_LOLS(char *fileName, int parts){
	// Error checking to see if the file name and parts given are incorrect
	if ((fileName == NULL) || (access(fileName, R_OK) == -1)){ // Checks to see if the given file name is accessible
		printf("ERROR: incorrect FILE name given.\n");
		return;
	}
	else if (parts < 1){ // Checks to see if the incorrect number of parts is given
		printf("ERROR: incorrect number of parts given.\n");
		return;
	}
	else if ((parts < 1) && ((fileName == NULL) || (access(fileName, R_OK) == -1))){ // Checks to see if both parameters are incorrect
		printf("ERROR: incorrect FILE name and number of parts given.\n");
		return;
	}

	/* Opens given file and checks to see if the length
	   of the string is less than the number of parts */
	FILE *uncompressed = fopen(fileName, "r");
	fseek(uncompressed, 0, SEEK_END);
	int inputLength = ftell(uncompressed);
	fseek(uncompressed, 0, SEEK_SET);
	//fclose(uncompressed);
	if (inputLength < parts){
		printf("ERROR: the number of parts is greater than the uncompressed string.\n");
		return;
	}

	// Creates a buffer to take the string into
	char buffer[inputLength + 1];
	fgets(buffer, (inputLength + 1), (FILE *)uncompressed);
	buffer[inputLength] = '\0';

	// Get the size of each part
	int partSize = inputLength / parts;
	int firstPart = partSize + (inputLength % parts);
	int index;
	int partitionSize = firstPart;
	int start = 0;

	// Converts arguments to type char * for execl() function
	char indexBuffer[20];
	char startBuffer[20];
	sprintf(startBuffer, "%d", start);
	char partitionSizeBuffer[20];
	sprintf(partitionSizeBuffer, "%d", partitionSize);

	// Array that stores the PIDs for each process
	pid_t pids[parts];
	int i;

	for (i = 0; i < parts; ++i){
		index = i;
		sprintf(indexBuffer, "%d", index);
		pids[i] = fork();

		if (pids[i] < 0){
			perror("fork");
			abort();
		}
		else if (pids[i] == 0){
			execl("./compressR_worker_LOLS", "compressR_worker_LOLS", fileName, buffer, indexBuffer, startBuffer, partitionSizeBuffer, NULL);
			exit(0);
		}

		if (i == 0){
			start = firstPart;
			sprintf(startBuffer, "%d", start);
			partitionSize = partSize;
			sprintf(partitionSizeBuffer, "%d", partitionSize);
		}
		else{
			start += partSize;
			sprintf(startBuffer, "%d", start);
		}
	}

	// Waits until all processes are complete
	int status;
	pid_t pid;
	int j;
	for (j = parts; j > 0; --j){
		pid = wait(&status);
		//printf("Child with PID: %ld exited with status 0x%x.\n", (long)pid, status);
	}

	fclose(uncompressed);
	return;
}

int main(int argc, char const *argv[]){
	compressR_LOLS("test.txt", 5);
	
	return 0;
}