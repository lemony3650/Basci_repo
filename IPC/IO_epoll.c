#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <fcntl.h>


/*******************************************************************************
 * epoll I/O复用
 * 将用户文件描述符集合放入一个事件表中，通过一个文件描述符指定
 * 同时通过使用一组函数来完成任务
 * 直接返回一组待处理的就绪事件，而不用去遍历查找
 -------------------------------------------------------------------------------
 * 1. 创建文件描述符事件表，size大于0便可，即创建一个epoll实例
      // 获得epoll句柄 
      int epoll_create(int size);
 * 2. 通过传入描述符表，来定义epoll事件的操作,增加|删除监控事件 
        int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event)
            epfd: epoll句柄      
            op: EPOLL_CTL_ADD | EPOLL_CTL_MOD | EPOLL_CTL_DEL
            fd：注册文件描述符
            event: 事件类型，并设置用户需要的数据
            * EPOLLIN：表示对应的文件描述字可以读；
            * EPOLLOUT：表示对应的文件描述字可以写；
            * EPOLLRDHUP：表示套接字的一端已经关闭，或者半关闭；
            * EPOLLHUP：表示对应的文件描述字被挂起；
            * EPOLLET：设置为 edge-triggered，默认为 level-triggered
 * 3.返回给用户空间需要处理的I/O事件的个数及具体事件
     数组的每个元素都是一个需要处理的事件，改善了poll机制需要索引查找就绪的事件
     epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout)
********************************************************************************/
struct epoll_event {
    uint32_t events;            // 注册事件
    epoll_data_t data;          // 存储用户数据
};

typedef union epoll_data 
{
    void     *ptr;
    int      fd;
    uint32_t u32;
    uint64_t u64;
}epoll_data_t;



// 案例一
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

    // 1. 创建epoll实例
    int efd = epoll_create1(0);
    if (efd == -1) 
        error(1, errno, "epoll create failed");
    
    // 2. 初始化 fd && event , 并注册到epoll中
    struct epoll_event event;

    event.events = EPOLLIN | EPOLLET;
    event.data.fd = listen_fd;

    if (epoll_ctl(efd, EPOLL_CTL_ADD, listen_fd, &event) == -1) 
        error(1, errno, "epoll_ctl add listen fd failed");
    
    // 3. 返回事件
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
                    // 添加socket连接
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