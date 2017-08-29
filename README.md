# iQ
iQ is a simulation of a queuing system with multiple options intended to run on the Linux operating system.

Installation:
In order to install iQ, a user simply needs to run the command "make" inside the directory the iQ file have been extracted to. This does not add any additional folders but a few temporary files will be created along with an executable named iQ.

Implementations and Design:
----------------------------------------------------------------------------------------------------------------------------------------------------------
iQ was designed to carefully simulate customers entering a queue and being processed by an entity of servers defaulting to 1. Utilizing POSIX threads, a minimum of three threads are created in order to complete the program and calculate statistics.

Thread 1 (creation thread) creates and places users into a queue for the process thread(s) to "service" the customer. The creation thread creates 1 customer, sleeps for the interval -log(1.0-(random-number-between-0-and-1))/lambda. Lambda can be changed by the user in an option flag discussed below, but defaults to 3.0. At the default rate, the average wait time to get into the queue is .33 seconds. Each customer is also given a service time using -log(1.0-(random-number-between-0-and-1))/Mu. Mu can also be set by the user but defaults to 4.0. At this rate, the service time for each customer is .25 seconds.

Thread 2 (process thread) processes users from the queue. Multiple servers can be implimented with an option choice but defaults to 1. In the process thread, the server will sleep for .01 seconds, wake up, and examine the queue. If there is a customer in the queue, that customer is processed and removed from the queue. This will continue until the number of processed customers is equal to the number of customers to be made. If, however, shorest job first is implimented, this will not be the case. When shortest job first is implemented, the process thread will sleep as usual but will examine the customers in the queue, locate the customer with the shortest service time, and process them first. This will, of course, continue until the number of processed is equal to the number of customers to be created.

Thread 3 (collection thread) collects data about the the size of the queue. Collection thread will sleep for .02 seconds, wake up, and store the number of people in the queue into an array. This will continue until all of the customers have been processed.

After all three have completed, calculations for the following are displayed:
-mean and standard deviation of waiting time
-mean and standard deviation of queue length
-the total service time of all customers in seconds
-percentage of the server's utilization

Options:
There are several options that can be implimented for iQ. They are as follows:
-S: value of 1 will implement shortest job first, anything else will run normally (FIFO).
-R: seeds the random number generator.
-N: number of servers (must be greater than 1 and less than 5).			default 1
-L: changes lambda to a new value (must NOT be greater than mu * lambda).	default 4.0
-M: changes mu to a new value.							default 3.0
-T: changes the total number of customers to make				default 1000
-V: allows the user to see the background processes of the threads.

Sample Outputs:
----------------------------------------------------------------------------------------------------------------------------------------------------------
iShell:> ./iQ -N3 -L5.1 -M1.8 -V -S1

The store times in the queue are as follows:
1.058614
1.850553
1.102270
Processing Customer: 994
The store times in the queue are as follows:
1.850553
1.102270
Processing Customer: 999
The store times in the queue are as follows:
1.850553
Processing Customer: 996
The mean wait time is 0 second(s) and 622774 microsecond(s).
The stadard deviation of the waiting time is 1 second(s) and 97269 microsecond(s).
The mean of the queue length is: 2.3130 people
The standard deviation for people in the queue is: 2.067062
total service time is: 542.175866
The utilization is: 97.086557 percent 

iShell:> ./iQ -S1 -R5 -T100 -V

The store times in the queue are as follows:
0.062729
0.140605
Processing Customer: 98
The store times in the queue are as follows:
0.140605
Processing Customer: 99
The mean wait time is 2 second(s) and 64731 microsecond(s).
The stadard deviation of the waiting time is 1 second(s) and 889444 microsecond(s).
The mean of the queue length is: 1.3076 people
The standard deviation for people in the queue is: 1.281240
total service time is: 27.211734
The utilization is: 80.551620 percent 

iShell:> ./iQ -V

The store times in the queue are as follows:
0.169896
0.001341
0.233589
Processing Customer: 997
The store times in the queue are as follows:
0.001341
0.233589
Processing Customer: 998
The store times in the queue are as follows:
0.233589
Processing Customer: 999
The mean wait time is 1 second(s) and 651785 microsecond(s).
The stadard deviation of the waiting time is 1 second(s) and 603745 microsecond(s).
The mean of the queue length is: 4.6194 people
The standard deviation for people in the queue is: 5.647452
total service time is: 264.899547
The utilization is: 82.009820 percent 

Special notes:
----------------------------------------------------------------------------------------------------------------------------------------------------------
-Added special option (-V) that allows the user to see the customers being added to and processed from the simulation. V was chosen for "Visible". THIS WILL INCREASE UTILIZATION TIME: since thread 2 has to go out and find the data for you to read, it becomes part of the processing time.

-Special code has been commented out, but would be very easy to implement a server case where multiple servers have their own queue lines. All that would be necessary to complete this, would be to uncomment out the commented line in the initilizations of the process thread and add [ps_id] in front of all the arrays to make them double arrays. (note that [ps_id+1] will be necessary for the initializations of the arrays).

-During testing, 1 run out of many did deadlock with 5 servers after about 75%. Currently, it is unknown if this was a fluke issue or a more underlying problem at closer to maximum number of servers. If this does happen, please rerun again as I have been unable to duplicate the issue since it happened.

-Progress bar will display one "." for every 5% complete. This is only based on the number of people created and not the servers ability to process them. At 100%, thread 1 will display "finished!". This isn't exactly true, but will likely only be off for the few moments that thread 2 finishes cleaning up the queue.

