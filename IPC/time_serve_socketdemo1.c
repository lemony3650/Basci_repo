#include <stdio.h>
#include <unistd.h>
#include <strings.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define PORTNUM 15000
#define HOSTLEN 128
#define oops(msg)   {perror(msg);exit(1);}

int main()
{
    struct sockaddr_in saddr;
    struct hostent     *hp;
    char    hostname[HOSTLEN];
    int     sock_id,sock_fd;
    FILE    *sock_fpi, *sock_fpo;
    FILE    *pipe_fp;
    char    dirname[BUFSIZ];
    char    dirlen,c;

    //ask kernel for a socket,return -1 error
    //sockid = socket(int domain,int type, int  protocol)   FP_INET通信域   SOCK_STREAM数据流类型
    sock_id = socket(PF_INET,SOCK_STREAM,0);
    if(sock_id == -1)   
        oops("socket");

    //bine address to socket  address is host & port        AF_INET use ip&port
    bzero((void *)&saddr,sizeof(saddr));
    gethostname(hostname,HOSTLEN);
    hp = gethostbyname(hostname);

    bcopy((void *)hp->h_addr,(void *)&saddr.sin_addr,hp->h_length);
    saddr.sin_port   = htons(PORTNUM);
    saddr.sin_family = AF_INET;

    //result = bind(int sock_id,struct sockaddr *addrp,socklen_t addrlen)       addrp指向包含地址结构的指针
    if(bind(sock_id,(struct sockaddr *)&saddr,sizeof(saddr)) != 0)
        oops("bind");

    //allow incoming calls with Qsize = 1 on socket
    if(listen(sock_id,1) != 0)    
         oops("listen");

    //main loop: accept() write() close()
    while(1){
        //accept 阻塞当前进程直到建立连接返回文件描述符
        sock_fd = accept(sock_id,NULL,NULL);
        if(sock_fd == -1)   
            oops("accept");
        
        if((sock_fpi = fdopen(sock_fd,"r") == NULL);
            oops("fdopen reading");
        if((fgets(dirname,BUFSIZ-5,sock_fpi)) == NULL)
            oops("reading dirname");
        
        sanititze(dirname);

        //open writing direction as buffered stram
        if((sock_fpo = fdopen(sock_fd,"w")) == NULL)
            oops("fdopen writing");
        sprintf(command,"ls %s",dirname);
        if((pipe_fp = popen(command,"r")) == NULL)
            oops("popen");

        while((c = getc(pipe_fp)) != EOF)
            putc(c,sock_fpo);
        
        pclose(pipe_fp);
        fclose(sock_fpo);
        fclose(sock_fpi);
    }
}

int sanitize(char *str)
{
    char *src,*dest;

    for(src = dest = str; *src; src++)
        if(*src == '/' || isalnum(*src))
            **dest ++ = *src;

    *dest = '\0';
}
