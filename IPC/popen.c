#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//popen 打开一个指向进程的带缓冲的连接
int main()
{
    FILE *fp;
    char buf[100];
    int  i = 0;

    fp = popen("who|sort","r");
    while(fgets(buf,100,fp) != NULL)
        printf("%3d %s",i++,buf);

    pclose(fp);
    return 0;
}