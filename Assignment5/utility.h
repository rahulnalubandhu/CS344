void error(const char *msg);
int fileLength(const char* filename);
int char_ascii (char c);
char ascii_char(int i);
void encrypt(char  plaintext[], char key[]);
void decrypt(char cipher[], char key[]);
void sendFile(int filelength,int socketFD, int fd,  char buffer [1024]);