#include <unistd.h>  
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>


int main()
{
	int status = 0;
	puts("start ...");

	pid_t pid = fork();

	if(pid == -1){
		perror("create child_process fails");
	}
	if(pid == 0){
		puts("i am child");
//		puts("sleep 5s ...");
//		sleep(5);
//		puts("sleep end");
		printf("child  pid is %d\n",getpid());
		printf("parent pid is %d\n",getppid());
		status++;
	}else{
		puts("i am parent");
		printf("child  pid is %d\n",pid);
		printf("parent pid is %d\n",getpid());	
		status++;
	}
	//wait(&status);
	printf("%d\n",status);
	puts("main end");
	return 0;
}

/*
int main(void)  
{  
	int i = 0;  
	printf("i    son/pa   ppid    pid   fpid\n"); 

	for(i = 0;i < 2;i++){  			        
		pid_t fpid = fork();  				       
		if(fpid == 0)  					              
			printf("%d child  %8d %8d %8d\n",i,getppid(),getpid(),fpid);  					          
		else  							             
			printf("%d parent %8d %8d %8d\n",i,getppid(),getpid(),fpid);  			     
	}  		    
	return 0;  
}
*/

