/**
 * 
 * Name : Rahul Kumar Nalubandhu
Citation for the following functions:
Date: 12/04/2022
Copied from /OR/ Adapted from /OR/ Based on: Source URL: Initial skeleton was taken from https://replit.com/@cs344/83clientc?lite=true#client.c
later few fuctions are refrenced from class notes 4.2 and 4.3 lecture slides*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>

void error(const char *msg) { errno = 1; perror(msg);exit(1); } // Error function used for reporting issues
/* geting legnth of the file*/

int fileLength(const char* filename){
	int character;
	int count = 0;

	//opening file in read mode
	FILE* file = fopen(filename, "r");

	//get the first character of the file
	character = fgetc(file);

	//read until the end of file or null
    	while (!(character == EOF || character == '\n')) 
	{
		if(!isupper(character) && character != ' ')
			error("File contains bad characters!");
		
		//get the next character
		character = fgetc(file);
        	++count;
    	}

	//close the file.
	fclose(file);

	//return the file legnth
	return count;
}


/*retrun the asci value of the characters*/
int char_ascii (char c){

	//if c is a space
	if (c == ' '){
		return 26;
	}
	//therwise subtract the ascii of A from the character
	else {
		return (c - 'A');
	}
	return 0;
}


/*returns the charcter for ascii value*/
char ascii_char(int i){

	//if 26, return a space
	if (i == 26){
		return ' ';
	}
	//else return the integer + ascii of A
	else 
	{
		return (i + 'A');
	}
}	

/*add each char of the messsage with each char of key and mod 27 to get a nnum using that found ascii charc*/

void encrypt(char  plaintext[], char key[]){ //encrypt a given message
	int index=0;
	char ascii;

	//for the length of the message
	while (plaintext[index] != '\n')
	{
			//get the character
	  		char c = plaintext[index];
	  		ascii = (char_ascii(plaintext[index]) + char_ascii(key[index])) % 27;
	  		plaintext[index] = ascii_char(ascii);
			index++;
	}
	
	plaintext[index] = '\0';
	return;
}


/*we decrpt the cipher text using the same key 
we subract the ascii value of each charc in cipher text with each ascii char value of key and % 27 to get the charc*/
void decrypt(char cipher[], char key[])
{
	int index=0;
	char ascii;
	//for the length of the message
	while (cipher[index] != '\n')
	{
		
		ascii = char_ascii(cipher[index]) - char_ascii(key[index]);
		//result is negative then add 27
	  	if (ascii<0)
			ascii += 27;
	  	cipher[index] = ascii_char(ascii);
		index++;
	  }

	  cipher[index] = '\0';
	  return;
}
