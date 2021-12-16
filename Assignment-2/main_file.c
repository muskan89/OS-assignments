#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdbool.h>
#include <stddef.h>


int max_no_of_threads,interval_for_deadlock_detection,no_of_resource,heuristic_case,termination_after_time;
int *max_instances,*available;
int **allocation,**request;
int no_of_deadlock=0,occured_first=0;
time_t end,begin,total_time_for_detection;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;


int min(int a,int b)
{
	if(a<=b)return a;
	return b;
}
int max(int a,int b)
{
	if(a>=b)return a;
	return b;
}
int heuristics1(int deadlocked_threads[])
{
	//if a deadlocked_thread has maximum amount of request of resources it can be consider for termination
	int i=0,j=0,thread_to_be_removed=0,maxi=-1,pre=-1;
	for(i=0;i<max_no_of_threads;i++)
	{
		if(deadlocked_threads[i]==1)
		{
			for(j=0;j<no_of_resource;j++)
			{
				pre=maxi;
				maxi=max(maxi,request[i][j]);
				if(pre!=maxi)
				{
					thread_to_be_removed=i;
				}
			}
		}
	}
	return thread_to_be_removed;
}
int heuristics2(int deadlocked_threads[])
{
	//if a deadlocked_thread has maximum sum of request of resources it can be consider for termination
	int i=0,j=0,thread_to_be_removed=0,maxi=-1,pre=-1,sum=0;
	for(i=0;i<max_no_of_threads;i++)
	{
		if(deadlocked_threads[i]==1)
		{
			sum=0;
			for(j=0;j<no_of_resource;j++)
			{
				sum+=request[i][j];
			}
			pre=maxi;
			maxi=max(maxi,sum);
			if(pre!=maxi)
			{
				thread_to_be_removed=i;
			}
		}
	}
	return thread_to_be_removed;
}
int heuristics3(int deadlocked_threads[])
{
	//if a deadlocked_thread has minimum allocation of request of resources it can be consider for termination
	int i=0,j=0,thread_to_be_removed=0,mini=1e5,pre=-1;
	for(i=0;i<max_no_of_threads;i++)
	{
		if(deadlocked_threads[i]==1)
		{
			for(j=0;j<no_of_resource;j++)
			{
				pre=mini;
				mini=min(mini,allocation[i][j]);
				if(pre!=mini)
				{
					thread_to_be_removed=i;
				}
			}
		}
	}
	return thread_to_be_removed;
}
void *thread_func(void *param)	//function for each threads t with parameter of thread index
{
	int thread_id=*((int *)param);

	while(1)
	{
		//set rt
		int rt[no_of_resource];
		int x=0,l=0;
		for(x=0;x<no_of_resource;x++)	//random set rt is decided
		{
			rt[x]=(rand() % max_instances[x]) + 1;
			pthread_mutex_lock(&lock);
			request[thread_id][x]=rt[x];	//request is added in request matrix for the thread's request to resource
			pthread_mutex_unlock(&lock);
		}
		int check=0;
		double time=0;
		//till we are done with set which we will be when set has either no request for resource or it is not available to fulfill  
		while(check==0)
		{
			//choosing the type of resource randomly
			x=(rand() % no_of_resource);
			if(available[x]<=0)continue;
			
			pthread_mutex_lock(&lock);
			l=min(available[x],rt[x]);	
			pthread_mutex_unlock(&lock);
			//thread gets resource
			if(l==rt[x])	//if request can be fulfilled or not
			{
				pthread_mutex_lock(&lock);
				request[thread_id][x]-=l;	//since request is fulfilled it is reduced from matrix
				available[x]-=l;			//since it is provided to thread ,so reduced from available
				rt[x]-=l;					//since request is fulfilled it is reduced from set
				allocation[thread_id][x]+=l;	//since it is allocated to thread, so added in matrix
				pthread_mutex_unlock(&lock);
			} 
			
			check=1;
			//checking if we are done with the set or not 
			for(x=0;x<no_of_resource;x++)
			{
				if(rt[x]>0 && rt[x]<=available[x])
				{
					check=0;
					break;
				}
			}
			//We are done with the set, so it is time to take pause of a random duration between (0.7d,1.5d) 
			if(check==1)
			{
				time=(rand() % 9) + 7;
	    		time*=interval_for_deadlock_detection;
	    		time=time/10;
	    		sleep(time);
	    		break;
			}
			
			//taking random pauses between requests , if we are not done with set till now
			time=(rand() % interval_for_deadlock_detection) + 1;
			sleep(time);
		}
		//repeat re-initialization ,since we are done with set so we are now repeating with thread
		//----------------------------
		int y=0,z=0;
		for(y=0;y<max_no_of_threads;y++)
		{
			for(z=0;z<no_of_resource;z++)
			{
				pthread_mutex_lock(&lock);
				request[y][z]=0;
				allocation[y][z]=0;
				pthread_mutex_unlock(&lock);
			}
		}
		for(y=0;y<no_of_resource;y++)
		{
			pthread_mutex_lock(&lock);
			available[y]=max_instances[y];
			pthread_mutex_unlock(&lock);
		}
		//---------------------------------
	}	
} 

void *detection()		//function for threads t' which will detect deadlock for current threads
{
	while(1)
	{ 
		//detection started
    	bool finish[max_no_of_threads];
    	int deadlocked_threads[max_no_of_threads];	//tracking which thread is involved in deadlock
    	int work[no_of_resource];			
    	int i=0,j=0;
    	
    	for(i=0;i<max_no_of_threads;i++)
		{
    		deadlocked_threads[i]=0;
		}
    	for(i=0;i<no_of_resource;i++)
		{
    		pthread_mutex_lock(&lock);
    		work[i]=available[i];          //available array is copied to work
    		pthread_mutex_unlock(&lock);
		}
		for(i=0;i<max_no_of_threads;i++)
		{
			int flag=1;
			for(j=0;j<no_of_resource;j++)
			{
				pthread_mutex_lock(&lock);
				if(allocation[i][j]!=0)	//if allocation for ith thread is all Zeros then it is finished and flag should remain 1
				{	
					flag=0;
					pthread_mutex_unlock(&lock);
					break;
				}
				pthread_mutex_unlock(&lock);
			}
			if(flag==1)
			{
				finish[i]=true;			//if allocation for ith thread is all Zeros then it is finished 
			}
			else
			{
				finish[i]=false;	//otherwise not
			}
		}
		for(i=0;i<max_no_of_threads;i++)
		{
			int track=1;
			for(j=0;j<no_of_resource;j++)
			{
				pthread_mutex_lock(&lock);
				if(request[i][j]>work[j])	//checking if resources is available to fulfill the request
				{	
					track=0;
					pthread_mutex_unlock(&lock);
					break;
				}
				pthread_mutex_unlock(&lock);
			}
			if(track==1 && finish[i]==false)	//if yes, then allocate the resource to thread
			{
				for(j=0;j<no_of_resource;j++)
				{
					pthread_mutex_lock(&lock);
					work[j]=work[j]+allocation[i][j];	
					pthread_mutex_unlock(&lock);
				}
				finish[i]=true;					//and finished
			}
		}
		
		int flagg=0;
		
		for(i=0;i<max_no_of_threads;i++)
		{
			
			if(finish[i]==false)	//if thread is still not finished with this, it means there is deadlock in (i+1)th thread.
			{
				deadlocked_threads[i]=1; 		//deadlock marked
				flagg=1;
			}
		}
		
		int thread_to_remove=-1;
		if(flagg==1)		//if deadlock has occured check which thread should be terminated through 3 heuristics
		{
			printf("Deadlock found in these threads: ");
			for(i=0;i<max_no_of_threads;i++){
				if(deadlocked_threads[i]==1){
					printf("%d ",i);
				}
			}
			printf("\n");
			pthread_mutex_lock(&lock);
			no_of_deadlock++;
			pthread_mutex_unlock(&lock);
			if(heuristic_case==1)		//if a deadlocked_thread has maximum amount of request of resources
			{
				thread_to_remove=heuristics1(deadlocked_threads);
			}else if(heuristic_case==2)	//if a deadlocked_thread has maximum sum of request of resources
			{
				thread_to_remove=heuristics2(deadlocked_threads);
			}else if(heuristic_case==3)		//if a deadlocked_thread has minimum allocation of request of resources
			{
				thread_to_remove=heuristics3(deadlocked_threads);
			}
			if(thread_to_remove>=0){	//if we get the thread to be removed, then terminate it after re-initializing its requests and allocation
				for(i=0;i<no_of_resource;i++)
				{
					pthread_mutex_lock(&lock);
					request[thread_to_remove][i]=0;
					available[i]+=allocation[thread_to_remove][i];	//all the allocated resources of the thread is returned to available
					allocation[thread_to_remove][i]=0;
					pthread_mutex_unlock(&lock);
				}
			}
			if(occured_first==0)
			{
				time(&begin);			//when deadlock occurs first time after execution started
				total_time_for_detection=0;
				occured_first=1;
			}else
			{
				time(&end);
				total_time_for_detection+=(double)(end - begin);	//time interval length is added
				time_t elapsed = end - begin;
				begin=end;
				printf("Time interval since last deadlock occured: %lf seconds.\n",(double)elapsed);
			}
			
		}else
		{
			printf("deadlock not occured\n");
		}
		
		printf("\n");
		sleep(interval_for_deadlock_detection);		//wait for d(interval_for_deadlock_detection) seconds
	}
}

int main(int argc,char **argv)
{
	//format of CLI arguments
	//a.out max_no_of_threads interval_for_deadlock_detection heuristic_case termination_after_time resouce1_max_instances resouce2_max_instances.......
	if(argc<6)
	{
		printf("Please enter the input in the format given below:\n");
		printf("a.out max_no_of_threads interval_for_deadlock_detection heuristic_case termination_after_time resouce1_max_instances_no. resouce2_max_instances_no. ......\n");
		exit(-1);
	}
	max_no_of_threads=atoi(argv[1]);	//maximum number of threads we have
	interval_for_deadlock_detection=atoi(argv[2]);	//time interval between deadlock detection through thread t'
	heuristic_case=atoi(argv[3]);				//which heuristic will be used to terminate the thread during deadlock
	termination_after_time=atoi(argv[4]);		//time interval after which you want the program to stop the detection of deadlock
	
	max_instances=(int *)malloc(sizeof(int)*(argc-5));	//array to store max instances of each resource
	available=(int *)malloc(sizeof(int)*(argc-5));		//resouces availablity of each type
	request=(int **)malloc(sizeof(int *) * max_no_of_threads);	//request matrix from thread to resource
	allocation=(int **)malloc(sizeof(int *) * max_no_of_threads);	//allocation matrix from resource to thread
	
	if(max_no_of_threads==0)
	{
		printf("You have provided 0 threads....please enter non-zero value of threads\n");
		exit(-1);
	}
	
	int i=0;
	no_of_resource=argc-5;
	
	for(i=0;i<argc-5;i++)
	{
		max_instances[i]=atoi(argv[i+5]);
		available[i]=max_instances[i];
	}
	for(i=0;i<max_no_of_threads;i++)
	{
		request[i]=(int *)malloc(sizeof(int) * no_of_resource);
		allocation[i]=(int *)malloc(sizeof(int) * no_of_resource);
	}
	pthread_t threads[max_no_of_threads];			//threads t are stored in this array
	int rc=0,rk=0;
	int *k = (int *)malloc(sizeof(int) * 1);
   	for(i=0;i<max_no_of_threads;i++)
	{
		*k=i;
		rc = pthread_create(&threads[i],NULL,thread_func,k);	//threads are created 
		if(rc>0)
		{
			printf("Sorry! we are unable to create a thread,%d\n",rc);
			exit(-1);
		}
	}
	
	pthread_t t_dash;			//thread t' is here for detection of deadlock
	rk = pthread_create(&t_dash,NULL,detection,NULL);	//created t'
	if(rk>0)
	{
		printf("Sorry! we are unable to create a thread,%d\n",rc);
		exit(-1);
	}
	sleep(termination_after_time);		//will sleep after time we want the program to stop the detection
	
	double avg_time_betn_deadlocks = (double)total_time_for_detection/(double)(no_of_deadlock-1);	//required average time between deadlocks 
	printf("Total deadlock occured: %d\n",no_of_deadlock);
	printf("Average between deadlocks: %lf\n",avg_time_betn_deadlocks);
	
	exit(-1);
	
	pthread_join(t_dash,NULL);
	
	
}
