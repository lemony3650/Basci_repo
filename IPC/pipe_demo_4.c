#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define oops(m,x)   {perror(m);exit(x);}

//pipe & fork()
int main(int argc, char *argv[])
{
    int pipe_fd[2];
    int new_fd;
    pid_t pid;

    if(argc != 3){
        fprintf(stderr,"usage: pipe cmd1 cmd2 \n");
        exit(1);
    }
    if( pipe(pipe_fd) == -1 )
        oops("can not get a pipe",1);
  
    if( (pid = fork()) == -1)
        oops("can not fork",2);
    if( pid > 0)
	{
		close(pipe_fd[1]);
		if( dup2(pipe_fd[0],0) == -1)
             oops("can not redirect stdin",3);

		close(pipe_fd[0]);
        execlp(argv[2],argv[2],NULL);
		oops("argv[2]",4);
	}
	//child process 
    close(pipe_fd[0]);
    if( dup2(pipe_fd[1],1) == -1)
            oops("can not redirect stdout",4);

    close(pipe_fd[1]);
    execlp(argv[1],argv[1],NULL);
    oops("argv[1]",5);

	return 0;
}