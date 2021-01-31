#include <stdio.h>
#include <string.h>


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