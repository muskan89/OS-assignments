Submitter name: Muskan Gupta

Roll No.:   2019csb1100

Course: CS303

=================================

# 1.What does this program do? 

        The code is to simulate the detection of deadlocks among a pool of worker threads.
        For this, a situation is created where the thread pool is requesting for resources all the time.
        Meanwhile, in a fixed time interval 'd' a thread t' is checking if there is any deadlock among the threads and we are also tracking time interval between deadlocks.
        For testing purpose heuristics are provided.
        After testing the same situation with different heuristics we can see average time interval between deadlocks in different cases.
        Different cases are used to decide which thread should be terminated and replaced so that deadlock gets end.



# 2.A description of how this program works (i.e. its logic)  


        First of all, program is taking input through CLI arguments. It is taking maximum no. of threads that will be used, interval between deadlock detection, heuristic case no. , time after which deadlock detection should be stopped,maximum no. of instance of resources.
        Then, all the threads(maximum no. as given in CLI) are created.
        And a function is defined for worker threads which creates a random set R_t of resources of each type.
        And then if resources are available then threads get the resource allocated and if not it is ignored.
        After the set is left with either 0 zero request for resources or requests which cannot be fulfilled because of resource non-availablity then the process repeats.
        A function for deadlock detection is also defined which tests after each time interval of d seconds (given in CLI) whether any thread is involved in deadlock or not.
        In the function, it is checked that whether the no. of resources is available to fulfill the request for the thread or not, if not then deadlock is there.
        When deadlock is detected then we need to terminate thread so that it gets end for that 3 heuristics are made.

        1st heuristic is "if a deadlocked_thread has maximum amount of request of resources"
        2nd heuristic is "if a deadlocked_thread has maximum sum of request of resources"
        3rd heuristic is "if a deadlocked_thread has minimum allocation of request of resources"

# Note 
        In CLI argument in the place of heuristic_case '1' is to be enter for 1st heuristic, '2' is to be enter for 2nd heuristic and '3' is to be enter for 3rd heuristic.
        Time interval between deadlocks are also tracked and at the end average time between deadlocks is also calculated.


# Observation
        After checking the same test case for different heuristics, I observed that longest average time between deadlocks is in the case of 1st heuristic (if a deadlocked_thread has maximum amount of request of resources) and shortest average time between deadlocks is in the case of 2nd heuristic (if a deadlocked_thread has maximum sum of request of resources).






 
--------------------------------------------------------------------------------------------------------
# 3. How to compile and run this program  
----------------------------------------

        Commands to compile the code
        gcc main_file.c -lpthread

        After that enter the command:  
        ./a.out max_no_of_threads interval_for_deadlock_detection heuristic_case termination_after_time resouce(1)_max_ins_no. resouce(2)_max_ins_no. .....

        where, 
        max_no_of_threads -> maximum no. of threads that will be used
        interval_for_deadlock_detection -> interval between deadlock detection
        heuristic_case -> there is 3 heuristic in my code so to run each the value could be 1 or 2 or 3;
        termination_after_time -> time after which deadlock detection should be stopped
        resouce(1)_max_ins_no. -> maximum no. of instance of first resource
        resouce(2)_max_ins_no. -> maximum no. of instance of second resource
        ....... -> resource(n)_max_ins_no.


