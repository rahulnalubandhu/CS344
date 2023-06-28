/*
Name : Rahul Kumar Nalubandhu
Citation for the following functions:
Date: 10/06/2022
Copied from /OR/ Adapted from /OR/ Based on: Source URL: Initial skeleton was taken from https://replit.com/@cs344/studentsc#main.c
later few fuctions like file process concepts are refrenced from https://www.geeksforgeeks.org/relational-database-from-csv-files-in-c/
few logics for list are taken from https://stackoverflow.com/questions/13465264/reading-csv-file-into-c-linkedlist
*/


#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h> 

int count_of_movies = 0;


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

   
    char* saveptr; // u8sing with strtok

    //The first token is the movie title
    char* token = strtok_r(fileLine, ",", &saveptr);
    currMovie->title = (char*)calloc(strlen(token) + 1, sizeof(currMovie->title)); 
    strcpy(currMovie->title, token); 

    //The 2nd token is the release year
    token = strtok_r(NULL, ",", &saveptr);
    currMovie->year = atoi(token); 

    //The 3rd token is the languages
    token = strtok_r(NULL, ",", &saveptr); 
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
    token = strtok_r(NULL, "\n", &saveptr);
    currMovie->rating = (char*)malloc(strlen(token) * sizeof(char*));
    strcpy(currMovie->rating, token);
    currMovie->next = NULL;
    return currMovie;
}

/*
* Return a linked list of movies by parsing data from
* each line of the specified file.
*/

struct movie* processFile(char* filePath) {

    FILE* datafile = fopen(filePath, "r");

    char* currLine = NULL; 
    size_t len = 0;
    ssize_t nread;


    // The head of the linked list
    struct movie* head = NULL;
    // The tail of the linked list
    struct movie* tail = NULL;


    // Read the file line by line
    while ((nread = getline(&currLine, &len, datafile)) != -1) {

        // new movie node
        struct movie* new_movie_Node = createMovie(currLine);

        if (head == NULL) { 
            // This is the first node in the linked link
            // Set the head and the tail to this node
            head = new_movie_Node;
            tail = new_movie_Node;
        } else { 
            // This is not the first node.
            // Add this node to the list and advance the tail
            tail->next = new_movie_Node; 
            tail = new_movie_Node; 
        }
    count_of_movies++;
    }


    struct movie* t_head = head->next;
    free(head); //free the first node as it's the CSV file guide
    free(currLine);
    fclose(datafile);
    return t_head;
}


void print_By_Year(struct movie* list) { //to process the movies by year
    int choose_year, count=0; 
    printf("Enter the year for which you want to see movies: ");
    scanf("%d", &choose_year);
    while (list != NULL) {
        if (list->year == choose_year) {
            printf("%s\n", list->title);
            count++;
        }
        list = list->next;
    }
    if (count == 0) {
        printf("No data about movies released in the year %d\n", choose_year);
    }
}

void print_by_language(struct movie* list) {
    char choose_lang[25];
    int count = 0, i;
    printf("Enter the language for which you want to see movies: \n");
    scanf("%s", choose_lang);

    while (list != NULL) {
        for (i = 0; i < list->language_count; i++) {
            if (strstr(list->language[i], choose_lang)!= NULL) { //if there is a matching language
                printf("%d %s\n", list->year, list->title);
                count++;
                break;
            }
        }
        list = list->next;
    }
    if (count == 0) {
        printf("No data about movies released in %s\n", choose_lang);
    }
}


void print_Best_of_Year(struct movie* list, int n) {
    struct best_of_year bst_yr[n + 1];
    int count = 0, i, replaceNode_flag = 0;

    while (list != NULL) {
        for (i = 0; i < count; i++) { 
            if (bst_yr[i].year == list->year) {
                float rtng = strtod(list->rating, NULL);
                float rating_temp = bst_yr[i].rating;
                if (rtng > rating_temp) { 
                    bst_yr[i].title = list->title;
                    bst_yr[i].rating = rtng;
                    replaceNode_flag = 1; 
                    break; //go to next node in list
                } else {
                    replaceNode_flag = 1;
                    break; //if  same break
                }
            }
        }
        if (replaceNode_flag == 0) { // if nothing is found add new entry
            bst_yr[count].title = list->title;
            bst_yr[count].year = list->year;
            bst_yr[count].rating = strtod(list->rating, NULL);
            count++;
        } else {
            replaceNode_flag = 0;
        }
        list = list->next;
    }

    for (i = 0; i < count; i++) {
        printf("%d %.2f %s\n", bst_yr[i].year, bst_yr[i].rating, bst_yr[i].title);
    }
}


int main(int argc, char** argv) {
    if (argc < 2) { 
        printf("Error ! You did not provide enough arguments!\n");
        return -1; 
    }
    struct movie* MovieList = processFile(argv[1]);

    printf("\nProcessed file %s and parsed data for %i movies\n", argv[1], count_of_movies-1);
    int choice=0;

    while (choice!=4) {
        printf("\n1. Show movies released in the specified year\n2. Show highest rated movie for each year\n3. Show the title and year of release of all movies in a specific language (case sensitive)\n4. Exit from the program\n\n");
        printf("Enter a choice from 1 to 4: ");
     
        scanf("%d", &choice);
        switch (choice) {
            case 1: //movies from one year
                print_By_Year(MovieList);
            break;
            case 2: //movies with highest rating
                print_Best_of_Year(MovieList, count_of_movies);
            break;
            case 3: //show title and year for specified language
                print_by_language(MovieList);
            break;
            case 4: //exit the program
                printf("Now Exiting! Thanks\n");
                return 0;
            break;
            default: 
                printf("You entered an incorrect choice. Try again.\n");
            break;
        };
    }
    return 0;
}
