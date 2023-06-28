#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

void key_gen(int size ){
    const char alphabets[] = {'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',' '};    
	char key[1 + size]; // 1 space for null char
    key[size] = '\0'; // last char is null	
	// randomly select a char from an array and create a cipher out of them    
	for (int i = 0; i < size; ++i)	
	 	key[i] = alphabets[random() % 27];
	// to stdout
	printf("%s\n", key);
	return 0;

}
int main(int argc, char* argv[]){
    int size;
    const char *argmnt_name;

  // call once for random time
	srand(time(0));    
	if (argc != 2){
        argmnt_name = argv[0];
		fprintf(stderr, "Usage: %s size\n", argmnt_name);
		exit(1);
	}
	size = atoi(argv[1]);
    
	key_gen(size);

    return 0;
}
