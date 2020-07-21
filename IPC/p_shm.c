#include <stdio.h>
#include <stdlib.h>

#include <sys/shm.h>        // �����ڴ� ͷ�ļ�
#include <time.h>

/*******************************************************************
 * �����ڴ�β������ڽ��̵Ĵ��ڣ�ӵ���Լ�������key
 * ��д���������������ͨ���ź������Ʊ���
----------------------------------------------------------------
 * int seg_id = shmget(key_t key, size_t size, int shmflg)
   ����|��ȡһ���µĹ����ڴ�
   key��ʶȫ��Ψһ�����ڴ棬sizeָ���ڴ��С���ֽڣ�����ȡ�Ļ�ָ��0
   flag�� SHM_HUGETLB | SHM_NORESERVE

-------------------------------------------------------------
 * shmat | shmdt
   �ٴ����깲���ڴ�����������ʣ���Ҫ��������ַ�ռ�
   void* shmat(int shm_id, const void* shm_addr, int shmflg)
   int shmdt(const void* shm_addr)

-------------------------------------------------
 * ����빲���ڴ���ж�д����,ͨ��ָ�����
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

    // ���������ڴ��
    int seg_id = shmget(TIME_MEM_KEY, SEG_SIZE, IPC_CREAT|0777);
    if (seg_id == -1)   oops("shmget",1);

    // ָ�����ڴ�ε�ָ��mem_ptr
    // һ��addrΪNULL����OSѡ�񱻹����ĵ�ַ����֤��ֲ��
    char* mem_ptr = shmat(seg_id, NULL, 0);
    if (mem_ptr == (void *) - 1)    oops("shmat", 2);

    long now;
    for (int n = 0; n < 60; n++) {
        time(&now);
        strcpy(mem_ptr, ctime(&now));
        sleep(1);
    }

    // �Ƴ������ڴ� IPC_RMID
    shmctl(seg_id, IPC_RMID, NULL);
}


// client
int main()
{
    char *ctime();

    // Ѱ��|���������ڴ��
    int seg_id = shmget(TIME_MEM_KEY, SEG_SIZE, 0777);
    if (seg_id == -1)   oops("shmget",1);

    // ָ�����ڴ�ε�ָ��mem_ptr
    char* mem_ptr = shmat(seg_id, NULL, 0);
    if (mem_ptr == (void *) - 1)    oops("shmat", 2);

    printf("the time, direct from memory:... %s ", mem_ptr);

    // ɾ�������ڴ棬���Բ���
    shmdt(mem_ptr);
}