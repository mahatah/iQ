/* 
 * File:   MM1.c
 * Author: Matthew Price
 *
 * Created on March 13, 2009, 4:52 PM
 */

#include <stdio.h>              //Pre-processor header files necessary
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <pthread.h>
#include "mm1system.h"

/*First pthread function created in order to place customers in the queue. This is NOT multi thread safe!*/
void *random1(){
    int progress_bar = 1;
    int i = 0;                          //need for "for loops"
    int err;                            //error handler int
    struct timespec sleepTime;          //necessary for the time require to sleep given the formula -log(1.0-"random-number-between-0-and-1")/lambda
    while (icanstart == 0){}            //this keeps thread 1 from creating customers until thread 3 is ready


    for (i = 0; i < MAX_CUSTOMERS; i++){        //loop will continue until there are customers created equal to the amount of MAX_CUSTOMERS
        pthread_mutex_lock(&count_mutex);       //we must add these here otherwise we will end up with a race condition where myData[i] is created and then the processing function will try to access it with nothing in it
        youcancollect = 1;                      //flags the pseudo-boolen variable "youcancollet" so that thread three can start collecting now
        myData[i].arrival_time = -log(1.0-drand48())/lambda;    //places the arrival time (which is really the waiting time this customer must wait before being added to the queue) into the struct
        myData[i].service_time = -log(1.0-drand48())/mu;        //places this customer's service time in the struct
        myData[i].id = i;                                       //gives the customer an ID that is equal to the number of customers that have been created minus 1
        myData[i].been_processed = 0;                           //flags this customer as not been processed yet
       
       sleepTime.tv_sec = (int)floor(myData[i].arrival_time);   //places the customer's "wait time before being queued" into the Sleep, first in seconds then in nano_seconds
       sleepTime.tv_nsec = ((myData[i].arrival_time)-sleepTime.tv_sec) * 1000000000L;
       pthread_mutex_unlock(&count_mutex);                      //unlocks the mutex so that other processes may continue
       err = nanosleep(&sleepTime, NULL);                       //sleeps for the time it takes for the customer to arrive
       pthread_mutex_lock(&count_mutex);
       gettimeofday(&wait_arrival[i], NULL);                    //stores the exact moment the customer was arrives
       if (show == 1){
           printf("Customer %d is in the queue\n", i);
       }
       queue++;
       pthread_mutex_unlock(&count_mutex);
       if (show == 0){                                          //this is for the progress bar, it will not display if show is set to 0
       while ((MAX_CUSTOMERS*.05)*progress_bar <= queue){
           fflush(stdout);
           if(progress_bar >= 5 && (progress_bar % 5) == 0){
               printf("%d%%", progress_bar*5);
               progress_bar++;
           } else if (progress_bar < MAX_CUSTOMERS -1) {
               progress_bar++;
               printf(".");
           } else {
               progress_bar++;
           }
           }
       if (MAX_CUSTOMERS == queue + 1 ){
               printf("100%% Finished!\n\n");
       }
       }
    }                                                           //exits the main for loop for thread 1
    return;                                                     //returns to the calling function
}                                                               //ends the function

/*This function is used to create the process thread that process the customers*/
void *short_job(int *ptr_i){                        //ptr_i is a point that point to the number of instance of the thread that has been created
    int i;                                          //used for for loops
    int j = 0;
    //int ps_id = (int)ptr_i;                         //takes the value of ptr_i as stores it in ps_id
    double store_times[MAX_CUSTOMERS];     //makes a double array the size of the max customers and ps_id
    int store_ids [MAX_CUSTOMERS];         //makes an array of the store ids that I need
    struct timespec helloTime;                      //creates a struct for me to hold the processing times in
    struct timespec quick_sleep;                    //used for the tight sleep loop that happens every .1 seconds
    while (youcancollect == 0){}                        //keeps this process from starting if thread 1 has not started
    gettimeofday(&utilization_begin[0], NULL);          //gets the moment that thread two starts
    quick_sleep.tv_nsec = (10000000L);                  //puts .01 seconds into the quick_sleep struct
    for (i = 0; have_finished < MAX_CUSTOMERS; i++){    //the main for loop that will continue until all the customers have been processed
        
        while (queue - (have_finished) < 0 && have_finished < MAX_CUSTOMERS){   //this is the tight sleep loop that will continue unless someone in the queue is detected
            process_sleep++;                                                    //adds 1 to process sleep which we later use to calculate the utilization of the server
            nanosleep(&quick_sleep, NULL);                                      //sleeps for .01 seconds
            if (have_finished == MAX_CUSTOMERS){
                pthread_exit(NULL);
            }
        }                                               //ends the while

        if (pthread_mutex_trylock(&count_mutex)==0){    //special way to lock the mutex in order to decrease probability of deadlock
        have_finished++;                                //increases have_finished immediatly so that other threads will not
        //first we have to find the lowest id that is still in the queue, this will be the lowest struct with the been_processed flag set to zero under the size of the queue (this will need to be mutex locked so that the queue doesn't increase)
        if (queue - (have_finished -1) >= 0){

        for (i = 0; i <= queue; i++){               //now we iterate through all the ones created to find (starting at the lowest number in the queue
            if (myData[i].been_processed == 0){     //this checks to see if the data has been processed
                store_times[j] = myData[i].service_time;     //stores the service time in the store_times array
                store_ids[j] = myData[i].id;                 //stores the id of the myData in the store_ids array
                j++;                                                //increases j
            }                                                       //end of if
        }                                                   //end of sub-for loop



        
        store_times[j] = -1;                                        //here we place a -1 in the array as a place for the next for loops to search for
        if (show == 1){
        int k;
        printf("The store times in the queue are as follows:\n");
        for (k = 0; store_times[k] != -1; k++){
            printf("%f\n", store_times[k]);
        }
        }

        if (sjf == 1){                                                      //if shortest job first is flagged then do the following

        for (i = 0; store_times[i] != -1; i++){                      //serach for the smallest element of time inside the arrays then break...i will be the place holder in the array for the shorest job
        for (j = 0; store_times[j] != -1; j++){
        if (store_times[i] > store_times[j]){                 //this breaks out of the for loop if the i is clearly not the shorest job
            break;
        }
        }
        if (store_times[i] <= store_times[j-1] && store_times[j] == -1){       //if this condition is met, then the i is the place holder for the shorest job in the array then breaks out of the array
            break;
        }
        }
        }
        
        if (sjf == 0){          //this checks if shortest job first is implemented
            i = 0;              //if not, then i is zero because the first job in the array is the first one in the queue
        }
        
        helloTime.tv_sec = (int)floor(store_times[i]);       //this stores the time of the selected job into a time struct
        helloTime.tv_nsec = ((store_times[i]-helloTime.tv_sec) * 1000000000L);
        i = store_ids[i];                                    //this stores the the id of the corresponding customer in the int i
        if (show == 1){
        printf("Processing Customer: %d\n", i);
        }
        gettimeofday(&wait_depart[i], NULL);                        //this stores the exact time of the system in the time struct wait_depart


        myData[i].been_processed = 1;                               //sets the customer's flag to processed
        j = 0;                                                      //sets j back to 0
        pthread_mutex_unlock(&count_mutex);                         //unlocks the mutex
        nanosleep(&helloTime, NULL);                                //sleeps for the stored time in hello time
        }                                                           //ends the if statement
        }
    }                                                               //ends the for loop

    gettimeofday(&utilization_end[0], NULL);                        //gets the exact system time and stores it in the utilization_end time struct
    pthread_exit(NULL);                                             //exits the pthread

    }

/*This is the collection thread used to store the queue lengths into an array of 1000000. This is NOT multi-thread safe!*/
void *collect(){
    struct timespec collect_sleep;         //creates a new timespec struct and stores the time of .02 seconds inside it
    collect_sleep.tv_nsec = (20000000L);

    icanstart = 1;                          //flags the icanstart flag so that thread 1 may start processing
    while (youcancollect == 0){}            //this stops the collection of data until thread 1 is ready
    while (have_finished < MAX_CUSTOMERS){  //while all the customers have not been processed, due the following
        nanosleep(&collect_sleep, NULL);    //sleep for .02 seconds

        thread_collection[array_counter] = (queue - have_finished + 1);     //stores the length of the queue into the enormous array
        array_counter++;                                                    //increases 1 to array_counter in order to move to the next slot in the array

    }                                                                       //ends the for loop
}                                                                           //ends the function

/*This function runs after all the threads have been joined in order and processes the information gathered*/
void calculate(){
    long int mega_store = 0;       //These are the multiple initilized variables used in this function
    long int mean_time = 0;
    mega_store = (long int)malloc(sizeof(long int));
    mean_time = (long int)malloc(sizeof(long int));
    double waiting_store;
    double waiting_diff = 0;
    double std_waiting;
    int std_time_seconds;
    long int std_time_useconds;
    std_time_useconds = (long int)malloc(sizeof(long int));
    int i;
    long int mean_time_useconds;
    mean_time_useconds = (long int)malloc(sizeof(long int));
    int mean_time_seconds;
    int std_store;
    int diff_points = 0;
    double std_deviation;

   for (i = 0; i <= array_counter; i++){                    //this for loop stores all the times in the thread_collection array
        sum += thread_collection[i];
    }
    mean = ((double)sum / ((double)array_counter+1));       //the mean is calculated as the sum of all the parts of thread_collection divided by the number of elements in it

    for (i = 0; i <= array_counter; i++){                   //this gets the numerator for the standard deviation
        std_store = pow((thread_collection[i] - mean), 2);
        diff_points +=  std_store;
    }

    for (i = 0; i < MAX_CUSTOMERS; i++){                //this gets the total time that each customer exisisted and stores it in the wait_total
            wait_total[i].tv_usec = (int)(((wait_depart[i].tv_sec - wait_arrival[i].tv_sec)*1000000L)+(wait_depart[i].tv_usec - wait_arrival[i].tv_usec));
            mega_store += (long int)wait_total[i].tv_usec; //sotres all the exisit, in microseconds, into one variable...mega_store
    }
        mean_time = mega_store/(long int)MAX_CUSTOMERS;                                //mean time is the mean time of the waiting customers in microseconds
        mean_time_seconds = (int) floor(mean_time / 1000000L);                        //mean_time_seconds is the mean times (in microseconds) divided divided by 1000000
        mean_time_useconds = (long int)((mean_time - (mean_time_seconds * 1000000L)));    //mean_time_useconds is the remainder of that time in microseconds

        for (i = 0; i < have_finished; i++){                                                //this calculates the numerator for the standard deviation of wait_times
          waiting_store = (double)(((wait_total[i].tv_usec) /1000000L)  - (mean_time/1000000L));
          waiting_store = pow(waiting_store, 2);
          waiting_diff += waiting_store;
        }

        std_waiting = (double)sqrt(waiting_diff/MAX_CUSTOMERS);      //std is the stdard deviation in microseconds
       std_time_seconds = (int) floor(std_waiting);                      //this is the std deviation in seconds
       std_time_useconds =  (long int)((std_waiting * 1000000L - (std_time_seconds * 1000000L)));   //this is the std deviation in micro seconds

    printf("The mean wait time is %d second(s) and %ld microsecond(s).\n", mean_time_seconds, mean_time_useconds);  //printf statments for the user
    printf("The stadard deviation of the waiting time is %d second(s) and %ld microsecond(s).\n", std_time_seconds, std_time_useconds);
    std_deviation = (double) sqrt(diff_points/((double) array_counter+1));      //this is the standard deviation of the queue lengths
    printf("The mean of the queue length is: %.4f people\n", mean);
    printf("The standard deviation for people in the queue is: %f\n", std_deviation);
}                                                                               //ends the function

/*this function calculates the utilization of the server*/
void utilize(){
    int i;                              //initialized variables needed for this function
    double total_service_time = 0;
    double utilization_server;
    double utilize_seconds;
    long int utilize_micro;

    utilize_seconds = (utilization_end->tv_sec - utilization_begin->tv_sec);        //this subtracts the two time stamps placed in the process_thread function and stores them in utilize seconds and microseconds
    utilize_micro = (utilization_end->tv_usec - utilization_begin->tv_usec);

    utilize_seconds += (utilize_micro/1000000L);                                    //this adds on the micro seconds to the seconds

    for (i = 0; i < MAX_CUSTOMERS; i++){                                            //this stores the total of all the service times into total_service_time
        total_service_time += myData[i].service_time;
    }
    process_sleep = (process_sleep / user_servers);
    printf("total service time is: %f second(s).\n", total_service_time);                      //this displays all the service time in seconds for all the customers combined
    utilization_server = (total_service_time / ((process_sleep*.01)+total_service_time))*100;   //the utilization of the server is the total service time of all the customers devided by the total service time of all the customers plus all the sleep time for the server
    printf("The utilization is: %f percent \n", utilization_server);                //prints out the percentage of utilization
}                                                                                   //ends the function

