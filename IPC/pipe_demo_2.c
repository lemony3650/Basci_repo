#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define CHILD_MESS  "i want a cookie \n"
#define PAR_MESS    "testing... \n"
#define oops(m,x)   {perror(m);exit(x);}

//pipe & fork()

int main()
{
    int pipe_fd[2];
    int len,read_len;
    char buf[BUFSIZ];

    if( pipe(pipe_fd) == -1 )
        oops("can not get a pipe",1);
  
    switch(fork()){
        case -1:
            oops("can't fork",2);
            
        case 0:
            len = strlen(CHILD_MESS);
            while(1){
                if(write(pipe_fd[1],CHILD_MESS,len) != len)        
                    oops("write",3);

                sleep(5);
            }
        default:
            len = strlen(PAR_MESS);
            while(1){
                if(write(pipe_fd[1],PAR_MESS,len) != len)        
                    oops("write",4);

                sleep(1);
                read_len = read(pipe_fd[0],buf,BUFSIZ);
                if(read_len <= 0 );
                    break;

                // write(1,buf,read_len); 
                printf("buf : %s\n",buf);         
            }
    } 
}