#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define oops(m,x) {perror(m); exit(x);}
#define HOSTLEN 256

int make_internet_address(char *hostname, int port, struct sockaddr_in *addrp);
int make_dgram_server_socket(int portnum);

int main(int argc, char *argv[])
{
    int port;
    int sock;
    char buf[BUFSIZ];
    size_t msglen;
    struct sockaddr_in saddr;
    socklen_t saddrlen;

    if (argc == 1 || (port = atoi(argv[1])) < 0) {
        fprintf(stderr, "usage: dgrecv portnumber\n");
        exit(1);
    }

    if ((sock = make_dgram_server_socket(port)) == -1)
        oops("cannot make socket", 2);

    saddrlen = sizeof(saddr);
    while ((msglen = recvfrom(sock, buf, BUFSIZ, 0, &saddr, &saddrlen)) > 0)
    {
        buf[msglen] = '\0';
        printf("dgrecv: got a message: %s\n", buf);
        say_who_called(&saddr);
    }
    return 0;
}

int get_internet_address(char *host, int len, int *portp, struct sockaddr_in *addrp)
{
    strncpy(host, inet_ntoa(addrp->sin_addr), len);
    *portp = ntohs(addrp->sin_port);

    return 0;
}

void say_who_called(struct sockaddr_in *addrp)
{
    char host[BUFSIZ];
    int port;

    get_internet_address(host, BUFSIZ, &port, addrp);
    printf("from: %s: %d\n", host, port);
}


void reply_to_sender(int sock, char *msg, struct sockaddr_in *addrp, socklen_t len)
{
    char reply[BUFSIZ+BUFSIZ];

    sprintf(reply, "thanks for your %d char message\n", strlen(msg));
    sendto(sock, reply, strlen(reply), 0, addrp, len);
}

int make_dgram_server_socket(int portnum)
{
    struct sockaddr_in saddr;
    char hostname[HOSTLEN];
    int sock_id;

    if ((sock_id = socket(PF_INET, SOCK_DGRAM, 0)) == -1)
        return -1;

    gethostname(hostname,HOSTLEN);
    make_internet_address(hostname, portnum, &saddr);

    if (bind(sock_id, (struct sockaddr*)&saddr, sizeof(saddr)) != 0)
        return -1;

    return sock_id;
}

int make_internet_address(char *hostname, int port, struct sockaddr_in *addrp)
{
    struct hostent *hp;
    bzero((void *)addrp, sizeof(struct sockaddr_in));

    hp = gethostbyname(hostname);
    if (hp == NULL)
        return -1;
    bcopy((void *)hp->h_addr, (void *)&addrp->sin_addr, hp->h_length);
    addrp->sin_port = htons(port);
    addrp->sin_family = AF_INET;

    return 0;
}