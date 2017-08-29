/* 
 * File:   system.h
 * Author: Matthew Price
 *
 * Created on March 21, 2009, 1:44 PM
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <pthread.h>


extern char *optarg;                    //external variables needed for processing
extern int optind, optopt, opterr;

struct customers{                       //struct of customers used throughout the program
    double service_time;
    double arrival_time;
    int id;
    int been_processed;                 //boolean variable that we use for SJF, 0 means that it has not been processed and 1 means that it has.
}var;
/**************************GLOBAL VARIABLES DEFINED HERE***********************/
pthread_mutex_t count_mutex = PTHREAD_MUTEX_INITIALIZER;
double mu = 4.0;
double lambda = 3.0;
int user_servers = 1;
int MAX_CUSTOMERS = 1000;
int icanstart = 0;
int youcancollect = 0;
int queue = -1;
int show = 0;
int have_finished = 0;
int sjf = 0;
int process_id;
int thread_collection[1000000];
long long int process_sleep = 0;
long int array_counter = 0;
long int sum = 0;
double mean;
struct customers *myData;
struct timeval *wait_arrival;
struct timeval *wait_depart;
struct timeval *wait_total;
struct timeval *utilization_begin;
struct timeval *utilization_end;
/******************************************************************************/
/*Precondition: all threads have join and all customers have been processed
 *PostCondtion: calculates and displays the mean and standard deviation of both the wait times and queue times*/
void calculate();

/*Precondition: Thread 1 is creating customers and placing them in the queue and thread 2 is processing them
 *Postcondition: All the queue lengths throughout the program and saved into an array*/
void *collect();

/*Precondition: A thread has been properly created and a pthread_create function has sent a thread to this function
 *Postcondition: All the users were created and placed in the queue at the specified times*/
void *random1();

/*Precondition: Thread 1 is creating customers and placing them in the queue
 *Postcondition: All the customers have been processed and the queue is empty*/
void *short_job(int *);

/*Precondition: All the threads have finished as well as the calculation function. All the stored times for wait_time are stored properly
 *Postcondition: Displays the percentage utilization of the server(s)*/
void utilize();

