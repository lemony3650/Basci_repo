#include <stdio.h>
#include <sys/shm.h>
#include <time.h>
#include <sys/types.h>
#include <sys/sem.h>        // 信号量头文件
#include <signal.h>

/******************************************************
 * 信号量SV
 * 是一个内核变量，能够被系统中任何进程访问，通过此变量协调竞争
 * 原子操作
   P： SV-1 为0则挂起
   V： 唤醒进程  SV+1
*******************************************************/
#define TIME_MEM_KEY 99 
#define TIME_SEM_KEY 9900
#define SEG_SIZE ((size_t)100)
#define oops(m,x) {perror(m);exit(x);}

union semun
{
    int val;
    struct semid_ds *buf;
    ushort *array;
};

/********************************************************************
 * 操作一组信号量集
-----------------------------------------------------
 * int semget(key_t key, int num_sems, int sem_flags)
    创建或获取一个新的信号量集，返回正整数即信号量标识符句柄
    key值标记全局唯一标识符 | num_sems信号量的个数，获取则指定0 
    flag低9bit为权限，IPC_CREATE | IPC_EXCL
   
-------------------------------------------------------------------
 * int semop(int sem_id, struct sembuf* sem_ops, size_t num_sem_ops)
    改变信号量的值，即执行PV操作，申请|释放
    semval 信号量的值  |  sempid 最后一次执行semop的进程pid
     1. sem_id 句柄
     2. struct sembuf {
         unsigned short int sem_num;        // 信号量编号 0 1 2
         short int sem_op;                  // 操作类型 + 0 -
         short int sem_flg;                 // IPC_NOWAIT 立即返回 | SEM_UNDO 
        }
     3. num_sem_ops 指定执行操作的个数

---------------------------------------------------------
 * int semctl(int sem_id, int sem_num, int command,...)
   对信号量进行直接控制
 
-------------------------------
   第4个参数用户定义，推荐格式
   union  semun
    {
        int val;
        struct semid_ds *buf;
        ushort *array;
        struct seminfo *__buf;
    };
**********************************************************************/

int seg_id, semset_id;
void cleanup(int);
// 服务器端-写者
int main()
{
    char *mem_ptr, *ctime();
    time_t now;
    int n;

    // 1. create a shared memory segment
    seg_id = shmget(TIME_MEM_KEY, SEG_SIZE, IPC_CREAT|0777);
    if (seg_id == -1)   oops("shmget",1);

    // 2. attach to it and get a pointer to where it attachs
    mem_ptr = shmat(seg_id, NULL, 0);
    if (mem_ptr == (void *) - 1)    oops("shmat", 2);


    // 1. 创建信号量  IPC_CREAT|IPC_EXCL确保创建新的唯一的信号量集合
    const int TIME_MEM_KEY = 99
    semset_id = semget(TIME_MEM_KEY, 2, (0666|IPC_CREAT|IPC_EXCL));
    if (semset_id == -1)    oops("semget", 3);

    // 2. 所有信号量置零
    set_sem_value(semset_id, 0, 0);
    set_sem_value(semset_id, 1, 0);

    signal(SIGINT, cleanup);

    for (n = 0; n < 60; n++) {
        time(&now);
            printf("\t shm_ts2 waiting for lock\n");
        wait_and_lock(semset_id);
            printf("\t shm_ts2 updating memory\n");
        strcpy(mem_ptr, ctime(&now));
            sleep(5);
        release_lock(semset_id);
            printf("\t shm_ts2 release lock\n");
        
        sleep(1);
    }

    cleanup(0);
}

void cleanup(int n)
{
    shmctl(seg_id, IPC_RMID, NULL);

    // 删除信号量
    semctl(semset_id, 0, IPC_RMID, NULL);
}

// initialize a semaphore
void set_sem_value(int semset_id, int semnum, int val)
{
    union semun initval;
    initval.val = val;

    // semnum 信号量下标  SETVAL通过initval赋值0
    if (semctl(semset_id, semnum, SETVAL, initval) == -1)
        oops("semctl", 4);
}

// build and execute a 2-element action set
// wait for 0
// 整个获得集合被做为组来完成，读者信号量为0，写着加1
void wait_and_lock(int semset_id)
{
    struct sembuf actions[2];
    actions[0].sem_num = 0;
    actions[0].sem_flg = SEM_UNDO;
    actiona[0].sem_op = 0;

    actions[1].sem_num = 1;
    actions[1].sem_flg = SEM_UNDO;
    actions[1].sem_op = +1;

    if (semop(semset_id, actions, 2) == -1)
        oops(" semop locking", 10);
}

// 写者-1
void release_lock(int semset_id)
{
    struct sembuf actions[1];

    actions[0].sem_num = 1;
    actions[0].sem_flg = SEM_UNDO;
    actiona[0].sem_op = -1;

    if (semop(semset_id, actions, 1) == -1)
        oops(" Semop locking", 10);
}


//**********************************************************************
// 客户端操作
int main()
{
    char *mem_ptr, *ctime();
    long now;
    int seg_id, semset_id;

    // 1. create a shared memory segment
    seg_id = shmget(TIME_MEM_KEY, SEG_SIZE, IPC_CREAT|0777);
    if (seg_id == -1)   oops("shmget",1);

    // 2. attach to it and get a pointer to where it attachs
    mem_ptr = shmat(seg_id, NULL, 0);
    if (mem_ptr == (void *) - 1)    oops("shmat", 2);


    // 3. connect to semaphore
    const int TIME_MEM_KEY = 99
    semset_id = semget(TIME_MEM_KEY, 2, 0);
    wait_and_lock(semset_id);

    printf("the time, direct from memory:..%s", mem_ptr);
 
    // 4. 
    release_lock(semset_id);

    semdt(mem_ptr);
}


void wait_and_lock(int semset_id)
{
    union semun sem_info;
    struct sembuf actions[2];
    actions[0].sem_num = 1;
    actions[0].sem_flg = SEM_UNDO;
    actiona[0].sem_op = 0;

    actions[1].sem_num = 0;
    actions[1].sem_flg = SEM_UNDO;
    actions[1].sem_op = +1;

    if (semop(semset_id, actions, 2) == -1)
        oops(" semop locking", 10);
}

// 读者-1
void release_lock(int semset_id)
{
    union semun sem_info;
    struct sembuf actions[1];

    actions[0].sem_num = 0;
    actions[0].sem_flg = SEM_UNDO;
    actiona[0].sem_op = -1;

    if (semop(semset_id, actions, 1) == -1)
        oops(" Semop locking", 10);
}