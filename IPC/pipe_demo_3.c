#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

//https://blog.csdn.net/rl529014/article/details/51464363

#define MAX_DATA_LEN	256
#define DELAY_TIME		1
const char data[] = "Pipe test program";

int main()
{
	pid_t pid;
	int pipe_fd[2],real_read,real_write;
	char BUFF[MAX_DATA_LEN];

	memset((void *)BUFF,0,sizeof(BUFF));
	if(pipe(pipe_fd) < 0)
	{
		printf("pipe create error\n");
		exit(1);
	}

	if( (pid = fork()) == 0)
	{
		close(pipe_fd[1]);
		sleep(DELAY_TIME *3);	//wait 3 second to buf arrived
		if( (real_read = read(pipe_fd[0],BUFF,MAX_DATA_LEN)) > 0)
			printf("%d bytes read from the pipe is '%s'\n", real_read, BUFF);
		close(pipe_fd[0]);
		exit(0);
	}
	else if(pid > 0)
	{
		close(pipe_fd[0]);
		sleep(DELAY_TIME);
		if((real_write = write(pipe_fd[1],data,strlen(data))) !=  -1)
			printf("Parent wrote %d bytes : '%s'\n", real_write, data);
		close(pipe_fd[1]);  
		waitpid(pid, NULL, 0); 
		exit(0);
	}
	return 0;
}

