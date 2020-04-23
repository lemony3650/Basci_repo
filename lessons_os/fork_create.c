#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <ctype.h>
#include <stdlib.h>

#define MAX_CHILD_NUMBER 10
#define SLEEP_INTERVAL   2

int proc_number = 0;	//存放进程序号
void do_something();

int main(int argc,char *argv[])
{
	int child_proc_number = MAX_CHILD_NUMBER;
	int i,ch;
	pid_t child_pid;	//承接生成的进程返回的pid
	pid_t pid[10] = {0};	//存放各进程id号码

	if(argc > 1){
		child_proc_number = atoi(argv[1]);
		child_proc_number = (child_proc_number > 10)?10:child_proc_number;
	}

	for(i = 0;i < child_proc_number;i++){
		child_pid = fork();

		if(child_pid == 0){		//子进�?
			proc_number = i;	//第i�?子进程序�?
			do_something();
		}else{
			pid[i] = child_pid;		//�?
			printf("i am parent id %d and my son is %d\n",getpid(),child_pid);
		}
	}

	while((ch=getchar())!='q'){
		if( isdigit(ch) ){
			kill(pid[ch-'0'],SIGTERM);	
		}
	}
	kill(0,SIGKILL);		
	return 0;;
}

void do_something()
{
	for(;;){
		printf("This is process No.%d and its pid is %d\n",proc_number,getpid());
		sleep(SLEEP_INTERVAL);/*主动阻�?�两秒钟*/
	}
}
