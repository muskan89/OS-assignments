1.What the program does?  |
--------------------------

We were suppose to write a program that arranges the communication between server and client.
The request of client is reached to dispatcher.
Dispatcher is using dll_invoker to execute the function requested.

Assumptions


Here, The client will send request message in the form of string to server which is in format "dll_name+function_name+paramer1+parameter2+.........."

"+" is used as delimiter.

WHERE   dll_name ->Name of a dynamically loaded library,

          function_name ->Name of a function to call from the DLL,

          parameter1,parameter2,...  ->They are the arguments of the function "function_name"

I have assumed that the request message will ask to execute certain functions from math.h library only.
cos, sin, tan, pow, floor, ceil, sqrt, exp, log, log10, fabs are the functions which can be executed after request.





-------------------------------------------------------------------------------------------------------------

------------------------------------------------------------
2.A description of how this program works (i.e. its logic)  |
-------------------------------------------------------------
Firstly it start a socket for communication between server and client. 
By starting I mean it makes a socket by creating a file named using the input in Command.
Then it listens for client and It will accept connect requests and will make worker threads each time to call dispatcher .
After extracting dll_name,function_name and parameters from the request message the program is using dispatcher through thread create in server.
In dispatcher function it will firstly extract the dll_name, function_name and parameters of functions.
Then it will call dll_invoker to execute the function requested.
It will first check through conditional statements that the function belongs to the collection of functions from library math.h which is mentioned in assumption or not.
If it does not belong to them then server will print that the function is not present to execute.
After execution server is printing the result it got after execution.


 
--------------------------------------------------------------------------------------------------------
3. How to compile and run this program  |
----------------------------------------

Commands to compile the code

gcc local_socket_client_server.c -lpthread -ldl


Open two terminal after compiling the code.
On one terminal enter the command:  

./a.out server ./cs303  (This will serve the purpose of server)

One another terminal enter the command: 

./a.out client ./cs303 "libm.so.6+sin+2"        (This will serve the purpose of client)  

NOTE: The command for client is just an example for running sin function from libm.so.6. We can use command for executing log10 function by entering        ./a.out client ./cs303 "libm.so.6+log10+100" 



