/**
 * 
 * Name : Rahul Kumar Nalubandhu
Citation for the following functions:
Date: 12/04/2022
Copied from /OR/ Adapted from /OR/ Based on: Source URL: Initial skeleton was taken from https://replit.com/@cs344/83clientc?lite=true#client.c
later few fuctions are refrenced from class notes 4.2 and 4.3 lecture slides
* Client code
* 1. Create a socket and connect to the server specified in the command arugments.
* 2. Prompt the user for input and send that input as a message to the server.
* 3. Print the message received from the server and exit the program.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <ctype.h>
#include "utility.h"

int main(int argc, char *argv[])
{
	int socketFD, portNumber, charsWritten, charsRead, bytesread;
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;
	char buffer[1024];
	char plaintext[100000];
// Check usage & args
	if (argc < 3) { 
		fprintf(stderr, "USAGE: %s hostname port\n", argv[0]); 
		exit(1); 
	} 

	//Set up the server address struct
	memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[3]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverHostInfo = gethostbyname("localhost"); // Convert the machine name into a special form of address
	
	if (serverHostInfo == NULL) { 
		fprintf(stderr, "CLIENT: ERROR, no such host\n"); 
		exit(0); 
	}

	memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address
	
	// Create a socket
	socketFD = socket(AF_INET, SOCK_STREAM, 0); 
	if (socketFD < 0) {
		error("CLIENT: ERROR opening socket");
	}

	////making socket to reuse
	int yes = 1;
	setsockopt(socketFD, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)); 

	// Connect to server
	if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0){ // Connect socket to address
		error("CLIENT: ERROR connecting");
	}

	
	//get the  file length.
	
	long filelength = fileLength(argv[1]);
	long keylength = fileLength(argv[2]);

	//check if the file is greater than the key.
	if(filelength > keylength)
		error("Key is too short!");
	
	//send message to server to conenct to correct server (enc_server).
	char* msg = "dec_client_connect_req";
	charsWritten = send(socketFD, msg, strlen(msg), 0);

	// Clear the buffer
	memset(buffer, '\0', sizeof(buffer)); 
	
	charsWritten = 0;
	//get return message from server, leaving room for a null terminator
	while(charsWritten == 0) 
		charsWritten = recv(socketFD, buffer, sizeof(buffer) - 1, 0);

	//checking whther the connection was successful
	//if conenction not successful throw error
	if(strcmp(buffer, "no") == 0) 
		error("dec_client : Bad client : Connection refused from server!");		
	
	//clear buffer again.
	memset(buffer, '\0', sizeof(buffer)); 

	//insert the file length into the buffer
	sprintf(buffer, "%d", filelength);
	
	//Send the file length to the server
	charsWritten = send(socketFD, buffer, sizeof(buffer), 0);
	
	//clear buffer.
	memset(buffer, '\0', sizeof(buffer)); 
	
	//open the plaintext file as a file descriptor.
	int file_desc = open(argv[1], 'r');
	charsWritten = 0;
		
	//send the plaintext to the server
	while(charsWritten <= filelength)
	{
		//read data from the file.
		bytesread = read(file_desc, buffer, sizeof(buffer)-1);
		//send the data to the server
		charsWritten += send(socketFD, buffer, strlen(buffer), 0);	
	}

	// //acknowledge msg sent  // debugging
	// if(charsWritten <= filelength){
	// 	printf("message sent\n");
	// }
	
	//clearing buffer
	memset(buffer, '\0', sizeof(buffer));
	
	//key.
	file_desc = open(argv[2], 'r');
	charsWritten = 0;

	//sending the key 
	while(charsWritten <= filelength)
	{
		bytesread = read(file_desc, buffer, sizeof(buffer)-1);
		charsWritten += send(socketFD, buffer, strlen(buffer), 0);
		
	}
	// //acknowledge key sent // debugging
	// if(charsWritten <= filelength){
	// 	printf("key sent\n");
	// }
	

	//clear the buffer.
	memset(buffer, '\0', sizeof(buffer)); 
	charsWritten = 0;

	// recieving the plain text
	while(charsWritten < filelength)
	{	
		//get the message
		charsWritten += recv(socketFD, buffer, sizeof(buffer)-1, 0);
		
		strcat(plaintext, buffer);// concatinate the recieved characters to cipher text
	}
	// //acknowledge cipher recv // debugging
	// if(charsWritten <filelength ){
	// 	printf("key recieved\n");
	// }

	//adding new line charc to end of plain
	strcat(plaintext, "\n");

	//print the plain 
	printf("%s", plaintext);
	
	close(socketFD); // Close the socket
	return 0;
}
