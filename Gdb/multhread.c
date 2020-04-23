#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// 声明一个都用的量
static int _old;

// 线程跑的函数
static void* _run(void* arg) {
    int piyo = 10;    
    int n = *(int*)arg;
    int i;
    
    //设置线程分离
    pthread_detach(pthread_self());
    
    for(i=0; i<n; ++i) {
        printf("n=%d, i=%d\n", n, i);
        ++_old;
        printf("n=%d, piyo = %d, _old=%d\n", n, piyo, _old);
    }

    return NULL;
}

#define _INT_PTX (3)

int main(void) {
    int i, rt, j;
    pthread_t tx[_INT_PTX];

    puts("main beign");    

    for(i=0; i<_INT_PTX; ++i) {
        // &i 是有问题的, 但是这里为了测试, 可以乱搞
        rt = pthread_create(tx+i, NULL, _run, &i);
        if(rt < 0) {
            printf("pthread_create create error! rt = %d, i=%d\n", rt, i);
            break;
        }
    }

    //CPU忙等待
    for(j=0; j<1000000000; ++j)
        ;        
    puts("end");    

    return 0;
}