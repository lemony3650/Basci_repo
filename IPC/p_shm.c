#include <stdio.h>
#include <stdlib.h>

#include <sys/shm.h>        // 共享内存 头文件
#include <time.h>

/*******************************************************************
 * 共享内存段不依赖于进程的存在，拥有自己的名字key
 * 读写会产生竞争，可以通过信号量机制避免
----------------------------------------------------------------
 * int seg_id = shmget(key_t key, size_t size, int shmflg)
   创建|获取一段新的共享内存
   key标识全局唯一共享内存，size指定内存大小（字节），获取的话指定0
   flag： SHM_HUGETLB | SHM_NORESERVE

-------------------------------------------------------------
 * shmat | shmdt
   再创建完共享内存后不能立即访问，需要关联到地址空间
   void* shmat(int shm_id, const void* shm_addr, int shmflg)
   int shmdt(const void* shm_addr)

-------------------------------------------------
 * 如何与共享内存进行读写交互,通用指针操作
   strcpy(ptr,"hello")
   memcpy()

--------------------------------------------------------------
 * shmctl
   int shmctl(int shm_id, int command, struct shmid_ds* buf)
   IPC_STAT | IPC_SET | IPC_RMID | IPC_INFO
******************************************************************/

#define TIME_MEM_KEY 99
#define SEG_SIZE ((size_t)100)
#define oops(m,x) {perror(m); exit(x);}

// service
int main()
{
    char *ctime();

    // 创建共享内存段
    int seg_id = shmget(TIME_MEM_KEY, SEG_SIZE, IPC_CREAT|0777);
    if (seg_id == -1)   oops("shmget",1);

    // 指向共享内存段的指针mem_ptr
    // 一般addr为NULL，让OS选择被关联的地址，保证移植性
    char* mem_ptr = shmat(seg_id, NULL, 0);
    if (mem_ptr == (void *) - 1)    oops("shmat", 2);

    long now;
    for (int n = 0; n < 60; n++) {
        time(&now);
        strcpy(mem_ptr, ctime(&now));
        sleep(1);
    }

    // 移除共享内存 IPC_RMID
    shmctl(seg_id, IPC_RMID, NULL);
}


// client
int main()
{
    char *ctime();

    // 寻找|创建共享内存段
    int seg_id = shmget(TIME_MEM_KEY, SEG_SIZE, 0777);
    if (seg_id == -1)   oops("shmget",1);

    // 指向共享内存段的指针mem_ptr
    char* mem_ptr = shmat(seg_id, NULL, 0);
    if (mem_ptr == (void *) - 1)    oops("shmat", 2);

    printf("the time, direct from memory:... %s ", mem_ptr);

    // 删除共享内存，可以不用
    shmdt(mem_ptr);
}