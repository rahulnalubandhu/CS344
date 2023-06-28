#ifndef SMALLSH_H
#define SMALLSH_H
/* constant literals */
#define MAXCHARACTERS 2048
#define MAXARG 513
#define MAXPROCESS 512
#define TRUE 1
#define FALSE 0
#define NEWLINE " \n"
#define INPUT_REDIRECTION "<"
#define OUTPUT_REDIRECTION ">"
#define EXPANSION "$$"
#define PROMPT ": "
#define ABNORMAL 1
struct sigaction sigintStruct={0};

// it will initialize sigstp sigint
void hook_interrupts();
// major function runs after everything is setup
void run_shell();
// will flush a file descriptor
void clearFD(FILE* fd);
// routine runs when a process receives sigstp signal
void sigtstpHandlerRoutine(int);
// takes a directory name in list form and does chdir on it
void navigate_to_directory(char**);
// converts a token = $$ to pid of parent
void apply_substitution(char*);
// kills all running processes before exists
void kill_bg_processes(int, pid_t []);
// true of two strings are equal
int equal(char *, char*);
// set up new file descriptors instead of old one
// e.g dup2 will set fp instead of 1
// to output to fp instead of terminal
void handle_background_io();
// reset the sigint so that it can be used next time
void reset_sigint();
// set up descriptor to files instead of stdin stdout 
void background_io(char*, char*);
// before taking next command from user
// make sure to check if a child has finished its job
// if finished then reap it
void reap_processes(int,int);
// a huge clutter code of child process is handled by this routine
void run_child_process(int,char* arg_list[MAXARG],char *, char *);
#endif
