#include <stdio.h>
#include <sys/shm.h>
#include <time.h>
#include <sys/types.h>
#include <sys/sem.h>        // �ź���ͷ�ļ�
#include <signal.h>

/******************************************************
 * �ź���SV
 * ��һ���ں˱������ܹ���ϵͳ���κν��̷��ʣ�ͨ���˱���Э������
 * ԭ�Ӳ���
   P�� SV-1 Ϊ0�����
   V�� ���ѽ���  SV+1
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
 * ����һ���ź�����
-----------------------------------------------------
 * int semget(key_t key, int num_sems, int sem_flags)
    �������ȡһ���µ��ź��������������������ź�����ʶ�����
    keyֵ���ȫ��Ψһ��ʶ�� | num_sems�ź����ĸ�������ȡ��ָ��0 
    flag��9bitΪȨ�ޣ�IPC_CREATE | IPC_EXCL
   
-------------------------------------------------------------------
 * int semop(int sem_id, struct sembuf* sem_ops, size_t num_sem_ops)
    �ı��ź�����ֵ����ִ��PV����������|�ͷ�
    semval �ź�����ֵ  |  sempid ���һ��ִ��semop�Ľ���pid
     1. sem_id ���
     2. struct sembuf {
         unsigned short int sem_num;        // �ź������ 0 1 2
         short int sem_op;                  // �������� + 0 -
         short int sem_flg;                 // IPC_NOWAIT �������� | SEM_UNDO 
        }
     3. num_sem_ops ָ��ִ�в����ĸ���

---------------------------------------------------------
 * int semctl(int sem_id, int sem_num, int command,...)
   ���ź�������ֱ�ӿ���
 
-------------------------------
   ��4�������û����壬�Ƽ���ʽ
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
// ��������-д��
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


    // 1. �����ź���  IPC_CREAT|IPC_EXCLȷ�������µ�Ψһ���ź�������
    const int TIME_MEM_KEY = 99
    semset_id = semget(TIME_MEM_KEY, 2, (0666|IPC_CREAT|IPC_EXCL));
    if (semset_id == -1)    oops("semget", 3);

    // 2. �����ź�������
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

    // ɾ���ź���
    semctl(semset_id, 0, IPC_RMID, NULL);
}

// initialize a semaphore
void set_sem_value(int semset_id, int semnum, int val)
{
    union semun initval;
    initval.val = val;

    // semnum �ź����±�  SETVALͨ��initval��ֵ0
    if (semctl(semset_id, semnum, SETVAL, initval) == -1)
        oops("semctl", 4);
}

// build and execute a 2-element action set
// wait for 0
// ������ü��ϱ���Ϊ������ɣ������ź���Ϊ0��д�ż�1
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

// д��-1
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
// �ͻ��˲���
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

// ����-1
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