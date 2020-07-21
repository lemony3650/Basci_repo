#include <stdio.h>
#include <sys/file.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>


#define oops(m,x) {perror(m); exit(x);}

// int res = fcntl(int fd, int cmd);
// int res = fcntl(int fd, int cmd, long arg);
// int res = fcntl(int fd, int cmd, struct flock* lockp);

void test01()
{
    int flag = fcntl(fd, F_GETFL);
    flag |= 0_SYNC;
    int res = fcntl(fd,F_SETFL,flag);
}

/***************************************************************
 * 文件读写锁 
 * flock lockf fcntl
-----------------------------------------------------------------
 * fcntl(fd, F_SETLKW, &lockinfo)   
    加读数据文件锁
    -------------------------------------
    struct flock lockinfo;
        lockinfo.l_whence = SEEK_SET;
        lockinfo.l_start = 0;
        lockinfo.l_pid = getpid();
        lockinfo.l_type = F_RDLCK; 
        设置类型为读锁 写锁  解锁 F_RDLCK | F_WRLCK | F_UNLCK

-------------------------------------
    fcntl(fd, F_SETLKW, &lock)
****************************************************************/
int main(int argc, char* argv[])
{
    int fd;
    time_t now;
    char *message;

    if (argc != 2){
        fprintf(stderr, "usage: file_ts filename\n");
        exit(1);
    }

    if ((fd = open(argv[1], O_CREAT|O_TRUNC|O_WRONLY, 0644)) == -1)
        oops(argv[1],2);

    while(1){
        time(&now);
        message = ctime(&now);

        lock_operation(fd, F_WRLCK);        // 设置写锁

        if (lseek(fd, 0L, SEEK_SET) == -1)
            oops("lseek", 3);
        if (write(fd, message, strlen(message)) == -1)
            oops("write", 4);

        lock_operation(fd, F_UNLCK);        // 解锁
        sleep(1);
    }
}

void lock_operation(int fd, int op)
{
    struct flock lock;

    lock.l_whence = SEEK_SET;
    lock.l_start = lock.l_len = 0;
    lock.l_pid = getpid();
    lock.l_type = op;       

    if (fcntl(fd, F_SETLKW, &lock) == -1)
        oops("lock operation", 6);
}


// 读客户端
int main(int argc, char* argv[])
{
    int fd, nread;
    char buf[BUFSIZ];

    if (argc != 2){
        fprintf(stderr, "usage: file_ts filename\n");
        exit(1);
    }

    if ((fd = open(argv[1], O_RDONLY) == -1))
        oops(argv[1],2);

    lock_operation(fd, F_RDLCK); 

    while((nread = read(fd, buf, BUFLEN)) > 0) 

    lock_operation(fd, F_UNLCK);        // 解锁
    close(fd);
}

void lock_operation(int fd, int op)
{
    struct flock lock;

    lock.l_whence = SEEK_SET;
    lock.l_start = lock.l_len = 0;
    lock.l_pid = getpid();
    lock.l_type = op;       

    if (fcntl(fd, F_SETLKW, &lock) == -1)
        oops("lock operation", 6);
}