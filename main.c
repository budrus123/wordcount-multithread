#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>
#include <unistd.h>


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

	if (!validateTextFile(fileName)) {
		printf("Make sure file is of type .txt!\n");
		return -1;

	}

	FILE *file = fopen(fileName, "r");

	if (file == 0) {
		printf("Could not open file: %s.\n",fileName );
	}

	if (pipe(parent2Child) == -1 || pipe(child2Parent) == -1){
		printf("Error creating pipe!\n");
		return 1;
	}

	pid = fork();

	if (pid < 0) {
		printf("Cant create thread!");
	}

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
		printf("Process 1: The total number of words is %d.\n", numberOfWordsFromChild);
		close(child2Parent[READ_END]);
	}
	
	else {
		close(parent2Child[WRITE_END]);
		long totalLength;
		read(parent2Child[READ_END], &totalLength, sizeof(totalLength));
		char rcvd[totalLength+1];
		read(parent2Child[READ_END], rcvd, totalLength);
		printf("Process 2 finishes receiving data from Process 1 ...\n");	
		printf("Process 2 is counting the words now ...\n");	
		int numberOfWords = wordCount(rcvd, totalLength);
		close(parent2Child[READ_END]);

		close(child2Parent[READ_END]);
		printf("Process 2 is sending the results back to Process 1 ...\n");	
		write(child2Parent[WRITE_END], &numberOfWords, sizeof(numberOfWords));
		close(child2Parent[WRITE_END]);
	}

}


/**
* Counts the words in the string in a simple manner.
* The counting is done by looking for spaces until the 
* string terminator is reached. Also, computes the completion
* rate, and sends it to the progreess bar drawing funtion.
*
* @param text: the string to count the words in.
* @param length: the total length of the string, used in progress
*				 bar drawing.
* 
* @returns the total number of words in the string.
*/
int wordCount(char *text, int length) {
	int counter=0;
	int i=0;
	float completion = 0;
	while(text[i] != '\0'){
		if (text[i] == ' '){
			counter++;
		}
		i++;
		completion = ((float)i) / length;
		progressBar((int) (completion * 100));

	}
	printf("\n");
	return counter + 1;
}

/**
* Checks if the <input_file> is a txt file or not
* by looking for '.txt' at the end.
*
* @param fileName: the name of the file to be checked.
* 
* @returns 1 if file is a .txt file and returns 0 if not.
*/
int validateTextFile(char *fileName) {
	int len = strlen(fileName);
	if (len < 4) {
		return 0;
	} else {
		char *lastFoure = &fileName[len-4];
		if(strcmp(lastFoure, ".txt") == 0){
			return 1;
		}
		return 0;
	}
	
}

/**
* Returns the total size (length of the file), 
* meaning the total number of chars in the file.
*
* @param file: file which length of chars will be calculated.
* 
* @returns length of file in chars (long).
*/
long fileLength(FILE *file) {

	fseek(file, 0, SEEK_END);
	long length = ftell(file);
	fseek(file, 0, SEEK_SET);
	return length;
}

/**
* Creates progress bar animation on the stdout 
* by filling up empty spaces with '#'.
*
* @param rate: completion rate out of 100.
* 
*/
void progressBar(int rate) {

	if (rate >= 100) {
		rate = 100;
	}
	int numberOfBars = (rate / 10) * 3;
	int totalNumberOfBars = 30;
	int i;
	fflush(stdout);
	printf("\rProgress: [");

	for (i = 0; i < numberOfBars; i++)
	{
		printf("#");
		// usleep(1);
	}
	for (i = 0; i < totalNumberOfBars - numberOfBars; i++)
	{
		printf(" ");
	}
	printf("] %d%%",rate);
}