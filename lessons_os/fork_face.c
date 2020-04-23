#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

//由于0号创建了0 1并输出两个-，而同时0 1运行第二次再输出两个-
//当i为1时，创建2 3进程以复制出两个-，再额外输出两个- 故为8个

int main(void)
{
	int i;
	for (i = 0; i < 2; i++){
		fork();
		printf("-");		//块设备存有缓存，字符设备没有，printf即将“-”放入缓存，暂不输出
		//printf("-\n");	//\n eof 或者缓冲区满，文件关闭 就将数据刷出缓存
	}
	wait(NULL);
	wait(NULL);
	return 0;
}
