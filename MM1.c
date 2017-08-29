/* 
 * File:   MM1.c
 * Author: Matthew Price
 *
 * Created on March 25, 2009, 7:29 PM
 */

#include <stdio.h>              //Pre-processor header files necessary
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <pthread.h>
#include "MM1_build.c"


/*
 * 
 */
int main(int argc, char** argv){
    int i;                          //initialized variables necessary for the main function
    int est_min;
    int est_sec;
    int *ptr_i;
    int opt_check;
    char *option_arg;
    double random_seed;
    int user_iterations;
    pthread_t *col_thread;
    pthread_t *queue_line;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    time_t seed;                        //the time here will be seeded but will be seeded with the user's new number if so desired by using the getopt request -R
    seed = time(NULL);
    srand48(seed);

    while ((opt_check = getopt(argc, argv, "S:R:N:L:M:T:V")) != -1){         //this function gets the options the user picked and processes them
        switch(opt_check){              //this checks the optcheck for the switch
            case 'S':                   //if case is S then do the following
                option_arg = optarg;    //option_arg copies what is in optarg
                if (strcmp(option_arg, "1")==0) {                          //if the user selected 1 then run with shortest job first
                    printf("Running simulation using SHORTEST JOB FIRST!\n");
                    sjf = 1;                                               //flag sjf to 1 for the processing functon
               }                                                            //ends the if statement
                break;
            case 'R':                                                       //if option R then do the following
                option_arg = optarg;                                        //copy optarg
                printf("Seeding random number generator with %s!\n", option_arg);       //tell the user that we are seeding the random number generator with the number they have supplied
                random_seed = atof(option_arg);                                         //turn the string in option_arg into a double that will be stored as random seed
                srand48(random_seed);                                                   //seed the random number generator with random_seed
                break;
            case 'T':                                                                 //if option T then do the following
                option_arg = optarg;                                                  //copy optarg
                printf("Setting the number of iterations to %s\n", option_arg);       //tell the user that we are changing the number of iterations to their desired amount
                user_iterations = atoi(option_arg);                                   //set the user_iterations to the integer of option_arg
                MAX_CUSTOMERS = user_iterations;                                      //change the number of MAX_CUSTOMERS to user_iterations
                break;
            case 'L':                                                                 //If option L then do the following
                option_arg = optarg;                                                  //copy option_arg
                printf("Setting lambda to: %s\n", option_arg);                      //tell the user that we are setting lambda to option_arg
                lambda = atof(option_arg);                                            //copy lambda to the double of string option_arg
                break;
            case 'M':                                                                 //if case M then do the following
                option_arg = optarg;                                                  //copy option_arg
                printf("Setting mu to: %s\n", option_arg);                          //tell the user that we are setting mu to option_arg
                mu = atof(option_arg);                                                //set mu to the double of the string of option_arg
                break;
            case'N':                                                                  //if case N then do the following
                option_arg = optarg;                                                  //copy option_arg
                user_servers = atoi(option_arg);                                      //user_servers is the double of option_arg
                if (user_servers < 1){                                                //if user_servers is less than 1 then
                    printf("Servers can not be set to less than 1!\nSetting servers to 1!\n");      //tell the user that the servers can not be less than 1 and are therefore are setting the number of servers to 1
                    user_servers = 1;                                                 //sets the user_servers to 1
                }                                                                     //closes the if statement
                if (user_servers > 5){                                                //if the user servers is greater than 5 then
                    printf("Servers can not be set to more than 5!\nSetting servers to 5!\n");  //tell the user that the number of servers cannot be greater than 5 and that we are now setting them to 5
                    user_servers = 5;                                                 //sets the number of servers to 5
                }
                break;
            case 'V':
                printf("Showing background processes\n");
                show = 1;
                break;
            default:
                break;
        }
    }
    myData = (struct customers *)malloc(sizeof(struct customers)*MAX_CUSTOMERS);    //mallocs all the memory needed for the customers
    wait_arrival = (struct timeval *)malloc(sizeof(struct timeval)*MAX_CUSTOMERS);  //mallocs memory needed for wait arrival (the time the customer enters the queue)
    wait_depart = (struct timeval *)malloc(sizeof(struct timeval)*MAX_CUSTOMERS);   //mallocs memory for the time of day that the customer departs the queue and starts to get serviced by the clerk
    wait_total = (struct timeval *)malloc(sizeof(struct timeval)*MAX_CUSTOMERS);    //mallocs memory for the total time needed
    utilization_begin = (struct timeval *)malloc(sizeof(struct timeval)*2);         //mallocs the memory needed for the utilization being struct pointer
    utilization_end = (struct timeval *)malloc(sizeof(struct timeval)*2);           //mallocs the memory needed for the utilization end struct pointer


    pthread_t *process_sjf;
    process_sjf = (pthread_t *)malloc(sizeof(pthread_t)*user_servers);              //we make and malloc the pthread pointer process_sjf here because the number of threads could be changed by the options
    queue_line = (pthread_t *)malloc(sizeof(pthread_t)*1);                          //we malloc here for the other threads
    col_thread = (pthread_t *)malloc(sizeof(pthread_t)*1);

    if (lambda >= (user_servers * mu)){                                             //lambda must be smaller than the number of servers X mu otherwise we exit with an error
      printf("Lambda MUST BE smaller than mu * number of servers!\nCannot continue simulation!\n");
      exit(-1);
    }

    est_min = (int)floor((1/lambda)*MAX_CUSTOMERS/60);                              //est_min is the estimated time in minutes
    est_sec = (int)((int)((1/lambda)*MAX_CUSTOMERS) % 60);                          //est_sec is the estimated time in seconds

    printf("Total Estimated Run Time is: %d minute(s) and %d second(s)\n(times are purely estimates)\n", est_min, est_sec);      //this tells the user the estimated time needed for completion
    
    printf("\nProcessing");
    fflush(stdout);
    pthread_mutex_lock(&count_mutex);                               //locks the mutex
    pthread_create(&queue_line[0], &attr, random1, NULL);           //creates the random1 thread to create customers
    pthread_mutex_unlock(&count_mutex);                             //unlocks the mutex

    pthread_mutex_lock(&count_mutex);                               //locks the mutex
    pthread_create(&col_thread[0], &attr, collect, NULL);           //creates the collect thread
    pthread_mutex_unlock(&count_mutex);                             //unlocks the mutex


    pthread_mutex_lock(&count_mutex);                               //locks the mutex
    for (i = 0; i<user_servers; i++){                               //creates the threads for the process thread...if the number of servers is changed by the user, that number of threads is created here.
    ptr_i = (int *)i;
    pthread_create(&process_sjf[i], &attr, (void *)short_job, ptr_i);
    }
    pthread_mutex_unlock(&count_mutex);                             //unlocks the mutex

    pthread_attr_destroy(&attr);                                    //destroys the attr for the joinable thread
    pthread_join(col_thread[0], NULL);                              //waits for thread 3 to finished
    for (i = 0; i < user_servers; i++){                             //this for loop waits for all the thread 2s to finish
        pthread_join(process_sjf[i], NULL);
    }
    //free(myData);
    pthread_join(queue_line[0], NULL);                              //waits for thread 1 to finish
    calculate();                                                    //calculates the statistics and displays the results
    utilize();                                                      //calculates the utilization of the server
    free(myData);
    free(wait_arrival);
    free(wait_total);
    free(utilization_begin);
    free(utilization_end);
    free(queue_line);
    free(col_thread);
    free(process_sjf);
    return (0);
}
