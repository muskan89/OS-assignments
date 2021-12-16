Submitter name: Muskan Gupta

Roll No.:   2019csb1100

Course: CS303

=================================

# 1.What does this program do? 
--------------------------
In this assignment I had to implement three algorithms (first-fit,next-fit and best-fit).
And make table of the output or results, I am getting after running my code.
Also I had to observe that which algorithm is efficient in terms of time complexity.
So, this program can be used to run any of the above three algorithm as per user choice.
This program generates random number of processes using a thread and generate various parameters like their sizes and durations.
Then three functions are implemented which is running with the help of thread in parallel.
And there is one more thread which is taking care of duration of processes allocated in memory.
-------------------------------------------------------------------------------------------------------------



------------------------------------------------------------
# 2.A description of how this program works (i.e. its logic)  
-------------------------------------------------------------

The objecting of the program is to observe that in which order algorithms among first-fit, next-fit and best-fit is effective.
And, these algorithm is for dynammic partitioning. 
If I talk about implementation then I used an array for memory where the unit index is supposed to be of 10 MBs.
And I have made functions of all the three algorithms over this array. 
For time calculating purpose I have used struct timeval from sys/time.h library.And two function was to for random process generation purpose and for reducing the duration of process as time passes.During allocation I have assigned the duration of the allocated process to the array.And I have taken one more array to store the time of request of process so that when the process gets allocation in the memory time interval can be find out.For average turnaround time, I have added those calculated intervals and also calculated no. of processes executed.And for memory utilisation percentage I have done the same. Taken sum of memory occupied every seconds and taken average of it as well.


Taking about observation part, According to my table I have observed that first-fit algorithm is taking minimum average turnaround time overall , then next-fit is taking little more than that . And best-fit takes much greater time than both.

Conclusion: 
I would prefer first-fit algorithm for dynammic partitioning.
for time-complexity I can say first-fit < next-fit < best-fit
for preference: first-fit > next-fit > best-fit

For getting steady state results and entering the data in table I ran my code for 10 minutes and for testing purpose(examples) which I pasted in this readme I ran this for 5 minutes.

 
--------------------------------------------------------------------------------------------------------
# 3. How to compile and run this program  
----------------------------------------

Commands to compile the code
gcc main_file.c -lpthread

After that enter the command:  
./a.out a.out p q n m t T algo


where, 
p,q,n,m,t are given parameter as per problem statement.
here I have taken t in second(s) and T in minute(s).

And, algo=1 for running first - fit algorithm
algo=2 for running next - fit algorithm
algo=3 for running best - fit algorithm

-------------------------------------------------------------------
Example_1 :  ./a.out 1000 200 16 10 10 5 3                     

Output:
--------------------------------------------------------------------
p=1000  q=200  n=16  m=10  t=10sec   T=5minute(s) algo=3
Already available memory = (p-q)mb which is 80 mb
So you are using best-fit algorithm
Please wait for 5 minute(s) for results

total no. of processes that get executed:2853
Average timearound: 0.004082
Memory utilisation: 98.246667 percent


-------------------------------------------------------------------------
Example_2 : ./a.out 1000 200 10 20 10 5 2                         

Output:
-------------------------------------------------------------------
p=1000  q=200  n=10  m=20  t=10sec   T=5minute(s) algo=2
Already available memory = (p-q)mb which is 80 mb
So you are using next-fit algorithm
Please wait for 5 minute(s) for results

total no. of processes that get executed:2085
Average timearound: 0.000036
Memory utilisation: 97.223333 percent


--------------------------------------------------------------------
-------------------------------------------------------------------------
Example_3 : ./a.out 1000 200 10 10 10 5 1                             

Output:
-------------------------------------------------------------------
p=1000  q=200  n=10  m=10  t=10sec   T=5minute(s) algo=1
Already available memory = (p-q)mb which is 80 mb
So you are using first-fit algorithm
Please wait for 5 minute(s) for results

total no. of processes that get executed:2085
Average timearound: 0.000041
Memory utilisation: 98.220000 percent

