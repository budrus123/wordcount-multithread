
/**
* Creates progress bar animation on the stdout 
* by filling up empty spaces with '#'.
*
* @param rate: completion rate out of 100.
* 
*/
void progressBar(int rate);

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
int wordCount(char *text, int length);

/**
* Checks if the <input_file> is a txt file or not
* by looking for '.txt' at the end.
*
* @param fileName: the name of the file to be checked.
* 
* @returns 1 if file is a .txt file and returns 0 if not.
*/
int validateTextFile(char *fileName);

/**
* Returns the total size (length of the file), 
* meaning the total number of chars in the file.
*
* @param file: file which length of chars will be calculated.
* 
* @returns length of file in chars (long).
*/
long fileLength(FILE *file);