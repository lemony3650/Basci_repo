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
 * select I/O��·���� 
   #include <sys/select.h>
   select(int maxfd, fd_set *read_set, fd_set *write_set, 
                fd_st *error_set, struct timeval *timeout)
 * ����ļ������� | �ļ��������� �ɶ�|��д|�쳣 �ļ��������ļ��ϣ������г�ʱ�ļ��
   �ļ���������С������1024
 * selectһֱ������ֱ�����пɶ�д�¼�����ʱ�����ں˸�֪�����ý��������ȴ�
------------------------------------------------------------------------------
 * 1.����fd_set��������ʼ�������ļ�������ע������������ж�Ӧλ��
 *      void FD_ZERO(fd_set *fdset);������������
 *      void FD_SET(int fd, fd_set *fdset);����
 *      void FD_CLR(int fd, fd_set *fdset);��
 * 2.����select����ע�ᵽ�ں��н��м�⣬�����ں�֪ͨ��������Ҫ�޸�����������
 *   ���ÿ��Ҫ��д��ע����ļ�������д�룬���ٴε���select
 *  int = select(int maxfd, fd_set *read_set, fd_set *write_set, 
                  fd_st *error_set, struct timeval *timeout)
        timeout == NULL | value | 0    �ȴ�|�̶�ʱ��|���ȴ�
* 3.�жϱ�־λ���д���ʱ����λ
        int FD_ISSET(int fd, fd_set *fdset);
**************************************************************************/

// ����һ
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
        // 1.��������������fd_set�����ڼ���fd
        FD_ZERO(&readfds);
        FD_SET(fd1, &readfds);
        FD_SET(fd2, &readfds);
        // ��ʱ����
        struct timeval timeout;
        timeout.tv_sec = atoi(argv[3]);
        timeout.tv_usec = 0;
        
        //2. select����I/O�¼�
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


// ������
#define MAXLINE (1 << 10)

int main(int argc, char *argv[]) 
{
    if (argc != 2) {
        error(1, 0, "usage: select01 <IPaddress>");
    }

    int socket_fd; 
    socket_fd = tcp_client(argv[1], SERV_PORT);     // �Ѿ������˷����socket

    char recv_line[MAXLINE], send_line[MAXLINE];
    int n;

    // ����fd_set������ʵ��Ϊ��������long int array[32]
    fd_set readmask��allreads;
    FD_ZERO(&allreads);

    // ע���ļ�������
    FD_SET(0, &allreads);
    FD_SET(socket_fd, &allreads);

    for (;;) {
        // ÿ�ζ�Ҫ���³�ʼ��fd_set��������Ϊ�ں�֪ͨ�����¼���ͨ���޸�fd_set��ʵ�ֵ�
        // ��Ӧ�ó���ÿ�θ���FD_ISSET�����ж�
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

            // �����stdout
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