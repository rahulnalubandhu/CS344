/* Name : Rahul Kumar Nalubandhu
Date:11/13/2022
Citation for the following functions:Copied from /OR/ 
Adapted from /OR/ Based on: Source URL:https://canvas.oregonstate.edu/courses/1890464
most of them are from lecture 2.3 Concurrency modules.
*/

#include <pthread.h>
#include <stdio.h>
/*created 2 conditional variable 'myCond1', 'myCond2' and one mutex named as 'myMutex'*/
pthread_mutex_t myMutex; 
pthread_cond_t myCond1; // space available
pthread_cond_t myCond2; // has data


/*created 1 flag variable known as 'emtpy' which shows that whether a consumer has consumed or not
created a variable which both threads are accessing known as 'value' */
#define SIZE 10 
int count = 0;
int value = 0;
int empty = 1; // empty

/* get_next_item function reads value and returns the next put_item updates the value
put_item is called inside the mutex therefore, body of put_item is not locked */

void put_item(int v){
    value = v;
    empty = 0; // full because it has produced
    count++; // it is called inside the mutex lock
    // therefore, no need to mutex lock it
}

int get_next_item(){
    empty = 1; // empty because it has been consumed
    return value+1;
}
/* producer
---------------
lock
while count < 10
      if value has not been consumed
        wait for consumer
      put value
      unlock
      signal the consumer to consume */

void* producer(void *arg){
    pthread_mutex_lock(&myMutex); // Lock the mutex before checking if the count < size
    while (count < SIZE)
    {        
        while (!empty){
            // Buffer is full; Wait for signal that space is available
            pthread_cond_wait(&myCond1, &myMutex);
            printf("PRODUCER: waiting on myCond1\n");
        }            
        printf("PRODUCER: myMutex locked\n");        
        put_item(count); // Space is free, add an item! This will increment int count
        
        pthread_mutex_unlock(&myMutex); // Unlock the mutex
        printf("PRODUCER: myMutex unlocked\n");
        pthread_cond_signal(&myCond2); // Signal consumer that the buffer is no longer empty
        printf("PRODUCER: signaling myCond2\n");
    }
}
/* consumer
---------------
lock
while count < 10
      if there is no value
         wait for producer to produce
      pick value
      unlock
      signal the producer to produce more */
     

void* consumer(void *arg){
    int value = 0;
    pthread_mutex_lock(&myMutex); // Lock the mutex before checking if the buffer has data    
    while (count < SIZE)
    {        
        while (empty){
            // Buffer is empty. Wait for signal that the buffer has data
            pthread_cond_wait(&myCond2, &myMutex);
            printf("CONSUMER: waiting on myCond2\n");
        }
        printf("CONSUMER: myMutex locked\n");
        int prev = value; 
        value = get_next_item(); // There's an item, get it! This will decrement int count
        printf("myCount: %d -> %d\n", prev, value);
        pthread_mutex_unlock(&myMutex); // Unlock the mutex
        printf("CONSUMER: myMutex unlocked\n");
        pthread_cond_signal(&myCond1); // Signal that the buffer has space
        printf("CONSUMER: signaling myCond1\n");
    }
    printf("PROGRAM END\n");
}

int main(){    
    pthread_t producer_t;
    pthread_t consumer_t;
    pthread_create(&producer_t, NULL, producer, NULL);
    // printf("PRODUCER THREAD CREATED\n");//debugging 
    printf("PROGRAM START\n");
    pthread_create(&consumer_t, NULL, consumer, NULL);
    printf("CONSUMER THREAD CREATED\n");
    
    pthread_join(producer_t, NULL);
    pthread_join(consumer_t, NULL);    
    return 0;
}
