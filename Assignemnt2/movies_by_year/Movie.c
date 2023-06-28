/*
Name : Rahul Kumar Nalubandhu
Citation for the following functions:
Date: 10/23/2022
Copied from /OR/ Adapted from /OR/ Based on: Source URL: Initial skeleton was taken from https://replit.com/@cs344/studentsc#main.c
later few fuctions like file process concepts are refrenced from https://www.geeksforgeeks.org/relational-database-from-csv-files-in-c/
few logics for list are taken from https://stackoverflow.com/questions/13465264/reading-csv-file-into-c-linkedlist
*/


#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h> 
#include <dirent.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/errno.h>
#include <time.h>  
#include <unistd.h>
#include <stdbool.h>




struct movie {
    char* title;
    int year;
    int language_count;
    char** language;
    char* rating;
    struct movie* next;
};

struct best_of_year {
    char* title;
    int year;
    float rating;
};


/* Parse the current line which is space delimited and create a
*  movie struct with the data in this line
*/

struct movie* createMovie(char* fileLine) {


    struct movie* currMovie = malloc(sizeof(struct movie)); //allocating memory for movie struct currMovie

   
    char* savepointer; // using with strtok

    //The first token is the movie title
    char* token = strtok_r(fileLine, ",", &savepointer);
    currMovie->title = (char*)calloc(strlen(token) + 1, sizeof(currMovie->title)); 
    strcpy(currMovie->title, token); 

    //The 2nd token is the release year
    token = strtok_r(NULL, ",", &savepointer);
    currMovie->year = atoi(token); 

    // The 3rd token is the languages
    token = strtok_r(NULL, ",", &savepointer); 
    char* cpy_string = (char*)malloc(255 * sizeof(char));  //just for cpyng string
    int i = 1;
    while(i<strlen(token)-1){
        cpy_string[i-1]=token[i];//removed braces
        i++;
    } 

   //taking ';' this out
    char** process_lang = (char**)malloc(20 * sizeof(char*));
    int k = 0; //intializing count
    char* token_for_lang = strtok(cpy_string, ";");
  
    for( k=0;token_for_lang!=NULL;k++){
        process_lang[k]=(char*)malloc(strlen(token_for_lang) * sizeof(char));
        strcpy(process_lang[k], token_for_lang);
        token_for_lang=strtok(NULL,";");
    }

    currMovie->language_count = k;
    currMovie->language = (char**)malloc(k * sizeof(char*));
    i=0;
    while(i<k){
        if (process_lang[i] != NULL) {
            currMovie->language[i] = (char*)malloc(strlen(process_lang[i]) * sizeof(char));
            currMovie->language[i] = process_lang[i];
        }   
        i++;
    }

   //clear used mem
    free(cpy_string);
    free(process_lang);


    // //fourth token is the movie rating 
    token = strtok_r(NULL, "\n", &savepointer);
    currMovie->rating = (char*)malloc(strlen(token) * sizeof(char*));
    strcpy(currMovie->rating, token);
    currMovie->next = NULL;
    return currMovie;
}


/*
 * itoa function is taken from http://www.strudel.org.uk/itoa/
 */
char* itoa(int val) {

	static char buf[32] = {0};

	int i = 30;

	for(; val && i ; --i, val /= 10)

		buf[i] = "0123456789abcdef"[val % 10];

	return &buf[i+1];
}

void getPrefix(char *dir_name, char prefix[256]){
    int index = 0;
    while(dir_name[index] != 0 && dir_name[index] != '_'){
        prefix[index] = dir_name[index];
        index++;
    }
    prefix[index] = '\0';
}
// to find smallest and largest file--------------
char* getFile(int factor) {
    DIR* curr_dir = opendir("."); //open current directory
    char* result_file = (char*)malloc(255 * sizeof(char));
    int result = 0;
    char prefix[256];
    if (curr_dir) { //making sure Dir is not null
        struct dirent* file_dir = readdir(curr_dir);
        while (file_dir != NULL) {
            char* extension = strrchr(file_dir->d_name, '.'); //split string by the '.'
            getPrefix(file_dir->d_name, prefix);
            if (extension) {
                if (strcmp(extension + 1 , "csv") == 0 && strcmp(prefix, "movies") == 0) {
                    FILE *file_pointer = fopen(file_dir->d_name, "r");
                    fseek(file_pointer, 0L, SEEK_END); //to check how many bytes the file is till end
                    int curr_file_size = ftell(file_pointer); //get size
                    if (result == 0 || (curr_file_size - result) * factor > 0) { //check if the new file is smaller than previous
                        result = curr_file_size;
                        result_file = file_dir->d_name;
                    }
                }
            } 
            file_dir = readdir(curr_dir);
        }
        closedir(curr_dir);
    }
    return result_file;
}

//----------------------------

void processFile(char* filePath) {
// printf("1st file is here : %s",filePath); // for debugging

    char filename[50] ;
    // printf("cur fileopath: %s\n", filePath);// for debugging

    FILE *datafile;
    strcpy(filename,filePath);
    if ((datafile = fopen(filename, "r")) == NULL){
        // printf("error no %d\n",errno);// for debugging
        // printf("its null\n");// for debugging
    }
    // printf("cur filename: %s\n", filename);// for debugging

     DIR *d;
    struct dirent *dir_new;
    d = opendir(".");
    if(d)
    {
        // printf("in cwd\n");// for debugging
        // printf("directry is here intially %s\n", dir_new->d_name);// for debugging
        // printf("error no intailly %d\n",errno);// for debugging
        
        for (dir_new = readdir(d);dir_new != NULL;dir_new=readdir(d))
        
        {
            // printf("directry is here %s\n", dir_new->d_name);// for debugging
        
        }
        closedir(d);
        
    }

    char* currLine =0;
    size_t len = 0;
    ssize_t nread;


    int count_of_movies = 0;

    srand(time(NULL));
    int random_index;

    // struct movie* movies;
    while (1) { //keep generating the random names till a free directory is found
        random_index = rand() % 100000;
        char name_prfx[26] = "nalubanr.movies.";
        strcat(name_prfx, itoa(random_index));
        // printf("attempting to create new dir %s\n", name_prfx); // for debugging

        DIR* dir = opendir(name_prfx);
        // printf("name %s\n",name_prfx);// for debugging
        if (dir) { //directory exists
            // printf("error! dir already exists.. running again.\n");// for debugging
            closedir(dir);
            continue;
        } else if (ENOENT == errno) { //directory does not exist! Create it..
            if (mkdir(name_prfx, S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP) == 0) { //user can: R,W,E, group: R
                printf("Created directory with name %s\n", name_prfx);
                // printf("Here1\n");// for debugging
                // Read the file line by line
                while ((nread = getline(&currLine, &len, datafile)) != -1) {
                    // printf("reading file\n");// for debugging
                    // new movie node
                    struct movie* new_movie_Node = createMovie(currLine);
                    // printf("movie struct created\n");// for debugging
                    if (count_of_movies != 0){
                        char* yearfile = (char*)malloc(300 * sizeof(char));
                        sprintf(yearfile, "%s/%d.txt", name_prfx, new_movie_Node->year);
                        FILE* year = fopen(yearfile, "a+"); //a for appending a write to the end of the file
                        //     //since we are appending to already existing files, we can simply
                        //     //just keep writing till we looped through all movies in the list
                        // printf("About to write %s\n", new_movie_Node->title);
                        fprintf(year, "%s\n", new_movie_Node->title);
                        
                        // printf("data written\n");
                        fclose(year);
                        // printf("file closed\n");
                    }
                    count_of_movies++;
                }
                free(currLine);

                break;
            }
            
        }fclose(datafile);
        //  printf("closed");
    }
}


// void print_By_Year(struct movie* list) { //to process the movies by year
//     int choose_year, count=0; 
//     printf("Enter the year for which you want to see movies: ");
//     scanf("%d", &choose_year);
//     while (list != NULL) {
//         if (list->year == choose_year) {
//             printf("%s\n", list->title);
//             count++;
//         }
//         list = list->next;
//     }
//     if (count == 0) {
//         printf("No data about movies released in the year %d\n", choose_year);
//     }
// }

// void print_by_language(struct movie* list) {
//     char choose_lang[25];
//     int count = 0, i;
//     printf("Enter the language for which you want to see movies: \n");
//     scanf("%s", choose_lang);

//     while (list != NULL) {
//         for (i = 0; i < list->language_count; i++) {
//             if (strstr(list->language[i], choose_lang)!= NULL) { //if there is a matching language
//                 printf("%d %s\n", list->year, list->title);
//                 count++;
//                 break;
//             }
//         }
//         list = list->next;
//     }
//     if (count == 0) {
//         printf("No data about movies released in %s\n", choose_lang);
//     }
// }



int if_file_exits(char* filename) {
    FILE* file;
    char* extnsn; // extension

    if ((file = fopen(filename,"r")) != NULL) //if there is a file then check for CSV file extension
    {
        extnsn = strrchr(filename, '.'); //split string by the dot
        if (extnsn) { //filename includes extension
            if (strcmp(extnsn + 1, "csv") == 0) { //extension is a CSV format
                return 1;
            }
        }
        
        fclose(file); //to clean memory leaks
    } 
    return 0;
}

int show_menu(){
    int choice = 0;
    int sub_choice = 0;
    do{
        printf("\n1: Select a file to process: \n");
        printf("2: Exit a program: \n");
        printf("\nEnter a choice 1 or 2: ");
        scanf("%d",&choice);
        if(choice == 1){
            int file_found;
            while(sub_choice < 1 || sub_choice > 3 || !file_found){
                file_found= 1;
                printf("\nWhich file do you want to process?\n");
                printf("Enter 1 to pick largest file\n");
                printf("Enter 2 to pick smallest file\n");
                printf("Enter 3 to specify the name of file\n");
                printf("\nEnter a choice from 1 to 3: ");
                scanf("%d",&sub_choice);
                char* processingFile = (char*)malloc(255 * sizeof(char));
                if(sub_choice == 3){
                    printf("Enter the complete file name: ");
                    scanf("%s", processingFile);
                    
                    if (if_file_exits(processingFile) == 1) {
                        printf("\nNow processing the chosen file named %s\n", processingFile);
                        processFile(processingFile);
                        } 
                    else {
                        file_found = 0;
                        printf("The file %s was not found. Try again\n", processingFile);
                    }
                }
                else if(sub_choice < 1 || sub_choice > 3){
                    printf("<< wrong option >>\n\n");                
                } 
            }
        }
        else if(choice == 2){
            printf("<< Now Exiting! >>\n");
            exit(0);
        }
        else{
            printf("<< wrong option >>\n\n");
        } 
    }while(choice < 1 || choice > 2);
    return sub_choice;
}

int main(){
    int main_choice=0;
    char* processingFile = (char*)malloc(255 * sizeof(char));
    while(1){
        main_choice=show_menu();
        if (main_choice == 1){
            processingFile = getFile(1);
            if(processingFile){
                printf("Now processing the chosen file named %s\n", processingFile);
                processFile(processingFile);

            }
            else{
                printf("no file starts from movies_\n");
            }

        }
        else if(main_choice==2){
            processingFile = getFile(-1);
            if(processingFile){
                printf("Now processing the chosen file named %s\n", processingFile);
                processFile(processingFile);
            }
            else{
                printf("no file starts from movies_\n");
            }

        }
    }
    return 0;
}