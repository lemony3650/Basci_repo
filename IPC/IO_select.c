#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>

#define oops(m,x) {perror(m); exit(x);}
void showdata(char *fname, int fd);

/****************************************************************************
 * select I/O多路复用 
   #include <sys/select.h>
   select(int maxfd, fd_set *read_set, fd_set *write_set, 
                fd_st *error_set, struct timeval *timeout)
 * 最大文件描述符 | 文件描述符的 可读|可写|异常 文件描述符的集合，并进行超时的检测
   文件描述符大小有限制1024
 * select一直阻塞，直到当有可读写事件发生时，由内核告知而不用进行阻塞等待
------------------------------------------------------------------------------
 * 1.定义fd_set变量并初始化，将文件描述符注册进整型数组中对应位置
 *      void FD_ZERO(fd_set *fdset);　　　　　　
 *      void FD_SET(int fd, fd_set *fdset);　　
 *      void FD_CLR(int fd, fd_set *fdset);　
 * 2.调用select函数注册到内核中进行监测，由于内核通知机制是需要修改描述符集合
 *   因此每次要重写将注册的文件描述符写入，即再次调用select
 *  int = select(int maxfd, fd_set *read_set, fd_set *write_set, 
                  fd_st *error_set, struct timeval *timeout)
        timeout == NULL | value | 0    等待|固定时间|不等待
* 3.判断标志位，有触发时会置位
        int FD_ISSET(int fd, fd_set *fdset);
**************************************************************************/

// 案例一
int main(int argc, char *argv[])
{
    int fd1, fd2;
    fd_set readfds;
    int retval;

    if (argc != 4) {
        fprintf(stderr,"usage: %s file file timeout", *argv);
        exit(1);
    }

    if ((fd1 = open(argv[1],O_RDONLY)) == -1)   
        oops(argv[1], 2);
    if ((fd1 = open(argv[2],O_RDONLY)) == -1)   
        oops(argv[2], 3);

    int maxfd = 1 + (fd1 > fd2? fd1:fd2);

    while(1)
    {
        // 1.设置描述符集合fd_set，用于监听fd
        FD_ZERO(&readfds);
        FD_SET(fd1, &readfds);
        FD_SET(fd2, &readfds);
        // 超时参数
        struct timeval timeout;
        timeout.tv_sec = atoi(argv[3]);
        timeout.tv_usec = 0;
        
        //2. select阻塞I/O事件
        retval = select(maxfd, &readfds, NULL, NULL, &timeout);
        if (retval == -1)
            oops("select", 4);

        if (retval > 0) {
            if (FD_ISSET(fd1, &readfds))
                showdata(argv[1], fd1);
            if (FD_ISSET(fd2, &readfds))
                showdata(argv[2], fd2);           
        }
        else 
            printf("no input after %d seconds\n", atoi(argv[3]));
    }
}

void showdata(char *fname, int fd)
{
    char buf[BUFSIZ];
    int n;

    printf("%s: ",fname,n);
    fflush(stdout);
    n = read(fd, buf, BUFSIZ);

    if (n == -1)
        oops(fname,5);

    write(1,buf,n);
    write(1,"\n",1);
}


// 案例二
#define MAXLINE (1 << 10)

int main(int argc, char *argv[]) 
{
    if (argc != 2) {
        error(1, 0, "usage: select01 <IPaddress>");
    }

    int socket_fd; 
    socket_fd = tcp_client(argv[1], SERV_PORT);     // 已经建立了服务端socket

    char recv_line[MAXLINE], send_line[MAXLINE];
    int n;

    // 创建fd_set变量，实则为整型数组long int array[32]
    fd_set readmask，allreads;
    FD_ZERO(&allreads);

    // 注册文件描述符
    FD_SET(0, &allreads);
    FD_SET(socket_fd, &allreads);

    for (;;) {
        // 每次都要重新初始化fd_set变量，因为内核通知就绪事件是通过修改fd_set来实现的
        // 而应用程序每次根据FD_ISSET进行判断
        readmask = allreads;
        int rc = select(socket_fd + 1, &readmask, NULL, NULL, NULL);
        if (rc <= 0) {
            error(1, errno, "select failed");
        }

        if (FD_ISSET(socket_fd, &readmask)) {
            n = read(socket_fd, recv_line, MAXLINE);
            if (n < 0) {
                error(1, errno, "read error");
            } else if (n == 0) {
                error(1, 0, "server terminated \n");
            }
            recv_line[n] = 0;

            // 输出到stdout
            fputs(recv_line, stdout);
            fputs("\n", stdout);
        }

        if (FD_ISSET(STDIN_FILENO, &readmask)) {
            if (fgets(send_line, MAXLINE, stdin) != NULL) {
                int i = strlen(send_line);
                if (send_line[i - 1] == '\n') {
                    send_line[i - 1] = 0;
                }

                printf("now sending %s\n", send_line);
                size_t rt = write(socket_fd, send_line, strlen(send_line));
                
                if (rt < 0) {
                    error(1, errno, "write failed ");
                }

                printf("send bytes: %zu \n", rt);
            }
        }
    }
}