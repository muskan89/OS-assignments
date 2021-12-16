#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdbool.h>
#include <stddef.h>
#include <math.h>
#include <limits.h>

int p,q,n,m,t,T,algo,instance=0,total_proc=0,nextIndex=0,memory_utilisation=0,count=0;
struct timeval end,begin,start;
int *memory;
struct timeval *proc_dur;
double turnAround=0,re=1;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

typedef struct Node
{
	int data;
	struct Node* next;
}Node;
typedef struct tnode
{
	struct timeval data;
	struct tnode* next;
}tnode;
Node *frnt1=NULL,*rr1=NULL,*frnt2=NULL,*rr2=NULL;
tnode *frnt3=NULL,*rr3=NULL;
void enqueue(Node** front,Node** rear,int item)		//for enqueuing in queue
{
	Node* nd=(Node*)malloc(sizeof(Node));
	if((*front)==NULL && (*rear)==NULL){
		nd->data=item;
		nd->next=NULL;
		*front=nd;
		*rear=nd;
	}else{
		nd->data=item;
		nd->next=NULL;
		(*rear)->next=nd;
		*rear=nd;
	}
}
int dequeue(Node** front,Node** rear){	//for dequeuing in queue
	int ans;
	if((*front)==NULL){
		ans=-1;
	}
	else if(*front==*rear && (*front)!=NULL){
		ans=(*front)->data;
		*front=NULL;
		*rear=NULL;
	}else{
		Node* nd=*front;
		ans=nd->data;
		*front=(*front)->next;
		nd->next=NULL;
		free(nd);
	}
	return ans;
}
int min(int a,int b)	//find minimum
{
	if(a<=b)return a;
	return b;
}
int max(int a,int b)	//find maximum
{
	if(a>=b)return a;
	return b;
}
void tenqueue(tnode** frnt,tnode** rar,struct timeval item){	//for enqueuing time in queue
	tnode* nd=(tnode*)malloc(sizeof(tnode));
	if((*frnt)==NULL && (*rar)==NULL){
		nd->data=item;
		nd->next=NULL;
		*frnt=nd;
		*rar=nd;
	}else{
		nd->data=item;
		nd->next=NULL;
		(*rar)->next=nd;
		*rar=nd;
	}
}
struct timeval tdequeue(tnode** frnt,tnode** rar){		//for dequeuing time in queue
	struct timeval ans;
	if((*frnt)==NULL){
		ans=start;
	}
	else if(*frnt==*rar && (*frnt)!=NULL){
		ans=(*frnt)->data;
		*frnt=NULL;
		*rar=NULL;
	}else{
		tnode* nd=*frnt;
		ans=nd->data;
		*frnt=(*frnt)->next;
		nd->next=NULL;
		free(nd);
	}
	return ans;
}
void *firstFit(){						//implemented first-fit algorithm
	int pop_ele=0,i=0,j=0,ele1=0;
	struct timeval ele2;
	while(1){
		
		pthread_mutex_lock(&lock);			//dequeueing is happening of size,duration and time of request if process
		pop_ele=dequeue(&frnt1,&rr1);//size 		
		ele1=dequeue(&frnt2,&rr2);//duration
		ele2=tdequeue(&frnt3,&rr3);//time
		pthread_mutex_unlock(&lock);
		
		if(pop_ele==-1){
			continue;			//if queue is empty come again
		}
		
		pop_ele/=10;			//each unit of memory array is of 10mb
		pthread_mutex_lock(&lock);
		int where=-1,num=p-q;
		pthread_mutex_unlock(&lock);
		num/=10;
		for(i=0;i<num;i++){			//checking for required amount of memory segment
			int flag=0;
			pthread_mutex_lock(&lock);
			if(memory[i]==0){
				int check=0;
				for(j=i;j<num;j++){
					if(memory[j]==0){
						check++;
					}else{
						break;
					}
				}
				
				if(check>=pop_ele){
					flag=1;				//found it
				}
			}
			pthread_mutex_unlock(&lock);
			if(flag==1){
				where=i;
				break;
			}
		}
		int sum=where+pop_ele-1,ent_loop=0;
		for(i=where;i<=sum;i++){		//allocation is going on if for loop runs
			ent_loop=1;
			pthread_mutex_lock(&lock);
			memory[i]=ele1;	//duration
			proc_dur[i]=ele2;	//time
			pthread_mutex_unlock(&lock);
		}
		if(ent_loop==0){
			continue;		//memory is full so process cannot get allocation
		}
		gettimeofday(&end, 0);
		long seconds = end.tv_sec - ele2.tv_sec;
		
		long microseconds = end.tv_usec - ele2.tv_usec;
    	double elapsed = seconds + (microseconds*1e-6);		//time between process request and allocation time
		
		pthread_mutex_lock(&lock);
		turnAround+=elapsed;
		total_proc++;
		pthread_mutex_unlock(&lock);
	}	
	
}


void *nextFit(){						//implemented next-fit algorithm
	int pop_ele=0,i=0,j=0,ele1=0;
	struct timeval ele2;
	while(1){
		pthread_mutex_lock(&lock);		//dequeueing is happening of size,duration and time of request if process
		pop_ele=dequeue(&frnt1,&rr1);//size
		ele1=dequeue(&frnt2,&rr2);//duration
		ele2=tdequeue(&frnt3,&rr3);//time
		pthread_mutex_unlock(&lock);
		
		if(pop_ele==-1){				//if queue is empty come again
			continue;
		}
		pop_ele/=10;				//each unit of memory array is of 10mb
		pthread_mutex_lock(&lock);
		int where=-1,num=p-q;
		pthread_mutex_unlock(&lock);
		num/=10;
		if(nextIndex>=num){
			nextIndex=0;
		}
		int flag=0;
		for(i=nextIndex;i<num;i++){				//checking for required amount of memory segment after previous iterated point
			flag=0;
			pthread_mutex_lock(&lock);
			if(memory[i]==0){
				int check=0;
				for(j=i;j<num;j++){
					if(memory[j]==0){
						check++;
					}else{
						break;
					}
				}
				
				if(check>=pop_ele){
					flag=1;		//found it
				}
			}
			pthread_mutex_unlock(&lock);
			if(flag==1){
				where=i;
				nextIndex=i;
				break;
			}
		}
		if(flag==0){
			for(i=0;i<nextIndex;i++){
				pthread_mutex_lock(&lock);
				if(memory[i]==0){
					int check=0;
					for(j=i;j<num;j++){
						if(memory[j]==0){
							check++;
						}else{
							break;
						}
					}
					
					if(check>=pop_ele){
						flag=1;
					}
				}
				pthread_mutex_unlock(&lock);
				if(flag==1){
					where=i;
					nextIndex=i;
					break;
				}
			}
		}
		
		int sum=where+pop_ele-1,ent_loop=0;
		for(i=where;i<=sum;i++){	//allocation is going on if for loop runs
			ent_loop=1;
			pthread_mutex_lock(&lock);
			memory[i]=ele1;	//duration
			proc_dur[i]=ele2;	//time
			nextIndex=i;
			pthread_mutex_unlock(&lock);
		}
		if(ent_loop==0){			//memory is full so process cannot get allocation
			continue;
		}
		
		gettimeofday(&end, 0);
		
		long seconds = end.tv_sec - ele2.tv_sec;
		
		long microseconds = end.tv_usec - ele2.tv_usec;
    	double elapsed = seconds + (microseconds*1e-6);		//time between process request and allocation time
		
		pthread_mutex_lock(&lock);
		turnAround+=elapsed;
		total_proc++;
		pthread_mutex_unlock(&lock);
	}	
	
	
	
}

void *bestFit(){						//implemented best-fit algorithm
	int pop_ele=0,i=0,j=0,ele1=0;
	struct timeval ele2;
	while(1){
		pthread_mutex_lock(&lock);		//dequeueing is happening of size,duration and time of request if process
		pop_ele=dequeue(&frnt1,&rr1);//size
		ele1=dequeue(&frnt2,&rr2);//duration
		ele2=tdequeue(&frnt3,&rr3);//time
		pthread_mutex_unlock(&lock);
		
		if(pop_ele==-1){		//if queue is empty come again
			continue;
		}
		pop_ele/=10;		//each unit of memory array is of 10mb
		pthread_mutex_lock(&lock);
		int where=-1,num=p-q;
		pthread_mutex_unlock(&lock);
		num/=10;
		int wer=0,mini=INT_MAX,here=-1;
		for(i=wer;i<num;i++){			//finding the minimum the block having minimum space but process could fit in it
			int flag=0,curr=i;
			pthread_mutex_lock(&lock);
			if(memory[i]==0){
				int check=0;
				for(j=i;j<num;j++){
					if(memory[j]==0){
						check++;
					}else{
						i=j;
						break;
					}
				}
				
				if(check>=pop_ele){
					i=j;
					if(check<mini){
						mini=check;
						here=curr;
					}
				}
			}
			
			pthread_mutex_unlock(&lock);
		}
		int sum=here+pop_ele-1,ent_loop=0;
		for(i=here;i<=sum;i++){			//allocation is going on if for loop runs
			ent_loop=1;
			pthread_mutex_lock(&lock);
			memory[i]=ele1;	//duration
			proc_dur[i]=ele2;	//time
			pthread_mutex_unlock(&lock);
		}
		if(ent_loop==0){		//memory is full so process cannot get allocation
			
			continue;
		}
		
		gettimeofday(&end, 0);
		long seconds = end.tv_sec - ele2.tv_sec;
		
		long microseconds = end.tv_usec - ele2.tv_usec;
    	double elapsed = seconds + (microseconds*1e-6);		
		
		pthread_mutex_lock(&lock);
		turnAround+=elapsed;						//time between process request and allocation time
		total_proc++;
		pthread_mutex_unlock(&lock);
		
	}	
	

}

void *letsgen()					//for generating 'r'  and 'r' processes along with their size 's' and duration 'd'
{
	pthread_mutex_lock(&lock);
	double lower=0.1*n;
	double upper=1.2*n;
	pthread_mutex_unlock(&lock);
	
	while(1){
		int kt=(upper - lower + 1),s=0,d=0;
		int i=0,r=(rand() % kt) + lower;
		for(i=0;i<r;i++){
			//----random s generated
			pthread_mutex_lock(&lock);
			double low=0.05*m;
			double upp=0.3*m;
			pthread_mutex_unlock(&lock);
			kt=(upp - low + 1);
			
			float ut=low;
			if(ut<1){
				ut=1;
			}
			int yt=(int)ut;
			if(kt!=0){
				s=(rand() % kt) + yt;
			}
			s*=10;
			//----random s generated
			pthread_mutex_lock(&lock);
			enqueue(&frnt1,&rr1,s);		//enqueued size of process
			
			gettimeofday(&end, 0);
			tenqueue(&frnt3,&rr3,end);		//enqueued time of process came
			pthread_mutex_unlock(&lock);
			//----random d generated
			pthread_mutex_lock(&lock);
			low=0.1*t;
			upp=1.2*t;
			pthread_mutex_unlock(&lock);
			kt=(upp - low + 1);
			
			float vt=low;
			if(vt<1){
				vt=1;
			}
			yt=(int)vt;
			if(kt!=0){
				d=(rand() % kt) + yt;
			}
			d*=5;
			//----random d generated
			pthread_mutex_lock(&lock);
			enqueue(&frnt2,&rr2,d);				//enqueued the duration
			pthread_mutex_unlock(&lock);
		}
		
		sleep(1);			//since the  'r' process is generated in 1 sec
		pthread_mutex_lock(&lock);
		instance++;
		pthread_mutex_unlock(&lock);
	}
	
}
void *ForDuration()				//It is taking care of duration of running process when it gets finished it deallocate it.
{
	struct timeval middle;
	int mem_part=0;
	while(1){
		int i=0,sze=(p-q);
		sze/=10;
		mem_part=0;
		pthread_mutex_lock(&lock);
		for(i=0;i<sze;i++){			//reducing the duration of  allcated process as the time passes and occupied memory is noticed for finding
			if(memory[i]>0){		//avg of memory utilisation %
				mem_part++;
				memory[i]-=1;
			}
		}
		
		
		mem_part*=10;
		mem_part+=q;
		memory_utilisation+=mem_part;
		count++;
		pthread_mutex_unlock(&lock);
		sleep(1);
	}
}

int main(int argc,char **argv)
{
	//format of CLI arguments
	//a.out p q n m t T algo
	if(argc<7)
	{
		printf("Please enter the input in the format given below:\n");
		printf("a.out p q n m t T algo\n");
		printf("FYI\n  T is in minutes and t is in seconds\n");
		exit(-1);
	}
	p=atoi(argv[1]);	
	q=atoi(argv[2]);	
	n=atoi(argv[3]);				
	m=atoi(argv[4]);
	t=atoi(argv[5]);				
	T=atoi(argv[6]);
	algo=atoi(argv[7]);
	
	printf("p=%d  q=%d  n=%d  m=%d  t=%dsec   T=%dminute(s) algo=%d\n",p,q,n,m,t,T,algo);
	
	
	int i=0,num=p-q;
	num/=10;
	
	printf("Already available memory = (p-q)mb which is %d mb\n",num);
	memory=(int *)malloc(sizeof(int)*num);
	
	proc_dur=(struct timeval *)malloc(sizeof(struct timeval)*num);
	
	gettimeofday(&begin, 0);
	gettimeofday(&start, 0);
	
	
	for(i=0;i<num;i++){
		memory[i]=0;
		proc_dur[i]=start;
	}
	pthread_t generate;
	
	
	int rk=0,ra,rb,rc,rd;
	
	double avg_timearound=re;
	rk = pthread_create(&generate,NULL,letsgen,NULL);	
	if(rk>0)
	{
		printf("Sorry! we are unable to generate\n");
		exit(-1);
	}
	pthread_t runALgo,durationHandler;
	if(algo==1){
		ra = pthread_create(&runALgo,NULL,firstFit,NULL);	
		if(ra>0)
		{
			printf("Sorry! we are unable to run\n");
			exit(-1);
		}else{
			re=1;
			printf("So you are using first-fit algorithm\n");
		}
	}else if(algo==2){
		rb = pthread_create(&runALgo,NULL,nextFit,NULL);	
		if(rb>0)
		{
			printf("Sorry! we are unable to run\n");
			exit(-1);
		}else{
			re=1;
			printf("So you are using next-fit algorithm\n");
		}
		
	}else if(algo==3){
		
		rc = pthread_create(&runALgo,NULL,bestFit,NULL);	
		if(rc>0)
		{
			printf("Sorry! we are unable to run\n");
			exit(-1);
		}else{
			re=100;
			printf("So you are using best-fit algorithm\n");
		}
		
	}	
	
	rd = pthread_create(&durationHandler,NULL,ForDuration,NULL);	
	if(rd>0)
	{
		printf("Sorry! we are unable to run\n");
		exit(-1);
	}	
	
	printf("Please wait for %d minute(s) for results\n",T);
	sleep(T*60);
	
	printf("\ntotal no. of processes that get executed:%d\n",total_proc);
	
	avg_timearound = (double)turnAround/(double)total_proc;
	avg_timearound*=re;	
	double mem_see=(double)memory_utilisation/(double)count;
	double uti_percent=mem_see/(double)p;
	double hundred=100;
	uti_percent*=hundred;
	printf("Memory utilisation: %lf percent\n",uti_percent);
	printf("Average timearound: %lf\n",avg_timearound);
	exit(-1);
	pthread_join(runALgo,NULL);
	pthread_join(durationHandler,NULL);
}
