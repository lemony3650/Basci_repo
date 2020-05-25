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

void *count_words(void *a);


/************************************************************************
 * int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex)
 * #include <pthread.h>
 * cond 指向某条件变量的指针  mutex 指向互斥锁对象的指针
 * 返回成功 0
 * 此函数会自动先释放锁变量，等待条件变量变化，
 * 若执行时互斥锁未锁住，则执行结果是不确定的
**************************************************************************/
int main(int argc, char *argv[])
{
    pthread_t t1, t2;
    struct arg_set args1, args2;
    int reports_in = 0;
    int total_words = 0;

    if (argc != 3) {
        printf("usage: %s file1 file2\n", argv[0]);
        exit(1);
    }

    // 1.加锁
    pthread_mutex_lock(&lock);
    
    // 2.起线程
    args1.fname = argv[1];
    args1.count = 0;
    pthread_create(&t1, NULL, count_words, (void *)&args1);

    args2.fname = argv[2];
    args2.count = 0;
    pthread_create(&t2, NULL, count_words, (void *)&args2);

    // 3.等待
    while (reports_in < 2) {
        printf("MAIN: waiting for flag to go up\n");
        // 阻塞等待
        pthread_cond_wait(&flag, &lock);
        printf("MAIN: Wow!flag was raised,i have the lock\n");
        printf("%7d: %s\n", mailbox->count, mailbox->fname);
        total_words += mailbox->count;

        if (mailbox == &args1)
            pthread_join(t1, NULL);
        if (mailbox == &args2)
            pthread_join(t2, NULL);

        mailbox = NULL;
        pthread_cond_signal(&flag);
        reports_in++;
    }
    printf("%7d: total words\n", total_words);

}

/************************************************************************
 * int pthread_cond_signal(pthread_cond_t *cond)
 * #include <pthread.h>
 * cond 指向某条件变量的指针
 * 返回成功 0
 * 此函数通过条件变量cond发送消息
**************************************************************************/
void *count_words(void *a)
{
    struct arg_set *args = a;
    FILE* fp;
    int c, prev = '\0';

    if ((fp = fopen(args->fname, "r")) != NULL) {
        while ((c = getc(fp)) != EOF) {
            if (!isalnum(c) && isalnum(prev))
                args->count++;

            prev = c;
        }
        fclose(fp);
    }else 
        perror(args->fname);

    printf("COUNT: waiting to get lock\n");
    // 1. 加锁
    pthread_mutex_lock(&lock);
    printf("COUNT: have lock, storing data\n");

    if (mailbox != NULL)
        pthread_cond_wait(&flag, &lock);

    // 2. 存储
    mailbox = args;
    printf("COUNT: rasing flag\n");
    // 3. 通知
    pthread_cond_signal(&flag);

    // 4. 释放锁
    printf("COUNT: unlocking box\n");
    pthread_mutex_unlock(&lock);

    return NULL;
}