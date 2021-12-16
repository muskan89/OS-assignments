#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <pthread.h>
#include <sys/un.h>
#include <stddef.h>
#include <dlfcn.h>

int create_worker_thread(int ,pthread_t );

/**
 * This code is adapted from the samples available at:
 * https://gist.github.com/bsodhi/a24e06e6806b685fb4d633c40638e77b
 *
 * Compile it using: gcc local_socket_client_server.c -lpthread -ldl

 */


//This function is used to check whether the function is from the given functions of math.h or not
int strcomparison(char a[],char b[])
{
	int i=0;
	while(a[i]!='\0' && b[i]!='\0')
	{
		if(a[i]!=b[i]){
			return 0;
		}
		i++;
	}
	if(a[i]!='\0' || b[i]!='\0')
	{
		return 0;
	}
	return 1;
}

//To print the message
void log_msg(const char *msg, bool terminate)
{
    printf("%s\n", msg);
    if (terminate) exit(-1); 
}

//Creating socket
int make_named_socket(const char *socket_file, bool is_client) 
{
    printf("Creating AF_LOCAL socket at path %s\n", socket_file);
    if (!is_client && access(socket_file, F_OK) != -1) 
	{
        log_msg("An old socket file exists, removing it.", false);
        if (unlink(socket_file) != 0) 
		{
            log_msg("Failed to remove the existing socket file.", true);
        }
    }
    struct sockaddr_un name;
    /* Create the socket. */
    int sock_fd = socket(AF_LOCAL, SOCK_STREAM, 0);
    if (sock_fd < 0) 
	{
        log_msg("Failed to create socket.", true);
    }

    /* Bind a name to the socket. */
    name.sun_family = AF_LOCAL;
    strncpy (name.sun_path, socket_file, sizeof(name.sun_path));
    name.sun_path[sizeof(name.sun_path) - 1] = '\0';

    /* The size of the address is
       the offset of the start of the socket_file,
       plus its length (not including the terminating null byte).
       Alternatively you can just do:
       size = SUN_LEN (&name);
   */
    size_t size = (offsetof(struct sockaddr_un, sun_path) +
                   strlen(name.sun_path));
    if (is_client) 
	{
        if (connect(sock_fd, (struct sockaddr *) &name, size) < 0) 
		{
            log_msg("connect failed", 1);
        }
    } else {
        if (bind(sock_fd, (struct sockaddr *) &name, size) < 0) 
		{
            log_msg("bind failed", 1);
        }
    }
    return sock_fd;
}

/**
 * Starts a server socket that waits for incoming client connections.
 * @param socket_file
 * @param max_connects
 */
//
void start_server_socket(char *socket_file, int max_connects) 
{
    int sock_fd = make_named_socket(socket_file, false);

    /* listen for clients, up to MaxConnects */
    if (listen(sock_fd, max_connects) < 0) 
	{
        log_msg("Listen call on the socket failed. Terminating.", true); /* terminate */
    }
    log_msg("Listening for client connections...\n", false);
    /* Listens indefinitely */
    
    
    
    pthread_t active_threads[100];
    int thread_index=0;
    while (1) 
	{
        struct sockaddr_in caddr; /* client address */
        int len = sizeof(caddr);  /* address length could change */

        printf("Waiting for incoming connections...\n");
        int client_fd = accept(sock_fd, (struct sockaddr *) &caddr, &len);  /* accept blocks */

        if (client_fd < 0) 
		{
            log_msg("accept() failed. Continuing to next.", 0); /* don't terminate, though there's a problem */
            continue;
        }
        
        
        
        /* Start a worker thread to handle the received connection. */
        if(thread_index>=100)
		{
        	printf("active_threads limit get exceeded\n");
        	break;
		}
        if (create_worker_thread(client_fd,active_threads[thread_index])==0) 
		{
            log_msg("Failed to create worker thread. Continuing to next.", 0);
            continue;
        }
        thread_index++;
		
    }  
}


/**
 * This functions is executed in a separate thread.
 * @param sock_fd
 */
void dispatcher_logic(int sock_fd) 
{
    log_msg("SERVER: thread_function: starting", false);
    char buffer[5000];
    memset(buffer, '\0', sizeof(buffer));
    int count = read(sock_fd, buffer, sizeof(buffer));
    if (count > 0) 
	{
        printf("SERVER: Received from client: %s\n", buffer);
        write(sock_fd, buffer, sizeof(buffer)); /* echo as confirmation */
        
        
        //Extract the DLL invocation request info from client socket
        int i=0,flag=0,j=0,k=-1;
	    char dll_name[501];
	    memset(dll_name, '\0', sizeof(dll_name));
	    char func_name[501];
	    memset(func_name, '\0', sizeof(func_name));
	    char func_args[501][501];
	    memset(func_args, '\0', sizeof(func_args));
	    while(buffer[i] != '\0')
		{		
		//here the client will ask through the string as "dll_name+func_name+parameter1+parameter2+....."
	    	if(buffer[i]=='+')
			{
	    		if(flag<=1)
				{
		    		flag++;
		    		j=0;	
				}
	    		
	    		if(flag==2)
				{
	    			k++;
	    			j=0;
				}
	    		i++;
	    		continue;
			}
			
			if(flag==0 && j<501)
			{
				dll_name[j]=buffer[i];
				j++;
			}else if(flag==1 && j<501)
			{
				func_name[j]=buffer[i];
				j++;
			}else if(flag==2 && k<501 && j<501)
			{
				//prepare the func params
				func_args[k][j]=buffer[i];
				j++;
			}
			
	    	i++;
		}
		
		
		void *handle;
	    char *error;
		//load the DLL
	    handle = dlopen (dll_name, RTLD_LAZY);
	    if (!handle) 
		{
	        fputs (dlerror(), stderr);
	        close(sock_fd); /* break connection */
		    log_msg("error in accessing library file\n", false);
		    pthread_exit(NULL); // Must be the last statement
		    return;
	    }
	
	    
	    if ((error = dlerror()) != NULL) 
		{
	        fputs(error, stderr);
	        close(sock_fd); /* break connection */
		    log_msg("error in calling function\n", false);
		    pthread_exit(NULL); 
	        return;
	    }
	
	    double ans=-1;
	    //invoke the dll function with the above parameter
	    if(strcomparison(func_name,"cos")==1)
		{ 		//If client is asking for execution of cos function
	    	double (*function)(double);
	    	function = dlsym(handle,func_name );
	    	if(!function)
			{
	    		printf("The requested function is not present\n");
	    		return;
			}
	    	ans=(*function)(atof(func_args[0]));
		}else if(strcomparison(func_name,"sin")==1)
		{	//If client is asking for execution of sin function
			double (*function)(double);
			function = dlsym(handle,func_name );
			if(!function)
			{
	    		printf("The requested function is not present\n");
	    		return;
			}
			ans=(*function)(atof(func_args[0]));
		}else if(strcomparison(func_name,"tan")==1)
		{	//If client is asking for execution of tan function
			double (*function)(double);
			function = dlsym(handle,func_name );
			if(!function)
			{
	    		printf("The requested function is not present\n");
	    		return;
			}
			ans=(*function)(atof(func_args[0]));
		}else if(strcomparison(func_name,"pow")==1)
		{		//If client is asking for execution of pow function
			double (*function)(double,double);
			function = dlsym(handle,func_name );
			if(!function)
			{
	    		printf("The requested function is not present\n");
	    		return;
			}
			ans=(*function)(atof(func_args[0]),atof(func_args[1]));
		}else if(strcomparison(func_name,"floor")==1)
		{		//If client is asking for execution of cos function
			double (*function)(double);
			function = dlsym(handle,func_name );
			if(!function)
			{
	    		printf("The requested function is not present\n");
	    		return;
			}
			ans=(*function)(atof(func_args[0]));
		}else if(strcomparison(func_name,"ceil")==1)
		{		//If client is asking for execution of ceil function
			double (*function)(double);
			function = dlsym(handle,func_name );
			if(!function)
			{
	    		printf("The requested function is not present\n");
	    		return;
			}
			ans=(*function)(atof(func_args[0]));
		}
		else if(strcomparison(func_name,"sqrt")==1)
		{		//If client is asking for execution of sqrt function
			double (*function)(double);
			function = dlsym(handle,func_name );
			if(!function)
			{
	    		printf("The requested function is not present\n");
	    		return;
			}
			ans=(*function)(atof(func_args[0]));
		}else if(strcomparison(func_name,"exp")==1)
		{		//If client is asking for execution of exp function
			double (*function)(double);
			function = dlsym(handle,func_name );
			if(!function)
			{
	    		printf("The requested function is not present\n");
	    		return;
			}
			ans=(*function)(atof(func_args[0]));
		}else if(strcomparison(func_name,"log")==1)
		{		//If client is asking for execution of log function
			double (*function)(double);
			function = dlsym(handle,func_name );
			if(!function)
			{
	    		printf("The requested function is not present\n");
	    		return;
			}
			ans=(*function)(atof(func_args[0]));
		}else if(strcomparison(func_name,"log10")==1)
		{		//If client is asking for execution of log10 function
			double (*function)(double);
			function = dlsym(handle,func_name );
			if(!function)
			{
	    		printf("The requested function is not present\n");
	    		return;
			}
			ans=(*function)(atof(func_args[0]));
		}else if(strcomparison(func_name,"fabs")==1)
		{		//If client is asking for execution of fabs function
			double (*function)(double);
			function = dlsym(handle,func_name );
			if(!function)
			{
	    		printf("The requested function is not present\n");
	    		return;
			}
			ans=(*function)(atof(func_args[0]));
		}
		else
		{
			printf("The requested function is not present :( \n");
		}	
		if(ans!=-1)
		{	//printing the answer we got after executing functions
			printf("%f\n",ans);
		}
		dlclose(handle);
	}
	
    close(sock_fd); /* break connection */
    log_msg("SERVER: thread_function: Done. Worker thread terminating.", false);
    pthread_exit(NULL); // Must be the last statement
}

/**
 * This function launches a new worker thread.
 * @param sock_fd
 * @return Return true if thread is successfully created, otherwise false.
 */
int create_worker_thread(int sock_fd,pthread_t thr_id) 
{
    log_msg("SERVER: Creating a worker thread.", false);
    
    int rc = pthread_create(&thr_id,
            /* Attributes of the new thread, if any. */
                            NULL,
            /* Pointer to the function which will be
             * executed in new thread. */
                            dispatcher_logic,
            /* Argument to be passed to the above
             * thread function. */
                            (void *)sock_fd);
    if (rc) 
	{
        log_msg("SERVER: Failed to create thread.", false);
        return 0;
    }
    return 1;
}

/**
 * Sends a message to the server socket.
 * @param msg Message to send
 * @param socket_file Path of the server socket on localhost.
 */
void send_message_to_socket(char *msg, char *socket_file) 
{
	
    int sockfd = make_named_socket(socket_file, true);

    /* Write some stuff and read the echoes. */
    log_msg("CLIENT: Connect to server, about to write some stuff...", false);
    if (write(sockfd, msg, strlen(msg)) > 0) 
	{
        /* get confirmation echoed from server and print */
        char buffer[5000];
        memset(buffer, '\0', sizeof(buffer));
        if (read(sockfd, buffer, sizeof(buffer)) > 0) 
		{
            printf("CLIENT: Received from server:: %s\n", buffer);
        }
    }
    log_msg("CLIENT: Processing done, about to exit...", false);
    close(sockfd); /* close the connection */
}



int main(int argc, char *argv[]) 
{
    if (argc < 3) 
	{
        printf("Usage: %s [server|client] [Local socket file path] [Message to send (needed only in case of client)]\n",
               argv[0]);
        exit(-1);
    }
    if (0 == strcmp("server", argv[1])) 
	{ 		
        start_server_socket(argv[2], 10);
    } else 
	{
        send_message_to_socket(argv[3], argv[2]);
    }
}
