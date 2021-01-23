#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>

#define READ_END 0
#define WRITE_END 1

int main(int argc, char **argv){

	if (argc < 2) {
		printf("Please enter a file name.\nUsage: ./programName <file_name>\n");
		return -1;
	}

	if (!validateFile(argv[1])){
		printf("Cannot open specified file!\nMake sure file is of type .txt!\n");
		return -1;

	}

	int status;
	int pid;
	int fdP2C[2];
	int fdC2P[2];
	

	FILE *file = fopen(argv[1], "r");
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    char sent[length+1];
	char rcvd[length+1];
    fseek(file, 0, SEEK_SET);

    
	if (pipe(fdP2C) == -1 || pipe(fdC2P) == -1){
		printf("Error creating pipe!\n");
		return 1;
	}

	pid = fork();

	if (pid < 0) {
		printf("Cant create thread!");
	}

	else if (pid > 0) {
		printf("Parent Thread:\n");	
		close(fdP2C[READ_END]);
    	fread(sent, 1, length, file);
    	fclose(file);
    	sent[length] = '\0';
		write(fdP2C[WRITE_END], sent, strlen(sent)+1);
		close(fdP2C[WRITE_END]);
		printf("Parent waiting for child to complete:\n");
		pid = wait(&status);
		printf("Parent continues after child finishes:\n");

		close(fdC2P[WRITE_END]);
		int numberOfWordsFromChild;
		read(fdC2P[READ_END], &numberOfWordsFromChild, sizeof(numberOfWordsFromChild));
		printf("Number of words in the file (child -> parent): %d\n", numberOfWordsFromChild);
		close(fdC2P[READ_END]);
	}
	
	else {
		printf("Chid Thread:\n");	
		close(fdP2C[WRITE_END]);
		read(fdP2C[READ_END], rcvd, length);
		int numberOfWords = wordCount(rcvd);
		printf("Child is Counting the words:\n");
		close(fdP2C[READ_END]);

		close(fdC2P[READ_END]);
		write(fdC2P[WRITE_END], &numberOfWords, sizeof(numberOfWords));
		close(fdC2P[WRITE_END]);
	}

}
	
int wordCount(char *text){

    int counter=0;
    int i=0;
    while(text[i] != '\0'){
        if (text[i] == ' '){
            counter++;
        }
        i++;
    }
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