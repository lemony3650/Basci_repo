#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <fcntl.h>


/*******************************************************************************
 * epoll I/O����
 * ���û��ļ����������Ϸ���һ���¼����У�ͨ��һ���ļ�������ָ��
 * ͬʱͨ��ʹ��һ�麯�����������
 * ֱ�ӷ���һ�������ľ����¼���������ȥ��������
 -------------------------------------------------------------------------------
 * 1. �����ļ��������¼���size����0��ɣ�������һ��epollʵ��
      // ���epoll��� 
      int epoll_create(int size);
 * 2. ͨ��������������������epoll�¼��Ĳ���,����|ɾ������¼� 
        int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event)
            epfd: epoll���      
            op: EPOLL_CTL_ADD | EPOLL_CTL_MOD | EPOLL_CTL_DEL
            fd��ע���ļ�������
            event: �¼����ͣ��������û���Ҫ������
            * EPOLLIN����ʾ��Ӧ���ļ������ֿ��Զ���
            * EPOLLOUT����ʾ��Ӧ���ļ������ֿ���д��
            * EPOLLRDHUP����ʾ�׽��ֵ�һ���Ѿ��رգ����߰�رգ�
            * EPOLLHUP����ʾ��Ӧ���ļ������ֱ�����
            * EPOLLET������Ϊ edge-triggered��Ĭ��Ϊ level-triggered
 * 3.���ظ��û��ռ���Ҫ�����I/O�¼��ĸ����������¼�
     �����ÿ��Ԫ�ض���һ����Ҫ������¼���������poll������Ҫ�������Ҿ������¼�
     epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout)
********************************************************************************/
struct epoll_event {
    uint32_t events;            // ע���¼�
    epoll_data_t data;          // �洢�û�����
};

typedef union epoll_data 
{
    void     *ptr;
    int      fd;
    uint32_t u32;
    uint64_t u64;
}epoll_data_t;



// ����һ
#define MAXEVENTS 128

char rot13_char(char c) 
{
    if ((c >= 'a' && c <= 'm') || (c >= 'A' && c <= 'M'))
        return c + 13;
    else if ((c >= 'n' && c <= 'z') || (c >= 'N' && c <= 'Z'))
        return c - 13;
    else
        return c;
}

int main(int argc, char **argv) {
    int socket_fd;
    int listen_fd = tcp_nonblocking_server_listen(SERV_PORT);

    // 1. ����epollʵ��
    int efd = epoll_create1(0);
    if (efd == -1) 
        error(1, errno, "epoll create failed");
    
    // 2. ��ʼ�� fd && event , ��ע�ᵽepoll��
    struct epoll_event event;

    event.events = EPOLLIN | EPOLLET;
    event.data.fd = listen_fd;

    if (epoll_ctl(efd, EPOLL_CTL_ADD, listen_fd, &event) == -1) 
        error(1, errno, "epoll_ctl add listen fd failed");
    
    // 3. �����¼�
    /* Buffer where events are returned */
    struct epoll_event *events;
    events = calloc(MAXEVENTS, sizeof(event));

    while (1) {
        int i;
        int n = epoll_wait(efd, events, MAXEVENTS, -1);
        printf("epoll_wait wakeup\n");
        for (i = 0; i < n; i++) {
            if ((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP) ||
                                                (!(events[i].events & EPOLLIN))) {
                fprintf(stderr, "epoll error\n");
                close(events[i].data.fd);
                continue;
            } 
            else if (listen_fd == events[i].data.fd) {
                struct sockaddr_storage ss;
                socklen_t slen = sizeof(ss);
                int fd = accept(listen_fd, (struct sockaddr *) &ss, &slen);

                if (fd < 0) {
                    error(1, errno, "accept failed");
                } else {
                    make_nonblocking(fd);
                    event.data.fd = fd;
                    event.events = EPOLLIN | EPOLLET; //edge-triggered
                    // ���socket����
                    if (epoll_ctl(efd, EPOLL_CTL_ADD, fd, &event) == -1) {
                        error(1, errno, "epoll_ctl add connection fd failed");
                    }
                }
                continue;
            } 
            else {
                socket_fd = events[i].data.fd;
                printf("get event on socket fd == %d \n", socket_fd);
                while (1) {
                    char buf[512];
                    if ((n = read(socket_fd, buf, sizeof(buf))) < 0) {
                        if (errno != EAGAIN) {
                            error(1, errno, "read error");
                            close(socket_fd);
                        }
                        break;
                    } else if (n == 0) {
                        close(socket_fd);
                        break;
                    } else {
                        for (i = 0; i < n; ++i) {
                            buf[i] = rot13_char(buf[i]);
                        }
                        if (write(socket_fd, buf, n) < 0) {
                            error(1, errno, "write error");
                        }
                    }
                }
            }
        }
    }

    free(events);
    close(listen_fd);
}