#include <stdio.h>
#include <string.h>
#include <wait.h>
#include "helpers.h"

#define READ_END 0
#define WRITE_END 1

long fileLength(FILE *file);
void progressBar(int rate);

/**
* Basic program steps
*
* 1.  Check to make sure the program has at least 2 args
* 2.  Make sure second argument is a text file name
* 3.  Make sure the file can be opened
* 4.  Create two pipes and handle errors
* 5.  Create the child process and handle errors
* 6.  Parent process reads file and calculates the total file length
* 7.  Parent process sends the length and data to the Child
* 8.  Child reads the length of the file from the buffer
* 9.  Child reads and stores the data (file string)
* 10. Child counts the number of words
* 11. Child sends number of words to Parent
* 12. Parent receives and prints the number of words
*/


/**
* Basic protocol for handling text different string (buffer lengths)
*
* Since the child does not know the size of the buffer to store the 
* string in, the parent first sends the total file length to the child.
* This way the child knows what to expect, size wise, for the string buffer.
*/

int main(int argc, char **argv) {

	int pid;
	int parent2Child[2];
	int child2Parent[2];
	char *fileName = argv[1];

	if (argc < 2) {
		printf("Please enter a file name.\nUsage: ./programName <file_name>\n");
		return -1;
	}

	FILE *file = fopen(fileName, "r");

	// Handling if the file cannot be opened.
	if (file == 0) {
		printf("Could not open file: %s.\n",fileName );
		return -1;
	}

	// Handling if the file is not of .txt extension.
	if (!validateTextFile(fileName)) {
		printf("Make sure file is of type .txt!\n");
		return -1;
	}

	// Handling error in pipe creation.
	if (pipe(parent2Child) == -1 || pipe(child2Parent) == -1){
		printf("Error creating pipe!\n");
		return 1;
	}

	pid = fork();

	if (pid < 0) {
		printf("Cant create thread!");
		return -1;
	}

	/*
		Parent Process:
		- Reads file
		- Sends file length
		- Sends file content
		- Waits for and prints the word count
	*/
	else if (pid > 0) {
		printf("Process 1 is reading the input file '%s' now ...\n", argv[1]);	
		close(parent2Child[READ_END]);
		long length = fileLength(file);
		char sent[length+1];
		fread(sent, 1, length, file);
		fclose(file);
		sent[length] = '\0';
		printf("Process 1 starts sending the data to Process 2 ...\n");	
		write(parent2Child[WRITE_END], &length, sizeof(long));
		write(parent2Child[WRITE_END], sent, strlen(sent)+1);

		close(parent2Child[WRITE_END]);
		pid = wait(NULL);
		close(child2Parent[WRITE_END]);
		int numberOfWordsFromChild;
		read(child2Parent[READ_END], &numberOfWordsFromChild, sizeof(numberOfWordsFromChild));
		printf("Process 1: The total number of words is %d.", numberOfWordsFromChild);
		if (numberOfWordsFromChild == 0) {
			printf(" File is empty.");
		}
		printf("\n");
		close(child2Parent[READ_END]);
	}
	
	/*
		Child Process:
		- Recieves length from Parent
		- Recieves file content
		- Counts the actual number of words
		- Sends the word count back to Parent
	*/
	else {
		close(parent2Child[WRITE_END]);
		long totalLength;
		read(parent2Child[READ_END], &totalLength, sizeof(totalLength));
		char rcvd[totalLength+1];
		read(parent2Child[READ_END], rcvd, totalLength);
		printf("Process 2 finishes receiving data from Process 1 ...\n");	
		printf("Process 2 is counting the words now ...\n");	
		int numberOfWords = 0;
		if (totalLength > 0) {
			numberOfWords = wordCount(rcvd, totalLength);
		}
		close(child2Parent[READ_END]);
		printf("Process 2 is sending the results back to Process 1 ...\n");	
		write(child2Parent[WRITE_END], &numberOfWords, sizeof(numberOfWords));
		close(child2Parent[WRITE_END]);
	}

}