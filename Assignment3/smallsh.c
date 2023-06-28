/* Name : Rahul Kumar Nalubandhu
Date:11/06/2022
Citation for the following functions:Copied from /OR/ 
Adapted from /OR/ Based on: Source URL://https://pubs.opengroup.org/onlinepubs/9699919799/utilities/V3_chap02.html
http://codewiki.wikidot.com/system:page-tags/tag/shell-script#pages
some of them are from lecture modules and stack over flow and geeks for geek.
*/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>

#include "smallsh.h"

//foreground toggle flag usefull for context switching
int forgroundMode=0; 

int main(){    
    hook_interrupts();
    run_shell();
	return 0;
}

void hook_sigint(){
    //Signal handler when CTRL + c	is recieved
	sigintStruct.sa_handler = SIG_IGN;
	sigaction(SIGINT, &sigintStruct,NULL);	
}

void hook_sigstp(){
    //Signal handler when CTRL + z is recieved
	struct sigaction sigtstpAct={0};	
	sigtstpAct.sa_handler = &sigtstpHandlerRoutine;
	sigfillset(&sigtstpAct.sa_mask);
	sigtstpAct.sa_flags=SA_RESTART;
	sigaction(SIGTSTP, &sigtstpAct, NULL);
}

void hook_interrupts(){
    hook_sigstp();
    hook_sigint();
}

void run_shell(){

	//for holding a command
	char* raw_command= (char*)malloc(MAXCHARACTERS);    
	int length; // length of command	
    char* cur_token=NULL;
	char substituted[MAXCHARACTERS];
    
	//variables for input/output redirection
	char* in_file = NULL;
	char* out_file = NULL;
    int fp=-1; // general file pointer for handling files

	//list of arguments.
	char* arg_list[MAXARG]; 
    
    //toggle switch for background process.
	int background=FALSE;
    int status=0;		
    //to hold list of background process's pids.
	pid_t bg_pids_list[MAXPROCESS];
    int npids=0;
    int pid;		
	    
	while(TRUE){	
		
		background =FALSE;	//this will set the background to 0	

		
		printf(PROMPT); //this prints ":"
		clearFD(stdout);

		ssize_t size =0;
		length = getline(&raw_command, &size, stdin);//it takes the argumentes here
		if(length == -1)
			clearerr(stdin);
		
		//start tokenizing
		int arguments =0;		 

        if(length > 1){
            // break at new line
            cur_token= strtok(raw_command, NEWLINE);
            //input file redirection
            while(cur_token != NULL){
                if (equal(cur_token, INPUT_REDIRECTION)){
                    // extract input filename
                    cur_token=(strtok(NULL, NEWLINE));
                    in_file = strdup(cur_token);
                    cur_token = strtok(NULL, NEWLINE);
                }
            
                //output redirection
                else if(equal(cur_token, OUTPUT_REDIRECTION)){
                    //extract output filename
                    cur_token=(strtok(NULL, NEWLINE));
                    out_file = strdup(cur_token);
                    cur_token = strtok(NULL, NEWLINE);
                }
                else {
                    // checking if part of the cur_token has $$
                    
                    if(strstr(cur_token, EXPANSION)){
                        //convert the $$ to the shell Pid.
                        strcpy(substituted, cur_token);
                        apply_substitution(substituted);
                        cur_token=substituted;
                    }
			
                    //insert the cur_token to the arg_list
                    arg_list[arguments]=strdup(cur_token);
                    // read next token
                    cur_token=strtok(NULL, NEWLINE);
                    //increment token counter
                    arguments++;
                }
            }
            // traverse each token						
            arguments--;

            // checking if last char is &
            // and there is also a command
            if(arg_list[arguments] && equal(arg_list[arguments],"&")){
                //remove the & in argument array. If this doesn't happen
                //sleep commands will return an error.
                arg_list[arguments]='\0';
                //if background commands are allowed, set the background flag.
                if(forgroundMode==FALSE){
                    background=TRUE;
                }
            }
            arguments++;
            arg_list[arguments] = NULL;	
		
            // empty line or a comment        
            if(arg_list[0] == NULL || strncmp(arg_list[0],"#",1)==0);		

            else if(equal(arg_list[0],"exit")){
                //cleanig and exiting the shell
                kill_bg_processes(npids, bg_pids_list);
                exit(EXIT_SUCCESS);
            }
            //if entered cd        
            else if (equal(arg_list[0],"cd")) navigate_to_directory(arg_list);
        
            //if entered exit
            else if(equal(arg_list[0],"status")){
                if(WIFEXITED(status)) printf("exit value %d \n", WEXITSTATUS(status));
                //otherwise, the child was terminated by a signal.
                else printf("terminated by signal %d\n", status);
                clearFD(stdout);			
            }

            //otherwise, execute non-builtin command
            // by forking a child process
            else{
                pid = fork();
                //if there was an error forking the child process
                if(pid < 0){
                    perror("failed to fork!/n");
                    kill_bg_processes(npids,bg_pids_list);
                    status =1;
                    break;
                }
                //child process will run non-built-in commands
                else if(pid == 0)
                    run_child_process(background,arg_list,in_file, out_file);
                //parent process will conditionally wait for child to complete
                else{
                    //wait for non-background processes
                    if(background==FALSE){
                        //wait for the process to complete				
                        pid=waitpid(pid, &status, 0);
                        // prompts a message upon termination
                        if(WIFSIGNALED(status)){
                            printf("terminated by signal %d\n", status);
                            clearFD(stdout);
                        }
                    }
                    //don't wait for background processes
                    if(background == TRUE){
                        //do not wait for the process to complete
                        waitpid(pid, &status, WNOHANG);
                        //add PID to background list of pids					
                        bg_pids_list[npids++] = pid;					
                        printf("background pid is %d\n", pid);
                        clearFD(stdout);                    
                    }

                }		
            }
        
            in_file = NULL;
            out_file = NULL;
            raw_command=NULL;
            //reset arg list to null

            int x=0;
            while(x<arguments) arg_list[x++]=NULL;

            //release resources
            free(raw_command);

            // check if a child has completed its task
            // and ready to be killed
            reap_processes(pid,status);
        }   
    }        
}
/*
  A CTRL-Z command from the keyboard sends a SIGTSTP signal to your parent shell process and all
  children at the same time (this is a built-in part of Linux).
  A child, if any, running as a foreground process must ignore SIGTSTP.
  Any children running as background process must ignore SIGTSTP.
  When the parent process running the shell receives SIGTSTP
  The shell must display an informative message (see below) immediately if it's sitting at the
  prompt, or immediately after any currently running foreground process has terminated
  The shell then enters a state where subsequent commands can no longer be run in the
  background.
  In this state, the & operator should simply be ignored, i.e., all such commands are run as if they
  were foreground processes.
  If the user sends SIGTSTP again, then your shell will
  Display another informative message (see below) immediately after any currently running
  foreground process terminates
  The shell then returns back to the normal condition where the & operator is once again honored
  for subsequent commands, allowing them to be executed in the background.
  See the example below for usage and the exact syntax which you must use for these two
  informative messages.
*/
void sigtstpHandlerRoutine(int sig){
    // normal mode
	if (forgroundMode){				
		forgroundMode=0;
		char* fgMessage = "\nExiting foreground-only mode\n: ";
		write(STDOUT_FILENO, fgMessage, strlen(fgMessage));		
	}
	//foreground-only mode
	else{
        forgroundMode=1;	
		char* fgMessage = "\nEntering foreground-only mode (& is now ignored)\n: ";
		write(STDOUT_FILENO, fgMessage, strlen(fgMessage));
	}
    clearFD(stdout);
}
void clearFD(FILE* fd){
    // clears the file descriptors
    fflush(fd);
}

/*
  The cd command changes the working directory of smallsh.
  By itself - with no arguments - it changes to the directory specified in the HOME environment
  variable
  This is typically not the location where smallsh was executed from, unless your shell executable
  is located in the HOME directory, in which case these are the same.
  This command can also take one argument: the path of a directory to change to. Your cd
  command should support both absolute and relative paths.
*/
void navigate_to_directory(char** arg_list){
	//If the user did not specify a directory
    char currDir[MAXCHARACTERS];
    char *dir;
	if(arg_list[1]== NULL){
        dir = getenv("HOME");
        getcwd(currDir, MAXCHARACTERS);
        chdir(dir); 
    }
    else{
        if(chdir(arg_list[1]) != 0)
            printf("Invalid directory name.\n");
    }            
}

void apply_substitution(char* cur_token){
    int pid = getpid();	
	char pidToStr[10]; // assuming max size of pid < 10
	char buffer[MAXCHARACTERS];
	char* copy = cur_token;
	
	sprintf(pidToStr, "%d", pid);
    
    char *needle = (copy = strstr(copy,EXPANSION));
	while(needle != NULL){
		//scan till $$
        int diff = copy-cur_token;
		strncpy(buffer, cur_token, diff);
		buffer[diff]='\0';
	
		strcat(buffer, pidToStr);
		strcat(buffer, copy + strlen(EXPANSION));	  
		strcpy(cur_token, buffer);
        
		copy = copy + 1;
        needle = (copy = strstr(copy,EXPANSION));
	}
	
}

void kill_bg_processes(int npids, pid_t bg_pids_list[]){
int i;
    for(i=0; i<npids; i++) kill(bg_pids_list[i], SIGTERM);
}

int equal(char *str1, char *str2){
    if(strcmp(str1,str2) == 0) return TRUE;
    else return FALSE;
}

void handle_background_io(int mode, int fd){
    // try to read from /dev/null
    // in case of failure, read from stdin
    int fp;
    fp = open("/dev/null", mode);
    if(fp==-1)
    {
        fprintf(stderr, "failed to open null device\n");
        clearFD(stdout);
        exit(ABNORMAL);
    }
    //read the data
    // fd = 0: in case of input null
    // fd = 1: in case of output null
    else if(dup2(fp,fd)==-1){
        fprintf(stderr, "error in redirecting output\n");
        clearFD(stdout);
        exit(ABNORMAL);
    }
    close(fp);
}

void reset_sigint(){
    //reset the sigint handler
    //this way, child process will be killed easily
    sigintStruct.sa_handler = SIG_DFL;
    sigaction(SIGINT, &sigintStruct, NULL);
}

void background_io(char *in_file, char *out_file){
    //if input file missing
    //then read from /dev/null OR stdin
    if (in_file==NULL) handle_background_io(O_RDONLY, 0);                        
    //if no output was missing
    //then write to /dev/null OR stdout                    
    if (out_file==NULL) handle_background_io(O_WRONLY, 1);
}

void reap_processes(int pid, int status){
    //check for any background processes that have completed
    pid = waitpid(-1, &status, WNOHANG);
    while(pid>0){
        //print the PID and exit value if the process ended normally
        if (WIFEXITED(status)){
            printf("background pid %d is done: exit value %d\n", pid,status);
            clearFD(stdout);
        }
        //print the PID and signal if the process was terminated
        else{
            printf("background pid %d is done: terminated by singal %d\n", pid, status);
            clearFD(stdout);
        }
        // check in next loop and so on

        pid = waitpid(-1,&status, WNOHANG);				
    }
}

void run_child_process(int background,
    char* arg_list[MAXARG],
    char *in_file, char *out_file){
    int fp;
    //if this is a foreground process
    if(background == FALSE){
        reset_sigint();
    }

    //if input file given
    if(in_file){
        //read only mode
        fp=open(in_file, O_RDONLY);
                    
        if(fp ==-1){
            fprintf(stderr, "cannot open %s for input\n", in_file);
            clearFD(stdout);
            exit(ABNORMAL);
        }
        // now use latest created file descriptor
        // instead of reading from stdin
        else if(dup2(fp,0)== -1){
            fprintf(stderr, "error in redirecting input\n");
            clearFD(stdout);
            exit(ABNORMAL);
        }
        close(fp);
    }
    //if output filename given
    if(out_file){
        // open file for writing and truncating if
        // it was not empty
        fp = open(out_file, O_CREAT|O_WRONLY|O_TRUNC, 0644);
					
        if(fp==-1){
            fprintf(stderr, "cannot open %s for output\n", out_file);
            clearFD(stdout);
            exit(ABNORMAL);
        }
        // now use latest created file descriptor
        // instead of writing to terminal
        else if(dup2(fp,1)== -1){
            fprintf(stderr, "error in redirecting output\n");
            clearFD(stdout);
            exit(ABNORMAL);
        }                    
        close(fp);
    }
    //Check if this is a background process
    if(background==TRUE){
        background_io(in_file, out_file);
    }
    //let us execute the command now
    if(execvp(arg_list[0], arg_list)){
        // in case command is not found
        fprintf(stderr, "%s: no such file or directory\n", arg_list[0]);
        clearFD(stdout);
        exit(ABNORMAL);
    }
}
