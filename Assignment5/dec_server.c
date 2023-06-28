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
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include "utility.h"


int main(int argc, char *argv[])
{
	int listenSocketFD, establishedConnectionFD, portNumber, charsWritten, status;
	socklen_t sizeOfClientInfo;
	struct sockaddr_in serverAddress, clientAddress;
	pid_t pid;

	if (argc < 2) { fprintf(stderr, "USAGE: %s port\n", argv[0]); exit(1); } // Check usage & args

	// Set up the address struct for this process (the server)
	memset((char *)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[1]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverAddress.sin_addr.s_addr = INADDR_ANY; // Any address is allowed for connection to this process

	// Create the socket
	listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); 
	if (listenSocketFD < 0) error("ERROR opening socket");

	// Enable the socket to begin listening
	if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to port
		error("ERROR on binding");

	//looping infinetly in server
	while(1)
	{
		// Flip the socket on - it can now receive up to 5 connections
		listen(listenSocketFD, 5); 	
		// Accept a connection, blocking if one is not available until one connects
		sizeOfClientInfo = sizeof(clientAddress); // Get the size of the address for the client that will connect
		establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept
		if (establishedConnectionFD < 0) error("ERROR on accept");

		//create children
		pid = fork();
		switch (pid){

			//fork errors
			case -1:
			{
				error("Error! Couldn't fork!\n");
			}
			//instructions for child 
			case 0:
			{
				char buffer[1024];
				char* encryptedMessage[100000];
				char message[100000];
				char key[100000];

				//initialize the buffer
				memset(buffer, '\0', sizeof(buffer));

				charsWritten = 0;

				//read the message from client
				while(charsWritten == 0)
					charsWritten = recv(establishedConnectionFD, buffer, sizeof(buffer)-1, 0);
			
				//check if it is valid
				if(strcmp(buffer, "dec_client_connect_req") != 0)
				{
					charsWritten = send(establishedConnectionFD, "no", 2, 0);
					exit(2);
				}
				else
				{
					//clear buffer
					memset(buffer, '\0', sizeof(buffer));

					//send "yes" 
					charsWritten = send(establishedConnectionFD, "yes", 3, 0);
					charsWritten = 0;

					//get the file length
					while(charsWritten == 0)
						charsWritten = recv(establishedConnectionFD, buffer, sizeof(buffer)-1, 0);
					//length to an integer		
					int size = atoi(buffer);
					
					charsWritten = 0;
			
					memset(buffer, '\0', sizeof(buffer));	
					//got the cipher text from the client
					while(charsWritten < size)
					{					
						//got the message
						charsWritten += recv(establishedConnectionFD, buffer, sizeof(buffer)-1, 0);
						
		
						strcat(message, buffer); //concatinate the recieved characters to cipher text
					}

					// //acknowledge msg recv 
					// if(charsWritten <size ){
					// 	printf("message recieved\n");
					// }

					//clear the buffer
					memset(buffer, '\0', sizeof(buffer));
					charsWritten = 0;
					
					//recieving the key
					while(charsWritten < size)
					{						
						charsWritten += recv(establishedConnectionFD, buffer, sizeof(buffer)-1, 0);
						strcat(key, buffer);
					}

					// //acknowledge key recv 
					// if(charsWritten <size ){
					// 	printf("kwy recieved\n");
					// }

					//clear the buffer
					memset(buffer, '\0', sizeof(buffer));
		
					//edecrypt the message using key 
					decrypt(message, key);
									
					//send decrypt text to client
					charsWritten = 0;
					while(charsWritten < size)
						charsWritten += send(establishedConnectionFD, message, sizeof(message), 0);
					
					// //acknowledge cipher sent 
					// if(charsWritten <= size){
					// 	printf("decrypt plain text sent\n");
					// }
					//exit
					exit(0);

				}
			}
			//parent process
			default:
			{
				// wait(NULL);
				pid_t actualpid = waitpid(pid, &status, WNOHANG);
			}
		}
		//close the client socket
		close(establishedConnectionFD);
	}
	//close the listen socket
	close(listenSocketFD); 
	return 0;
}
