#include <stdio.h>
#include <pthread.h>
#include <sys/time.h>

#define MAX		5

#define UNUSED	0
#define USED	1

int GnChopstick[5] = {0};		//同步共享变量，五支筷子

pthread_mutex_t mutex[5];		//5个互斥信号量，能否被访问
pthread_t		GpThread[5];	//线程ID

void *Aname();
void *Bname();
void *Cname();
void *Dname();
void *Ename();
int thread_create();
void thread_wait();

int main()
{
	int i;
	for(i = 0; i < MAX; i++)
		pthread_mutex_init(&mutex[i],NULL);
	
	printf("now the philosopher prepare to dinner!\n");
	for(i = 0; i < MAX; i++)
	{
		thread_create();
		thread_wait();
	}
	return 0;
}

int thread_create()
{
	int nNum;
	
	if((nNum = pthread_create(&GpThread[0],NULL,Aname,NULL)) != 0)
	{
		fprintf(stderr,"fail to create the Aname thread");
		return -1;
	
	}
	if((nNum = pthread_create(&GpThread[1],NULL,Bname,NULL)) != 0)
	{
		fprintf(stderr,"fail to create the Bname thread");
		return -1;
	}
	if((nNum = pthread_create(&GpThread[2],NULL,Cname,NULL)) != 0)
	{
		fprintf(stderr,"fail to create the Cname thread");
		return -1;
	}
	if((nNum = pthread_create(&GpThread[3],NULL,Dname,NULL)) != 0)
	{
		fprintf(stderr,"fail to create the Dname thread");
		return -1;
	}
	if((nNum = pthread_create(&GpThread[4],NULL,Ename,NULL)) != 0)
	{
		fprintf(stderr,"fail to create the Ename thread");
		return -1;
	}
}
void thread_wait()
{
	int i;

	for(i = 0; i < MAX; i++)
	{
		if(GpThread[i] != 0)
			pthread_join(GpThread[i],NULL);
	}
}

void *Aname()
{
	if(GnChopstick[0] == UNUSED && GnChopstick[4] == UNUSED)
	{
		pthread_mutex_lock(&mutex[0]);
		pthread_mutex_lock(&mutex[4]);
		
//		GnChopstick[0] = USED;
//		GnChopstick[4] = USED;
		printf("Aname is eating......\n");
		sleep(2);
//		GnChopstick[0] = UNUSED;
//		GnChopstick[4] = UNUSED;

		pthread_mutex_unlock(&mutex[0]);
		pthread_mutex_unlock(&mutex[4]);
	}
	else
		printf("Aname is thinking\n");
	
	pthread_exit(NULL);
}
void *Bname()
{
	if(GnChopstick[4] == UNUSED && GnChopstick[3] == UNUSED)
	{
		pthread_mutex_lock(&mutex[4]);
		pthread_mutex_lock(&mutex[3]);
		
//		GnChopstick[4] = USED;
//		GnChopstick[3] = USED;
		printf("Bname is eating......\n");
		sleep(2);
//		GnChopstick[4] = UNUSED;
//		GnChopstick[3] = UNUSED;

		pthread_mutex_unlock(&mutex[4]);
		pthread_mutex_unlock(&mutex[3]);
	}
	else
		printf("Bname is thinking\n");

	pthread_exit(NULL);
}
void *Cname()
{
	if(GnChopstick[3] == UNUSED && GnChopstick[2] == UNUSED)
	{
		pthread_mutex_lock(&mutex[3]);
		pthread_mutex_lock(&mutex[2]);
		
//		GnChopstick[3] = USED;
//		GnChopstick[2] = USED;
		printf("Cname is eating......\n");
		sleep(2);
//		GnChopstick[3] = UNUSED;
//		GnChopstick[2] = UNUSED;

		pthread_mutex_unlock(&mutex[3]);
		pthread_mutex_unlock(&mutex[2]);
	}
	else
		printf("Cname is thinking\n");
	
	pthread_exit(NULL);
}
void *Dname()
{
	if(GnChopstick[2] == UNUSED && GnChopstick[1] == UNUSED)
	{
		pthread_mutex_lock(&mutex[2]);
		pthread_mutex_lock(&mutex[1]);
	
//		GnChopstick[2] = USED;
//		GnChopstick[1] = USED;
		printf("Dname is eating......\n");
		sleep(2);
//		GnChopstick[2] = UNUSED;
//		GnChopstick[1] = UNUSED;

		pthread_mutex_unlock(&mutex[2]);
		pthread_mutex_unlock(&mutex[1]);
	}
	else
		printf("Dname is thinking\n");
	
	pthread_exit(NULL);
}
void *Ename()
{
	if(GnChopstick[1] == UNUSED && GnChopstick[0] == UNUSED)
	{
		pthread_mutex_lock(&mutex[1]);
		pthread_mutex_lock(&mutex[0]);
		
//		GnChopstick[1] = USED;
//		GnChopstick[0] = USED;
		printf("Ename is eating......\n");
		sleep(2);
//		GnChopstick[1] = UNUSED;
//		GnChopstick[0] = UNUSED;

		pthread_mutex_unlock(&mutex[1]);
		pthread_mutex_unlock(&mutex[0]);
	}
	else
		printf("Ename is thinking\n");
	
	pthread_exit(NULL);
}

