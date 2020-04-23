#include <stdio.h>
#include <pthread.h>
#include <ctype.h>

struct arg_set {
    char *fname;
    int count;
};

struct arg_set *mailbox;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  flag = PTHREAD_COND_INITIALIZER;

int main()
{


    
}