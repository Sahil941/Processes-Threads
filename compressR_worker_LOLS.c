/* Assignment 2
   Authors: Sahil Kumbhani, Sandeep Behera */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>

/* Compresses the input string using the RLE/LOLS
   compression algorithm and returns 0 if the
   string was compressed else, return -1 */
void *RLE(char *fileName, char *string, int part, int begin, int size){
	if (string == NULL){
		printf("ERROR: empty File was given.\n");
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
	
	// Creating renamed files that will contain process output
	FILE *pFile;
	if (part == 0){
		cmpName[i] = '_';
		cmpName[i + 1] = 'L';
		cmpName[i + 2] = 'O';
		cmpName[i + 3] = 'L';
		cmpName[i + 4] = 'S';
		cmpName[i + 5] = '0';
		cmpName[i + 6] = '\0';
		pFile = fopen(cmpName, "a+");
	}
	else{
		// numBuffer is used to convert integer to character for files names
		char numBuffer[4];
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
		pFile = fopen(cmpName, "a+");
	}
	
	int len = begin + size;
	int letterCounter = 0;
	char currLetter = string[begin];
	int b = begin;
	
	// RLE Compression 
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
				fprintf(pFile, "%c", currLetter);
			}
			else if (letterCounter == 2){
				fprintf(pFile, "%c%c", currLetter, currLetter);
			}
			else{
				if (letterCounter == 0){
					continue;
				}
				fprintf(pFile, "%d%c", letterCounter, currLetter);
			}
		}
		else{
			if (letterCounter == 1){
				fprintf(pFile, "%c", currLetter);
			}
			else if (letterCounter == 2){
				fprintf(pFile, "%c%c", currLetter, currLetter);
			}
			else{
				if (letterCounter == 0){
					continue;
				}
				fprintf(pFile, "%d%c", letterCounter, currLetter);
			}
			
			currLetter = string[b];
			letterCounter = 1;
		}
	}
	
	fclose(pFile);
	return NULL;
}


int main(int argc, char** argv){
	char *fileName = argv[1];
	char *string = argv[2];
	int part = atoi(argv[3]);
	int begin = atoi(argv[4]);
	int size = atoi(argv[5]);
	
	RLE(fileName, string, part, begin, size);
	
	return 0;
}