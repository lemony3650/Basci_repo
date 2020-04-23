#include <stdio.h>
#include <unistd.h>
#include <strings.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define BACKLOG 1
#define HOSTLEN 128

int make_server_socket_q(int portnum,int backlog);

int make_server_socket(int portnum)
{
    return make_server_socket_q(portnum,int backlog)
}

int make_server_socket_q(int portnum,int backlog)
{
    struct sockaddr_in saddr;
    struct hostent     *hp;
    char    hostname[HOSTLEN];
    int     sock_id;

    sock_id = socket(PF_INET,SPCL_STREAM,0);
    if(sock_id == -1)
        return -1;

    bzero((void *)&saddr,sizeof(saddr));
    gethostname(hostname,HOSTLEN);
    hp = gethostbyname(hostname);

    bcopy((void *)hp->h_addr,(void *)&saddr.sin_addr,hp->h_length);
    saddr.sin_port   = htons(portnum);
    saddr.sin_family = AF_INET;

    if(bind(sock_id,(struct sockaddr *)&saddr,sizeof(saddr)) != 0)
        return -1;

    if(listen(sock_id,1) != 0)    
        return -1;
    return sock_id;
}

int connect_to_server(char *host,int portnum)
{
    int sock_id;
    struct sockaddr_in servadd;
    struct hostent *hp;

    sock_id = socket(PF_INET,SOCK_STREAM,0);
    if(sock_id == -1)   
        return -1;

    bzero(&servadd,sizeof(servadd));
    hp = gethostbyname(argv[1]);
    if(hp == NULL)
        return -1;

    bcopy(hp->h_addr,(struct sockaddr *)&servadd.sin_addr,hp->h_length);
    servadd.sin_port = htons(portnum);
    servadd.sin_family = AF_INET;

    if(connect(sock_id,(struct sockaddr *)&servadd,sizeof(servadd)) != 0)
        return -1;

    return sock_id;
}

//client
int main()
{
    int sock_fd;
    sock_fd = connect_to_server(host,port);
    if(sock_fd == -1)
        exit(1);

    //chat with server
    talk_with_server(sock_fd);  
    close(sock_fd);
}

//server
int main()
{
    int sock_id,sock_fd;
    sock_id = make_server_socket(port);
    if(sock_id == -1)
        exit(1);

    while(1){
        sock_fd = accept(sock_id,NULL,NULL);
        if(sock_fd == -1)
            break;

        process_request(sock_fd);
        close(sock_fd);
    } 
}
