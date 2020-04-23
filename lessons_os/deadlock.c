#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>
#include <pthread.h>


#define LOOP_TIMES 10000
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
void *thread_worker(void *);
void critical_section(int thread_num,int i);


int main(void)
{
	int rtn,i;
	pthread_t pthread_id = 0;
	rtn = pthread_create(&pthread_id,NULL,(void *)thread_worker,NULL);
	if(rtn != 0){
		printf("pthread_createERROR!\n");
		return -1;
	}
	for(i = 0;i < LOOP_TIMES;i++){
		pthread_mutex_trylock(&mutex1);
		pthread_mutex_trylock(&mutex2);
		critical_section(1,i);
		pthread_mutex_unlock(&mutex2);
		pthread_mutex_unlock(&mutex1);
	}
	pthread_mutex_destroy(&mutex1);
	pthread_mutex_destroy(&mutex2);
	return 0;
}

void *thread_worker(void *p)
{
	int i;
	for(i = 0;i < LOOP_TIMES; i++){
		pthread_mutex_trylock(&mutex2);
		pthread_mutex_trylock(&mutex1);
		critical_section(2,i);
		pthread_mutex_unlock(&mutex2);
		pthread_mutex_unlock(&mutex1);
	}
}

void critical_section(int thread_num,int i)
{
	printf("Thread %d:%d\n",thread_num,i);
}
