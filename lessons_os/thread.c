#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_THREAD 3
unsigned long long main_counter,counter[MAX_THREAD];
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
void *thread_worker(void *p);

int main(int argc,char *argv[])
{
	int i,rtn,ch;
	int *para;
	pthread_t pthread_id[MAX_THREAD] = {0};
	for(i = 0;i < MAX_THREAD;i++){
		para = (int *)malloc(sizeof(int));
		*para = i;
		rtn = pthread_create(&pthread_id[i],NULL,(void *)thread_worker,para);//unique_addr value dymastic_addr
	}
	do{
		pthread_mutex_lock(&mutex);
	
		unsigned long long sum = 0;
		for(i = 0;i < MAX_THREAD;i++){
			sum += counter[i];
			printf("%llu\n",counter[i]);
		}
		printf("%llu   %llu\n",main_counter,sum);	
		pthread_mutex_unlock(&mutex);
	}while( (ch=getchar()) !='q');
	pthread_mutex_destroy(&mutex);
	return 0;
}

void *thread_worker(void *p)
{
	int thread_num = *(int *)p;
	free((int *)p);
	printf("thread_id:%lu   counter[%d]\n",pthread_self(),thread_num);

	for(;;){
		pthread_mutex_lock(&mutex);
		counter[thread_num]++;
		main_counter++;
		pthread_mutex_unlock(&mutex);
	}
}

