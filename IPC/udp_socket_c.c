#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>

#define oops(m,x) {perror(m); exit(x);}

int make_dgram_client_socket();
int make_internet_address(char *hostname, int port, struct sockaddr_in *addrp);


int main(int argc, char *argv[])
{
    int sock;
    char *msg;
    struct sockaddr_in saddr;

    if (argc != 4) {
        fprintf(stderr, "usage: dgsend host port 'message'\n");
        exit(1);
    }

    msg = argv[3];
    if ((sock = make_dgram_client_socket()) == -1)
        oops("cannot make socket", 2);

    make_internet_address(argv[1], atoi(argv[2]), &saddr);

    if (sendto(sock, msg, strlen(msg), 0, &saddr, sizeof(saddr)) == -1)
        oops("sendto failed", 3);
    return 0;
}


int make_dgram_client_socket()
{
    return socket(PF_INET, SOCK_DGRAM, 0);
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