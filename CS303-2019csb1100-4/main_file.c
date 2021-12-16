#include<stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <stdbool.h>
#include <stddef.h>
#include <math.h>
#include <limits.h>
#include <time.h>

int Ts,N,F;
typedef struct address
{		//coded a struct having platter,cylinder,sector and number of requested sectors
	int platter;
	int cylinder;
	int sector;
	int numOfReqSec;
}address;

address* arr;			//global variables
int* check;
double* remember_response;
double miNans=(double)INT_MAX,maxAns=(double)INT_MIN,r,seektime,avgAns,rotational_Delay,avg_sec_acc_tme,trans,ans=0,crossing_track_time,throughputPerSec=0,sumAns=0;

double minm(double a,double b)		//to find out minimum
{
	if(a<b)
	{
		return a;
	}
	return b;
}

double maxm(double a,double b)	//to find out maximum
{
	if(a>b)
	{
		return a;
	}
	return b;
}

double stdev()
{				//to calculate standard deviation
	int i=0;
	double sumSqr=0,temp;
	if(remember_response[100]==0 && remember_response[101]==0)
	{
		for(i=0;i<100;i++)
		{
			temp=(remember_response[i]-avgAns);
			temp=pow(temp,2);
			temp/=100.0;
			sumSqr+=temp;
		}
		sumSqr=pow(sumSqr,0.5);
	}else if(remember_response[100]!=0 && remember_response[101]==0)
	{
		for(i=0;i<101;i++)
		{
			temp=(remember_response[i]-avgAns);
			temp=pow(temp,2);
			temp/=101.0;
			sumSqr+=temp;
		}
		sumSqr=pow(sumSqr,0.5);
	}else if(remember_response[100]!=0 && remember_response[101]!=0)
	{
		for(i=0;i<102;i++)
		{
			temp=(remember_response[i]-avgAns);
			temp=pow(temp,2);
			temp/=102.0;
			sumSqr+=temp;
		}
		sumSqr=pow(sumSqr,0.5);
	}
	return sumSqr;
}


void randomm()			//requests are chosen randomly at each step
{
	srand(time(0));
	int i=0,ind=-1,flag=0,prevTrack=(rand() % 25) + 1;
	while(flag==0)
	{
		ind=(rand() % 100);
		if(check[ind]==0)
		{
			check[ind]=1;
			seektime=(double)(abs(arr[ind].cylinder-prevTrack)*crossing_track_time);
			rotational_Delay=avg_sec_acc_tme;
			trans=(double)(3000 * arr[i].numOfReqSec/ r);
			ans+=(seektime + rotational_Delay + trans);
			
			remember_response[i]=ans;
			sumAns+=(double)(ans/100.0);
			miNans=minm(miNans,ans);
			maxAns=maxm(maxAns,ans);
			
			prevTrack=arr[ind].cylinder;
		}else
		{
			continue;		//if that particular request is already serviced
		}
		flag=1;
		for(i=0;i<100;i++)
		{			//if all requests are done
			if(check[i]==0)
			{	
				flag=0;
				break;
			}
		}
		
	}
	
	return;
}

void fifo()			//implementation of FIFO 
{
	srand(time(0));
	int i=0,prevTrack=(rand() % 25) + 1;
	for(i=0;i<100;i++)
	{
		seektime=(double)(abs(arr[i].cylinder-prevTrack)*crossing_track_time);
		rotational_Delay=avg_sec_acc_tme;
		trans=(double)(3000 * arr[i].numOfReqSec/ r);
		ans+=(seektime + rotational_Delay + trans);	
		
		remember_response[i]=ans;
		sumAns+=(double)(ans/100.0);
		miNans=minm(miNans,ans);
		maxAns=maxm(maxAns,ans);
			
		
		prevTrack=arr[i].cylinder;
	}
	
	return;
}

void sstf()			//implementation of SSTF
{
	int cnt=0;
	srand(time(0));
	int prevTrack=(rand() % 25) + 1;
	while(cnt<100)
	{
		int i=0,ind=-1,mini=INT_MAX;
		double diff;
		for(i=0;i<100;i++)
		{
			diff=(double)(abs(arr[i].cylinder-prevTrack)*crossing_track_time);
			if(diff<mini)
			{
				mini=diff;
				ind=i;
			}
		}
		seektime=(double)(abs(arr[ind].cylinder-prevTrack)*crossing_track_time);
		rotational_Delay=avg_sec_acc_tme;
		trans=(double)(3000 * arr[ind].numOfReqSec/ r);
		ans+=(seektime + rotational_Delay + trans);	
		
		remember_response[ind]=ans;
		sumAns+=(double)(ans/100.0);
		miNans=minm(miNans,ans);
		maxAns=maxm(maxAns,ans);
			
		
		prevTrack=arr[ind].cylinder;
		arr[ind].cylinder=INT_MAX;
		cnt++;
	}
	
	return;
}


void bubble(address a[],int n)
{			//bubble sort for sorting the cylinder addresses in SCAN and C-SCAN implementations
	int isSwapped=1,i=0,j=0;
	for(i=0;i<n-1 && isSwapped;i++)
	{
		isSwapped=0;
		for(j=0;j<n-i-1;j++)
		{
			if(a[j].cylinder >a[j+1].cylinder)
			{
				int temp=a[j].cylinder;
				a[j].cylinder=a[j+1].cylinder;
				a[j+1].cylinder=temp;
				isSwapped=1;
			}
		}
	}
}



void scan()			//implementation of SCAN
{
	bubble(arr,100);
	srand(time(0));
	int i=0,prevTrack=(rand() % 25) + 1,ind=-1;
	for(i=0;i<100;i++)
	{
		if(arr[i].cylinder > prevTrack)
		{
			ind=i;
			break;
		}
	}
	for(i=ind;i<100;i++)
	{
		seektime=(double)(abs(arr[i].cylinder-prevTrack)*crossing_track_time);
		rotational_Delay=avg_sec_acc_tme;
		trans=(double)(3000 * arr[i].numOfReqSec/ r);
		ans+=(seektime + rotational_Delay + trans);	
		
		remember_response[i]=ans;
		sumAns+=(double)(ans/101.0);
		miNans=minm(miNans,ans);
		maxAns=maxm(maxAns,ans);
		
		prevTrack=arr[i].cylinder;
	}
	seektime=(double)((25-prevTrack)*crossing_track_time);
	rotational_Delay=avg_sec_acc_tme;
	ans+=(seektime + rotational_Delay);
	
	remember_response[100]=ans;
	sumAns+=(double)(ans/101.0);
	miNans=minm(miNans,ans);
	maxAns=maxm(maxAns,ans);

	prevTrack=25;
	for(i=ind-1;i>=0;i--)
	{
		seektime=(double)(abs(arr[i].cylinder-prevTrack)*crossing_track_time);
		rotational_Delay=avg_sec_acc_tme;
		trans=(double)(3000 * arr[ind].numOfReqSec/ r);
		ans+=(seektime + rotational_Delay + trans);	
		
		remember_response[i]=ans;
		sumAns+=(double)(ans/100.0);
		miNans=minm(miNans,ans);
		maxAns=maxm(maxAns,ans);

			
		prevTrack=arr[i].cylinder;
	}
	
	
	
	return;
}

void cscan()		//implementation of C-SCAN
{
	bubble(arr,100);
	srand(time(0));
	int i=0,prevTrack=(rand() % 25) + 1,ind=-1;
	for(i=0;i<100;i++)
	{
		if(arr[i].cylinder > prevTrack)
		{
			ind=i;
			break;
		}
	}
	for(i=ind;i<100;i++)
	{
		seektime=(double)(abs(arr[i].cylinder-prevTrack)*crossing_track_time);
		rotational_Delay=avg_sec_acc_tme;
		trans=(double)(3000 * arr[ind].numOfReqSec/ r);
		ans+=(seektime + rotational_Delay + trans);
		
		remember_response[i]=ans;
		sumAns+=(double)(ans/102.0);
		miNans=minm(miNans,ans);
		maxAns=maxm(maxAns,ans);

			
		prevTrack=arr[i].cylinder;
	}
	seektime=(double)((25-prevTrack)*crossing_track_time);
	ans+=(seektime + rotational_Delay);
	
	remember_response[100]=ans;
	sumAns+=(double)(ans/102.0);
	miNans=minm(miNans,ans);
	maxAns=maxm(maxAns,ans);

	
	seektime=(double)(24*crossing_track_time);
	ans+=(seektime + rotational_Delay);
	
	remember_response[101]=ans;
	sumAns+=(double)(ans/102.0);
	miNans=minm(miNans,ans);
	maxAns=maxm(maxAns,ans);

	
	prevTrack=1;
	for(i=0;i<ind;i++)
	{
		seektime=(double)(abs(arr[i].cylinder-prevTrack)*crossing_track_time);
		rotational_Delay=avg_sec_acc_tme;
		trans=(double)(3000 * arr[ind].numOfReqSec/ r);
		ans+=(seektime + rotational_Delay + trans);	
		
		remember_response[i]=ans;
		sumAns+=(double)(ans/102.0);
		miNans=minm(miNans,ans);
		maxAns=maxm(maxAns,ans);

		
		prevTrack=arr[i].cylinder;
	}
	
	return;
}


int main(int argc,char **argv)
{
	//format of CLI arguments
	//a.out r N Ts F
	if(argc<5)
	{
		printf("Please enter the input in the format given below:\n");
		printf("a.out r N Ts F\n");
		printf("FYI\n r is in revolution per minute,  N is in byte(s) and Ts is in ms, F is 1 for Random, 2 for FIFO, 3 for SSTF, 4 for SCAN and 5 for C-SCAN\n");
		exit(-1);
	}
	r=atof(argv[1]);	//taken all arguments from CLI
	N=atoi(argv[2]);	
	Ts=atoi(argv[3]);				
	F=atoi(argv[4]);

	printf("r=%lf revolution per minute  Ts=%dms  N=%d byte(s) F=%d\n",r,Ts,N,F);
	int i=0;
	arr=(address *)malloc(sizeof(address)*100);		//array of addresses of <platter,cylinder,sector>,number of requested sectors
	check=(int*)malloc(sizeof(int)*100);		//array useful in  Random function to see if all requests are done or not
	remember_response=(double*)malloc(sizeof(double)*102);	//array useful to find out standard deviation
	srand(time(0));
	for(i=0;i<100;i++)
	{
		arr[i].platter=(rand() % 4) + 1;
		arr[i].cylinder=(rand() % 25) + 1;
		arr[i].sector=(rand() % 20) + 1;
		arr[i].numOfReqSec=(rand() % 20) + 1;
		check[i]=0;
		remember_response[i]=0;
	}
	double stdDevans;
	avg_sec_acc_tme=(60*1000)/2;
	avg_sec_acc_tme/=(double)r;
	crossing_track_time=(double)(3*Ts)/(double)25.0;
	
	if(F==1)						//calling function according to choice of user
	{
		printf("So you have chosen Random to service requests\n");
		randomm();
	}else if(F==2)
	{
		printf("So you have chosen FIFO to service requests\n");
		fifo();
	}else if(F==3)
	{
		printf("So you have chosen SSTF to service requests\n");
		sstf();
	}else if(F==4)
	{
		printf("So you have chosen SCAN to service requests\n");
		scan();
	}else if(F==5)
	{
		printf("So you have chosen C-SCAN to service requests\n");
		cscan();
	}else
	{
		printf("You had to choose the value of F between 1 to 5\n");
		exit(-1);
	}
	throughputPerSec=(100.0*1000.0)/ans;			//final results
	printf("Throughput (requests/sec) : %lf\n",throughputPerSec);
	avgAns=sumAns;
	printf("Average of response time(ms) : %lf\n",avgAns);
	printf("Minimum of response time(ms) : %lf\n",miNans);
	printf("Maximum of response time(ms) : %lf\n",maxAns);
	stdDevans=stdev();
	printf("StdDev. of response time(ms) : %lf\n",stdDevans);
	
	return 0;
	
	
}
