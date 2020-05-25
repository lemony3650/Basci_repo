#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <fcntl.h>

#define MAXLINE (1 << 10)
#define INIT_SIZE 128


/*****************************************************************
 * select�ļ��������̶������д�С����
 * pollÿ�μ��󲻻��޸�ԭ�������ֵ�������������revents��
*****************************************************************/

struct pollfd {
    int fd;
    short events;
    short revents;
};

// events
#define    POLLIN    0x0001    /* any readable data available */
#define    POLLPRI   0x0002    /* oob/urgent readable data */ 
#define    POLLOUT   0x0004    /* file descriptor is writeable */

// returned events
#define    POLLIN    0x0001    /* any readable data available */
#define    POLLPRI   0x0002    /* oob/urgent readable data */ 
#define    POLLRDNORM   0x0040   /* non-oob/urgent readable data */
#define    POLLRDBAND   0x0080   /* oob/urgent readable data */ 

#define POLLOUT     0x0004     /* file descriptor is writeable */



/****************************************************************
 * poll I/O��·����
 * int poll(struct pollfd *fds, unsigned long nfds, int timeout); 
 * nfds fd�����С
 * ����ֵ�����о�����������Ϊ�����������ʱ��Ϊ0����������Ϊ-1
 * fd = -1 ��������¼�
*****************************************************************/
int main(int argc, char **argv) 
{
    int listen_fd, connected_fd;
    int ready_number;
    ssize_t n;
    char buf[MAXLINE];
    struct sockaddr_in client_addr;

    listen_fd = tcp_server_listen(SERV_PORT);

    //1. ��ʼ��pollfd���飬�������ĵ�һ��Ԫ����listen_fd��
    // �����������¼��Ҫ���ӵ�connect_fd
    struct pollfd event_set[INIT_SIZE];
    event_set[0].fd = listen_fd;
    event_set[0].events = POLLRDNORM;

    //2. ��-1��ʾ�������fdλ�û�û�б�ռ��
    int i;
    for (i = 1; i < INIT_SIZE; i++) {
        event_set[i].fd = -1;
    }

    for (;;) {
        if ((ready_number = poll(event_set, INIT_SIZE, -1)) < 0)
            error(1, errno, "poll failed ");
        
        if (event_set[0].revents & POLLRDNORM) {
            socklen_t client_len = sizeof(client_addr);
            connected_fd = accept(listen_fd, (struct sockaddr *) &client_addr, &client_len);

            //�ҵ�һ�����Լ�¼�������׽��ֵ�λ��
            for (i = 1; i < INIT_SIZE; i++) {
                if (event_set[i].fd < 0) {
                    event_set[i].fd = connected_fd;
                    event_set[i].events = POLLRDNORM;
                    break;
                }
            }

            if (i == INIT_SIZE) 
                error(1, errno, "can not hold so many clients");
            if (--ready_number <= 0)
                continue;
        }

        for (i = 1; i < INIT_SIZE; i++) {
            int socket_fd;
            if ((socket_fd = event_set[i].fd) < 0)
                continue;
            if (event_set[i].revents & (POLLRDNORM | POLLERR)) {
                if ((n = read(socket_fd, buf, MAXLINE)) > 0) {
                    if (write(socket_fd, buf, n) < 0) {
                        error(1, errno, "write error");
                    }
                } else if (n == 0 || errno == ECONNRESET) {
                    close(socket_fd);
                    event_set[i].fd = -1;
                } else {
                    error(1, errno, "read error");
                }

                if (--ready_number <= 0)
                    break;
            }
        }
    }
}