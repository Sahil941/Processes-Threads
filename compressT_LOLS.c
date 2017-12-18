/* Assignment 2
   Authors: Sahil Kumbhani, Sandeep Behera */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>


/* Compresses given string pointed to by string into RLE/LOLS
   forms and returns pointer to new string */
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

// Struct for thread arguments
struct t_args{
	char *fileName;
	char *string;
	int part;
	int begin;
	int size;
};

void *RLE(void *thread_args){
	struct t_args *args = thread_args;
	char *fileName = args->fileName;
	char *string = args->string;
	int part = args->part;
	int size = args->size;
	int begin = args->begin;
	
	// Sends parents thread signal to end wait
	pthread_mutex_lock(&m);
	pthread_cond_signal(&c);
	pthread_mutex_unlock(&m);

	if (string == NULL){
		printf("ERROR: empty File given.\n");
		return NULL;
	}

	// Creating new files that will contain compressed parts 
	char cmpName[strlen(fileName) + 9];
	memcpy(cmpName, fileName, strlen(fileName));
	int i;
	for (i = 0; i < strlen(fileName); i++){
		if (cmpName[i] == '.'){
			cmpName[i] = '_';
		}
	}
	
	// Creating renamed files that will contain thread output
	FILE *tFile; 
	if (part == 0){
		cmpName[i] = '_';
		cmpName[i + 1] = 'L';
		cmpName[i + 2] = 'O';
		cmpName[i + 3] = 'L';
		cmpName[i + 4] = 'S';
		cmpName[i + 5] = '0';
		cmpName[i + 6] = '\0';
		tFile = fopen(cmpName, "a+");
	}
	else{
		char numBuffer[4]; // numBuffer is used to convert int to char for files names
		sprintf(numBuffer, "%d", part);

		cmpName[i] = '_';
		cmpName[i + 1] = 'L';
		cmpName[i + 2] = 'O';
		cmpName[i + 3] = 'L';
		cmpName[i + 4] = 'S';
		cmpName[i + 5] = numBuffer[0];
		cmpName[i + 6] = numBuffer[1];
		cmpName[i + 7] = numBuffer[2];
		cmpName[i + 8] = '\0';
		tFile = fopen(cmpName, "a+");
	}

	int len = begin + size;
	int letterCounter = 0;
	char currLetter = string[begin];

	// RLE compression 
	for (b = begin; b <= len; b++){
		if (!isalpha(currLetter)){
			currLetter = string[b];
			letterCounter = 1;
		}
		else if ((currLetter == string[b]) && (b != len)){
			letterCounter++;
		}
		else if (b == len){
			if (letterCounter == 1){
				fprintf(tFile, "%c", currLetter);
			}
			else if (letterCounter == 2){
				fprintf(tFile, "%c%c", currLetter, currLetter);
			}
			else{
				if (letterCounter == 0){
					continue;
				}
				fprintf(tFile, "%d%c", letterCounter, currLetter);
			}
		}
		else{
			if (letterCounter == 1){
				fprintf(tFile, "%c", currLetter);
			}
			else if (letterCounter == 2){
				fprintf(tFile, "%c%c", currLetter, currLetter);
			}
			else{
				if (letterCounter == 0){
					continue;
				}
				fprintf(tFile, "%d%c", letterCounter, currLetter);
			}

			currLetter = string[b];
			letterCounter = 1;
		}
	}
	
	fclose(tFile);
	return NULL;
}

void compressT_LOLS(char *fileName, int parts){
	// Checks to see if enough parts exists
	if (parts < 1){
		printf("ERROR: incorrect number of parts given.\n");
		return;
	}
	
	/* Following code checks if file exists, then
	   finds size of all partitions for each thread */
	if ((fileName == NULL) || (access(fileName, R_OK) == -1)){
		printf("ERROR: incorrect File-name give.\n");
		return;
	}

	FILE *uncompressed = fopen(fileName, "r");
	fseek(uncompressed, 0, SEEK_END);
	int size = ftell(uncompressed);
	fseek(uncompressed, 0, SEEK_SET); 
	if (size < parts){
		printf("ERROR: size of uncompressed string is less than number of parts.\n");
		return;
	}
	
	// Create buffer to copy string into
	char buffer[size + 1];
	fgets(buffer, size + 1, (FILE *)uncompressed);
	buffer[size] = '\0';
	
	// Get partition sizes
	int partSize = size / parts;
	int firstPart = partSize + (size % parts);
	//printf("%d, %d\n", partSize, firstPart);
	
	// Creates argument struct for thread creation
	struct t_args *targs = (struct t_args *)malloc(sizeof(struct t_args));
	targs->fileName = fileName;
	targs->string = buffer;
	targs->size = firstPart;
	targs->begin = 0;
	
	// tid array to keep track of threads
	pthread_t tids[parts];
	int i;
	for (i = 0; i < parts; i++){
		targs->part = i;
		pthread_create(&tids[i], NULL, *RLE, targs);

		/* Waits for signal from child thread.
		   Wait is needed because args struct is changed
		   by for-loop before thread can access it */
		pthread_mutex_lock (&mutex);
		pthread_cond_wait (&cond, &mutex);
		pthread_mutex_unlock (&mutex);
		
		// Adjusts partition sizes
		if (i == 0){
			targs->begin = firstPart;
			targs->size = partSize;
		}
		else{
			targs->begin += partSize;
		}
	}

	// Joins all threads before program ends
	int j;
	for(j = 0; j < parts; j++){
		pthread_join(tids[j], NULL);
	}
}

int main(int argc, char** argv){
	compressT_LOLS("test.txt", 5);

	return 0;
}