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

int main(int argc, char **argv){

	int status;
	int pid;
	int parent2Child[2];
	int child2Parent[2];
	
	if (argc < 2) {
		printf("Please enter a file name.\nUsage: ./programName <file_name>\n");
		return -1;
	}

	if (!validateFile(argv[1])){
		printf("Cannot open specified file!\nMake sure file is of type .txt!\n");
		return -1;

	}

	FILE *file = fopen(argv[1], "r");
	long length = fileLength(file);

    char sent[length+1];
	char rcvd[length+1];

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
    	fread(sent, 1, length, file);
    	fclose(file);
    	sent[length] = '\0';
    	printf("Process 1 starts sending the data to Process 2 ...\n");	
		write(parent2Child[WRITE_END], sent, strlen(sent)+1);
		close(parent2Child[WRITE_END]);
		// printf("Parent waiting for child to complete:\n");
		pid = wait(&status);
		// printf("Parent continues after child finishes:\n");

		close(child2Parent[WRITE_END]);
		int numberOfWordsFromChild;
		read(child2Parent[READ_END], &numberOfWordsFromChild, sizeof(numberOfWordsFromChild));
		printf("Process 1: The total number of words is %d.\n", numberOfWordsFromChild);
		close(child2Parent[READ_END]);
	}
	
	else {
		close(parent2Child[WRITE_END]);
		read(parent2Child[READ_END], rcvd, length);
		printf("Process 2 finishes receiving data from Process 1 ...\n");	
		printf("Process 2 is counting the words now ...\n");	
		int numberOfWords = wordCount(rcvd, length);
		close(parent2Child[READ_END]);

		close(child2Parent[READ_END]);
		printf("Process 2 is sending the results back to Process 1 ...\n");	
		write(child2Parent[WRITE_END], &numberOfWords, sizeof(numberOfWords));
		close(child2Parent[WRITE_END]);
	}

}
	
int wordCount(char *text, int length){

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

int validateFile(char *fileName) {
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

long fileLength(FILE *file) {

	fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);
    return length;
}

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
		usleep(5);
	}
	for (i = 0; i < totalNumberOfBars - numberOfBars; i++)
	{
		printf(" ");
	}
	printf("] %d%%",rate);
}