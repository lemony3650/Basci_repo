#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#define READER_MAX	3
#define WRITER_MAX	2

pthread_rwlock_t rw_lock;
void *reader_thread(void* arg);
void *writer_thread(void* arg);

int main(int argc,char* argv[])
{
	pthread_t reader,writer;
	
	int i = 0;
	pthread_rwlock_init(&rw_lock,NULL);

	for(i = 0; i < READER_MAX; i++)
		pthread_create(&reader,NULL,(void*)reader_thread,NULL);
	
	for(i = 0; i < WRITER_MAX; i++)
		pthread_create(&writer,NULL,(void*)writer_thread,NULL);
	
	sleep(10);
	
	return 0;
}

void *reader_thread(void *arg)
{
	while(1)
	{	
		if(pthread_rwlock_tryrdlock(&rw_lock))
		{
			printf("read %u can't read data\n",(unsigned int)pthread_self());
			sleep(1);
		}
		else{
			printf("read %u is reading.......\n",(unsigned int)pthread_self());
			sleep(1);
			printf("read %u is readed\n",(unsigned int)pthread_self());
			pthread_rwlock_unlock(&rw_lock);
			sleep(2);
		}
	}	
}
void *writer_thread(void *arg)
{
	while(1)
	{
		if(pthread_rwlock_trywrlock(&rw_lock))
		{
			printf("writer %u can't write data\n",(unsigned int)pthread_self());
			sleep(1);
		}
		else{
			printf("writer %u is writting.......\n",(unsigned int)pthread_self());
			sleep(2);
			printf("writer %u is writed\n",(unsigned int)pthread_self());
			pthread_rwlock_unlock(&rw_lock);
			sleep(3);
		}
	}
}
