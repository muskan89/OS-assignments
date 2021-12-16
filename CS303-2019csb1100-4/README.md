Submitter name: Muskan Gupta

Roll No.:   2019csb1100

Course: CS303

=================================

# 1.What does this program do? 

    This program is implementation of Random, FIFO, SSTF, SCAN and C-SCAN.
    Rotational speed of disk, r revolutions per minute.
    Average seek time, Ts in ms.
    Sector size, N in bytes.
    The above arguments are taken as CLI.
    The request addresses are generated randomly as well as number of requested sectors.
    The requests are to send to disk schedular, and it orders and send the requests for read, write and seek operations.
    This program finds out final throughput, Average, Minimum, Maximum and Standard deviation of response time.



# 2.A description of how this program works (i.e. its logic)  

        This program first of all accepts arguments in the format mentioned in third point. Then according to the choice of user for argument F it runs the particular algorithm function. Like it runs Random scheduled policy for F=1, FIFO for F=2,3 for SSTF, 4 for SCAN and 5 for C-SCAN.
        In Random schedule policy function the indices of request array is chosen randomly and response time is calculated.
        In FIFO, the generated requests are considered in increasing order of their time of generation.
        In SSTF, the one having minimum track difference with previous track is considered first.
        In SCAN, the requests' tracks are sorted first according to track or cylinder's positions. Then the request service starts from an initial track then it goes till one end and then it takes a reverse turn then again checks for request and service it.
        IN C-SCAN things happens in similar way as SCAN but here it does not take reverse turn instead it starts with other end and comes till the initial track so it is considered efficient than SCAN.

        After these throughput,  Average, Minimum, Maximum and Standard deviation of response time are calculated through formulation and function.
        And for the given entries in the problem statement output is entered in a table and is present in the same submitted folder.






 
--------------------------------------------------------------------------------------------------------
# 3. How to compile and run this program  |
----------------------------------------

Commands to compile the code

        gcc main_file.c -lm


After that enter the command:  
        
        ./a.out r N Ts F


where, 
        r is in revolution per minute,
        N is in byte(s),
        Ts is in ms,
        F is 1 for Random, 2 for FIFO, 3 for SSTF, 4 for SCAN and 5 for C-SCAN

Example_1 :   

        ./a.out 7500 512 4 1           

Output:

        r=7500.000000 revolution per minute  Ts=4ms  N=512 byte(s) F=1
        So you have chosen Random to service requests
        Throughput (requests/sec) : 94.482237
        Average of response time(ms) : 490.737600
        Minimum of response time(ms) : 8.400000
        Maximum of response time(ms) : 1058.400000
        StdDev. of response time(ms) : 486.193158

Example_2 :    

        ./a.out 15000 512 4 1          

Output:

        r=15000.000000 revolution per minute  Ts=4ms  N=512 byte(s) F=1
        So you have chosen Random to service requests
        Throughput (requests/sec) : 103.177879
        Average of response time(ms) : 496.650800
        Minimum of response time(ms) : 6.080000
        Maximum of response time(ms) : 969.200000
        StdDev. of response time(ms) : 496.050346

Example_3 :   

        ./a.out 7500 512 4 2         

Output:

        r=7500.000000 revolution per minute  Ts=4ms  N=512 byte(s) F=2
        So you have chosen FIFO to service requests
        Throughput (requests/sec) : 83.333333
        Average of response time(ms) : 593.396000
        Minimum of response time(ms) : 8.080000
        Maximum of response time(ms) : 1200.000000
        StdDev. of response time(ms) : 348.020394

Example_4 :   
        
        ./a.out 15000 512 4 2        

Output:

        r=15000.000000 revolution per minute  Ts=4ms  N=512 byte(s) F=2
        So you have chosen FIFO to service requests
        Throughput (requests/sec) : 117.046678
        Average of response time(ms) : 439.048400
        Minimum of response time(ms) : 9.520000
        Maximum of response time(ms) : 854.360000
        StdDev. of response time(ms) : 247.248733





